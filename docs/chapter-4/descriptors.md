---
layout: default
title: Descriptor Sets
parent: Chapter 4
nav_order: 11
---

Until now, we have been using push constants to upload data from CPU to GPU. While push constants are useful, there are a lot of limitations to them. For example you cant upload an array, you cant point to a buffer, and you cant use textures with them.
To do that, you need to use descriptor sets, which is the main way of connecting CPU data to the GPU.

Descriptor sets can be very convoluted to use compared to all other similar alternatives used in other graphic APIs. For that reason we are going to start very simple with them, and use them only for buffers, and do more things with them as the chapter continues. The texture side of descriptor sets will be for chapter 5.

## Mental model
Think of a single descriptor as a handle or pointer into a resource. That resource being a Buffer or a Image, and also holds other information, such as the size of the buffer, or the type of sampler if its for an image.
A descripor set is a pack of those pointers that are bound together. Vulkan does not allow you to bind individual resources in shaders. They have to be grouped in the sets. If you still insist on being able to bind them individually, then you will need a descriptor set for each resource. This is very inefficient and wont work in many hardware.
If you look at this https://vulkan.gpuinfo.org/displaydevicelimit.php?name=maxBoundDescriptorSets&platform=windows , you will see that some devices will only allow up to 4 descriptor sets to be bound to a given pipeline, on PC. Due to this, we can really only use up to 4 descriptor sets in our pipelines if we want the engine to run on Intel integrated GPUs.
A common and performant way of dealing with that limitation of 4 descriptors, is to group them by binding frequency.

The descriptor set number 0 will be used for engine-global resources, and bound once per frame.
The descriptor set number 1 will be used for per-pass resources, and bound once per pass.
The descriptor set number 2 will be used for material resources, and the number 3 will be used for per-object resources.
This way, the inner render loops will only be binding descriptor sets 2 and 3, and performance will be high.


## Descriptor allocation
Descriptor sets have to be allocated directly by the engine from a VkDescriptorPool. A descriptor set allocation will typically be allocated in a section of GPU VRAM. Once a descriptor set is allocated, you need to write it to make it point into your buffers/texures. Once you bind a descriptor set and use it in a vkCmdDraw function, you can no longer modify it unless you specify the `VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT` flag. When a descriptor pool is allocated, you have to tell the driver how many descriptors sets, and what number of resources you will be using. 
A common thing to do is to default to some high numbers, like 1000 descriptors, and when the descriptor pool is out of space, allocating a new descriptor will return with an error. Then you can just go and create a new pool to hold more descriptors.

Allocating descriptor sets can be very cheap if you explicitly disallow freeing individual sets by setting the `VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT` flag. By using that flag, the driver might make the descriptor pool just use a very cheap linear allocator. If you are allocating descriptor sets per frame, you should be using that, and then you reset the entire pool instead of individual descriptor sets.
For your global descriptor sets, its fine to allocate them once, and reuse them from frame to frame. This is what we will be doing in the tutorial, as it also ends with simpler code.

## Writing descriptors.
A freshly allocated descriptor set is just a bit of GPU memory, you need to make it point to your buffers. For that you use vkUpdateDescriptorSets, which takes an array of VkWriteDescriptorSet for each of the resources that a descriptor set points to. If you were using the Update After Bind flag, it is possible to use descriptor sets, and bind them in command buffers, and update it right before submitting the command buffer. This is mostly a niche use case, and not commonly used.
You can only update a descriptor set before its bound for the first time, unless you use that flag, in which case you can only update it before you submit the command buffer into a queue.
When a descriptor set is being used, its inmutable, and trying to update it will cause errors. The validation layers catch that. To be able to update the descriptor sets again, you need to wait until the command has finished executing.

## Binding descriptors
Descriptor sets bind into specific "slots" on a vulkan pipeline. When creating a pipeline, you have to specify the layouts for each of the descriptor sets that can be bound to the pipeline. This is commonly done automatically, generated from reflection on the shader. We will be doing it manually to show how its done. 
Once you bind a pipeline in a command buffer, the pipeline has slots for the different descriptor sets, and then you can bind a set into each of the slots. If the descriptor set doesnt match the slot, there will be errors.
If you bind a descriptor set to slot 0, and then you switch pipelines by binding another one, the descriptor set will stay bound, IF the slot is the same on the new pipeline. If the slot isnt exactly the same, then the slot will be "unbound", and you need to bind it again.
For example, lets say we have 2 pipelines, one of which has a descriptor set 0 that binds to a buffer, and descriptor set 1 that binds to 4 images. Then the other pipeline has descriptor set 0 that binds to a buffer (same the same slot in the other pipeline), but in descriptor set 1 it has a descriptor set that binds to 3 images, not 4. If you bind the second pipeline, the descriptor set 0 will stay bound, but the descriptor 1 will be unbound because it no longer matches. This is why we assign a frequency to the descriptor slots, to minimize binding.

## Descriptor Set Layout.
Used in both the pipelines and when allocating descriptors, a descriptor set layout is the "shape" of the descriptor. For example, a possible layout will be one where it binds 2 buffers and 1 image. When creating pipelines or allocating the descriptor sets themselves, you have to use the layout. In the engine, we will be reusing the layout object for everything, but its not necesary needed. 2 Differently created descriptor set layouts can be compatible with each other if they match completely.

## Uniform buffers
Descriptor sets point into buffers, but we didnt explain that. Right now we are creating GPU buffers that hold vertex data, but you can also create buffers that hold arbitrary data for you to use in the shaders. For that type of data, Uniform Buffers are the common thing. They are small size (up to a few kilobytes), but are very fast to read, so they are perfect for shader parameters. By creating a Uniform Buffer and writing to it from the CPU, you can send data to the GPU in a much more efficient way than push constants. We will be using it for the camera information.