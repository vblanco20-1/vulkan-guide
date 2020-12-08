---
layout: default
title: Draw Indirect
parent: GPU Driven Rendering
nav_order: 4
---

# Draw Indirect API
In the gpu-driven article, draw indirect was explained in a quick overview of how it works in the codebase, but lets have a better look at how DrawIndirect works.

The draw-indirect call in vulkan looks like this.

```cpp
//indexed draw
VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

//non indexed draw
VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride);
```


A draw-indirect command takes a VkBuffer as the first parameter, and this is where the commands are stored. You can also set DrawCount to whatever number you want, and in that case it will execute multiple draw commands from the buffer, adding stride to the offset every time.
Each command will read 5 integers from `buffer + offset + (stride * index)`, according to this layout. Note that both the indexed and non-indexed calls are exactly the same. They change only in their command struct



```cpp

//indexed 
struct VkDrawIndexedIndirectCommand {
    uint32_t    indexCount;
    uint32_t    instanceCount;
    uint32_t    firstIndex;
    int32_t     vertexOffset;
    uint32_t    firstInstance;
};

//non indexed
typedef struct VkDrawIndirectCommand {
    uint32_t    vertexCount;
    uint32_t    instanceCount;
    uint32_t    firstVertex;
    uint32_t    firstInstance;
} VkDrawIndirectCommand;

```

Its important to know that you dont need to have the data be a packed array of command structs. You can have more things in the buffer, as long as you set the offset and stride correctly. In the engine we store extra data in the buffer.

To create a draw-indirect buffer, it can be on both CPU side and GPU side buffers, and it doesnt really matter that much which one is it if you are doing read-only. In the engine we have the draw-indirect buffer in the gpu because we are writing to it from the culling compute shaders.

Here you can see an example of creating a CPU-writeable indirect buffer
```cpp

create_buffer(MAX_COMMANDS * sizeof(VkDrawIndexedIndirectCommand),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	
```

When creating a indirect draw buffer, you need to add `VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT` to the usage flags. The gpu will error otherwise. You can also have them have the transfer and storage buffer usages, as they are very useful to write and read from shaders.

Executing a draw-indirect call will be equivalent to doing this.

```cpp

void FakeDrawIndirect(VkCommandBuffer commandBuffer,void* buffer,VkDeviceSize offset, uint32_t drawCount,uint32_t stride);

    char* memory = (char*)buffer + offset;

    for(int i = 0; i < drawCount; i++)
    {
        VkDrawIndexedIndirectCommand* command = VkDrawIndexedIndirectCommand*(memory + (i * stride));

        VkCmdDrawIndexed(commandBuffer, 
        command->indexCount, 
        command->instanceCount, 
        command->firstIndex, 
        command->vertexOffset,
        command->firstInstance);
    }
}   
```

There is also a very popular extension that makes draw indirect even more powerful known as DrawIndirectCount. Its default in Vulkan 1.2, and works on pretty much all PC hardware. Sadly its not supported in nintendo switch, so the tutorial will not use it.
Draw Indirect Count is the same as a normal draw indirect call, but "drawCount" is grabbed from another buffer. This makes it possible to let the GPU decide how many draw indirect commands to draw, which makes it possible to remove culled draws easily so that there is no wasted work.


# Using Draw Indirect
There are many ways of using draw-indirect. To show the simplest way of doing it, we will change the code from end of chapter 4 to run with draw indirect. We will be doing 1 draw-indirect command, without instancing, for each render object. This is more or less the same as doing a deep loop of "VkCmdDraw()" calls that we were doing already. 


The Render loop looks like this by the end of chapter 4 (pseudocode)

```cpp
{
    //initial global setup omitted

    //write object matrices
	GPUObjectData* objectSSBO = map_buffer(get_current_frame().objectBuffer);
	
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = objects[i];
		objectSSBO[i].modelMatrix = object.transformMatrix;
	}
	
	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = objects[i];

		//only bind the pipeline if it doesnt match with the already bound one
		if (object.material != lastMaterial) {

			bind_descriptors(object.material);
            lastMaterial = object.material;
		}	

		//only bind the mesh if its a different one from last bind
		if (object.mesh != lastMesh) {
			bind_mesh(object.mesh)
            lastMesh = object.mesh;
		}
		//we can now draw
		vkCmdDraw(cmd, object.mesh->_vertices.size(), 1,0 , i /*using i to access matrix in the shader */   );
	}
}
```

For each of the objects, we render each of them one at a time. If the material or the mesh changes, then we re-bind it.

That re-binding will be our stopping point. While we can do multiple draw commands in 1 call with draw-indirect by setting drawCount to more than 1, we cant rebind mesh or material. So we will have to do one draw call every time that the material and mesh changes.
For that, we are going to do a pre-pass in the array of objects where it will "compact" it into sections where it uses the same mesh and material.


