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

^code create_image chapter-4/vk_engine.cpp

This is the same we already did when creating a draw image, just copied into its own function. We begin by storing the size and format as part of the AllocatedImage, then we make a VkImageCreateInfo with the size, format, and usages, then we allocate the image with VMA, and finally create the image-view. Something we werent doing before is setting up the aspect flag. We will default it to`VK_IMAGE_ASPECT_COLOR_BIT` unless the image is on a D32 float depth format. 

To write image data, it works very similar to what we did last chapter with the buffers. We need to create a temporal staging buffer, copy our pixels to it, and then do an immediate submit where we call a VkCmdCopyBufferToImage. Lets write that function too. we will do it as a overloaded version of the same create_image function, but taking a void* data parameter for pixels. We will be hardcoding our textures to just be RGBA 8 bit format here, as thats the format most image files are at.

^code upload_image chapter-4/vk_engine.cpp

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

^code default_img chapter-4/vk_engine.cpp

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
^code mesh_shader chapter-4/vk_engine.cpp
}
```

Now, on our draw function, we can dynamically create the descriptor set needed when binding this pipeline, and use it to display textures we want to draw.

This goes into the `draw_geometry()` function, changing the draw for the monkey mesh

^code draw_tex chapter-4/vk_engine.cpp

We allocate a new descriptor set from the frame descriptor set allocator, using the _singleImageDescriptorLayout that the shader uses.

Then we use a descriptor writer to write a single image descriptor on binding 0, which will be the _errorCheckerboardImage. We give it the nearest-sampler, so that it doesnt blend between pixels. Then we update the descriptor set with the writer, and bind the set. Then we proceed with the draw.

The result should be that the monkey head now has a magenta pattern on it.

![chapter2]({{site.baseurl}}/diagrams/texmonkey.png)

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}