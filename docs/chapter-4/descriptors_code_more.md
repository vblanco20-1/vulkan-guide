---
layout: default
title: Dynamic Descriptor sets
parent: Chapter 4
nav_order: 15
---

We have 2 descriptor sets right now. Each of them points into a different VkBuffer, and we hold the camera information there.
But we dont need to have 1 descriptor point to 1 buffer. We can have multiple descriptors point into different parts of the same buffer.

We are going to change the code a bit. We will add a "environment data" buffer, holding information like ambient tint and fog settings. But instead of holding it as 2 buffers, we will store it on one buffuer.

Allocating multiple things into the same buffer is generally a good idea in vulkan. There are some very nice techniques that can be done with that, like dynamic descriptors, that allow to reuse the same descriptor set over and over again but every time with a different offset into the buffer.

The main complication that comes from suballocating data on a buffer, is that you need to be very mindful of alignement. The GPUs often cant read from an arbitrary adress, and your buffer offsets have to be aligned into a certain minimum size.

## Looking up GPU stats
To know what is the minimum alignement size for buffers, we need to query it from the GPU.  The limit we are looking for is called `minUniformBufferOffsetAlignment`. You can look at said limit in the vulkaninfo page https://vulkan.gpuinfo.org/displaydevicelimit.php?name=minUniformBufferOffsetAlignment&platform=windows

We can see that all gpus at least support an alignement of 256 bytes, but we are going to check it at runtime anyway.
To do that, we are going to add some code to `init_vulkan()` where we are going to request gpu information and store the minimum alignement that we need.

We are going to add a `VkPhysicalDeviceProperties` member to the VulkanEngine class, and make vulkan fill it.  The struct will contain most gpu properties, so its useful to keep it around.

```cpp
class VulkanEngine {
	VkPhysicalDeviceProperties _gpuProperties;
}
```

```cpp
void VulkanEngine::init_vulkan()
{
	// other initialization code .....

	vkGetPhysicalDeviceProperties(_chosenGPU, &_gpuProperties);

	std::cout << "The gpu has a minimum buffer alignement of " << _gpuProperties.limits.minUniformBufferOffsetAlignment << std::endl;
}
```

Run this now, and note what the alignement for the uniform buffers is. In a Nvidia 2080 RTX, the offset alignement is 64 bytes. Other GPUs might have different alignements.

Now that we know whats the offset alignement that we need, we are going to create a struct to hold our SceneParameters, and bind it to the shaders. Lets begin by creating a new struct to hold the state we are going to be used in the shaders.

```cpp
struct GPUSceneData {
	glm::vec4 fogColor; // w is for exponent
	glm::vec4 fogDistances; //x for min, y for max, zw unused.
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; //w for sun power
	glm::vec4 sunlightColor;
};
```

```cpp
class VulkanEngine {
	GPUSceneData _sceneParameters;
	AllocatedBuffer _sceneParameterBuffer;
}

```

We will be adding some default parameters to the scene data.
We will stick fog, ambient color, and a sunlight here. We wont be using all of the data, its there as example data that you can use however you want.
Note how everything is on glm::vec4s, and with packed data.
GPUs dont read the data in the exact same way as CPUs do. The rules for that are complicated. A simple way to bypass said rules is by sticking to only vec4s and mat4s, and pack things yourself. Be very careful if you decide to mix types into the buffers. 

This struct is 4 (float) * 4 (vec4) * 5 bytes, 80 bytes. 80 bytes does not meet any of the alignements for any gpu. If we want to have them packed in a buffer as if it was a normal array wont work. To have it work we will have to pad the buffer so that things align. 

We are going to need a function that pads the size of something to the alignement boundary, so lets add it.

```cpp
size_t VulkanEngine::pad_uniform_buffer_size(size_t originalSize)
{
	// Calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = _gpuProperties.limits.minUniformBufferOffsetAlignment;
	size_t alignedSize = originalSize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}

```

Thanks to Sascha Willems and his Vulkan samples for the snippet. https://github.com/SaschaWillems/Vulkan/tree/master/examples/dynamicuniformbuffer


On the `init_descriptors` function, we are going to create the buffer for this. Due to the alignement, we will have to increase the size of the buffer so that it fits 2 padded GPUSceneData structs, we will use the above function for that.

```cpp
const size_t sceneParamBufferSize = FRAME_OVERLAP * pad_uniform_buffer_size(sizeof(GPUSceneData));

_sceneParameterBuffer = create_buffer(sceneParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
```

With this, our _sceneParameterBuffer will hold enough space for FRAME_OVERLAP count of scene data structs. Keep in mind that this buffer will be of different size depending on the GPU due to the padding.

