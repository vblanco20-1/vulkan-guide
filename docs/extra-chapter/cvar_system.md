---
layout: default
title: Configurable options through a CVAR system
parent: Extra Chapter
nav_order: 33
---


## CVars
As the engine grows, we find that we have more and more configuration options in the engine. Having them tweable in the UI requires us to add some edit code every time we add a variable. This gets annoying very quickly, and those configuration variables also aren't globally accessible.

If you look at how engines like Unreal Engine or IDTech engines do it, they have a Console Variable system. Users can declare any console variable (CVAR) they want, and then access it through the code. Having the configuration and debug options all inside the same centralized location gives some very nice properties, such as having 1 centralized way to edit them, and being able to easily save them to disk.

For the vulkan engine, we are going to implement a system for CVARs very similar to the one in Unreal Engine, but highly simplified. This is also something that can be done for any Cpp project, it doesn't require anything from the rest of the engine. The CVar system explained here is the same as the one that was created for the Novus Core project, but with some small tweaks.

## Usage
To declare a CVAR, it's done this way:
```cpp

//checkbox CVAR
AutoCVar_Int CVAR_TestCheckbox("test.checkbox", "just a checkbox", 0, CVarFlags::EditCheckbox);

//int CVAR
AutoCVar_Int CVAR_TestInt("test.int", "just a configurable int", 42);

//float CVAR
AutoCVar_Int CVAR_TestFloat("test.float", "just a configurable float", 13.37);

//string CVAR
AutoCVar_String CVAR_TestString("test.string", "just a configurable string", "just a configurable string");
```

We declare it as a global inside any cpp file. Doing this will abuse Cpp static initialization to make sure that it registers itself with the CVarSystem, which is a singleton. It can be added anywhere, including inside functions.

Once a cvar is created, you can use it directly.

```cpp
// get value
int var = CVAR_TestInt.Get();
//set value
CVAR_TestInt.Set(7);
```

Using the cvar property is the quickest and most efficient way to access a CVAR, as it caches the data properly. But this is not the only way of doing so. Part of the reason to have CVars like this is that they can be accessed anywhere.

```cpp

//returns a pointer because it will be nullptr if the cvar doesn't exist
int* value = CVarSystem::Get()->GetIntCvar("test.int");
if(value)
{
	int var = *value;
}

//for the setter it can be done directly.
//if the cvar doesn't exist, this does nothing
 CVarSystem::Get()->SetIntCvar("test.int",3);

```

This makes CVars a sort of global database. Do not use it like that. CVars are meant for configuration variables and other similar globals, so it's best to avoid using it as a global variable storage.

Lastly, it's possible to create cvars at runtime. Most useful if you are loading cvars from files or as part of a modding system or scripts.
The AutoCvar things above use this

```cpp

CVarParameter* cvar =  CVarSystem::Get()->CreateIntCVar("test.int2", "another int cvar", 3/*default value */, 3/* current value*/);

```

It's not possible to remove cvars. This is by design, as it doesn't make a lot of sense to delete these global variables.

All cvars will get a spot into the cvar editor menu on the Imgui menu bar. This makes it very easy to see what the cvar values are and allows a centralized spot to edit them from inside the engine.

Now lets talk about how the code works

## Architecture
You can look at the code here:
[Header](https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/cvars.h)
[Source](https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/cvars.cpp)

We are going to start coding the main CVarSystem class. This is going to be a singleton so that it can be easuly accessed anywhere by demand.

A very important detail is that we are going to employ a inheritance-based pImpl pattern. We are going to create 2 classes, one will be the interface class, in the header, while the other will be the actual implementation.
Every function inside CVarSystem will be a pure virtual, and we will have the implementation in the cpp file

We are being very aggresive with the private implementation because a cvar system is something that will very quickly be included in the entire codebase. We use Imgui in the implementation alongside some of the STL, so we *really* do not want those to leak in the public interface.

