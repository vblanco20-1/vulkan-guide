---
layout: default
title: Faster Draw
parent: "New 5. GLTF loading"
nav_order: 10
---



When we made the draw loop on chapter 4, we did not try to skip vulkan calls if they are the same between RenderObjects. Lets improve that.


## Timing UI
Before we begin to optimize performance, we need some way of keeping track of how fast stuff goes. For that, we will be using std::chrono and imgui to setup a really basic benchmark timing. If you want, you can try using Tracy instead, but this can give a simple ui-based timing display. 
We will not be profiling the GPU side, as doing that requires pipeline queries and others and its a much more complex system. For our needs, it will work better to run the program in NSight or other equivalent GPU profiling programs.

Lets add a struct to vk_engine.h to hold timing info.

```cpp
struct EngineStats {
    float frametime;
    int triangle_count;
    int drawcall_count;
    float scene_update_time;
    float mesh_draw_time;
};
```

frametime will be our global timing, and will likely just be locked to your monitor refresh rate as we are doing vsync. The others will be useful to measure.

Lets begin by calculating frametime.

On the engine main loop at `run()`, we will add some code at the start of the loop, and the end
```cpp
// main loop
while (!bQuit) {
    //begin clock
    auto start = std::chrono::system_clock::now();

    //everything else

    //get clock again, compare with start clock
    auto end = std::chrono::system_clock::now();
     
     //convert to microseconds (integer), and then come back to miliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.frametime = elapsed.count() / 1000.f;
}
```

using `auto start = std::chrono::system_clock::now();` gives us high precision clock for "now". By calling it again later, we can find how much time a given section of code took.
To convert it into a frametime in miliseconds, we need to first cast it to microseconds (1/1000th of a milisecond) and then multiply it by 1000.f This way we get 3 decimal places.

On draw_geometry, we will add some code for this timing, and to calculate the number of triangles and draws.

```cpp
void VulkanEngine::draw_geometry(VkCommandBuffer cmd)
{
    //reset counters
    stats.drawcall_count = 0;
    stats.triangle_count = 0;
    //begin clock
    auto start = std::chrono::system_clock::now();

    /* code */

    auto draw = [&](const RenderObject& r) {

        /* drawing code */

        vkCmdDrawIndexed(cmd, draw.indexCount, 1, draw.firstIndex, 0, 0);

        //add counters for triangles and draws
        stats.drawcall_count++;
        stats.triangle_count += draw.indexCount / 3;   
    }

    /* code */


    auto end = std::chrono::system_clock::now();

    //convert to microseconds (integer), and then come back to miliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.mesh_draw_time = elapsed.count() / 1000.f;
}
```

We get the start time, and reset counters to 0. Then from the draw lambda, we add drawcall count and triangle count after the draw logic. At the very end of the function, we get final time and store it on the stats struct.

Do the same code on `update_scene()` with the start/end clocks, storing it on `scene_update_time`

Now we need to display them using imgui.

In the `run()` function, between the call to `ImGui::NewFrame()` and `ImGui::Render();`, add this code to draw a new imgui window.

```cpp
        ImGui::Begin("Stats");

        ImGui::Text("frametime %f ms", stats.frametime);
        ImGui::Text("draw time %f ms", stats.mesh_draw_time);
        ImGui::Text("update time %f ms", stats.scene_update_time);
        ImGui::Text("triangles %i", stats.triangle_count);
        ImGui::Text("draws %i", stats.drawcall_count);
        ImGui::End();
```

If you run the engine, you will see the timings. Right now we have validation layers enabled, and likely also debug mode on. Turn on release mode in your compiler settings, and disable validation layers by setting `constexpr bool bUseValidationLayers = true;` to false



## Draw sorting
Right now, we are calling the vulkan calls a lot more than we should because we keep rebinding pipeline every draw and others. 
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
    //stats
    stats.drawcall_count++;
    stats.triangle_count += r.indexCount / 3;
 };
