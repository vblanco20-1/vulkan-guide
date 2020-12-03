#include "cvars.h"


#include <unordered_map>

#include <array>
#include <algorithm>
#include "imgui.h"
#include "imgui_stdlib.h"

enum class CVarType : char
{
	INT,
	FLOAT,
	STRING,
	FLOATVEC,
	INTVEC
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
	std::vector<CVarStorage<T>> cvars;
	int32_t lastCVar{ 0 };

	CVarArray(size_t size)
	{
		cvars.resize(size);
	}

	CVarStorage<T>* GetCurrentStorage(int32_t index)
	{
		return &cvars[index];
	}

	T* GetCurrentPtr(int32_t index)
	{
		return &((cvars.data() + index)->current);
	};

	T GetCurrent(int32_t index)
	{
		return cvars[index].current;
	};

	void SetCurrent(const T& val, int32_t index)
	{
		cvars[index].current = val;

		bool noEdit = ((uint32_t)cvars[index].parameter->flags & (uint32_t)CVarFlags::Noedit) != 0;
		bool readOnly = ((uint32_t)cvars[index].parameter->flags & (uint32_t)CVarFlags::EditReadOnly) != 0;
		if (!noEdit && !readOnly)
		{
			CVarSystemImpl::Get()->MarkDirty();
		}
	}

	int Add(const T& value, CVarParameter* param)
	{
		int index = lastCVar;

		cvars[index].current = value;
		cvars[index].initial = value;
		cvars[index].parameter = param;

		param->arrayIndex = index;
		lastCVar++;

		CVarSystemImpl::Get()->MarkDirty();
		return index;
	}

	int Add(const T& initialValue, const T& currentValue, CVarParameter* param)
	{
		int index = lastCVar;

		cvars[index].current = currentValue;
		cvars[index].initial = initialValue;
		cvars[index].parameter = param;

		param->arrayIndex = index;
		lastCVar++;

		CVarSystemImpl::Get()->MarkDirty();
		return index;
	}
};

uint32_t Hash(const char* str)
{
	return StringUtils::fnv1a_32(str, strlen(str));
}

class CVarSystemImpl : public CVarSystem
{
public:
	CVarParameter* GetCVar(StringUtils::StringHash hash) override final;

	CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue) override final;
	CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue) override final;
	CVarParameter* CreateIntCVar(const char* name, const char* description, int32_t defaultValue) override final;
	CVarParameter* CreateIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue) override final;
	CVarParameter* CreateStringCVar(const char* name, const char* description, const char* defaultValue) override final;
	CVarParameter* CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) override final;
	
	double* GetFloatCVar(StringUtils::StringHash hash) override final;
	int32_t* GetIntCVar(StringUtils::StringHash hash) override final;
	const char* GetStringCVar(StringUtils::StringHash hash) override final;
	

	void SetFloatCVar(StringUtils::StringHash hash, double value) override final;

	void SetIntCVar(StringUtils::StringHash hash, int32_t value) override final;

	void SetStringCVar(StringUtils::StringHash hash, const char* value) override final;

	void DrawImguiEditor() override final;

	void EditParameter(CVarParameter* p);

	constexpr static int MAX_INT_CVARS = 1000;

	CVarArray<int32_t> intCVars2{ MAX_INT_CVARS };

	constexpr static int MAX_FLOAT_CVARS = 1000;
	CVarArray<double> floatCVars{ MAX_FLOAT_CVARS };

	constexpr static int MAX_STRING_CVARS = 200;
	CVarArray<std::string> stringCVars{ MAX_STRING_CVARS };


	//using templates with specializations to get the cvar arrays for each type.
	//if you try to use a type that doesnt have specialization, it will trigger the static assert
	template<typename T>
	CVarArray<T>* GetCVarArray()
	{
		static_assert(false);
		return nullptr;
	}

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

	//templated get-set cvar versions for syntax sugar
	template<typename T>
	T* GetCVarCurrent(uint32_t namehash) {
		auto par = GetCVar(namehash);
		return GetCVarArray<T>()->GetCurrentPtr(par->arrayIndex);
	}

	template<typename T>
	void SetCVarCurrent(uint32_t namehash, const T& value)
	{
		GetCVarArray<T>()->SetCurrent(value, GetCVar(namehash)->arrayIndex);
	}

	static CVarSystemImpl* Get()
	{
		return static_cast<CVarSystemImpl*>(CVarSystem::Get());
	}

private:

	CVarParameter* InitCVar(const char* name, const char* description);

	std::unordered_map<uint32_t, CVarParameter> savedCVars;

	std::vector<CVarParameter*> cachedEditParameters;
};

double* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<double>(hash);
}

int32_t* CVarSystemImpl::GetIntCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<int32_t>(hash);
}

const char* CVarSystemImpl::GetStringCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<std::string>(hash)->c_str();
}


CVarSystem* CVarSystem::Get()
{
	static CVarSystemImpl cvarSys{};
	return &cvarSys;
}


CVarParameter* CVarSystemImpl::GetCVar(StringUtils::StringHash hash)
{
	auto it = savedCVars.find(hash);

	if (it != savedCVars.end())
	{
		return &(*it).second;
	}

	return nullptr;
}

void CVarSystemImpl::SetFloatCVar(StringUtils::StringHash hash, double value)
{
	SetCVarCurrent<double>(hash, value);
}

void CVarSystemImpl::SetIntCVar(StringUtils::StringHash hash, int32_t value)
{
	SetCVarCurrent<int32_t>(hash, value);
}

void CVarSystemImpl::SetStringCVar(StringUtils::StringHash hash, const char* value)
{
	SetCVarCurrent<std::string>(hash, value);
}


CVarParameter* CVarSystemImpl::CreateFloatCVar(const char* name, const char* description, double defaultValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::FLOAT;

	GetCVarArray<double>()->Add(defaultValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::FLOAT;

	GetCVarArray<double>()->Add(defaultValue, currentValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(const char* name, const char* description, int32_t defaultValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::INT;

	GetCVarArray<int32_t>()->Add(defaultValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::INT;

	GetCVarArray<int32_t>()->Add(defaultValue, currentValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(const char* name, const char* description, const char* defaultValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::STRING;

	GetCVarArray<std::string>()->Add(defaultValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr;

	param->type = CVarType::STRING;

	GetCVarArray<std::string>()->Add(defaultValue, currentValue, param);

	return param;
}

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

AutoCVar_Float::AutoCVar_Float(const char* name, const char* description, double defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateFloatCVar(name, description, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

double AutoCVar_Float::Get()
{
	return CVarSystemImpl::Get()->GetCVarArray<double>()->GetCurrent(index);
}

double* AutoCVar_Float::GetPtr()
{
	return CVarSystemImpl::Get()->GetCVarArray<double>()->GetCurrentPtr(index);
}

float AutoCVar_Float::GetFloat()
{
	return static_cast<float>(Get());
}

float* AutoCVar_Float::GetFloatPtr()
{
	float* result = reinterpret_cast<float*>(GetPtr());
	return result;
}

void AutoCVar_Float::Set(double f)
{
	CVarSystemImpl::Get()->GetCVarArray<double>()->SetCurrent(f, index);
}

AutoCVar_Int::AutoCVar_Int(const char* name, const char* description, int32_t defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateIntCVar(name, description, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

int32_t AutoCVar_Int::Get()
{
	return CVarSystemImpl::Get()->GetCVarArray<int32_t>()->GetCurrent(index);
}

int32_t* AutoCVar_Int::GetPtr()
{
	return CVarSystemImpl::Get()->GetCVarArray<int32_t>()->GetCurrentPtr(index);
}

void AutoCVar_Int::Set(int32_t val)
{
	CVarSystemImpl::Get()->GetCVarArray<int32_t>()->SetCurrent(val, index);
}

void AutoCVar_Int::Toggle()
{
	bool enabled = Get() != 0;

	Set(enabled ? 0 : 1);
}

AutoCVar_String::AutoCVar_String(const char* name, const char* description, const char* defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateStringCVar(name, description, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

const char* AutoCVar_String::Get()
{
	return CVarSystemImpl::Get()->GetCVarArray<std::string>()->GetCurrentPtr(index)->c_str();
};

void AutoCVar_String::Set(std::string&& val)
{
	CVarSystemImpl::Get()->GetCVarArray<std::string>()->SetCurrent(val, index);
}


void CVarSystemImpl::DrawImguiEditor()
{
	static std::string searchText = "";

	ImGui::InputText("Filter", &searchText);
	static bool bShowAdvanced = false;
	ImGui::Checkbox("Advanced", &bShowAdvanced);

	cachedEditParameters.clear();

	auto addToEditList = [&](auto parameter)
	{
		bool bHidden = ((uint32_t)parameter->flags & (uint32_t)CVarFlags::Noedit);
		bool bIsAdvanced = ((uint32_t)parameter->flags & (uint32_t)CVarFlags::Advanced);

		if (!bHidden)
		{
			if (!(!bShowAdvanced && bIsAdvanced) && parameter->name.find(searchText) != std::string::npos)
			{
				cachedEditParameters.push_back(parameter);
			};
		}
	};

	for (int i = 0; i < GetCVarArray<int32_t>()->lastCVar; i++)
	{
		addToEditList(GetCVarArray<int32_t>()->cvars[i].parameter);
	}
	for (int i = 0; i < GetCVarArray<double>()->lastCVar; i++)
	{
		addToEditList(GetCVarArray<double>()->cvars[i].parameter);
	}
	for (int i = 0; i < GetCVarArray<std::string>()->lastCVar; i++)
	{
		addToEditList(GetCVarArray<std::string>()->cvars[i].parameter);
	}
	
	if (cachedEditParameters.size() > 10)
	{
		std::unordered_map<std::string, std::vector<CVarParameter*>> categorizedParams;

		//insert all the edit parameters into the hashmap by category
		for (auto p : cachedEditParameters)
		{
			int dotPos = -1;
			//find where the first dot is to categorize
			for (int i = 0; i < p->name.length(); i++)
			{
				if (p->name[i] == '.')
				{
					dotPos = i;
					break;
				}
			}
			std::string category = "";
			if (dotPos != -1)
			{
				category = p->name.substr(0, dotPos);
			}

			auto it = categorizedParams.find(category);
			if (it == categorizedParams.end())
			{
				categorizedParams[category] = std::vector<CVarParameter*>();
				it = categorizedParams.find(category);
			}
			it->second.push_back(p);
		}

		for (auto [category, parameters] : categorizedParams)
		{
			//alphabetical sort
			std::sort(parameters.begin(), parameters.end(), [](CVarParameter* A, CVarParameter* B)
				{
					return A->name < B->name;
				});

			if (ImGui::BeginMenu(category.c_str()))
			{
				for (auto p : parameters)
				{
					EditParameter(p);
				}

				ImGui::EndMenu();
			}
		}
	}
	else
	{
		//alphabetical sort
		std::sort(cachedEditParameters.begin(), cachedEditParameters.end(), [](CVarParameter* A, CVarParameter* B)
			{
				return A->name < B->name;
			});

		for (auto p : cachedEditParameters)
		{
			EditParameter(p);
		}
	}
}

void CVarSystemImpl::EditParameter(CVarParameter* p)
{
	const bool readonlyFlag = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditReadOnly);
	const bool checkboxFlag = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditCheckbox);
	const bool dragFlag = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditFloatDrag);

	switch (p->type)
	{
	case CVarType::INT:

		if (readonlyFlag)
		{
			std::string displayFormat = p->name + "= %i";
			ImGui::Text(displayFormat.c_str(), GetCVarArray<int32_t>()->GetCurrent(p->arrayIndex));
		}
		else
		{
			if (checkboxFlag)
			{
				bool bCheckbox = GetCVarArray<int32_t>()->GetCurrent(p->arrayIndex) != 0;
				if (ImGui::Checkbox(p->name.c_str(), &bCheckbox))
					GetCVarArray<int32_t>()->SetCurrent(bCheckbox ? 1 : 0, p->arrayIndex);

				// intCVars[p->arrayIndex].current = bCheckbox ? 1 : 0;
			}
			else
			{
				if (ImGui::InputInt(p->name.c_str(), GetCVarArray<int32_t>()->GetCurrentPtr(p->arrayIndex)))
					MarkDirty();
			}
		}
		break;

	case CVarType::FLOAT:

		if (readonlyFlag)
		{
			std::string displayFormat = p->name + "= %f";
			ImGui::Text(displayFormat.c_str(), GetCVarArray<double>()->GetCurrent(p->arrayIndex));
		}
		else
		{
			if (dragFlag)
			{
				if (ImGui::InputDouble(p->name.c_str(), GetCVarArray<double>()->GetCurrentPtr(p->arrayIndex)))
					MarkDirty();
			}
			else
			{
				if (ImGui::InputDouble(p->name.c_str(), GetCVarArray<double>()->GetCurrentPtr(p->arrayIndex)))
					MarkDirty();
			}
		}
		break;

		case CVarType::STRING:

		if (readonlyFlag)
		{
			std::string displayFormat = p->name + "= %s";
			ImGui::Text(displayFormat.c_str(), GetCVarArray<std::string>()->GetCurrent(p->arrayIndex));
		}
		else
		{
			if (ImGui::InputText(p->name.c_str(), GetCVarArray<std::string>()->GetCurrentPtr(p->arrayIndex)))
				MarkDirty();
		}
		break;

	default:
		break;
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(p->description.c_str());
	}
}