```cpp
class CVarSystem
{

public:
	//just a Get() for now
	static CVarSystem* Get();
}
```

On the cpp file, we are going to add the implementation class
```cpp
class CVarSystemImpl : public CVarSystem
{
}

//static initialized singleton pattern
CVarSystem* CVarSystem::Get()
{
	static CVarSystemImpl cvarSys{};
	return &cvarSys;
}
```

The Impl class is empty for now, but we implement the `CVarSystem::Get()` function. In there, we declare a static CVarSystemImpl object, and return its address.
This is known as the statically initialized singleton, and it's the modern way of doing Singletons in Cpp11 and more. It has the very interesting property that Get() is fully threadsafe due to the rules of static variables inside functions.

To store the cvars, we are going to implement a way to store the cvars into the System, for that we start by implementing a very simple cvar struct

`cvars.h`
```cpp
enum class CVarFlags : uint32_t
{
	None = 0,
	Noedit = 1 << 1,
	EditReadOnly = 1 << 2,
	Advanced = 1 << 3,

	EditCheckbox = 1 << 8,
	EditFloatDrag = 1 << 9,
};
```

`cvars.cpp`
```cpp
enum class CVarType : char
{
	INT,
	FLOAT,
	STRING,
};

class CVarParameter
{
public:
	friend class CVarSystemImpl;

	int32_t arrayIndex;

	CVarType type;
	CVarFlags flags;
	std::string name;
	std::string description;
};
```

This CVarParameter is going to store the name, description, and type.
It also stores flags and arrayIndex. The flags are declared in the header because they will be part of the public interface, but the CVarParameter itself is going to be private details.

We store arrayIndex inside the parameter because we are going to store the cvar data (the actual ints/floats) into arrays, so we need to access them. This way we can use this same struct for all types of data.

Lets implement that data array.

`cvars.cpp`
```cpp


template<typename T>
struct CVarStorage
{
	T initial;
	T current;
	CVarParameter* parameter;
};

template<typename T>
struct CVarArray
{
	CVarStorage<T>* cvars;
	int32_t lastCVar{ 0 };

	CVarArray(size_t size)
	{
		cvars = new CVarStorage<T>[size]();
	}
	~CVarArray()
	{
		delete cvars;
	}

	T GetCurrent(int32_t index)
	{
		return cvars[index].current;
	};

	void SetCurrent(const T& val, int32_t index)
	{
		cvars[index].current = val;
	}

	int Add(const T& value, CVarParameter* param)
	{
		int index = lastCVar;

		cvars[index].current = value;
		cvars[index].initial = value;
		cvars[index].parameter = param;

		param->arrayIndex = index;
		lastCVar++;
		return index;
	}

	//functions elided to keep it short. look at source code for the extras.
};
```

We will be using templates for this to make sure it's much simpler to implement the multiple types of cvars.

Starting by the `CVarStorage<T>` type, where we store the current and initial values of a given CVar, alongside the pointer to the `CVarParamter` this Storage holds the value for.

`CVarArray<T>` is an array of those CVarStorage objects, with a few functions to handle said array. It's going to be a very typical heap-allocated array like a vector, but we won't support resizing.

On the array, we hold an integer to know how filled it is, and we use the index to get the data. This index will correlate to the index stored in a CVarStorage.

We can now add this arrays into the CVarSystemImpl class

`cvar.cpp`
```cpp
class CVarSystemImpl : public CVarSystem
{
public:

	constexpr static int MAX_INT_CVARS = 1000;
	CVarArray<int32_t> intCVars2{ MAX_INT_CVARS };

	constexpr static int MAX_FLOAT_CVARS = 1000;
	CVarArray<double> floatCVars{ MAX_FLOAT_CVARS };

	constexpr static int MAX_STRING_CVARS = 200;
	CVarArray<std::string> stringCVars{ MAX_STRING_CVARS };

	//using templates with specializations to get the cvar arrays for each type.
	//if you try to use a type that doesn't have specialization, it will trigger a linked error
	template<typename T>
	CVarArray<T>* GetCVarArray();

	template<>
	CVarArray<int32_t>* GetCVarArray()
	{
		return &intCVars2;
	}
	template<>
	CVarArray<double>* GetCVarArray()
	{
		return &floatCVars;
	}
	template<>
	CVarArray<std::string>* GetCVarArray()
	{
		return &stringCVars;
	}
}
```

