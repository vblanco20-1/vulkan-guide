---
layout: default
title: Setting up vulkan commands
parent: Chapter 1
nav_order: 11
---

We will first start by adding the handles we are going to need to the VulkanEngine class, and the `init_commands()` function


```cpp

class VulkanEngine {
public:

// ---- other code -----
VkQueue _graphicsQueue; //queue we will submit to
uint32_t _graphicsQueueFamily; //family of that queue

VkCommandPool _commandPool; //the command pool for our commands
VkCommandBuffer _mainCommandBuffer; //the buffer we will record into

private:

//----- other code----
void init_commands();

};

```

Make sure to call the `init_commands()` function from our main `init()` function
```cpp
void VulkanEngine::init()
{
	// --- other code (SDL Stuff)---
	init_vulkan();

	init_swapchain();

	init_commands();
}
```
## Grabbing the Queue

Luckily, the VkBootstrap library does allow us to get the Queue and Family directly.

Go to the end of the `init_vulkan()` function, where we initalized the core vulkan structures. 

At the end of it, add this code


```cpp
void VulkanEngine::init_vulkan(){

// ---- other code ----

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	
	// use vkbootstrap to get a Graphics queue
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}
```

The above part should already be there from the init-code section. We just add the 2 new lines to request a Queue from vkbDevice, and let the library take care of that for us.

We are requesting a graphics queue, which supports all we need for the guide.

## Creating the VkCommandPool

For the pool, we start adding code into `init_commands()` unlike before, from now on the VkBootstrap library will not do anything for us, and we will start calling the vulkan commands directly.

```cpp
void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.	
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;

	//the command pool will be one that can submit graphics commands
	commandPoolInfo.queueFamilyIndex = _graphicsQueueFamily;
	//we also want the pool to allow for resetting of individual command buffers
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));
}
```

Most vulkan Info structures, used for all of the VkCreateX functions, and a lot of the other vulkan structures, need sType and pNext set. This is used for extensions, as some extensions will still call the VkCreateX function, but with structs of a different type than the normal one. The sType helps the implementation know what struct is being used in the function.

With vulkan structures, it is very important that we do this
```cpp
VkCommandPoolCreateInfo commandPoolInfo = {};
```

By doing that ` = {}` thing, we are letting the compiler initialize the entire struct to zero. This is critical, as in general vulkan structs will have their defaults set in a way that 0 is relatively safe. By doing that, we make sure we dont leave unitialized data in the struct.

We set queueFamilyIndex to the _graphicsQueueFamily that we grabbed before. This means that the command pool will create commands that are compatible with any queue of that "graphics" family.

We are also setting somethin in the .flags parameter. A lot of vulkan structures will have that .flags parameter, for extra options. We are sending VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT , which tells vulkan that we expect to be able to reset individual command buffers made from that pool.

At the end, we finally call VkCreateCommandPool, giving it our VkDevice, the commandPoolInfo for create parameters, and a pointer to the _commandPool member, which will get overwritten if it succeeds.

To check if the command succeeds, we use the VK_CHECK() macro. It will just inmediately abort if something happens.

## Creating a VkCommandBuffer

Now that we have the VkCommandPool created, and stored in the _commandPool member, we can allocate our command buffer from it.

```cpp

void VulkanEngine::init_commands()
{
	
	// --- other code ----

	//allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = {};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.pNext = nullptr;

	//commands will be made from our _commandPool
	cmdAllocInfo.commandPool = _commandPool;
	//we will allocate 1 command buffer
	cmdAllocInfo.commandBufferCount = 1;
	// command level is Primary
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
}
```

As with the command pool, we need to fill the sType and pNext parameters, and then continue the rest of the Info struct.

We let vulkan know that the parent of our command will be the _commandPool we just created, and we want to create only one command buffer.

The .commandBufferCount parameter allows you to allocate multiple buffers at once. Make sure that the pointer you send to VkAllocateCommandBuffer has space for those. 

The .level is set to Primary . Command buffers can be Primary of Secondary level.
Primary level are the ones that are sent into a VkQueue, and do all of the work. This is what we will use in the guide.
Secondary level are ones that can act as "subcommands" to a primary buffer. They are most commonly used in advanced multithreading scenarios. We arent going to use them.

## The VkInit module

If you remember the article that explored the project files, we commented that the vk_initializers module will contain abstraction over the initialization of vulkan structures. Lets go abstract the 2 Info structures into there, for easier readability.


vk_initializers.h
```cpp
namespace vkinit {

	VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolResetFlags flags = 0);

	VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

}
```

2 new functions, `command_pool_create_info()`, and `command_buffer_allocate_info()`. We also use default arguments `flags = 0` to not have to input all arguments for basic stuff
Now, lets copy the code into the implementation of those 2 functions


vk_initializers.cpp
```cpp

VkCommandPoolCreateInfo vkinit::command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolResetFlags flags /*= 0*/)
{
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;

	info.queueFamilyIndex = queueFamilyIndex;
	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo vkinit::command_buffer_allocate_info(VkCommandPool pool, uint32_t count /*= 1*/, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
{
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}
```

We have now abstracted the calls, so lets go change VulkanEngine::init_commands() to use this.

```cpp
void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	//allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
}
```

Much better and shorter. Over the guide, the vk_initializers module will keep growing with more and more structs. You will be able to reuse that module in other projects safely given how simple it is.

## Cleanup

Same as before, what we have created, we have to delete

```cpp
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		vkDestroyCommandPool(_device, _commandPool, nullptr);

		// --- rest of code
	}
}
```

As the command pool is the most recent vulkan object, we need to destroy it before the other objects.
Its not possible to individually destroy VkCommandBuffer, as destroying their parent pool will destroy all of the command buffers allocated from it.

VkQueue-s also cant be destroyed, as, like with the VkPhysicalDevice, they arent really created objects, more like a handle to something that already exists.

Now that we have the Queue and the CommandBuffer, we are ready to start executing commands, but their usability will be limited, as we still lack the structures needed to execute graphics commands.

At this point, it is possible to execute pure-compute commands, if you wanted to perform offline computation.


Next: [Vulkan render passes]({{ site.baseurl }}{% link docs/chapter-1/vulkan_renderpass.md %})