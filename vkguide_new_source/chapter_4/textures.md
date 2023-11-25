---
layout: default
title: Textures
parent: "New 4. Textures and Engine Architecture"
nav_order: 2
---

We already showed how to use images when we did compute based rendering, but there are things about images we still need to deal with, specially how to use them in graphics shaders for rendering and display. We will begin here by creating a set of default textures for our engine, and then load a texture from a file.

First, we need to add functions to the VulkanEngine class to deal with creating images.

^code create_image chapter-4/vk_engine.cpp

This is the same we already did when creating a draw image, just copied into its own function. We begin by storing the size and format as part of the AllocatedImage, then we make a VkImageCreateInfo with the size, format, and usages, then we allocate the image with VMA, and finally create the image-view. Something we werent doing before is setting up the aspect flag. We will default it to`VK_IMAGE_ASPECT_COLOR_BIT` unless the image is on a D32 float depth format. 

To write image data, it works very similar to what we did last chapter with the buffers. We need to create a temporal staging buffer, copy our pixels to it, and then do an immediate submit where we call a VkCmdCopyBufferToImage. Lets write that function too. we will do it as a overloaded version of the same create_image function, but taking a void* data parameter for pixels. We will be hardcoding our textures to just be RGBA 8 bit format here, as thats the format most image files are at.

^code upload_image chapter-4/vk_engine.cpp

We start by allocating a staging buffer with enough space for the pixel data, on the CPU_TO_GPU memory type. We then memcpy the pixel data into it.

After that, we call the normal create_image function, but we add the `VK_IMAGE_USAGE_TRANSFER_DST_BIT` so that its allowed to copy data into it.

Once we have the image and the staging buffer, we run an immediate submit that will copy the staging buffer pixel data into the image. 

Similar to how we do it with the swapchain images, we first transition the image into `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`. Then we create a copyRegion structure, where we have the parameters for the copy command. This will require the image size and the target image layers and mip levels. Image layers is for textures that have multiple layers, one of the most common examples is a cubemap texture, which will have 6 layers, one per each cubemap face. We will do that later when we setup reflection cubemaps.

For mip level, we will copy the data into mip level 0 which is the top level. The image doesnt have any more mip levels.


With those 2 functions, we can set up some default textures. We will create a default-white, default-black, default-grey, and a checkerboard texture. This way we have some textures we can use when something fails to load.

