---
layout: default
title: GLTF Textures
parent: "New 5. GLTF loading"
nav_order: 10
---

# Improving Performance

When we made the draw loop on chapter 4, we did not try to skip vulkan calls if they are the same between RenderObjects. Lets improve that.

We need to keep track of what state we are binding, and only call it again if we have to as it changes with the draw. 

Lets use lambdas for it, creating a draw() function within the `draw_geometry` function. Then  we call it while looping through the RenderObjects. This lambda will keep track of the state as we want to. We could add it as function to Vulkan Engine, but we would need to keep track of that state in the class. Better to have it limited to the function only

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

The render object loop now looks like this

```cpp
for (auto& r : drawCommands.OpaqueSurfaces) {
    draw(r);
}
``