```cpp

struct IndirectBatch{
    Mesh* mesh;
    Material* material;
    uint32_t first;
    uint32_t count;
}
std::vector<IndirectBatch> compact_draws(RenderObject* objects, int count)
{
    std::vector<IndirectBatch> draws;

    IndirectBatch firstDraw;
    firstDraw.mesh = objects[0]->mesh;
    firstDraw.material = objects[0]->material;
    firstDraw.first = 0;
    firstDraw.count = 1;

    draws.push_back(firstDraw);

    for (int i = i; i < count; i++)
    {
        //compare the mesh and material with the end of the vector of draws
        bool sameMesh = objects[i]->mesh == draws.back().mesh;
        bool sameMaterial = objects[i]->material ==draws.back().material;

        if(sameMesh && sameMaterial)
        {
            //all matches, add count
            draws.back().count++;
        }
        else    
        {
            //add new draw
            IndirectBatch newDraw;
            newDraw.mesh = objects[i]->mesh;
            newDraw.material = objects[i]->material;
            newDraw.first = i;
            newDraw.count = 1;

            draws.push_back(newDraw);
        }
    }
    return draws;
}
```

With the draws compacted in this way, we can rewrite the draw loop into this, which maps much better to draw indirect.

```cpp
{

    std::vector<IndirectBatch> draws = compact_draws(objects, count);

	for (IndirectBatch& draw : draws)
	{
		bind_descriptors(draw.material);      

		bind_mesh(draw.mesh)
        
		//we can now draw
        for(int i = draw.first ;i < draw.count;i++)
        {       
		    vkCmdDraw(cmd, draw.mesh->_vertices.size(), 1,0 , i /*using i to access matrix in the shader */   );
        }
	}
}
```

Note how now we have a direct vkCmdDraw() loop. This maps exactly to a draw indirect command. With the loop like this, we can now write the commands, and execute it that way. 

Im assuming the indirect buffer is allocated as shown above, but using VkDrawIndirectCommand, instead of VkDrawInstancedIndirectCommand. as in the chapter 4 code base indexed rendering wasnt implemented.


```cpp

std::vector<IndirectBatch> draws = compact_draws(objects, count);


VkDrawIndirectCommand* drawCommands = map_buffer(get_current_frame().indirectBuffer);
	

//encode the draw data of each object into the indirect draw buffer
for (int i = 0; i < count; i++)
{
	RenderObject& object = objects[i];
	VkDrawIndirectCommand[i].vertexCount = object.mesh->_vertices.size();
    VkDrawIndirectCommand[i].instanceCount = 1;
    VkDrawIndirectCommand[i].firstVertex = 0;
    VkDrawIndirectCommand[i].firstInstance = i; //used to access object matrix in the shader
}
	

for (IndirectBatch& draw : draws)
{
    bind_descriptors(draw.material);      

    bind_mesh(draw.mesh)
    
    //we can now draw

    VkDeviceSize indirect_offset = draw.first * sizeof(VkDrawIndirectCommand);
    uint32_t draw_stride = sizeof(VkDrawIndirectCommand);

    //execute the draw command buffer on each section as defined by the array of draws
    vkCmdDrawIndirect(cmd,get_current_frame().indirectBuffer,indirect_offset, draw.count,draw_stride);
}
```
Thats it, now the render loop is indirect and should go a bit faster. But the most important think to take into account here, is that the draw commands buffer can be cached and written/read from compute shaders. If you wanted, you can just write it once at load, and just do the loop of vkCmdDrawIndirect every frame. This is also a design where adding culling is extremelly simple.
You just do a compute shader that sets instanceCount to 0 if the object is culled, and thats it. But keep in mind such a thing is not very optimal given that empty draw commands still have overhead, so its better to compact them somehow, either by using a design that uses instancing (like what we are doing in the tutorial engine), or by using DrawIndirectCount after removing the empty draws.

There is also something you can see very clearly there. The less combinations of mesh buffer, descriptors, and pipeline you have, the more you can draw on each DrawIndirect execution. This is why generally you want to be as indirect as possible when doing draw indirect. 


# Draw Indirect architectures

The system explained above is the simplest way you can do draw indirect. It is useful, but generally you want to do a lot more things on top of it. Engines use many different architectures, using draw indirect in many different ways.  Those ways depend on what exactly do you want to do in the engine, and how that maps to draw indirect usage and features. 

In the tutorial engine, we cant use DrawIndirectCount, which is quite a limitation. Instead, we prefer to do *very* few draw indirect commands, and use instancing instead. In there, we are doing 1 draw indirect instanced. 

The pipeline of the tutorial starts by writing the draw-indirect command structs sorted by material and mesh. Then, it performs culling, and for each surviving mesh, it does a +1 on the instance-count of the relevant draw-indirect command. 

In other engines, something that is quite popular to do with draw indirect is to do fine grained culling and mesh merging.






















{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}