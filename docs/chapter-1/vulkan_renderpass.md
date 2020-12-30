---
layout: default
title: Renderpasses
parent:  "1. Initialization and Render Loop"
nav_order: 30
---


# Renderpasses
In Vulkan, all of the rendering happens inside a VkRenderPass. It is not possible to do rendering commands outside of a renderpass, but it is possible to do Compute commands without them.

A VkRenderPass is a Vulkan object that encapsulates the state needed to setup the "target" for rendering, and the state of the images you will be rendering to. 

The renderpass is a concept that only exists in Vulkan. It's there because it allows the driver to know more about the state of the images you render.

A Renderpass will render into a Framebuffer. The framebuffer links to the images you will render to, and it's used when starting a renderpass to set the target images for rendering.

The general use of a renderpass, when encoding commands, is like this

```cpp
vkBeginCommandBuffer(cmd, ...);

vkCmdBeginRenderPass(cmd, ...);

//rendering commands go here

vkCmdEndRenderPass(cmd);

vkEndCommandBuffer(cmd)
```

When beginning a renderpass, you set the target framebuffer, and the clear color (if available). In this first chapter, we will change the clear color dynamically over time.

## Subpasses
A renderpass also contains subpasses, which are a bit like the rendering "steps". They can be very useful in mobile GPUs, as they allow the driver to optimize things quite a lot. 
In desktop GPUs, they are less important, so we aren't going to use them. When creating the renderpass, it will only have 1 subpass which is the minimum required for rendering.

## Image Layouts
A very important thing that the renderpass does, is that it performs image layout changes when entering and exiting the renderpass.

Images in the GPU aren't necessarily in the format you would expect. For optimization purposes, the GPUs perform a lot of transformation and reshuffling of them into internal opaque formats. For example, some GPUs will compress textures whenever they can, and will reorder the way the pixels are arranged so that they mipmap better.
In Vulkan, you don't have control of that, but there is control over the layout for the image, which lets the driver transform the image to those optimized internal formats.

In this first chapter, we are only going to use a few image layouts

- `VK_IMAGE_LAYOUT_UNDEFINED` : Don't care of what the layout is, can be whatever.
- `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` : Image is on a layout optimal to be written into by rendering commands.
- `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` : Image is on a layout that allows displaying the image on the screen.
- `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` : (Used later) Image is on a format optimized to be read from shaders.

Next: [Setting up render passes]({{ site.baseurl }}{% link docs/chapter-1/vulkan_renderpass_code.md %})

{% include comments.html term="1. Comments" %}
