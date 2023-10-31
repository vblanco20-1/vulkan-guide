---
layout: default
title: Rendering Loop
parent:  "New 1. Initializing Vulkan"
nav_order: 40
---

We have our render pass, we also have our command buffer, and we have Vulkan itself initialized. It is time to actually begin writing the render loop itself. On here, we are going to use VkCmdClearColorImage to "draw" our frame. This will give us a flashing color. To achieve this, we need to be able to interact with the swapchain so that we get a image from the swapchain, clear it with a color, and then display that image on the screen. For that we are going to setup the syncronization structures that will let us syncronize the OS/GPU operations from the swapchain with our clear command.

# Synchronization
Vulkan offers explicit sync structures to allow the CPU to sync execution of commands with the GPU. And also to control the order of executions in the GPU.
By default, once you send some commands to the GPU through a queue or other operation, those will have no restrictions and the driver/gpu will execute them as it sees fit. If we want to do multiple operations and we want them to execute on a given order, we need to use the syncronization systems. We have Fences and Semaphores for that


## VkFence
This is used for GPU -> CPU communication. A lot of Vulkan operations, such as vkQueueSubmit allow an *optional* fence parameter. If this is set, we can know from the CPU if the GPU has finished these operations. We will use it to sync the main loop in the CPU with the GPU. A fence will be signaled once submitted as part of a command, and then we can use VkWaitForFences to have the CPU stop until those commands have executed. We will have 2 fences, one for each of our FrameData structures.


![cmake]({{site.baseurl}}/diagrams/commandtimeline.png)


Pseudocode example:
```cpp
//we have a fence object created from somewhere
VkFence myFence;

//start some operation on the GPU
VkSomeOperation(whatever, myFence);

// block the CPU until the GPU operation finishes
VkWaitForFences(myFence);
//fences always have to be reset before they can be used again
VkResetFences(myFence);
```


## VkSemaphore
This is used for GPU to GPU sync. Semaphores allow defining order of operations on GPU commands, and for them to run one after another. Some Vulkan operations (like VkQueueSubmit) support to either Signal or Wait semaphores. 

A given semaphore acts as a link between multiple gpu operations. One operation must signal the semaphore, and other operation must wait on it. It is possible to have multiple GPU operations waiting on a given semaphore, but you can only signal a semaphore from one operation. If a operation waits on a semaphore, that means that it will not begin executing until that semaphore is signaled at completion from other operation.

Pseudocode example of linearizing 3 operations:
```cpp
VkSemaphore Task1Semaphore;
VkSemaphore Task2Semaphore;

VkOperationInfo OpAlphaInfo;
// Operation Alpha will signal the semaphore 1
OpAlphaInfo.signalSemaphore = Task1Semaphore;

VkDoSomething(OpAlphaInfo);

VkOperationInfo OpBetaInfo;

// Operation Beta signals semaphore 2, and waits on semaphore 1
OpBetaInfo.signalSemaphore = Task2Semaphore;
OpBetaInfo.waitSemaphore = Task1Semaphore;

VkDoSomething(OpBetaInfo);

VkOperationInfo OpGammaInfo;
//Operation gamma waits on semaphore 2
OpGammaInfo.waitSemaphore = Task2Semaphore;

VkDoSomething(OpGammaInfo);
```

This code will do the 3 DoSomethings in strict order in the GPU. The execution order of the GPU-side commands will be Alpha->Beta->Gamma . Alpha signals Task1 once it executes, which will begin execution on Beta, and then once Beta finishes its execution it will signal Gamma for execution.

If you don't use semaphores in this case, the commands of the 3 operations might execute in parallel, interleaved with each other.
It is also possible to use semaphores for cross-queue operations, for example you might want to do that to execute compute shaders in the background while the main graphics queue is busy on some pass.

# Render Loop
For our render loop, we are going to do double-buffered render structures. This way while the gpu is busy executing a frame worth of commands, the CPU can continue with the next frame. But once the next frame is calculated, we need to stop the CPU until the first frame is executed so that we can make its commands again.

For our render work, we need to syncronize it with the swapchain structure. If we were doing headless drawing where we dont need to sync with the screen, we wouldnt need this. But we are drawing into a window, so we need to request the OS for an image to draw, then draw on it, and then signal the OS that we want to display that image on the screen. 

# Image layouts
GPUs store images in different formats for different needs in their memory. An image layout is the vulkan abstraction over these formats. A image that is read-only is not going to be on the same layot as one that will be written to. To change the layout of an image, vulkan does pipeline barriers. A pipeline barrier is a way to syncronize the commands from a single command buffer, but it also can do things such as transition the image layouts. How the layouts are implemented varies beetween vendors, and some transitions will actually be a no-op on some hardware. To get the transitions right, it is imperative you use the validation layers, which are going to check that images are on their correct layouts for any given GPU operation. If you dont, its very common to have code that works completely fine on NVidia hardware, but causes glitches on AMD or the opposite.

The image we get from the swapchain is going to be in an invalid state, so if we want to use a VkCmdDraw on it, or any other draw operation, we need to first transition the image into a writeable layout. And once we are done with the draw commands, we need to transition it into the layout that the swapchain wants for scren output. 

On older versions of vulkan, these layout transitions would be done as part of a RenderPass. but we are on vulkan 1.3 and we have dynamic rendering, which means we will do those transitions manually, on the other side, we save all of the work and complexity of building a full renderpass. If you want to learn about renderpasses, the older version of the tutorial explains them in [HERE](https://vkguide.dev/docs/chapter-1/vulkan_renderpass/)



Let's begin the code for the renderloop.


