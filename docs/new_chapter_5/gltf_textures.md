---
layout: default
title: GLTF Textures
parent: "New 5. GLTF loading"
nav_order: 7
---

We left the textures behind last article to allow them to have a bit more detail here.

For the textures, we are going to load them using stb_image. This is a single-header library to load png, jpeg, and a few others. Sadly, it does not load KTX or DDS formats, which are much better for graphics usages as they can be uploaded almost directly into the GPU and are a compressed format tha the GPU reads directly so it saves VRAM.

fastgltf has a few different possibilites when loading images, so we need to support them. 

Lets write the logic into a load_image function

<!-- codegen from tag loadimg on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
std::optional<AllocatedImage> load_image(VulkanEngine* engine, fastgltf::Asset& asset, fastgltf::Image& image)
{
    AllocatedImage newImage {};

    int width, height, nrChannels;

    std::visit(
        fastgltf::visitor {
            [](auto& arg) {},
            [&](fastgltf::sources::URI& filePath) {
                assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                assert(filePath.uri.isLocalPath()); // We're only capable of loading
                                                    // local files.

                const std::string path(filePath.uri.path().begin(),
                    filePath.uri.path().end()); // Thanks C++.
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT,false);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::Vector& vector) {
                unsigned char* data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()),
                    &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT,false);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(fastgltf::visitor { // We only care about VectorWithMime here, because we
                                               // specify LoadExternalBuffers, meaning all buffers
                                               // are already loaded into a vector.
                               [](auto& arg) {},
                               [&](fastgltf::sources::Vector& vector) {
                                   unsigned char* data = stbi_load_from_memory(vector.bytes.data() + bufferView.byteOffset,
                                       static_cast<int>(bufferView.byteLength),
                                       &width, &height, &nrChannels, 4);
                                   if (data) {
                                       VkExtent3D imagesize;
                                       imagesize.width = width;
                                       imagesize.height = height;
                                       imagesize.depth = 1;

                                       newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                                           VK_IMAGE_USAGE_SAMPLED_BIT,false);

                                       stbi_image_free(data);
                                   }
                               } },
                    buffer.data);
            },
        },
        image.data);

    // if any of the attempts to load the data failed, we havent written the image
    // so handle is null
    if (newImage.image == VK_NULL_HANDLE) {
        return {};
    } else {
        return newImage;
    }
}
```

Long function, but its really just 3 versions of the same thing.

The initial one is for the cases where the textures are stored outside of the gltf/glb file, which is common. In that case, we call stb_load with the path to the texture, and if it succeeds, we create the image.

The second one is when fastgltf loads the texture into a std::vector type structure. This is seen if the texture is on base64, or if we instruct it to load  external image files. We grab the bytes and send them to stbi_load_from_memory. 

The third one is is loading from a BufferView, when the image file is embedded into the binary GLB file. We do the same as on the second one, and use stbi_load_from_memory.

If you try to compile the engine at this point, you will see that the STB_image functions are missing their definitions. STB is a single-header library that requires a macro to be included into one translation unit for it to compile the function definitions. Add this into vk_images.cpp . Can also be another cpp file. This will make stb_image add the definitions of the functions into this cpp file for linking.

```
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

Lets go back into the gltf loading function and load the images from there.

```cpp
    // load all textures
	for (fastgltf::Image& image : gltf.images) {
		std::optional<AllocatedImage> img = load_image(engine, gltf, image);

		if (img.has_value()) {
			images.push_back(*img);
			file.images[image.name.c_str()] = *img;
		}
		else {
			// we failed to load, so lets give the slot a default white texture to not
			// completely break loading
			images.push_back(engine->_errorCheckerboardImage);
			std::cout << "gltf failed to load texture " << image.name << std::endl;
		}
	}
```

We try to load the image, and if the load succeeded, we store the image into the list. if it fails, we use the error image.

If you try to run the project again now, you will see we have textures on the objects.

Lets fill the clearAll function on the LoadedGLTF so that the resources can be freed properly

