---
layout: default
title: Descriptor Sets
parent: Chapter 4
nav_order: 11
---

Until now, we have been using push constants to upload data from CPU to GPU. While push constants are useful, there are a lot of limitations to them. For example you cant upload an array, you cant point to a buffer, and you cant use textures with them.
To do that, you need to use descriptor sets, which is the main way of connecting CPU data to the GPU.

Descriptor sets can be very convoluted to use compared to all other similar alternatives used in other graphic APIs. For that reason we are going to start very simple with them, and use them only for buffers, and do more things with them as the chapter continues. The texture side of descriptor sets will be for chapter 5.

<div class="mxgraph" style="max-width:100%;border:1px solid transparent;" data-mxgraph="{&quot;highlight&quot;:&quot;#0000ff&quot;,&quot;nav&quot;:true,&quot;resize&quot;:true,&quot;toolbar&quot;:&quot;zoom layers lightbox&quot;,&quot;edit&quot;:&quot;_blank&quot;,&quot;xml&quot;:&quot;&lt;mxfile host=\&quot;app.diagrams.net\&quot; modified=\&quot;2020-10-02T16:02:40.267Z\&quot; agent=\&quot;5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\&quot; etag=\&quot;G0Tl161wDfz5oobdgIFU\&quot; version=\&quot;13.7.7\&quot; type=\&quot;device\&quot;&gt;&lt;diagram id=\&quot;6hGFLwfOUW9BJ-s0fimq\&quot; name=\&quot;Page-1\&quot;&gt;7Vpdk5owFP01PG4HCCA+6qptZ7Yzne706zFKRLrR0BBX6a9vAkEgQdc6wLRs9UFyE0hyzj03N0ED3G+PbymMNx9IgLBhm8HRADPDti0bjPiPsKS5xfNAbghpFMhGpeEx+oWk0ZTWfRSgpNaQEYJZFNeNK7LboRWr2SCl5FBvtia43msMQ6QZHlcQ69avUcA2udW3R6X9HYrCTdGz5Y3zmiVcPYWU7HeyP8MGzkx88+otLJ4lJ5psYEAOlU7B3AD3lBCWX22P9wgLbAvY8vsWZ2pP46Zox665AXhyHCwt5o4CDoUsEso2JCQ7iOeldZrND4knmLxUtnkgJOZGixt/IMZSySvcM8JNG7bFsnZNduyeYEKzHsHcE19uTxglT6hSMxuLL6/R5yWnmpA9XcmRS3bEBFREuaciskWMprwBRRiy6LnONZQuE57anW79SCLeq21K93ZdSZ50bsc0649gkIaIybtK8CeUwrTSLBYNkgv9mM39LG5rzy/yERSlCialKfOPZl+R8D5DvJfQfXmaoWRFo5gR+oiY5kqlowjWD5uIoccYZnQdeOBQnCLCuEI9tE3TdpucwluY/KO70Tr7XHKWZ0QZOl70DVkLnDqWYCTLhzIcWIWIN5VQ4Cm+UHWnChMXgbZvESWPJLGoXWN0nIgQ2IZQOYA0/Sbuf+MWxe/ycVlhdqyVUlnqSOC+XAoyfXWk+atJ8jU5TDAmK8iQYXuYD2+6pDUSvZ97EdYzcO6SDPIJb2D58TEDoKjnVyE7QaI4wQNc8rW2RhLEUbjj1ysOHeK4ToWbR3wxm8iKbRQEeeBGvFu4zJ4naJIRiD/cnRruTI5NeoPlN9JSOKempdNqLDso1p6zGrsz3wBzbNV0dmfnxVtDdNGErNcJYhqffxbwdIarAe8jT0gGHPFO0a2PiAf8AaUhwO4nD/HGdcY0JlrKQ0ZKXgHcy3mI2r4YV1t5SIFvRZdmHkqn+/UaiaBbqnRACnU9hQfnSoXaLSi06GsYCgX9KNT3blPoLaIAmiisVymKaxP1VkRhaUj+w6JwehKFun12uhOFo4nCzkXxfitOgGwPboVz75aJ+HkVAjmdVvQiEDAkgbj9CGQE6oy5oDuBuJpAwGtcNRzQoyiAfpD2juAg0fDlk2HdbfWFbhK51W9XNNfz4Izc+urt6zy4XdEw1miYfl4s5p+qxzdnTmJe8PUWkNE2dw0eCrqCxtF3WO8/TN7O/wpkfHAmf6gg4zUA08Y5haMnFH+Rz2gLR9MGsQkadYG5CRpPg2aZirNQkW+8CM7wQ93IUgTdsFHpTM/6ijPNuXH/s5MHldHL7DQppw123CElyY48ye06SQbq0YrbWZLsXH4PkL34VCX0AFOy11+IvrAGqCJigss23NtSgo81vjLftVRYbwo/eqalHNgOZmNhmeO6W/a5sSi4On8GOByc1dXU6xNnS8O5fqw0XJivjRutwKzvQsArcWdw7cvXVnDWNzXZsolo8j83NPSteNMhRUu5IS+W//3LU5XyD5Zg/hs=&lt;/diagram&gt;&lt;/mxfile&gt;&quot;}"></div>
<script type="text/javascript" src="https://viewer.diagrams.net/js/viewer-static.min.js"></script>

