---
layout: default
title: Drawing Images
parent: "5. Textures"
nav_order: 15
---

There are a few things we have to modify to get textures working in the shaders.
The first of them is modifying the vertex format so that we store UV coordinates.

```cpp
#include <glm/vec2.hpp> //now needed for the Vertex struct

// other code ....

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	static VertexInputDescription get_vertex_description();
};
```

We just add a new `glm::vec2 uv;` to hold the UV coordinates in the Vertex struct.
We also need to add the new attribute to the vertex description

```cpp
VertexInputDescription Vertex::get_vertex_description()
{

	//position, normal, and color descriptions

	//UV will be stored at Location 3
	VkVertexInputAttributeDescription uvAttribute = {};
	uvAttribute.binding = 0;
	uvAttribute.location = 3;
	uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttribute.offset = offsetof(Vertex, uv);

	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(normalAttribute);
	description.attributes.push_back(colorAttribute);
	description.attributes.push_back(uvAttribute);
	return description;
}
```
We store the UVs in attribute location 3, and the format is R32G32 float, which is perfect for `glm::vec2`

Now we need to load the texture coordinates from the obj files.


```cpp
bool Mesh::load_from_obj(const char* filename)
{
	//load code

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

			//other parameters


			//vertex uv
			tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
			tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

			new_vert.uv.x = ux;
			new_vert.uv.y = 1-uy;
		}
	}
}
```
We access the texcoords array in a similar way as we do with the other parameters.
Now the obj load code will load texture coordinates. It's very important to do the 1-y on the uv.y because Vulkan UV coordinates work like that.

Before we start changing the descriptors and descriptor layouts to point to the texture, we are going to create new shaders for the textured-lit shaders.
We begin by modifying the `tri_mesh.vert` shader so that it passes the UV coordinates from the attributes to the pixel shader

tri_mesh.vert
```glsl
#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 texCoord;

//uniforms and ssbos


void main()
{
	mat4 modelMatrix = objectBuffer.objects[gl_InstanceIndex].model;
	mat4 transformMatrix = (cameraData.viewproj * modelMatrix);
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outColor = vColor;
	texCoord = vTexCoord;
}
```

And we create a new pixel shader where we display the texture coordinates as output color. This is to check that we are loading the parameters correctly.

textured_lit.frag
```glsl
//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 texCoord;
//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{
	vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;


void main()
{
	outFragColor = vec4(texCoord.x,texCoord.y,0.5f,1.0f);
}
```

On the `init_pipelines()` function, we are going to create a second pipeline that will be this textured-pipeline.

```cpp
void VulkanEngine::init_pipelines()
{

	VkShaderModule texturedMeshShader;
	if (!load_shader_module("../../shaders/textured_lit.frag.spv", &texturedMeshShader))
	{
		std::cout << "Error when building the textured mesh shader" << std::endl;
	}

	//setup for the other pipeline

	//build the mesh triangle pipeline
	VkPipeline meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	create_material(meshPipeline, texturedPipeLayout, "defaultmesh");


	//create pipeline for textured drawing
	pipelineBuilder._shaderStages.clear();
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, texturedMeshShader));

	VkPipeline texPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
	create_material(texPipeline, texturedPipeLayout, "texturedmesh");

	// other code ....
	vkDestroyShaderModule(_device, texturedMeshShader, nullptr);

	// add pipeline and pipeline layout to deletion code
	_mainDeletionQueue.push_function([=]() {
		vkDestroyPipeline(_device, texPipeline, nullptr);
		vkDestroyPipelineLayout(_device, texturedPipeLayout, nullptr);

		// other code ....
	}
}
```

We can now add a new render object that will be the textured mesh. For that, I'm using lost_empire.obj, which is a minecraft map. This makes for a good test case to check textured rendering.

```cpp
void VulkanEngine::load_meshes()
{
	//other meshes
	Mesh lostEmpire{};
	lostEmpire.load_from_obj("../../assets/lost_empire.obj");

	upload_mesh(lostEmpire);

	_meshes["empire"] = lostEmpire;
}

void VulkanEngine::init_scene()
{
	//others
	RenderObject map;
	map.mesh = get_mesh("empire");
	map.material = get_material("texturedmesh");
	map.transformMatrix = glm::translate(glm::vec3{ 5,-10,0 });

	_renderables.push_back(map);
```


This should work now, giving us this image if we run it. Or similar

![map]({{site.baseurl}}/diagrams/empireUntextured.png)


It is time now to expose the image to the shader.
We will change `textured_lit.frag` so that it accesses a texture

```glsl

layout(set = 2, binding = 0) uniform sampler2D tex1;

void main()
{
	vec3 color = texture(tex1,texCoord).xyz;
	outFragColor = vec4(color,1.0f);
}
```

We are binding the sampler2d texture into Set 2, binding 0.
At the moment we are using sets 0 and 1, so this will be the 3rd descriptor set.
As we have a new descriptor set, we need to create its layout and add it to the engine.
We will add a texture descriptor parameter into our Material struct, and we will also store the descriptor layout into the engine class