```

We store the last pipeline, the last material, and the last index buffer.
We begin by checking if the pipeline has changed, and if it did, we bind the pipeline again, and also re-bind the global descriptor set. We also have to call the SetViewport and SetScissor commands too.

Then, we bind the descriptor set it for material parameters and textures if the material instance changed. 
And last, the index buffer gets bound again if it changed.

We now should get a performance win, specially as we only have 2 pipelines, so a lot of those calls to VkCmdBindPipeline now dissapear. But lets improve it further.

We are going to sort the render objects by those parameters to minimize the number of calls. We will only do it this way for the opaque objects, as the transparent objects need a different type of sorting (depth sort) that we arent doing as we dont have the information about whats the center of the object.

To implement the sorting, we wont be sorting the draw array itself, as the objects are big. Instead, we are going to sort an array of indices to this draw array. This is a common technique in big engines.

At the beggining of the draw_geometry() function, add this

```cpp
    std::vector<uint32_t> opaque_draws;
    opaque_draws.reserve(drawCommands.OpaqueSurfaces.size());

    for (uint32_t i = 0; i < drawCommands.OpaqueSurfaces.size(); i++) {
        opaque_draws.push_back(i);
    }

    // sort the opaque surfaces by material and mesh
    std::sort(opaque_draws.begin(), opaque_draws.end(), [&](const auto& iA, const auto& iB) {
    const RenderObject& A = drawCommands.OpaqueSurfaces[iA];
    const RenderObject& B = drawCommands.OpaqueSurfaces[iB];
    if (A.material == B.material) {
        return A.indexBuffer < B.indexBuffer;
    } else {
        return A.material < B.material;
    }
    });
```

std::algorithms has a very handy sort function we can use to sort the opaque_draws vector. We give it a lambda that defines a `<` operator, and it sorts it efficiently for us. 

We will first index the draw array, and check if the material is the same, and if it is, sort by indexBuffer. But if its not, then we directly compare the material pointer. Another way of doing this is that we would calculate a sort key , and then our opaque_draws would be something like 20 bits draw index, and 44 bits for sort key/hash. That way would be faster than this as it can be sorted through faster methods.

Now, for the draws, we draw from the sorted array. Replace the draw loop with this one.

```cpp
for (auto& r : opaque_draws) {
    draw(drawCommands.OpaqueSurfaces[r]);
}
```


With this the renderer will minimize the number of descriptor set bindings, as it will go material by material. We still have the index buffer binding to deal with but thats faster to switch.

By doing this, the engine should now have significantly more performance. If you run it in release mode, you should be able to draw scenes with tens of thousands of meshes no problem. But out GPU cost is high as we are processing meshes outside the view. That gets improved by doing Frustum Culling.

## Frustum Culling
Right now we render every object in the map, but we dont have to draw things that are ouside of the view. As we have the draw list, we will filter it to check what objects are in view, and skip the ones that dont. As long as the cost of the filtering is cheaper than the cost of rendering objects, we have a win.

There are multiple ways of doing Frustum culling, but with the data and architecture we have, we will use oriented bounding boxes. We will calculate bounds for each GeoSurface, and then check if the bounds are in view.

Update the structures in vk_loader.h with the bounds.
```cpp
struct Bounds {
    glm::vec3 origin;
    float sphereRadius;
    glm::vec3 extents;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
	std::shared_ptr<GLTFMaterial> material;
};
```

Also add the Bounds structure to RenderObject

```cpp
struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;
    
    MaterialInstance* material;
    Bounds bounds;
    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};
```

Our bounds are a origin, extent (box size), and sphere radius. The sphere radius can be used in case we want to use other frustum culling algorithms and has other uses.

To calculate it, we must add it to the loader code. 

This code goes inside the loadGLTF function, at the end of the loop that loads the mesh data.

```cpp
//code that writes vertex buffers

//loop the vertices of this surface, find min/max bounds
glm::vec3 minpos = vertices[initial_vtx].position;
glm::vec3 maxpos = vertices[initial_vtx].position;
for (int i = initial_vtx; i < vertices.size(); i++) {
    minpos = glm::min(minpos, vertices[i].position);
    maxpos = glm::max(maxpos, vertices[i].position);
}
// calculate origin and extents from the min/max, use extent lenght for radius
newSurface.bounds.origin = (maxpos + minpos) / 2.f;
newSurface.bounds.extents = (maxpos - minpos) / 2.f;
newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);

