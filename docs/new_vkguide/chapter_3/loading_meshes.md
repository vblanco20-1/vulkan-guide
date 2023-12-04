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

<!-- codegen from tag openmesh on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_loader.cpp --> 
```cpp
	std::cout << "Loading GLTF: " << filePath << std::endl;	

	fastgltf::GltfDataBuffer data;
	data.loadFromFile(filePath);

    constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

	fastgltf::Asset gltf;
    fastgltf::Parser parser{};

	auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
	if (load) {
		gltf = std::move(load.get());
	}
	else {
        fmt::print("Failed to load glTF: {} \n",fastgltf::to_underlying(load.error()));		
		return {};
     }
```

We will only be supporting binary GLTF for this for now. So first we open the file with `loadFromFile` and then we call loadBinaryGLTF to open it. This requires the parent path to find relative paths even if we wont have it yet.

Next we will loop each mesh, copy the vertices and indices into temporary std::vector, and upload them as a mesh to the engine. We will be building an array of `MeshAsset` from this.

<!-- codegen from tag loadmesh on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_loader.cpp --> 
```cpp
     std::vector<std::shared_ptr<MeshAsset>> meshes;

	// use the same vectors for all meshes so that the memory doesnt reallocate as
	// often
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	for (fastgltf::Mesh& mesh : gltf.meshes) {
		MeshAsset newmesh;
		
		newmesh.name = mesh.name;

		// clear the mesh arrays each mesh, we dont want to merge them by error
		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives) {
			GeoSurface newSurface;
			newSurface.startIndex = (uint32_t)indices.size();
			newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor, [&](std::uint32_t idx) {
					indices.push_back(idx + initial_vtx);
				});
			}

			fastgltf::Accessor& posAccessor =gltf.accessors[p.findAttribute("POSITION")->second];

			size_t vidx = initial_vtx;
			fastgltf::iterateAccessor<glm::vec3>(gltf, posAccessor,
				[&](glm::vec3 v) { 
                    Vertex newvtx;
                    newvtx.position = v;
                    newvtx.normal = {1,0,0};
                    newvtx.color = glm::vec4{1.f};
                    newvtx.uv_x = 0;
                    newvtx.uv_y = 0;
                    vertices.push_back(newvtx);
             });

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec3>(gltf, gltf.accessors[(*normals).second],
					[&](glm::vec3 v) { vertices[vidx++].normal = v; });
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v) {

					vertices[vidx].uv_x = v.x;
					vertices[vidx].uv_y = v.y;
					vidx++;
				});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec4>(gltf, gltf.accessors[(*colors).second],
					[&](glm::vec4 v) { vertices[vidx++].color = v; });
			}
			newmesh.surfaces.push_back(newSurface);
		}

        //display the vertex normals
        constexpr bool OverrideColors = false;
        if(OverrideColors){
			for (Vertex& vtx : vertices) {
				vtx.color = glm::vec4(vtx.normal, 1.f);
			}
        }
		newmesh.meshBuffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newmesh)));
	}

	return meshes;

```

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

<!-- codegen from tag meshdraw on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	push_constants.vertexBuffer = testMeshes[2]->meshBuffers.vertexBufferAddress;

	vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(cmd, testMeshes[2]->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmd, testMeshes[2]->surfaces[0].count, 1, testMeshes[2]->surfaces[0].startIndex, 0, 0);
```

You will see that the monkey head is completely white due to the vertex color.  By changing the `OverrideColors` on the loader function, we will store the vertex normals as vertex colors, which will give some colors to the mesh.

Now we have the monkey head and its visible, but its also upside down. Lets fix up that matrix.

In GLTF, the axis are meant to be for opengl, which has the Y up. Vulkan has Y down, so its flipped. We have 2 possibilities here. One would be to use negative viewport height, which is supported and will flip the entire rendering, this would make it closer to directx. On the other side, we can apply a flip that changes the objects as part of our projection matrix. We will be doing that.

From the render code, lets give it a better matrix for rendering.

<!-- codegen from tag matview on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	glm::mat4 view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_windowExtent.width / (float)_windowExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	projection[1][1] *= -1;

	push_constants.worldMatrix = projection * view;
```

First we calculate the view matrix, which is from the camera. a translation matrix that moves backwards will be fine for now. 

For the projection matrix, we are doing a trick here. Note that we are sending 10000 to the "near" and 0.1 to the "far". We will be reversing the depth, so that depth 1 is the near plane, and depth 0 the far plane. This is a technique that greatly increases the quality of depth testing.

Talking about depth testing, if you run the engine at this point, you will find that the monkey head is drawing a bit glitched. We havent setup depth testing, so triangles of the back side of the head can render on top of the front, creating a wrong image. Lets go and implement depth testing.

Begin by adding a new image into the VulkanEngine class, by the side of the draw-image as they will be paired together while rendering.

```cpp
AllocatedImage _drawImage;
AllocatedImage _depthImage;
```

Now we will initialize it alongside the drawImage, in the init_swapchain function

<!-- codegen from tag depthimg on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	_depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;

	VkImageUsageFlags depthImageUsages{};
	depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthImage.imageFormat, depthImageUsages, drawImageExtent);

	//allocate and create the image
	vmaCreateImage(_allocator, &dimg_info, &rimg_allocinfo, &_depthImage.image, &_depthImage.allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(_depthImage.imageFormat, _depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

	VK_CHECK(vkCreateImageView(_device, &dview_info, nullptr, &_depthImage.imageView));
```

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

<!-- codegen from tag depth_info on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkRenderingAttachmentInfo vkinit::depth_attachment_info(
    VkImageView view, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo depthAttachment {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;

    depthAttachment.imageView = view;
    depthAttachment.imageLayout = layout;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue.depthStencil.depth = 0.f;

    return depthAttachment;
}
```

Its similar to what we had for color attachment, but we make the loadOP to be clear, and set the depth value on the clear structure to 0.f. As explained above, we are going to use depth 0 as the "far" value, and depth 1 as the near value. 

The last thing is to enable depth testing as part of the pipeline.
We made the depth option when the pipelinebuilder was made, buit left it dsiabled. Lets fill that now. Add this function to PipelineBuilder

<!-- codegen from tag depth_enable on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::enable_depthtest(bool depthWriteEnable,VkCompareOp op)
{
	_depthStencil.depthTestEnable = VK_TRUE;
	_depthStencil.depthWriteEnable = depthWriteEnable;
	_depthStencil.depthCompareOp = op;
	_depthStencil.depthBoundsTestEnable = VK_FALSE;
	_depthStencil.stencilTestEnable = VK_FALSE;
	_depthStencil.front = {};
	_depthStencil.back = {};
	_depthStencil.minDepthBounds = 0.f;
	_depthStencil.maxDepthBounds = 1.f;
}
```

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

You can run the engine now, and the monkey head will be setup properly. The other draws with the triangle and rectangle render behind it because we have no depth testing set for them so they neither write or read from the depth attachment.

Last thing in this chapter, setting up blending for transparent objects.

Next: [ Blending]({{ site.baseurl }}{% link docs/new_vkguide/chapter_3/blending.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
