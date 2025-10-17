---
layout: default
title: Compute Shaders
parent: GPU Driven Rendering
nav_order: 6
---

During this GPU driven chapter compute shaders have been mentioned many times, but it was never shown how do they work, and in what way are they used in the engine.

## A primer on GPU Hardware
The GPU is a computation machine, much in the same way as a CPU is. A GPU executes arbitrary code to do things like running the shaders or rendering triangles.
While GPUs at first used to be just triangle-render machines, over time, they became more and more like CPUs due to the need of executing pixel/vertex shaders. Eventually, graphic APIs added compute shaders, which are a special type of shader that doesn't use the fixed graphics pipeline, and just allows to run arbitrary computations on the GPU.

Modern GPUs are parallel compute machines first, with a few fixed hardware that accelerates graphics such as texture accessors and triangle rasterizers. A GPU is composed of a set of "Compute units", which are roughly equivalent to a CPU core. But because a GPU is about parallelism first, their cores are different in some major ways.

First of all, the cores execute instructions in a very wide SIMD (Single Instruction, Multiple Data). While a CPU executes instructions on items one-at-a-time by default unless you use special instructions, a GPU executes everything in a wide manner, often 32 or 64 items at a time. Those are often called threads/lanes/cuda-cores. Also, each of the cores in the GPU overlaps the execution of multiple of those 32/64 wide instruction streams at a time, so that if one stream is waiting for a memory access, it can execute a different one. This works a lot like Hyperthreading does on CPUs, but it's a much more extreme version of the same concept.

If you look at a GPU like a RTX 2080ti, it has 68 "Streaming Multiprocessors", which is the "core" thing above. Each of those has 64 "cuda cores", each of them mapping into a SIMD lane, with each "core" having 2 32-wide executors.

This is a total of 4352 possible execution lanes at a time. You then want at least a few of them overlapping so that memory accesses can be masked by the hyperthreading, so at the end, to put a 2080ti at full power, you are going to want at least 20.000+ "threads" at a time, better if it's on the millions.

A very important detail is that because executions run 32 or 64 at a time, they cant do branching on a per-element basis. If you have a branch that is taken 50% of the time, the GPU will have to execute those 2 branches one after another.

## The GPU Compute model
To tame all of this power, GPUs run a different programming model than CPUs do. In a CPU you program scalar-first, workling on elements one at a time, while on the GPU want to do the same operations on thousands of elements at a time to let the wide GPU do its thing.
It's for that reason the vertex shaders and pixel shaders run the way they do, one pixel or vertex at a time. On the driver, your pixel shader will be called in parallel with many threads at a time.

For compute shaders, you access this a bit more directly. In compute shaders, there is a split between individual elements, and "work groups", which are groups of individual elements. Elements within the same workgroup can do some features such as access workgroup-local memory in a fast way, which is useful for many operations.

To define a workgroup size, you have to set that up in the shader itself.

```glsl
layout (local_size_x = 256) in;
```

By doing this, we are letting vulkan know that we want our compute shader to be executed in groups of 256 elements at a time. This maps very nicely to what was said above that the GPU executes instructions 32 or 64 at a time, and that it overlaps multiple execution streams in the same core. A workgroup can map to one of those cores, but that's for the driver to decide. Most of the time people choose a workgroup size that is a multiple of 64, and not very big, as a bigger workgroup size has to reserve more memory, and could be split within multiple cores.
The workgroup can be multidimensional. It's very common to have things like 16x16 workgroup size when doing postprocess filters or other similar rendering shaders.

Within the compute shader itself, you can find what "element" you are by using

```glsl
gl_GlobalInvocationID.x;
```
This will be unique per shader invocation and is what you will use for almost everything. You can also access `gl_LocalInvocationID`, which gives you what index you have within the workgroup, and `gl_WorkGroupID` to know what workgroup is being executed.


Once you have your ID, it's up to you how you use it. An example could be a matrix-multiply shader that multiplies all of the matrices in a buffer by a camera matrix and stores them in another buffer.

```glsl
layout (local_size_x = 256) in;

layout(set = 0, binding = 0) uniform Config{
    mat4 transform;
    int matrixCount;
} opData;

layout(set = 0, binding = 1) readonly buffer  InputBuffer{
    mat4 matrices[];
} sourceData;

layout(set = 0, binding = 2) buffer  OutputBuffer{
    mat4 matrices[];
} outputData;


void main()
{
    //grab global ID
	uint gID = gl_GlobalInvocationID.x;
    //make sure we don't access past the buffer size
    if(gID < matrixCount)
    {
        // do math
        outputData.matrices[gID] = sourceData.matrices[gID] * opData.transform;
    }
}

```

To execute this shader, you would do this.

```cpp

int groupcount = ((num_matrices) / 256) + 1;

vkCmdDispatch(cmd,groupcount , 1, 1);
```

When you do a `vkCmdDispatch` call, you aren't dispatching individual elements, you are dispatching workgroups. If our workgroup size is 256, we need to divide the number of elements by 256 and round up.

## Compute shaders and barriers

All of the GPU commands sent to a VkQueue will start in order, but will not end in order. The GPU is free to schedule and reorganize the commands in whatever way it sees fit. This means that if we run a shader like above, and then try to do some rendering that uses said matrix buffer, it's possible that the compute shader hasnt finished executing before the buffer is used, thus doing a data race and likely causing bad things to happen.

To syncronize this, vulkan has the concept of barriers. We have been using barriers before for data transfers and image transitions, but we will need to use them in here too.

In a case like above, where we run a compute shader to prepare some data to then be used in rendering, we will need to barrier it like this.

```cpp

VkBufferMemoryBarrier barrier = vkinit::buffer_barrier(matrixBuffer, _graphicsQueueFamily);
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);

```

In this barrier, we have "src access mask" as Shader Write, as we are writing it in the original compute shader, and then the "dst access mask" is Shader Read, as we will be reading it from a vertex shader.

On the barrier itself, we are barriering from Compute Shader Stage to Vertex Shader Stage, as we finish writing the buffer in the shader stage, and then we use it in the vertex shader.

When doing barriers, they might stop the execution of commands until its finished, and the GPU has a ramp-up and ramp-down time while it fills all the execution units. For that reason you really want to group barriers together for maximum performance.

In the tutorial engine, there are 3 cull compute shaders, and the barriers are only executed after the 3 dispatch calls of the cull shaders. This means that the cull shaders will likely overlap on top of each other in the GPU, and then the gpu will wait for all the compute shaders to finish before continuing with the rendering that uses those shaders. If the barriers were done per each dispatch, the gpu could not reach high utilization due to all the ramp up and ramp down time.





{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}