```cpp
void LoadedGLTF::clearAll()
{
    VkDevice dv = creator->_device;

    descriptorPool.destroy_pools(dv);
    creator->destroy_buffer(materialDataBuffer);

    for (auto& [k, v] : meshes) {

		creator->destroy_buffer(v->meshBuffers.indexBuffer);
		creator->destroy_buffer(v->meshBuffers.vertexBuffer);
    }

    for (auto& [k, v] : images) {
        
        if (v.image == creator->_errorCheckerboardImage.image) {
            //dont destroy the default images
            continue;
        }
        creator->destroy_image(v);
    }

	for (auto& sampler : samplers) {
		vkDestroySampler(dv, sampler, nullptr);
    }
}
```

At the start we destroy the descriptorPools and the shared material buffer. Then we loop all meshes and destroy the mesh buffers, both index and vertex. Then we loop through the images, and for every image that isnt the error image we destroy them (as seen above, we might use the error image if it failed to load. We dont want to be deleting that multiple times).

Last, we loop the samplers and destroy each of them. 

Important detail with this. You cant delete a LoadedGLTF within the same frame its being used. Those structures are still around. If you want to destroy a LoadedGLTF at runtime, either do a VkQueueWait like we have in the cleanup function, or add it into the per-frame deletion queue and defer it. We are storing the shared_ptrs to hold LoadedGLTF, so it can abuse the lambda capture functionality to do this.


# Transparent objects

We omitted those before, but gltf files not only have opaque draws, they also have transparent objects. When we created the GLTF main material and compiled its pipeline, we enabled blending for it. We already have the code in the loader that sets the pass mode to transparent, but we arent handling the rendering correctly for transparent objects. 

Transparent objects do not write to the depth buffer, so if a transparent object is drawn, it can then have a opaque object drawn on top of it, causing visual glitches. We need to move the transparent objects so that they draw at the end of the frame. 

For that, we could do sorting on the RenderObjects, but transparent objects also sort in a different way to opaque objects, so a better option is to make the DrawContext structure hold 2 different arrays of RenderObjects, one for opaque, and other for transparent. Separating the objects like this is very useful for various reasons like doing a depth pass only on the opaque surfaces, or other shader logic. Its also common to see things like rendering the transparent objects into a different image and then compositing them on top.

```cpp
struct DrawContext {
    std::vector<RenderObject> OpaqueSurfaces;
    std::vector<RenderObject> TransparentSurfaces;
};
```

Now we change the `draw_geometry` function. As we need to call the vulkan call from 2 loops, we are going to move the inner draw loop into a draw() lambda, and then call it from the loops.

```cpp
auto draw = [&](const RenderObject& draw) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 0, 1, &globalDescriptor, 0, nullptr);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 1, 1, &draw.material->materialSet, 0, nullptr);

    vkCmdBindIndexBuffer(cmd, draw.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    GPUDrawPushConstants pushConstants;
    pushConstants.vertexBuffer = draw.vertexBufferAddress;
    pushConstants.worldMatrix = draw.transform;
    vkCmdPushConstants(cmd, draw.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);

    vkCmdDrawIndexed(cmd, draw.indexCount, 1, draw.firstIndex, 0, 0);
};

for (auto& r : mainDrawContext.OpaqueSurfaces) {
    draw(r);
}

for (auto& r : mainDrawContext.TransparentSurfaces) {
    draw(r);
}
```

By doing this, we now have proper transparent objects. If you load the structure gltf, you will see that the light halos should not glitch anymore. 

Make sure to reset the array of transparent draws too at the end of the function

```
// we delete the draw commands now that we processed them
mainDrawContext.OpaqueSurfaces.clear();
mainDrawContext.TransparentSurfaces.clear();
```

The basic features of the engine are now done. You can use this as a base to implement games. But we are going to do some tweaks that are optional as part of this chapter, improving its performance and quality.

Next: [ Faster Draw]({{ site.baseurl }}{% link docs/new_chapter_5/faster_draw.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
