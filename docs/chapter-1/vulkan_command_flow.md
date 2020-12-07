---
layout: default
title: Executing Vulkan Commands
parent:  "1.. Initialization and Render Loop"
nav_order: 20
---


![cmake]({{site.baseurl}}/diagrams/vkcommands.png)



## Vulkan Command Execution
Unlike OpenGL or DirectX pre-11, in Vulkan, all gpu commands have to go through a command buffer.
Command buffers are allocated from a Command Pool, and executed on Queues.

The general flow to execute commands is:
- You allocate a `VkCommandBuffer` from a `VkCommandPool`
- You record commands into the command buffer, using `VkCmdXXXXX` functions.
- You submit the command buffer into a `VkQueue`, which starts executing the commands.

It is possible to submit the same command buffer multiple times. In tutorials and samples its very common to record the commands once, and then submit them every frame in the render loop.
In this tutorial, we are going to record the commands every frame, as its more relevant to how a real render engine works.

Recording commands in Vulkan is relatively cheap. Most of the work goes into the `vkQueueSubmit` call, where the commands are validated in the driver and translated into real gpu commands.

One very important part with command buffers is that they can be recorded in parallel. You can record different command buffers from different threads safely. To do that, you need to have 1 `VkCommandPool` and 1 `VkCommandBuffer` per thread (minimum), and make sure that each thread only uses their own command buffers & pools. Once that is done, its possible to submit the command buffers in one of the threads. `vkQueueSubmit` is not thread-safe, only one thread can push the commands at a time. 

## VkQueue
Queues in Vulkan are an "execution port" for GPUs. Every gpu has multiple queues available, and you can even use them at the same time to execute different command streams. Commands submitted to separate queues may execute at once. This is very useful if you are doing background work that doesn't exactly map to the main frame loop. You can create a `VkQueue` specifically for said background work and have it separated from the normal rendering.

All queues in Vulkan come from a Queue Family. A Queue Family is the "type" of queue it is, and what type of commands it supports. What we want in this guide is a queue that supports both Graphics and Present. This means that the same queue will be used for both rendering and to display images into the screen.

Different GPUs support different Queue Families. An example is this NVidia GT 750ti from Vulkan Hardware Info <https://vulkan.gpuinfo.org/displayreport.php?id=8859#queuefamilies>. It has 2 Queue families, one that supports up to 16!  queues that have all features, and then a family that can support 1 queue for transfer only.

Transfer-only queue families in Vulkan are very interesting, as they are used to perform background asset upload and they often can run completely asynchronously from rendering. So if you want to upload assets to the GPU from a background thread, using a Transfer-only queue will be a great choice.


## VkCommandPool
A `VkCommandPool` is created from the `VkDevice`, and you need the index of the queue family this command pool will create commands from.

Think of the `VkCommandPool` as the background allocator for a `VkCommandBuffer`. You can allocate as many `VkCommandBuffer` as you want from a given pool, but you can only record commands from one thread at a time. If you want multithreaded command recording, you need more `VkCommandPool` objects.

Commands are typically allocated sparingly, and then reset every time. The fast-path for this is to reset the entire command pool, which will reset all of the command buffers allocated from it. It is also possible to reset them directly, but if you are making multiple command buffers from one pool, resetting the pool will be quicker.

## VkCommandBuffer

All commands for gpu get recorded in a VkCommandBuffer. All of the functions that will execute gpu work wont do anything until the command buffer is submitted to the GPU. 

Command buffers start in the Ready state. When in the Ready state, you can call `vkBeginCommandBuffer()` to put it into the Recording state. Now you can start inputting commands into it with `vkCmdXXXXX` functions.
Once you are done, call `vkEndCommandBuffer()` to finish the recording the commands and put it in the Executable state where it is ready to be submitted into the GPU.

To submit the command buffer, you call `vkQueueSubmit()`, using both the command and the queue to submit into. `vkQueueSubmit` also accepts submitting multiple command buffers together. Any command buffer that is submitted is put in the Pending state.

Once a command buffer has been submitted, its still "alive", and being consumed by the GPU, at this point it is NOT safe to reset the command buffer yet. You need to make sure that the GPU has finished executing all of the commands from that command buffer until you can reset it and reuse. 

To reset a command buffer, you use `vkResetCommandBuffer()`.

For more detailed information on the command buffer lifecycle, refer to the Vulkan specification chapter on them
<https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap6.html#commandbuffers-lifecycle>.

Next: [Vulkan commands code]({{ site.baseurl }}{% link docs/chapter-1/vulkan_commands_code.md %})

{% include comments.html term="1. Comments" %}