We are going to hardcode the array sizes for all the cvars. This is done to simplify the system. Generally you don't really have that many cvars, so it's not really wasting much space. 1000 float and int cvars is bigger than what UnrealEngine uses, and Quake 3 only had a few dozens.

One thing to note here is the `GetCVarArray` template trick. By doing this, we can allow a very significant code repetition when dealing with the different data arrays.
We start by declaring the GetCVarArray template function, with a empty implementation that will error, in case it's not implemented. We then follow with 3 separated specializations of the GetCVarArray template.
The end result of this is that we can then do this.

```cpp

auto array = GetCVarArray<int32_t>() //gets array for int32 cvars

```

We continue with the actual code to create and register a cvar.
```cpp
class CVarSystem
{
public:
	virtual CVarParameter* GetCVar(StringUtils::StringHash hash) = 0;
	virtual CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue) = 0;
	//other getters
}
```

```cpp
class CVarSystemImpl : public CVarSystem
{
public:

	CVarParameter* GetCVar(StringUtils::StringHash hash) override final;

	CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue) override final;
	//more versions of Create for ints/strings

private:
	CVarParameter* InitCVar(const char* name, const char* description);

	std::unordered_map<uint32_t, CVarParameter> savedCVars;
}
```

In here we introduce the `StringUtils::StringHash` object. This is a hash for a string. On the internals of the cvars we will often refer to them by hash. StringHash object lets us do the hashing at compile time, which is a very nice thing. It means that
```cpp

GetCVar("cvar.int")

```
will compute the hash of the string at compile time.
You can look at the implementation of the struct in `string_utils.h`.

We are also adding a CreateFloatCVar (and others) to both the public interface class and the private implementation.

On the private, we also have InitCVar, and a hashmap of CVarParameters which is where we will be storing the cvars. In most implementations using a uint32_t as key in a unordered_map will not hash it.

Lets implement the creation functions.


```cpp
CVarParameter* CVarSystemImpl::InitCVar(const char* name, const char* description)
{
	if (GetCVar(name)) return nullptr; //return null if the cvar already exists

	uint32_t namehash = StringUtils::StringHash{ name };
	savedCVars[namehash] = CVarParameter{};

	CVarParameter& newParam = savedCVars[namehash];

	newParam.name = name;
	newParam.description = description;

	return &newParam;
}
```

For the InitCVar function we aren't doing much, we hash the name of the cvar, set its name and description, and insert it into the savedCVars hashmap.

The second part of the initialization is the typed part. We will be showing float, but it works the same for all types.
```cpp
CVarParameter* CVarSystemImpl::CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::FLOAT;

	GetCVarArray<double>()->Add(defaultValue, currentValue, param);

	return param;
}
```
On the typed CreateFloatCVar function, we initialize the cvar with the above function, and then we use
`GetCVarArray<double>()` to grab the correct data array to insert the cvar into. We also make sure to set the param->type of the cvar to Float type.

For int and string it's the same just replacing the types.

Now that we can create cvars, lets continue on the GetCVar function
```cpp
CVarParameter* CVarSystemImpl::GetCVar(StringUtils::StringHash hash)
{
	auto it = savedCVars.find(hash);

	if (it != savedCVars.end())
	{
		return &(*it).second;
	}

	return nullptr;
}
```
Which just grabs the cvar from the hashmap.

We can now create and grab the different types of CVArs from the system, but we still havent implemented the functions to store and retrieve data from them.