## Mental model
Think of a single descriptor as a handle or pointer into a resource. That resource being a Buffer or a Image, and also holds other information, such as the size of the buffer, or the type of sampler if its for an image.
A `VkDescriptorSet` is a pack of those pointers that are bound together. Vulkan does not allow you to bind individual resources in shaders. They have to be grouped in the sets. If you still insist on being able to bind them individually, then you will need a descriptor set for each resource. This is very inefficient and wont work in many hardware.
If you look at this https://vulkan.gpuinfo.org/displaydevicelimit.php?name=maxBoundDescriptorSets&platform=windows , you will see that some devices will only allow up to 4 descriptor sets to be bound to a given pipeline, on PC. Due to this, we can really only use up to 4 descriptor sets in our pipelines if we want the engine to run on Intel integrated GPUs.
A common and performant way of dealing with that limitation of 4 descriptors, is to group them by binding frequency.

The descriptor set number 0 will be used for engine-global resources, and bound once per frame.
The descriptor set number 1 will be used for per-pass resources, and bound once per pass.
The descriptor set number 2 will be used for material resources, and the number 3 will be used for per-object resources.
This way, the inner render loops will only be binding descriptor sets 2 and 3, and performance will be high.


## Descriptor allocation
Descriptor sets have to be allocated directly by the engine from a `VkDescriptorPool`. A descriptor set allocation will typically be allocated in a section of GPU VRAM. Once a descriptor set is allocated, you need to write it to make it point into your buffers/texures. Once you bind a descriptor set and use it in a `vkCmdDraw()` function, you can no longer modify it unless you specify the `VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT` flag. When a descriptor pool is allocated, you have to tell the driver how many descriptors sets, and what number of resources you will be using. 
A common thing to do is to default to some high numbers, like 1000 descriptors, and when the descriptor pool is out of space, allocating a new descriptor will return with an error. Then you can just go and create a new pool to hold more descriptors.

Allocating descriptor sets can be very cheap if you explicitly disallow freeing individual sets by setting the `VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT` flag. By using that flag, the driver might make the descriptor pool just use a very cheap linear allocator. If you are allocating descriptor sets per frame, you should be using that, and then you reset the entire pool instead of individual descriptor sets.
For your global descriptor sets, its fine to allocate them once, and reuse them from frame to frame. This is what we will be doing in the tutorial, as it also ends with simpler code.

A common technique used in production engines is to have a set of descriptor pools per frame. Once a descriptor allocation fails, you create a new pool and add it to a list. When the frame is submitted and you have waited on its fence, you reset all of those descriptor pools.

## Writing descriptors.
A freshly allocated descriptor set is just a bit of GPU memory, you need to make it point to your buffers. For that you use `vkUpdateDescriptorSets()`, which takes an array of `VkWriteDescriptorSet` for each of the resources that a descriptor set points to. If you were using the Update After Bind flag, it is possible to use descriptor sets, and bind them in command buffers, and update it right before submitting the command buffer. This is mostly a niche use case, and not commonly used.
You can only update a descriptor set before its bound for the first time, unless you use that flag, in which case you can only update it before you submit the command buffer into a queue.
When a descriptor set is being used, its immutable, and trying to update it will cause errors. The validation layers catch that. To be able to update the descriptor sets again, you need to wait until the command has finished executing.

## Binding descriptors
Descriptor sets bind into specific "slots" on a Vulkan pipeline. When creating a pipeline, you have to specify the layouts for each of the descriptor sets that can be bound to the pipeline. This is commonly done automatically, generated from reflection on the shader. We will be doing it manually to show how its done. 
Once you bind a pipeline in a command buffer, the pipeline has slots for the different descriptor sets, and then you can bind a set into each of the slots. If the descriptor set doesn't match the slot, there will be errors.
If you bind a descriptor set to slot 0, and then you switch pipelines by binding another one, the descriptor set will stay bound, IF the slot is the same on the new pipeline. If the slot isn't exactly the same, then the slot will be "unbound", and you need to bind it again.
For example, lets say we have 2 pipelines, one of which has a descriptor set 0 that binds to a buffer, and descriptor set 1 that binds to 4 images. Then the other pipeline has descriptor set 0 that binds to a buffer (same the same slot in the other pipeline), but in descriptor set 1 it has a descriptor set that binds to 3 images, not 4. If you bind the second pipeline, the descriptor set 0 will stay bound, but the descriptor 1 will be unbound because it no longer matches. This is why we assign a frequency to the descriptor slots, to minimize binding.

## Descriptor Set Layout.
Used in both the pipelines and when allocating descriptors, a `VkDescriptorSetLayout` is the shape of the descriptor. For example, a possible layout will be one where it binds 2 buffers and 1 image. When creating pipelines or allocating the descriptor sets themselves, you have to use the layout. In the tutorial, we will be reusing the layout object for everything, but its not necessary needed. Descriptor set layouts can be compatible if they are the same even if they are created in two different places.

## Uniform buffers
Descriptor sets point into buffers, but we didn't explain that. Right now we are creating GPU buffers that hold vertex data, but you can also create buffers that hold arbitrary data for you to use in the shaders. For that type of data, Uniform Buffers are the common thing. They are small size (up to a few kilobytes), but are very fast to read, so they are perfect for shader parameters. By creating a Uniform Buffer and writing to it from the CPU, you can send data to the GPU in a much more efficient way than push constants. We will be using it for the camera information.
It is possible to have multiple descriptor sets pointing to one uniform buffer, and its also possible to have a big uniform buffer, and then each descriptor sets point to a section of the buffer. The shader will not know the difference.



Next: [Setting up descriptor sets]({{ site.baseurl }}{% link docs/chapter-4/descriptors_code.md %})