---
layout: default
title: Faster Draw
parent: "New 5. GLTF loading"
nav_order: 10
---

# Improving Performance

When we made the draw loop on chapter 4, we did not try to skip vulkan calls if they are the same between RenderObjects. Lets improve that.

We need to keep track of what state we are binding, and only call it again if we have to as it changes with the draw. 

We are going to modify the draw() lambda seen in the last article, and give it state tracking. It will only call the vulkan functions if the parameters change.

```cpp
//defined outside of the draw function, this is the state we will try to skip
 MaterialPipeline* lastPipeline = nullptr;
 MaterialInstance* lastMaterial = nullptr;
 VkBuffer lastIndexBuffer = VK_NULL_HANDLE;

 auto draw = [&](const RenderObject& r) {
     if (r.material != lastMaterial) {
         lastMaterial = r.material;
         //rebind pipeline and descriptors if the material changed
         if (r.material->pipeline != lastPipeline) {

             lastPipeline = r.material->pipeline;
             vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->pipeline->pipeline);
             vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,r.material->pipeline->layout, 0, 1,
                 &globalDescriptor, 0, nullptr);

            VkViewport viewport = {};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = (float)_windowExtent.width;
            viewport.height = (float)_windowExtent.height;
            viewport.minDepth = 0.f;
            viewport.maxDepth = 1.f;

            vkCmdSetViewport(cmd, 0, 1, &viewport);

            VkRect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent.width = _windowExtent.width;
            scissor.extent.height = _windowExtent.height;

            vkCmdSetScissor(cmd, 0, 1, &scissor);
         }

         vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->pipeline->layout, 1, 1,
             &r.material->materialSet, 0, nullptr);
     }
    //rebind index buffer if needed
     if (r.indexBuffer != lastIndexBuffer) {
         lastIndexBuffer = r.indexBuffer;
         vkCmdBindIndexBuffer(cmd, r.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
     }
     // calculate final mesh matrix
     GPUDrawPushConstants push_constants;
     push_constants.worldMatrix = r.transform;
     push_constants.vertexBuffer = r.vertexBufferAddress;

     vkCmdPushConstants(cmd, r.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);

     vkCmdDrawIndexed(cmd, r.indexCount, 1, r.firstIndex, 0, 0);
 };
```

We store the last pipeline, the last material, and the last index buffer.
We begin by checking if the pipeline has changed, and if it did, we bind the pipeline again, and also re-bind the global descriptor set. We also have to call the SetViewport and SetScissor commands too.

Then, we bind the descriptor set it for material parameters and textures if the material instance changed. 
And last, the index buffer gets bound again if it changed.

We now should get a performance win, specially as we only have 2 pipelines, so a lot of those calls to VkCmdBindPipeline now dissapear. But lets improve it further.

We are going to sort the render objects by those parameters to minimize the number of calls. We will only do it this way for the opaque objects, as the transparent objects need a different type of sorting (depth sort) that we arent doing as we dont have the information about whats the center of the object.

At the beggining of the draw_geometry() function, add this

```cpp
// sort the opaque surfaces by material and mesh
std::sort(drawCommands.OpaqueSurfaces.begin(), drawCommands.OpaqueSurfaces.end(), [](const auto& A, const auto& B) {
    if (A.material == B.material) {
        return A.indexBuffer < B.indexBuffer;
    } else {
        return A.material < B.material;
    }
});
```

std::algorithms has a very handy sort function we can use to sort the OpaqueSurfaces vector. We give it a lambda that defines a `<` operator, and it sorts it efficiently for us. 

We will first check if the material is the same, and if it is, sort by indexBuffer. But if its not, then we directly compare the material pointer.

With this the renderer will minimize the number of descriptor set bindings, as it will go material by material. We still have the index buffer binding to deal with but thats faster to switch.

By doing this, the engine should now have significantly more performance. If you run it in release mode, you should be able to draw scenes with tens of thousands of meshes no problem. We arent doing frustrum culling, so the GPU gets a lot of wasted work which does affect its perf.


{% include comments.html term="Vkguide 2 Beta Comments" %}