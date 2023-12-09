---
layout: default
title: Mesh Loading
parent: "New 3. Graphics Pipelines"
nav_order: 12
---

We will do proper scene loading later, but until we go there, we need something better than a rectangle as a mesh. For that, we will begin to load GLTF files but in a very simplified and wrong way, getting only the geometry data and ignoring everything else. 

There is a gltf file that came with the starting-point repository, called basic.glb. That one has a cube, a sphere, and a monkey head meshes centered in the origin. Being a file as simple as that one is, its easy to load it correctly without having to setup up real gltf loading.

A GLTF file will contain a list of meshes, each mesh with multiple primitives on it. This separation is for meshes that use multiple materials, and thus need multiple draw calls to draw it. The file also contains a scene-tree of scenenodes, some of them containing meshes. We will only be loading the meshes now, but later we will have the full scene-tree and materials loaded. 

Our loading code will all be on the files vk_loader.cpp/h . 

Lets start by adding a couple classes for the loaded meshes

```cpp
struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};
```

A given mesh asset will have a name, loaded from the file, and then the mesh buffers. But it will also have an array of GeoSurface that has the sub-meshes of this specific mesh. When rendering each submesh will be its own draw.  We will use StartIndex and count for that drawcall as we will be appending all the vertex data of each surface into the same buffer.

Our load function will be this
```cpp
std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::string_view filePath);
```

This is the first time see see std::optional being used. This is standard class that wraps a type (the vector of mesh assets here) and allows for it to be errored/null. As file loading can fail for many reasons, it returning null is a good idea. We will be using fastGltf library, which uses all of those new stl features for its loading.

Lets begin by opening the file

^code openmesh chapter-3/vk_loader.cpp

We will only be supporting binary GLTF for this for now. So first we open the file with `loadFromFile` and then we call loadBinaryGLTF to open it. This requires the parent path to find relative paths even if we wont have it yet.

Next we will loop each mesh, copy the vertices and indices into temporary std::vector, and upload them as a mesh to the engine. We will be building an array of `MeshAsset` from this.

^code loadmesh chapter-3/vk_loader.cpp

As we iterate each primitive within a mesh, we use the iterateAccessor functions to access the vertex data we want. We also build the index buffer properly while appending the different primitives into the vertex arrays. At the end, we call uploadMesh to create the final buffers, then we return the mesh list.

With the OverrideColors as a compile time flag, we override the vertex colors with the vertex normals which is useful for debugging.

The Position array is going to be there always, so we use that to initialize the Vertex structures. For all the other attributes we need to do it checking that the data exists.

Lets draw them.

```
	GPUMeshBuffers rectangle;
	std::vector<MeshAsset> testMeshes;
```

we begin by adding them to the VulkanEngine class. Lets load them from init_default_data()

```
testMeshes = loadGltfMeshes(this,"..\\..\\assets\\structure.glb").value();
```

In the file provided, index 0 is a cube, index 1 is a sphere, and index 2 is a blender monkeyhead. we will be drawing that last one, draw it right after drawing the rectangle from before

^code meshdraw chapter-3/vk_engine.cpp

You will see that the monkey head is completely white due to the vertex color.  By changing the `OverrideColors` on the loader function, we will store the vertex normals as vertex colors, which will give some colors to the mesh.

Now we have the monkey head and its visible, but its also upside down. Lets fix up that matrix.

In GLTF, the axis are meant to be for opengl, which has the Y up. Vulkan has Y down, so its flipped. We have 2 possibilities here. One would be to use negative viewport height, which is supported and will flip the entire rendering, this would make it closer to directx. On the other side, we can apply a flip that changes the objects as part of our projection matrix. We will be doing that.

From the render code, lets give it a better matrix for rendering.

^code matview chapter-3/vk_engine.cpp

First we calculate the view matrix, which is from the camera. a translation matrix that moves backwards will be fine for now. 

For the projection matrix, we are doing a trick here. Note that we are sending 10000 to the "near" and 0.1 to the "far". We will be reversing the depth, so that depth 1 is the near plane, and depth 0 the far plane. This is a technique that greatly increases the quality of depth testing.

Talking about depth testing, if you run the engine at this point, you will find that the monkey head is drawing a bit glitched. We havent setup depth testing, so triangles of the back side of the head can render on top of the front, creating a wrong image. Lets go and implement depth testing.

Begin by adding a new image into the VulkanEngine class, by the side of the draw-image as they will be paired together while rendering.

```cpp
AllocatedImage _drawImage;
AllocatedImage _depthImage;
```

Now we will initialize it alongside the drawImage, in the init_swapchain function

^code depthimg chapter-3/vk_engine.cpp

The depth image is initialized in the same way as the draw image, but we are giving it the `VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT` usage flag, and we are using `VK_FORMAT_D32_SFLOAT` as depth format. 

Make sure to also add the depth image into the deletion queue.

```cpp
_mainDeletionQueue.push_function([=]() {
	vkDestroyImageView(_device, _drawImage.imageView, nullptr);
	vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);

	vkDestroyImageView(_device, _depthImage.imageView, nullptr);
	vmaDestroyImage(_allocator, _depthImage.image, _depthImage.allocation);
});
```

From the draw loop, we will transition the depth image from undefined into depth attachment mode, In the same way we do with the draw image

```
vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
vkutil::transition_image(cmd, _depthImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
```

Now we need to change the render pass begin info to use this depth attachment and clear it correctly.

```cpp
	VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(_drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingAttachmentInfo depthAttachment = vkinit::depth_attachment_info(_depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	VkRenderingInfo renderInfo = vkinit::rendering_info(_windowExtent, &colorAttachment, &depthAttachment);
```

We already left space for the depth attachment on the vkinit structure for rendering_info, but we also do need to set up the depth clear to its correct value. Lets look at the implementation of the vkinit for depth_attachment_info.

^code depth_info shared/vk_initializers.cpp

Its similar to what we had for color attachment, but we make the loadOP to be clear, and set the depth value on the clear structure to 0.f. As explained above, we are going to use depth 0 as the "far" value, and depth 1 as the near value. 

The last thing is to enable depth testing as part of the pipeline.
We made the depth option when the pipelinebuilder was made, buit left it dsiabled. Lets fill that now. Add this function to PipelineBuilder

^code depth_enable shared/vk_pipelines.cpp

We will leave the stencil parts all off, but we will enable depth testing, and pass the depth OP into the structure.

Now time to use it from the place where we build the mesh pipeline. `init_mesh_pipeline`

```cpp
	//pipelineBuilder.disable_depthtest();
	pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(_depthImage.imageFormat);
```

We call the enable_depthtest function on the builder, and we give it depth write, and as operator GREATER_OR_EQUAL. As mentioned, because 0 is far and 1 is near, we will want to only render the pixels if the current depth value is greater than the depth value on the depth image.

Modify that `set_depth_format` call on the `init_triangle_pipeline` function too. Even if depth testing is disabled for a draw, we still need to set the format correctly for the render pass to work.

You can run the engine now, and the monkey head will be setup properly. The other draws with the triangle and rectangle render behind it because we have no depth testing set for them so they neither write or read from the depth attachment.

Last thing in this chapter, setting up blending for transparent objects.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}