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
                    filePath.uri.path().end()); 
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

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

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

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
                                           VK_IMAGE_USAGE_SAMPLED_BIT);

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

Lets go back into the gltf loading function and load the images from there.

```cpp
    // load all textures
    for (fastgltf::Image& image : asset->images) {
        std::optional<AllocatedImage> img = load_image(engine, *asset, image);

        if (img.has_value()) {
            images.push_back(*img);
            file.images[image.name.c_str()] = *img;
        } else {
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

The engine is now done. You can use this as a base to implement games. But we are going to do some tweaks that are optional as part of this chapter, improving its performance.


