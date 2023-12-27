---
layout: default
title: Setting up renderpass code
parent:  "(OLD) 1. Initialization and Render Loop"
nav_order: 31
---


Let's start by adding the new members we are going to need to the class.

vk_engine.h
```cpp
class VulkanEngine {
public:

	//--- other code ---
	VkRenderPass _renderPass;

	std::vector<VkFramebuffer> _framebuffers;
private:

	//--- other code ---
	void init_default_renderpass();

	void init_framebuffers();
}
```

We add a handle to the RenderPass we are going to create, and an array of framebuffers. This array will be equivalent to the _swapchainImages and _swapchainImageViews.


On the cpp side, we add it to the init function, after the others.
vk_engine.cpp

```cpp
void VulkanEngine::init()
{
	// SDL STUFF ----


	init_vulkan();

	init_swapchain();

	init_commands();

	init_default_renderpass();

	init_framebuffers();
}

```

## Renderpass

We need to create the renderpass BEFORE the framebuffers, because the framebuffers are created for a specific renderpass.

Let's start filling the `init_default_renderpass()` function


```cpp
void VulkanEngine::init_default_renderpass()
{
	// the renderpass will use this color attachment.
	VkAttachmentDescription color_attachment = {};
	//the attachment will have the format needed by the swapchain
	color_attachment.format = _swapchainImageFormat;
	//1 sample, we won't be doing MSAA
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// we Clear when this attachment is loaded
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// we keep the attachment stored when the renderpass ends
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//we don't care about stencil
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//we don't know or care about the starting layout of the attachment
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//after the renderpass ends, the image has to be on a layout ready for display
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}
```

First thing we start writing is the color attachment. This is the description of the image we will be writing into with rendering commands.

The image will use the format we got from the swapchain (so it's compatible), and we will clear it when the renderpass begins. When the renderpass ends, we will store the image so that it can be read later.

Before the renderpass, the image layout will be undefined, which means "we don't care". And after the renderpass ends, the layout will be ready to be displayed with the swapchain.

Now that our main image target is defined, we need to add a subpass that will render into it. This goes right after defining the attachment

```cpp
	VkAttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
```

We are going to describe 1 subpass, which will use the color attachment above. The color attachment above will also be used on the layout `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` during this subpass. Subpass `vkAttachmentReference` will point into the attachment array when creating the renderpass.

The image life will go something like this:

UNDEFINED -> RenderPass Begins -> Subpass 0 begins (Transition to Attachment Optimal)  -> Subpass 0 renders -> Subpass 0 ends -> Renderpass Ends (Transitions to Present Source)

The Vulkan driver will perform the layout transitions for us when using the renderpass. If we weren't using a renderpass (drawing from compute shaders) we would need to do the same transitions explicitly.

Now that the main attachment and the subpass is done, we can create the renderpass


```cpp
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	//connect the color attachment to the info
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	//connect the subpass to the info
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;


	VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));
```
We will have only 1 attachment, which will be the color_attachment defined above, for our main color target. Then we also connect the subpass into it.

That's it, we have now created our very basic renderpass. We will go back to this code later when we add depth testing, which needs more attachments defined.

## Framebuffers
Once the renderpass is made, we can use it to create the framebuffers. Framebuffers are created from a given renderpass, and they act as link between the attachments of the renderpass and the real images that they should render to.

```cpp
void VulkanEngine::init_framebuffers()
{
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = nullptr;

	fb_info.renderPass = _renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = _windowExtent.width;
	fb_info.height = _windowExtent.height;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {

		fb_info.pAttachments = &_swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));
	}
}
```

We fill the VkFrameBufferCreateInfo with the parameters that are general, and then we create one framebuffer for each of the images of the swapchain.

When rendering, the swapchain will give us the index of the image to render into, so we will use the framebuffer of the same index.

With the render pass setup, we can go forward to the render loop itself.


## Cleanup

As usual, we add the newly created objects to the `cleanup()` function. Because the framebuffers are created from images, they need to be deleted alongside them. We also need to delete the render pass.

in `VulkanEngine::Cleanup()`
```cpp
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		//destroy the main renderpass
		vkDestroyRenderPass(_device, _renderPass, nullptr);

		//destroy swapchain resources
		for (int i = 0; i < _framebuffers.size(); i++) {
			vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);

			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
		}
```

Next: [Main Loop]({{ site.baseurl }}{% link docs/chapter-1/vulkan_mainloop.md %})


{% include comments.html term="1. Comments" %}