```cpp
struct Material {
	VkDescriptorSet textureSet{VK_NULL_HANDLE}; //texture defaulted to null
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

class VulkanEngine {
public:

VkDescriptorSetLayout _singleTextureSetLayout;
}
```

On `init_descriptors()`, we are going to create the layout, but not allocate the descriptor yet.

```cpp
void VulkanEngine::init_descriptors()
{
	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
		//add combined-image-sampler descriptor types to the pool
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
	};

	//other descriptor layouts

	//another set, one that holds a single texture
	VkDescriptorSetLayoutBinding textureBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

	VkDescriptorSetLayoutCreateInfo set3info = {};
	set3info.bindingCount = 1;
	set3info.flags = 0;
	set3info.pNext = nullptr;
	set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set3info.pBindings = &textureBind;

	vkCreateDescriptorSetLayout(_device, &set3info, nullptr, &_singleTextureSetLayout);
}
```

With the set layout initialized, we can attach it to the layout for the textured pipeline.

On `init_pipelines()`.
```cpp

	//create pipeline layout for the textured mesh, which has 3 descriptor sets
	//we start from  the normal mesh layout
	VkPipelineLayoutCreateInfo textured_pipeline_layout_info = mesh_pipeline_layout_info;

	VkDescriptorSetLayout texturedSetLayouts[] = { _globalSetLayout, _objectSetLayout,_singleTextureSetLayout };

	textured_pipeline_layout_info.setLayoutCount = 3;
	textured_pipeline_layout_info.pSetLayouts = texturedSetLayouts;

	VkPipelineLayout texturedPipeLayout;
	VK_CHECK(vkCreatePipelineLayout(_device, &textured_pipeline_layout_info, nullptr, &texturedPipeLayout));

	pipelineBuilder._shaderStages.clear();
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, texturedMeshShader));

	//connect the new pipeline layout to the pipeline builder
	pipelineBuilder._pipelineLayout = texturedPipeLayout;
	VkPipeline texPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
	create_material(texPipeline, texturedPipeLayout, "texturedmesh");

```

We now need to create the descriptor set in the `init_scene()` function, so that our `texturedmesh` material has the texture set


We add a new initializers to vk_initializers, for sampler creation and descriptor image write
```cpp
//header
VkSamplerCreateInfo sampler_create_info(VkFilter filters, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);

//implementation
VkSamplerCreateInfo vkinit::sampler_create_info(VkFilter filters, VkSamplerAddressMode samplerAddressMode /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/)
{
	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext = nullptr;

	info.magFilter = filters;
	info.minFilter = filters;
	info.addressModeU = samplerAddressMode;
	info.addressModeV = samplerAddressMode;
	info.addressModeW = samplerAddressMode;

	return info;
}
VkWriteDescriptorSet vkinit::write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding)
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pImageInfo = imageInfo;

	return write;
}
```

Now we can use it on `init_scene`.

```cpp
	//create a sampler for the texture
	VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(VK_FILTER_NEAREST);

	VkSampler blockySampler;
	vkCreateSampler(_device, &samplerInfo, nullptr, &blockySampler);



	Material* texturedMat=	get_material("texturedmesh");

	//allocate the descriptor set for single-texture to use on the material
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext = nullptr;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &_singleTextureSetLayout;

	vkAllocateDescriptorSets(_device, &allocInfo, &texturedMat->textureSet);

	//write to the descriptor set so that it points to our empire_diffuse texture
	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.sampler = blockySampler;
	imageBufferInfo.imageView = _loadedTextures["empire_diffuse"].imageView;
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet texture1 = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texturedMat->textureSet, &imageBufferInfo, 0);

	vkUpdateDescriptorSets(_device, 1, &texture1, 0, nullptr);
```

We need to create a sampler first. For that sampler we will use `VK_FILTER_NEAREST`. That will make the texture look blocky, which is exactly what we want.

For the descriptor allocation, it's the same as we were doing in `init_descriptors()` with the others.

To write the image descriptor, we need to fill a `VkDescriptorImageInfo` with the `VkImageView` to use, the `VkSampler` to use, and the texture layout.

With the descriptor sets and the pipelines set up, we can now bind the descriptor set to draw using the texture.


In `draw_objects()`, inside the render loop, we do this alongside the other binds

```cpp
	if (object.material != lastMaterial) {

		//other bindings

		if (object.material->textureSet != VK_NULL_HANDLE) {
			//texture descriptor
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 2, 1, &object.material->textureSet, 0, nullptr);

		}
	}
```

We will only bind the texture set (set #2), if the texture set handle isn't null. This will make it so that the rest of our drawcalls still render just fine.

At this point, you should get this image as result.
![map]({{site.baseurl}}/diagrams/empireTextured.png)


{% include comments.html term="Chapter 5 Comments" %}