We can now modify the descriptor layout for our global set so that we add the link to the scene parameter buffer.

Seeing that we are creating more DescriptorSetLayoutBindings, lets abstract that into vk_initializers first. We will also abstract for VkWriteDescriptorSet

```cpp
VkDescriptorSetLayoutBinding vkinit::descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stageFlags;

	return setbind;
}

VkWriteDescriptorSet vkinit::write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo , uint32_t binding)
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = bufferInfo;

	return write;
}
```



We can now use it in `init_descriptors` when creating the descriptor layout.

```cpp

	//binding for camera data at 0
	VkDescriptorSetLayoutBinding cameraBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0);

	//binding for scene data at 1
	VkDescriptorSetLayoutBinding sceneBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	
	VkDescriptorSetLayoutBinding bindings[] = { cameraBind,sceneBind };

	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.bindingCount = 2;
	setinfo.flags = 0;
	setinfo.pNext = nullptr;
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pBindings = bindings;

```

For the scene binding, we have also changed the shader stages to include Fragment shader, as we will want to read it from the fragment shader.

With the layout done, we now need to modify the descriptor set writes so that the point to correct buffer and the correct offset in it.

We continue on init_descriptors, but inside the frame loop. We replace the older VkWriteDescriptor set part with the new abstracted version.

```cpp

		VkDescriptorBufferInfo cameraInfo;
		cameraInfo.buffer = _frames[i].cameraBuffer._buffer;
		cameraInfo.offset = 0;
		cameraInfo.range = sizeof(GPUCameraData);

		VkDescriptorBufferInfo sceneInfo;
		sceneInfo.buffer = _sceneParameterBuffer._buffer;
		sceneInfo.offset = pad_uniform_buffer_size(sizeof(GPUSceneData)) * i;
		sceneInfo.range = sizeof(GPUSceneData);

		VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _frames[i].globalDescriptor,&cameraInfo,0);
		
		VkWriteDescriptorSet sceneWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _frames[i].globalDescriptor, &sceneInfo, 1);

		VkWriteDescriptorSet setWrites[] = { cameraWrite,sceneWrite };

		vkUpdateDescriptorSets(_device, 2, setWrites, 0, nullptr);
```

The important thing to note here is that in sceneInfo, we are pointing to the  scene parameter buffer, but we are doing so at an offset. And that offset is using the padded size from the struct. By doing that, you will point to a uniform buffer but at an offset. This is invisible to the shader, and will let you suballocate a lot of data into one buffer. You can even mix and match different data into the same buffer, it doesnt have to be all from the same struct.

Writing the descriptor set is same as before, but instead of writing 1 binding, we are writing 2 bindings, as our global descriptor set now points to two buffers instead of one.

Now we need to modify the shader so that it uses something from the scene parameter buffer. We are going to do the simplest thing, which is to add the ambient color to the pixel color.

We are going to copy the shader that we were using until now, colored_triangle.frag, and we will call it default_lit.frag, as we will be adding lighting to it.

default_lit.frag
```glsl
//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;

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
	outFragColor = vec4(inColor + sceneData.ambientColor.xyz,1.0f);
}
```

Note how we have the SceneData uniform on `binding = 1`, so that it matches the cpp side. We do not need to add the binding 0 for the camera data here.

Replace the reference to colored_triangle.frag in `init_pipelines`, switching it for this new shader, and make sure to recompile the shaders.
```cpp
VkShaderModule colorMeshShader;
if (!load_shader_module("../../shaders/default_lit.frag.spv", &colorMeshShader))
{
	std::cout << "Error when building the colored mesh shader" << std::endl;
}
```

If you try to run this now, it should work, but the color of the objects will be unknown, as we arent writing to the buffers yet. Its probably zero initialized, which will mean the ambient light does nothing.

Lets write into the buffer from our core render loop.
On `draw_objects()` , before or after when we map the camera buffer and write to it.

```cpp
	float framed = (_frameNumber / 120.f);

	_sceneParameters.ambientColor = { sin(framed),0,cos(framed),1 };

	char* sceneData;
	vmaMapMemory(_allocator, _sceneParameterBuffer._allocation , &(void*)sceneData);

	int frameIndex = _frameNumber % FRAME_OVERLAP;

	sceneData += pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;

	memcpy(sceneData, &_sceneParameters, sizeof(GPUSceneData));

	vmaUnmapMemory(_allocator, _sceneParameterBuffer._allocation);
```

We need to do evil pointer arithmetic to offset the data pointer and make it point where we want to. Other than that, its more or less the same as the camera buffer.

If you run this now, you will see that the objects have a tint that changes across time.