newmesh->surfaces.push_back(newSurface);
```

From the MeshNode::Draw() function, make sure that the bounds are copied to the RenderObject. It should look like this now.

```cpp
void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx) {
    glm::mat4 nodeMatrix = topMatrix * worldTransform;

    for (auto& s : mesh->surfaces) {
        RenderObject def;
        def.indexCount = s.count;
        def.firstIndex = s.startIndex;
        def.indexBuffer = mesh->meshBuffers.indexBuffer.buffer;
        def.material = &s.material->data;
        def.bounds = s.bounds;
        def.transform = nodeMatrix;
        def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;

        if (s.material->data.passType == MaterialPass::Transparent) {
            ctx.TransparentSurfaces.push_back(def);
        } else {
            ctx.OpaqueSurfaces.push_back(def);
        }
    }

    // recurse down
    Node::Draw(topMatrix, ctx);
}
```

Now we have the bounds on the GeoSurface, and just need to check for visibility on the RenderObject. Add this function to vk_engine.cpp, its a global function.

^code visfn chapter-5/vk_engine.cpp

This is just one of the multiple possible functions we could be using for frustum culling. The way this works is that we are transforming each of the 8 corners of the mesh-space bounding box into screenspace, using object matrix and view-projection matrix. For those, we find the screen-space box bounds, and we check if that box is inside the clip-space view. This way of calculating bounds is on the slow side compared to other formulas, and can have false-positives where it things objects are visible when they arent. All the functions have different tradeoffs, and this one was selected for code simplicity and parallels with the functions we are doing on the vertex shaders.
 
To use it, we change the loop we added to fill the opaque_draws array.

```cpp
for (int i = 0; i < drawCommands.OpaqueSurfaces.size(); i++) {
    if (is_visible(drawCommands.OpaqueSurfaces[i], sceneData.viewproj)) {
        opaque_draws.push_back(i);
    }
}
```

Now instead of adding `i` to it, we check for visibility. 

The renderer now will skip objects outside of the view. It should look the same as it did, but run faster and with less draws. If you get visual glitches, double-check the building of the bounding box for the `GeoSurface` and see if there is a typo in the is_visible function.

The code for doing the same cull and sort but on the transparent objects has been skipped, but its the same as with the opaque objects, so you can try doing it yourself.

With the transparent objects, you want to also change the sorting code so that it checks distance from bounds to the camera, so that objects draw more correct. But sorting by depth is incompatible with sorting by pipeline, so you will need to decide what works better for your case.

## Creating Mipmaps

When we added the texture loading, we didnt make mipmaps. Unlike in OpenGL, there isnt a direct one-call to generate them. We need to do it ourselves.

`create_image` already had mipmap support, but we need to change the version that uploads the data so that it generates the mipmaps. For that we will change the function

^code create_mip_2 chapter-5/vk_engine.cpp

`immediate_submit` now can call into `vkutil::generate_mipmaps()` function if we want to use mipmapping on this image. Lets add that one to vk_images.h

```cpp
namespace vkutil {
void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent3D imageSize);
}
```

There are multiple options for generating the mipmaps. We also dont have to generate them at load time, and could use formats like KTX or DDS which can have the mipmaps pregenerated. A popular option is to generate them in a compute shader that generates multiple levels at once, and that can improve performance. The way we are going to do mipmaps is with a chain of VkCmdImageBlit calls.

For each level, we need to copy the image from the level before it into the next level, lowering the resolution by half each time. On each copy, we transition the mipmap level to `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL` . Once all copies are done, we add another barrier, this time for all the mipmap levels at once, to transition the image into `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`. 

The pseudocode looks like this.

```cpp
//image already comes with layout VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL on all mipmap levels from image creation

int miplevels = calculate_mip_levels(imageSize);
for (int mip = 0; mip < mipLevels; mip++) {

    barrier( image.mips[mip] , VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)

    //not the last level
    if (mip < mipLevels - 1)
    {
        copy_image(image.mips[mip], image.mips[mip+1];)
    }
}

barrier( image , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
```

Lets now look at the actual code

^code mipgen shared/vk_images.cpp

The barrier is very similar to the one we have on `transition_image`, and the blit is similar to what we have in `copy_image_to_image` but with mip levels. In a way, this function combines the two.

At each loop, we divide the image size by two, transition the mip level we copy from, and perform a VkCmdBlit from one mip level to the next.

With this, now we automatically generate the mipmaps needed. We were already creating the samplers with the correct options, so it should work directly. 

{% include comments.html term="Vkguide 2 Beta Comments" %}