---
layout: default
title: Drawing a triangle
parent: chapter_2
nav_order: 11
---

The first thing we are going to need to draw our first triangle, is to set the shaders that it will use.
Foe the triangle, we will only need 2 shaders, which is the minimum to render 3d objects.
We are going to have a fragment shader that just colors the fragment red, and a vertex shader that forms a triangle.

Shaders are written in GLSL, which is OpenGL Shading Language. Its similar to C, but with some small differences. 

Vertex shaders will execute the function (in this case we will use `main()`, but it can have other names) once per vertex in the drawcall. The job of a vertex shader is to write to gl_Position to output the final location of the vertex, and output more variables to the fragment shader.

The fragment shader will use the variables that come from the vertex shader, and it will be executed once per pixel that each triangle covers. Its job is to output the final color.


We are going to add 2 new files to our shader folder. triangle.frag and triangle.vert


## Vertex Shader
Lets open triangle.vert and write the shader:

```glsl
//we will be using glsl version 4.5 syntax
#version 450

void main() 
{
	//const array of positions for the triangle
	const vec3 positions[3] = vec3[3](
		vec3(1.f,1.f, 0.0f),
		vec3(-1.f,1.f, 0.0f),
		vec3(0.f,-1.f, 0.0f)
	);

	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex], 1.0f);
}
```

In our first vertex shader, we create a constant array of 3 vector-3s, which will be the positions for each of the vertices of the triangle. 

We then have to write to `gl_Position` to tell the GPU whats the position for the vertex. This is obligatory for vertex shaders to work. In here we will access our positions array with gl_VertexIndex, which is the number of the vertex that this shader is being executed for. We then convert it into a vec4 because thats what `gl_Position` expects.

## Fragment shader

```glsl
//glsl version 4.5
#version 450

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	//return red
	outFragColor = vec4(1.f,0.f,0.f,1.0f);
}
```

Our fragment shader is even simpler. We will just return harcoded red color.

The output line is very important
```glsl
layout (location = 0) out vec4 outFragColor;
```
In here, we are declaring the variable for the output of the fragment shader. We are declaring that we will output a vec4 on location 0. If we were writing to multiple images at once, like from a GBuffer, we would have more output variables, but in this case only one is enough.


## Compiling the shaders.

If the shaders are on the correct folder, project/shaders, they will be detected by CMake.
Re-generate the cmake visual studio project, and they should be detected. 
Check the project "Shaders" on the generated visual studio solution, and the new 2 files should be in there. If you rebuild the Shaders project, the shaders should be compiled. In the build output it will give the errors if they happen.

If you look at the CMakeLists.txt at the root project folder, you will see that its creating a custom Shader targets building from grabbing all the files that end in *.frag and *.vert from the shaders/ folder. I recommend you read that section. It is commented explaining how it works.

## Vulkan Shader workflow
Vulkan doesnt understand GLSL directly, it understands SPIRV. SPIRV is shader bytecode for vulkan. Think of SPIRV as a binary optimized version of GLSL. 

We need to convert the GLSL we have just written into spirv, so that vulkan can understand it. Thats what we did above, and the result is some .spv files that we can load onto vulkan.

The Vulkan SDL comes with a built version of the glslang shader compiler, which is what we are using here to compile the shaders offline. It is possible to use the same compiler as a library, and compile GLSL shaders on-the-fly in your game engine, but we are not going to do that *yet*. 

## Loading the shaders in the code

Now that we have our .spv files, we can attempt to load them.

In vulkan, loaded shaders are stored in a VkShaderModule. You can use and combine multiple of them with multiple pipelines, so we are going to begin by creating a "load_module()" function that will load and compile the SPIRV file into a VkShaderModule

Lets begin by adding a new function to our VulkanEngine class 
```cpp

//loads a shader module from a spir-v file. Returns false if it errors
bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);
```

And start its implementation

```cpp
bool VulkanEngine::load_shader_module(const char* filePath, VkShaderModule* outShaderModule)
{
	return false;
}
```

We are going to use standard file output from Cpp. Make sure that you add `<fstream>` to the list of includes in vk_engine.cpp


First thing we are going to do in the load_shader_module function is to open the file
```cpp
bool VulkanEngine::load_shader_module(const char* filePath, VkShaderModule* outShaderModule)
{
	//open the file. With cursor at the end
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return false;
	}
}
```

We are going to use the flags `std::ios::binary` to open the stream in binary mode, and the `std::ios::ate` to put the stream cursor AT End

In cpp, the file operations are done on streams, and it has a cursor that we will have to use. Because the cursor is now at the end of the file, we can use it to know how big the file is, create a `std::vector<uint32_t>` big enough to hold the whole shader file, and then copy the entire file into the vector

```cpp

//find what the size of the file is by looking up the location of the cursor
//because the cursor is at the end, it gives the size directly in bytes
size_t fileSize = (size_t)file.tellg();

//spirv expects the buffer to be on uint32, so make sure to reserve a int vector big enough for the entire file
std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

//put file cursor at beggining
file.seekg(0);

//load the entire file into the buffer
file.read((char*)buffer.data(), fileSize);

//now that the file is loaded into the buffer, we can close it
file.close();

```

With this, we now have loaded the entire shader file into the `buffer` std vector, and can load it onto vulkan.


```cpp

//create a new shader module, using the buffer we loaded
VkShaderModuleCreateInfo createInfo = {};
createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
createInfo.pNext = nullptr;

//codeSize has to be in bytes, so multply the ints in the buffer by size of int to know the real size of the buffer
createInfo.codeSize = buffer.size() * sizeof(uint32_t); 
createInfo.pCode = buffer.data();

//check that the creation goes well.
VkShaderModule shaderModule;
if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
	return false;
}
*outShaderModule = shaderModule;
return true;

```

Its very common to have errors in the shader that will give a fail on vkCreateShaderModule, so we will not use the VK_Check macro here. 

Creating a VkShaderModule is very straightforward, we just need to fill the create info with the typical vulkan sType and pnext boilerplate, and then set the code pointer to the vector where we are storing the file. 
Then we just call the function with it to get the result.



## Loading the shaders on initialization.
We are going to add yet another init_ function to VulkanEngine class, `init_pipelines()`, where we are going to initialize the pipelines for the objects we are going to render. We arent going to write the pipeline yet, but we will attempt to load the SPIRV shaders to see if everything is going well.

Add it to the other init_ declarations in the class too
```cpp
void VulkanEngine::init_pipelines(){

VkShaderModule triangleFragShader;
	if (!load_shader_module("../../shaders/triangle.frag.spv", triangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else {
		std::cout << "Triangle fragment shader succesfully loaded" << std::endl;
	}

	VkShaderModule triangleVertexShader;
	if (!load_shader_module("../../shaders/triangle.vert.spv", triangleVertexShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
		
	}
	else {
		std::cout << "Triangle vertex shader succesfully loaded" << std::endl;
	}
}
```

Lets load both shader modules using relative paths. Its very easy to have it not work well, so we will print to the console if the shaders loaded or not.

Only thing left is to call the init_pipelines() function from our main init() function. We can call it at any point we want as long as its after init_vulkan(), but we will just add it to the end.

```cpp
void VulkanEngine::init()
{
	// ... other stuff ...


	init_pipelines();

	//everything went fine
	_isInitialized = true;
}

```


If you run the code at this point, you should see the "succesfully loaded" outputs in the console window.


![triangle]({{site.baseurl}}/diagrams/redTriangle.png)

Congratulations on your first triangle! 
We can now proceed to do interesting things with the shaders to make it more interesting.