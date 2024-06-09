---
layout: default
title: Textures
parent: "4. Textures and Engine Architecture"
nav_order: 6
---

We already showed how to use images when we did compute based rendering, but there are things about images we still need to deal with, specially how to use them in graphics shaders for rendering and display. We will begin here by creating a set of default textures for our engine, and then load a texture from a file.

First, we need to add functions to the VulkanEngine class to deal with creating and destroying images.

Add these functions to the class in the header.

```cpp
class VulkanEngine {

AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
void destroy_image(const AllocatedImage& img);
}
```

Now we start writing the functions on vk_engine.cpp

<!-- codegen from tag create_image on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
AllocatedImage VulkanEngine::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
{
	AllocatedImage newImage;
	newImage.imageFormat = format;
	newImage.imageExtent = size;

	VkImageCreateInfo img_info = vkinit::image_create_info(format, usage, size);
	if (mipmapped) {
		img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
	}

	// always allocate images on dedicated GPU memory
	VmaAllocationCreateInfo allocinfo = {};
	allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// allocate and create the image
	VK_CHECK(vmaCreateImage(_allocator, &img_info, &allocinfo, &newImage.image, &newImage.allocation, nullptr));

	// if the format is a depth format, we will need to have it use the correct
	// aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT) {
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	// build a image-view for the image
	VkImageViewCreateInfo view_info = vkinit::imageview_create_info(format, newImage.image, aspectFlag);
	view_info.subresourceRange.levelCount = img_info.mipLevels;

	VK_CHECK(vkCreateImageView(_device, &view_info, nullptr, &newImage.imageView));

	return newImage;
}
```

This is the same we already did when creating a draw image, just copied into its own function. We begin by storing the size and format as part of the AllocatedImage, then we make a VkImageCreateInfo with the size, format, and usages, then we allocate the image with VMA, and finally create the image-view. Something we werent doing before is setting up the aspect flag. We will default it to`VK_IMAGE_ASPECT_COLOR_BIT` unless the image is on a D32 float depth format. 

To write image data, it works very similar to what we did last chapter with the buffers. We need to create a temporal staging buffer, copy our pixels to it, and then do an immediate submit where we call a VkCmdCopyBufferToImage. Lets write that function too. we will do it as a overloaded version of the same create_image function, but taking a void* data parameter for pixels. We will be hardcoding our textures to just be RGBA 8 bit format here, as thats the format most image files are at.

<!-- codegen from tag upload_image on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
AllocatedImage VulkanEngine::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
{
	size_t data_size = size.depth * size.width * size.height * 4;
	AllocatedBuffer uploadbuffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	memcpy(uploadbuffer.info.pMappedData, data, data_size);

	AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

	immediate_submit([&](VkCommandBuffer cmd) {
		vkutil::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = size;

		// copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
			&copyRegion);

		vkutil::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

	destroy_buffer(uploadbuffer);

	return new_image;
}
```

We start by allocating a staging buffer with enough space for the pixel data, on the CPU_TO_GPU memory type. We then memcpy the pixel data into it.

After that, we call the normal create_image function, but we add the `VK_IMAGE_USAGE_TRANSFER_DST_BIT` and `VK_IMAGE_USAGE_TRANSFER_SRC_BIT` so that its allowed to copy data into and from it.

Once we have the image and the staging buffer, we run an immediate submit that will copy the staging buffer pixel data into the image. 

Similar to how we do it with the swapchain images, we first transition the image into `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`. Then we create a copyRegion structure, where we have the parameters for the copy command. This will require the image size and the target image layers and mip levels. Image layers is for textures that have multiple layers, one of the most common examples is a cubemap texture, which will have 6 layers, one per each cubemap face. We will do that later when we setup reflection cubemaps.

For mip level, we will copy the data into mip level 0 which is the top level. The image doesnt have any more mip levels. For now we are just passing the mipmapped boolean into the other create_image, but we arent doing anything else. We will handle that later.

For image destruction, lets fill that destroy_image() function
```cpp
void VulkanEngine::destroy_image(const AllocatedImage& img)
{
    vkDestroyImageView(_device, img.imageView, nullptr);
    vmaDestroyImage(_allocator, img.image, img.allocation);
}
```
We first destroy the image-view, and then destroy the image itself using VMA. This will free the image and its memory correctly.

With those functions, we can set up some default textures. We will create a default-white, default-black, default-grey, and a checkerboard texture. This way we have some textures we can use when something fails to load.

Lets add those test images into the VulkanEngine class, and a couple samplers too that we can use with those images and others.

```cpp
	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;
```

lets go and create those as part of the `init_default_data()` function, after the code we have that creates the rectangle mesh.