Lets add more functions to the public interface.
```cpp
class CVarSystem
{

public:
	virtual double* GetFloatCVar(StringUtils::StringHash hash) = 0;
	virtual void SetFloatCVar(StringUtils::StringHash hash, double value) = 0;
}
```

We will have a GetFloatCVar function that gets a stringhash and returns a pointer to the data. We also have the equivalent Setter.

Continue to the implementation class

```cpp
class CVarSystemImpl : public CVarSystem
{
public:

	double* GetFloatCVar(StringUtils::StringHash hash) override final;
	void SetFloatCVar(StringUtils::StringHash hash, double value) override final;


//templated get-set cvar versions for syntax sugar
	template<typename T>
	T* GetCVarCurrent(uint32_t namehash) {
		CVarParameter* par = GetCVar(namehash);
		if (!par) {
			return nullptr;
		}
		else {
			return GetCVarArray<T>()->GetCurrentPtr(par->arrayIndex);
		}
	}

	template<typename T>
	void SetCVarCurrent(uint32_t namehash, const T& value)
	{
		CVarParameter* cvar = GetCVar(namehash);
		if (cvar)
		{
			GetCVarArray<T>()->SetCurrent(value, cvar->arrayIndex);
		}
	}
}
```

We add the Get and Set as overriden functions for the implementation, but we are also creating a templated version of Get and Set, which will be shared for the different types.

For the GetCVarCurrent, we grab the cvar by hash, and then we grab the data by the array index in the cvar. The setter works in a similar way.

Lets now implement the actual interface functions for the get/set

```cpp
double* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<double>(hash);
}
void CVarSystemImpl::SetFloatCVar(StringUtils::StringHash hash, double value)
{
	SetCVarCurrent<double>(hash, value);
}
```

In those, we just call the templated version. This way we avoid having a lot of repeated code for implementing this per type.

The core API of the CVarSystem is implemented now. This is all it is. The AutoCVar objects are just syntactic sugar for these functions.

Lets implement the AutoCVar_Float one to show how they work.

```cpp
template<typename T>
struct AutoCVar
{
protected:
	int index;
	using CVarType = T;
};

struct AutoCVar_Float : AutoCVar<double>
{
	AutoCVar_Float(const char* name, const char* description, double defaultValue, CVarFlags flags = CVarFlags::None);

	double Get();
	void Set(double val);
};
```

All of the typed AutoCVar objects will inherit from AutoCVar which has the index. This index will directly index into the data arrays for the given types.

Back on the cpp file, we now fill the AutoCVar functions.

```cpp

//get the cvar data purely by type and array index
template<typename T>
T GetCVarCurrentByIndex(int32_t index) {
	return CVarSystemImpl::Get()->GetCVarArray<T>()->GetCurrent(index);
}

//set the cvar data purely by type and index
template<typename T>
void SetCVarCurrentByIndex(int32_t index,const T& data) {
	CVarSystemImpl::Get()->GetCVarArray<T>()->SetCurrent(data, index);
}

//cvar float constructor
AutoCVar_Float::AutoCVar_Float(const char* name, const char* description, double defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateFloatCVar(name, description, defaultValue, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

double AutoCVar_Float::Get()
{
	return GetCVarCurrentByIndex<CVarType>(index);
}
void AutoCVar_Float::Set(double f)
{
	SetCVarCurrentByIndex<CVarType>(f,index);
}

```
We are back to more template trickery. For all of the autocvars to have some abstracted syntax, we create 2 Get and Set cvar functions that are global (not inside the class) and directly set and get the values by the template type itself.

We can then implement AutoCVar_Float Get/Set from them using the CVarType typedef which is there by inheritance from `AutoCVar<double>` The constructor registers the cvar and stores the index.

Again, this works exactly the same for all other types.

If you want to add more types to the cvar system, you just need to create the functions in the public API, implement them, and add a storage array to the implementation. Some common types to add are vector properties, or maybe even other objects.

For the imgui editor itself. It's just normal imgui edit functions that are used from the data arrays.
