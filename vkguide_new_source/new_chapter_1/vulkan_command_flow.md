---
layout: default
title: Executing Vulkan Commands
parent:  "1. Initializing Vulkan"
nav_order: 20
---


![cmake]({{site.baseurl}}/diagrams/vkcommands.png)

## Vulkan Command Execution
Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer and be executed through a Queue.

The general flow to execute commands is:
- You allocate a `VkCommandBuffer` from a `VkCommandPool`
- You record commands into the command buffer, using `VkCmdXXXXX` functions.
- You submit the command buffer into a `VkQueue`, which starts executing the commands.

It is possible to submit the same command buffer multiple times. In tutorials and samples it's very common to record the commands once, and then submit them every frame in the render loop.
In this tutorial, we are going to record the commands every frame, as it's more relevant to how a real render engine works.

Recording commands in Vulkan is relatively cheap. Often, the costly operation is the VkQueueSubmit call, where the driver validates the command buffer and executes it on the GPU.

One very important part with command buffers is that they can be recorded in parallel. You can record different command buffers from different threads safely. To do that, you need to have 1 `VkCommandPool` and 1 `VkCommandBuffer` per thread (minimum), and make sure that each thread only uses their own command buffers & pools. Once that is done, it's possible to submit the command buffers in one of the threads. `vkQueueSubmit` is not thread-safe, only one thread can push the commands on a given queue at a time. It's common in big engines to have the submit being done from a background thread, and that way the main render-loop thread can continue executing.

## VkQueue
Queues in Vulkan are an "execution port" for GPUs. Every GPU has multiple queues available, and you can even use them at the same time to execute different command streams. Commands submitted to separate queues may execute at once. This is very useful if you are doing background work that doesn't exactly map to the main frame loop. You can create a `VkQueue` specifically for said background work and have it separated from the normal rendering.

All queues in Vulkan come from a Queue Family. A Queue Family is the "type" of queue it is, and what type of commands it supports. 

Different GPUs support different Queue Families. An example is this NVidia GT 750ti from Vulkan Hardware Info [Link](https://vulkan.gpuinfo.org/displayreport.php?id=8859#queuefamilies). It has 2 queue families, one that supports up to 16 queues that have all features, and then a family that can support 1 queue for transfer only. [Here](https://vulkan.gpuinfo.org/displayreport.php?id=24407#queuefamilies) you have an example for a high end AMD card, there are 5 queue families, and only up to 2 queues per type. It has 1 queue that supports everything, up to 2 queues that support compute and transfer, 2 dedicated transfer queues, and then 2 other queues for present alone. As you can see, the queues supported by each GPU can vary significantly.

It is common to see engines using 3 queue families. One for drawing the frame, another for async compute, and another for data transfer. In this tutorial, we use a single queue that will run all our commands for simplicity.

## VkCommandPool
A `VkCommandPool` is created from the `VkDevice`, and you need the index of the queue family this command pool will create commands from.

Think of the `VkCommandPool` as the allocator for a `VkCommandBuffer`. You can allocate as many `VkCommandBuffer` as you want from a given pool, but you can only record commands from one thread at a time. If you want multithreaded command recording, you need more `VkCommandPool` objects. For that reason, we will pair a command buffer with its command allocator. 

## VkCommandBuffer

All commands for the GPU get recorded in a VkCommandBuffer. All of the functions that will execute GPU work won't do anything until the command buffer is submitted to the GPU through a `VkQueueSubmit` call. 

Command buffers start in the Ready state. When in the Ready state, you can call `vkBeginCommandBuffer()` to put it into the Recording state. Now you can start inputting commands into it with `vkCmdXXXXX` functions.
Once you are done, call `vkEndCommandBuffer()` to finish recording the commands and put it in the Executable state where it is ready to be submitted to the GPU.

To submit the command buffer, you call `vkQueueSubmit()`, using both the command and the queue to submit into. `vkQueueSubmit` also accepts submitting multiple command buffers together. Any command buffer that is submitted is put in the Pending state.

Once a command buffer has been submitted, it's still "alive", and being consumed by the GPU, at this point, it is NOT safe to reset the command buffer yet. You need to make sure that the GPU has finished executing all of the commands from that command buffer before you can reset and reuse it 

To reset a command buffer, you use `vkResetCommandBuffer()`.

As we will want to continue drawing the next frame while the command buffer is executed, we are going to double-buffer the commands. This way, while the GPU is busy rendering and processing one buffer, we can write into a different one.

For more detailed information on the command buffer lifecycle, refer to the Vulkan specification chapter on them:
<https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap6.html#commandbuffers-lifecycle>.


^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}