<!-- codegen from tag default_img on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
	//3 default textures, white, grey, black. 1 pixel each
	uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
	_whiteImage = create_image((void*)&white, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
	_greyImage = create_image((void*)&grey, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
	_blackImage = create_image((void*)&black, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	//checkerboard image
	uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
	std::array<uint32_t, 16 *16 > pixels; //for 16x16 checkerboard texture
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			pixels[y*16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
		}
	}
	_errorCheckerboardImage = create_image(pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

	sampl.magFilter = VK_FILTER_NEAREST;
	sampl.minFilter = VK_FILTER_NEAREST;

	vkCreateSampler(_device, &sampl, nullptr, &_defaultSamplerNearest);

	sampl.magFilter = VK_FILTER_LINEAR;
	sampl.minFilter = VK_FILTER_LINEAR;
	vkCreateSampler(_device, &sampl, nullptr, &_defaultSamplerLinear);

	_mainDeletionQueue.push_function([&](){
		vkDestroySampler(_device,_defaultSamplerNearest,nullptr);
		vkDestroySampler(_device,_defaultSamplerLinear,nullptr);

		destroy_image(_whiteImage);
		destroy_image(_greyImage);
		destroy_image(_blackImage);
		destroy_image(_errorCheckerboardImage);
	});
```

for the 3 default color images, we create the image with that color as the single pixel. For the checkerboard, we write a 16x16 array of pixel color data with some simple math for a black/magenta check pattern.

On the samplers, we will leave all parameters as default except the min/mag filters, which we will have as either Linear or Nearest. Linear will blur pixels, while Nearest will give a pixelated look.

## Binding images to shaders
When we did the compute based rendering, we bound the image using a `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`, which was the type we use for a read/write texture with no sampling logic. This is roughly equivalent to binding a buffer, just a multi-dimensional one with different memory layout. But when we do drawing, we want to use the fixed hardware in the GPU for accessing texture data, which needs the sampler. We have the option to either use `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`, which packages an image and a sampler to use with that image, or to use 2 descriptors, and separate the two into `VK_DESCRIPTOR_TYPE_SAMPLER` and `VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE`. According to gpu vendors, the separated approach can be faster as there is less duplicated data. But its a bit harder to deal with so we wont be doing it for now. Instead, we will use the combined descriptor to make our shaders simpler. 

We will be modifying the rectangle draw we had before into a draw that displays a image in that rectangle. We need to create a new fragment shader that will show the image. Lets create a new fragment shader for that. We will call it `tex_image.frag`

```c
//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;
//output write
layout (location = 0) out vec4 outFragColor;

//texture to access
layout(set =0, binding = 0) uniform sampler2D displayTexture;

void main() 
{
	outFragColor = texture(displayTexture,inUV);
}
```

We have 2 inputs to the fragment shader, color and UV. The shader doesnt use color but we want to keep using the same vertex shader we had before.

To sample a texture, you do `texture( textureSampler, coordinates )`. There are other functions for things like directly accessing a given pixel. The texture object is declared as `uniform sampler2D`. 

This does change our pipeline layout, so we are going to need to update it too. 

Lets add the layout into VulkanEngine class, as we will keep it around.

```cpp
class VulkanEngine {
VkDescriptorSetLayout _singleImageDescriptorLayout;
}
```

On init_descriptors(), lets create it alongside the rest

```cpp
{
	DescriptorLayoutBuilder builder;
	builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	_singleImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_FRAGMENT_BIT);
}
```

A descriptor set with just a single image-sampler descriptor. We can now update the `init_mesh_pipeline()` function with this. We will be modifying the start part, changing the fragment shader and connecting the descriptor set layout to the pipelinelayout creation.

```cpp
void VulkanEngine::init_mesh_pipeline()
{
	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("../../shaders/tex_image.frag.spv", _device, &triangleFragShader)) {
		fmt::print("Error when building the fragment shader \n");
	}
	else {
		fmt::print("Triangle fragment shader succesfully loaded \n");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle_mesh.vert.spv", _device, &triangleVertexShader)) {
		fmt::print("Error when building the vertex shader \n");
	}
	else {
		fmt::print("Triangle vertex shader succesfully loaded \n");
	}

	VkPushConstantRange bufferRange{};
	bufferRange.offset = 0;
	bufferRange.size = sizeof(GPUDrawPushConstants);
	bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	pipeline_layout_info.pPushConstantRanges = &bufferRange;
	pipeline_layout_info.pushConstantRangeCount = 1;
	pipeline_layout_info.pSetLayouts = &_singleImageDescriptorLayout;
	pipeline_layout_info.setLayoutCount = 1;
	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_meshPipelineLayout));

}
```

Now, on our draw function, we can dynamically create the descriptor set needed when binding this pipeline, and use it to display textures we want to draw.

This goes into the `draw_geometry()` function, changing the draw for the monkey mesh

<!-- codegen from tag draw_tex on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

	//bind a texture
	VkDescriptorSet imageSet = get_current_frame()._frameDescriptors.allocate(_device, _singleImageDescriptorLayout);
	{
		DescriptorWriter writer;
		writer.write_image(0, _errorCheckerboardImage.imageView, _defaultSamplerNearest, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		writer.update_set(_device, imageSet);
	}

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipelineLayout, 0, 1, &imageSet, 0, nullptr);

	glm::mat4 view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_drawExtent.width / (float)_drawExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	projection[1][1] *= -1;

	GPUDrawPushConstants push_constants;
	push_constants.worldMatrix = projection * view;
	push_constants.vertexBuffer = testMeshes[2]->meshBuffers.vertexBufferAddress;

	vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(cmd, testMeshes[2]->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmd, testMeshes[2]->surfaces[0].count, 1, testMeshes[2]->surfaces[0].startIndex, 0, 0);
```

We allocate a new descriptor set from the frame descriptor set allocator, using the _singleImageDescriptorLayout that the shader uses.

Then we use a descriptor writer to write a single image descriptor on binding 0, which will be the _errorCheckerboardImage. We give it the nearest-sampler, so that it doesnt blend between pixels. Then we update the descriptor set with the writer, and bind the set. Then we proceed with the draw.

The result should be that the monkey head now has a magenta pattern on it.

![chapter2]({{site.baseurl}}/diagrams/texmonkey.png)

Next: [ Engine Architecture]({{ site.baseurl }}{% link docs/new_chapter_4/engine_arch.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
