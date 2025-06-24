---
layout: default
title: Implementing an asset system.
parent: Extra Chapter
nav_order: 31
---
## Deprecation warning
This article is outdated for the current version of vkguide and its valid for Legacy Vkguide. This article is no longer needed due to the movement to loading from GLTF on the new tutorial, removing the need for this.


## Loading Assets

In the tutorial, we have been loading .obj and .png files directly. The issue with doing that is that we depend on 3rd party libraries (stb_image and tinyobj), and loading the formats like that is quite inefficient. You have probably seen that the engine takes a couple seconds to load when you are in debug mode.

On real engines, you don't load those formats at runtime. Instead, you convert those formats into a engine-specific fast-loadable format, and then load that.

The advantages are considerable:
* Orders of magnitude faster loading.
* Can customize the formats for what the engine needs.
* Formats can be stored in a platform specific way so that at runtime the bulk data can be copied directly into VRAM
* Only one format to deal with in the main runtime engine, convert every other format to that one.
* Heavy asset loading third party libraries like Assimp or FBX SDK are left outside of the main executable.
* Heavily simplified runtime logic for loading

On the other side, there are a few disadvantages to take into account.
* Need to develop and maintain the custom formats.
* Need to develop and maintain a converter that converts from classic formats into the custom one.
* Custom formats won't be readable by any tool.

In general, it's a good idea to implement an asset system into your engine, unless you don't care about the load times or the executable bloat. While maintaining the custom formats is extra work, it's not that much compared to the productivity increase of having an engine that loads scenes very quickly.

# Asset System architecture

For the asset system, we are going to implement it into multiple targets and libraries.
We are going to have one executable target that will be a converter. This converter will look at a folder, and attempt to convert every file in it into an optimized format.
The other target is going to be the shared library for asset loading. We will implement the core save/load logic into here. Both the main runtime engine and the asset converter are going to link into this shared library. This way it will be easier to have everything in sync.


<div class="mxgraph" style="max-width:100%;border:1px solid transparent;" data-mxgraph="{&quot;highlight&quot;:&quot;#0000ff&quot;,&quot;nav&quot;:true,&quot;resize&quot;:true,&quot;toolbar&quot;:&quot;zoom layers lightbox&quot;,&quot;edit&quot;:&quot;_blank&quot;,&quot;xml&quot;:&quot;&lt;mxfile host=\&quot;app.diagrams.net\&quot; modified=\&quot;2020-11-01T10:28:40.080Z\&quot; agent=\&quot;5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36\&quot; etag=\&quot;0Fv1aVHmEs7kiYTDFfky\&quot; version=\&quot;13.8.8\&quot; type=\&quot;device\&quot;&gt;&lt;diagram id=\&quot;xavLt3keOgy3rKo3dEt6\&quot; name=\&quot;Page-1\&quot;&gt;7Zlbb5swGIZ/DZedsDk1l03SwzpNa5VJba8qBxzwBJgZp4H++plgTiZV2igpiVapUu3XR97viT+TaMYkyq4ZSoKf1MOhBnUv04ypBiEwIdSKP93LS8WxpeAz4slOjTAjr1iKulSXxMNppyOnNOQk6YoujWPs8o6GGKOrbrcFDburJsjHPWHmorCvPhCPB6V6Dp1Gv8HED6qVgT0qWyJUdZZPkgbIo6uWZFxqxoRRystSlE1wWJhX+VKOu3qjtd4YwzF/z4Dr5P7u9c+tecdvHmB+b39frBZncpYXFC7lA2vQDsV843mxZZ5LH+y/y2Kf4wWN+Vm6jtKF6ACcJGsaRcmX/8Oq8+6zXKQp5iGZ1xti6vx9RTx9uWhPnjca7OwJMrqMPVyYpIvmVUA4niXILVpXgmmhBTwKRQ3Uo18w4zh7Mw6gjq74WGAaYc5y0UUOMAwJhPxEAEfWVw1fwJRa0GLLlhqSSPv11E3URUEG/gMQwOOF4DL2SYxPO+Bg1A24YQ0dcPN4Az6h8dpsdtoxN61ji7m1IeaKSSJDJUVxEeLsosidwgsce7I4dUOUpsTteiX8YPlj4es3q6o+SZvXlWnWqeWyVi6OvV72VRwW6R4xH/Ntx1c/Ei2nrQ1GVxrDIeLkpbuNTe7LFe4oWfNdnea21Qk0NJQApnTJXCxHtdOzMpFpKsSoJJQ+9CZaw1A/9u582AfiIyO8xMOxZPWp1dTQUVTyNioHhaqMyvaL0Vb4zCHhM2GXGejsCp+jTATfB58IPcpb3ZKiQ3oIPJ0enuJ+SKJk8JPeVrJ7fQC0T3r4mSf9ec+q3yTOf41vj84rAw7t1ajn1dX4UQiz6Y+jc8sEQ7tVbaBlV8rnzyQqXqKPzS5LH9yuTS9W+8yprYQqM+X2lAo6CbXJr4dPqc4ppFRDPaLAnu5zPcwOfJ8Dxhd8/aT4Bd/nwLfpG4b/F77RKcBnWW/kz4/CZ+vKvcX6ZPgO9VXHDvDtESKgD0qH8oZomTvSYaln3Pm+6BDV5teUsnvzm5Rx+Q8=&lt;/diagram&gt;&lt;/mxfile&gt;&quot;}"></div>
<script type="text/javascript" src="https://viewer.diagrams.net/js/viewer-static.min.js"></script>

Engine and Converter will both link AssetLib. Converter will link all the different asset format libraries.  We will keep Engine completely free from those formats. The asset system is 100% separated from the main vulkan engine, so feel free to copypaste it directly into your engine or project.


# Libraries
We will add 2 libraries to the project. The first one is LZ4 compression library, which we will be using to compress the binary blobs. The second one is nlohmann's Json library, which is the easiest to use json library for modern Cpp and it's really nice to use.

* LZ4: https://github.com/lz4/lz4
* Json: https://github.com/nlohmann/json


# Asset Format
We are going to keep the file format very simple, while still being very fast to load.
We are taking a hint from how the glTF format works, and have a binary file that holds a json header alongside a binary blob.
The json header will contain metadata and information about the object, and the binary blob will contain the raw data of the object, like the pixels of a texture.

For the binary blob, we will also compress it using LZ4 format. LZ4 is a compression codec that is optimized around speed, and if we use it, it's very likely that we will go faster than reading uncompressed data from disk. But we will also keep space in the format for other compression systems, as things like the new consoles have .zip or kraken compression implemented in hardware.

All assets will follow the format of json metadata + compressed binary blob. This way the code will be unified and easier to handle.

# Texture asset
For the textures, we will store their data in the json. Width, Depth, format, etc. The actual pixel data will go into the blob, compressed.
The uncompressed pixel data is going to be exactly the same as vulkan expects to find in a buffer to then copy into a VkImage.

When loading a texture asset, we will first load the file as usual, but we will keep the texture in its uncompressed state. When copying the texture data into a VkBuffer for upload, we will decompress the data on the fly.

For compression we still just use LZ4, but later down the line, we can also have the pixel data into a BCn format, which will also save us VRAM.

# Mesh asset
The mesh asset will be similar to the textures, except that we will not copy the uncompressed blob directly into the GPU. Instead, the compressed blob is just an array of Vertex structs, with multiple different types of vertices. When loading, we will transform those Vertex structs into whatever the engine uses. For some of the vertex formats, it would be possible to have them synced with the renderer and just copy directly into the buffer without translation.


## CODE
We will begin by writing the core Asset logic. This will handle the generic "json + blob" structure which we will then process as different kinds of assets.
On the code for the asset library, there are 2 incredibly important things to keep in mind. We want to make absolutely sure that the headers do not include libraries such as nlohman json or lz4. Those libraries will be compiled into the AssetLib library, and will be invisible to the vulkan engine itself. Also, we are going to make the api completely stateless. No classes or state kept beetween function calls. This way we make sure that the library will be possible to use from multiple threads safely.

We only need 2 functions for the api of the "Core" asset file, and the AssetFile struct.

```cpp
namespace assets {
	struct AssetFile {
		char type[4];
		int version;
		std::string json;
		std::vector<char> binaryBlob;
	};

	bool save_binaryfile(const char* path, const AssetFile& file);

	bool load_binaryfile(const char* path, AssetFile& outputFile);
}
```

That will be our entire public API for the core asset system. Textures and meshes go into separated files and work on top of the AssetFile struct.

Loading a file will copy the json into the string, and will copy the compressed binary blob into the vector inside the asset. Be very careful with that, as the AssetFile structs will be really big, so do not store them anywhere to avoid bloating up RAM usage.

Let's start by showing the code to save an asset to disk
```cpp
bool assets::save_binaryfile(const  char* path, const assets::AssetFile& file)
{
	std::ofstream outfile;
	outfile.open(path, std::ios::binary | std::ios::out);

	outfile.write(file.type, 4);
	uint32_t version = file.version;
	//version
	outfile.write((const char*)&version, sizeof(uint32_t));

	//json length
	uint32_t length = file.json.size();
	outfile.write((const char*)&length, sizeof(uint32_t));

	//blob length
	uint32_t bloblength = file.binaryBlob.size();
	outfile.write((const char*)&bloblength, sizeof(uint32_t));

	//json stream
	outfile.write(file.json.data(), length);
	//blob data
	outfile.write(file.binaryBlob.data(), file.binaryBlob.size());

	outfile.close();

	return true;
}
```

We will be doing purely binary files.

We begin by storing the 4 chars that are the asset type. For textures this will be `TEXI`, and for meshes it will be `MESH`. We can use this to easily identify if the binary file we are loading is a mesh or a texture, or some wrong format.

Next, we store Version, which is a single uint32 number. We can use this if we change the format at some point, to give an error when trying to load it. It's critical to always version your file formats.

After the version, we store the length, in bytes, of the json string, and then the length of the binary blob.

With the header written, now we just write the json and the blob directly to the file. We begin by writing the entire json string, and then directly the binary blob.

To load the file from disk, we do the same, but in reverse.

```cpp
bool assets::load_binaryfile(const char* path, assets::AssetFile& outputFile)
{
	std::ifstream infile;
	infile.open(path, std::ios::binary);

	if (!infile.is_open()) return false;

	//move file cursor to beginning
	infile.seekg(0);

	infile.read(outputFile.type, 4);
	infile.read((char*)&outputFile.version, sizeof(uint32_t));

	uint32_t jsonlen = 0;
	infile.read((char*)&jsonlen, sizeof(uint32_t));

	uint32_t bloblen = 0;
	infile.read((char*)&bloblen, sizeof(uint32_t));

	outputFile.json.resize(jsonlen);
	infile.read(outputFile.json.data(), jsonlen);

	outputFile.binaryBlob.resize(bloblen);
	infile.read(outputFile.binaryBlob.data(), bloblen);

	return true;
}
```

We read version, type, and length of json and blob.
Then we read the json string by using the length stored in the header, and same thing with the blob.

We are not doing any version or type check yet here. The functions will just return false if the file isn't found, but there is no error checking.

That's all we needed for the asset file itself. It's just a very simple dump of the json string and the binary drop into a packed file.

The more interesting thing is handling textures and meshes. We will only walkthrough the Texture save/load logic, as meshes work in the same way, you can look at the codebase to see the differences.

# Textures

```cpp
namespace assets {
	enum class TextureFormat : uint32_t
	{
		Unknown = 0,
		RGBA8
	};


	struct TextureInfo {
		uint64_t textureSize;
		TextureFormat textureFormat;
		CompressionMode compressionMode;
		uint32_t pixelsize[3];
		std::string originalFile;
	};

	//parses the texture metadata from an asset file
	TextureInfo read_texture_info(AssetFile* file);

	void unpack_texture(TextureInfo* info, const char* sourcebuffer, size_t sourceSize, char* destination);

	AssetFile pack_texture(TextureInfo* info, void* pixelData);
}
```

Like with the main asset file, we are going to keep the API very small and stateless.
The `read_texture_info` will parse the metadata json in a file and convert it into the TextureInfo struct, which is the main data of the texture.

`unpack_texture` will work with a texture info alongside the binary blob of pixel data, and will decompress the texture into the destination buffer. It's very important that the destination buffer is big enough, or it will overflow. This is meant to be used to unpack the blob directly into a buffer.

```cpp
	//prepare asset file and texture info

	//example of how to load the data

	AllocatedBuffer stagingBuffer = engine.create_buffer(textureInfo.textureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);

	assets::unpack_texture(&textureInfo, file.binaryBlob.data(), file.binaryBlob.size(), (char*)data);

	vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);

	//upload texture to gpu with commands
```
Let's look at the implementation to pack a texture into an AssetFile

```cpp
assets::AssetFile assets::pack_texture(assets::TextureInfo* info, void* pixelData)
{
	nlohmann::json texture_metadata;
	texture_metadata["format"] = "RGBA8";
	texture_metadata["width"] = info->pixelsize[0];
	texture_metadata["height"] = info->pixelsize[1];
	texture_metadata["buffer_size"] = info->textureSize;
	texture_metadata["original_file"] = info->originalFile;


	//core file header
	AssetFile file;
	file.type[0] = 'T';
	file.type[1] = 'E';
	file.type[2] = 'X';
	file.type[3] = 'I';
	file.version = 1;

	//compress buffer into blob
	int compressStaging = LZ4_compressBound(info->textureSize);

	file.binaryBlob.resize(compressStaging);

	int compressedSize = LZ4_compress_default((const char*)pixelData, file.binaryBlob.data(), info->textureSize, compressStaging);

	file.binaryBlob.resize(compressedSize);

	texture_metadata["compression"] = "LZ4";

	std::string stringified = texture_metadata.dump();
	file.json = stringified;


	return file;
}
```

We start by converting from the Info struct into json. Then we prepare the header of the asset file by encoding the `TEXI` type + version 1. Then we compress the pixel data into the binary blob of the asset, and finally convert the json into a string and store it into the asset.

The lz4 library is used like this.
```cpp
std::vector<char> blob;

//find the maximum data needed for the compression
int compressStaging = LZ4_compressBound(sourceSize);

//make sure the blob storage has enough size for the maximum
blob.resize(compressStaging);

//this is like a memcpy, except it compresses the data and returns the compressed size
int compressedSize = LZ4_compress_default((const char*)source, blob.data(), sourceSize, compressStaging);

//we can now resize the blob down to the final compressed size.
blob.resize(compressedSize);
```

For the unpacking code, it looks like this.

First we need to get the texture info from the asset

```cpp
assets::TextureInfo assets::read_texture_info(AssetFile* file)
{
	TextureInfo info;

	nlohmann::json texture_metadata = nlohmann::json::parse(file->json);

	std::string formatString = texture_metadata["format"];
	info.textureFormat = parse_format(formatString.c_str());

	std::string compressionString = texture_metadata["compression"];
	info.compressionMode = parse_compression(compressionString.c_str());

	info.pixelsize[0] = texture_metadata["width"];
	info.pixelsize[1] = texture_metadata["height"];
	info.textureSize = texture_metadata["buffer_size"];
	info.originalFile = texture_metadata["original_file"];

	return info;
}
```
This is pretty much the same as above except mirrored. We read the data from the json and store it into the TextureInfo.
With a texture info, we can now call `unpack_texture`


```cpp
void assets::unpack_texture(TextureInfo* info, const char* sourcebuffer, size_t sourceSize, char* destination)
{
	if (info->compressionMode == CompressionMode::LZ4) {
		LZ4_decompress_safe(sourcebuffer, destination, sourceSize, info->textureSize);
	}
	else {
		memcpy(destination, sourcebuffer, sourceSize);
	}
}
```

When unpacking, we just decompress directly into the target destination. If the file isn't compressed, we then just memcpy directly.

That's all for the texture asset logic. For the mesh logic, it works in a similar way, so you can look at the code.

* Core asset system: https://github.com/vblanco20-1/vulkan-guide/blob/engine/assetlib/asset_loader.cpp
* Texture loader: https://github.com/vblanco20-1/vulkan-guide/blob/engine/assetlib/texture_asset.cpp
* Mesh loader: https://github.com/vblanco20-1/vulkan-guide/blob/engine/assetlib/mesh_asset.cpp


## Converter

With the texture save/load logic implemented, we can now look at the converter itself.
The converter will be a separate executable from the normal engine. This is to isolate all of the libs it will use so that they don't pollute the engine. This also means we can compile it in release mode and have it convert everything very fast, and then we load that from our debug mode engine.

The entire codebase for the converter is here https://github.com/vblanco20-1/vulkan-guide/blob/engine/asset-baker/asset_main.cpp

The converter is used by giving it a target folder to process. It will then go through the files in the folder and attempt to convert them.

```cpp
fs::path path{ argv[1] };

fs::path directory = path;

std::cout << "loading asset directory at " << directory << std::endl;

for (auto& p : fs::directory_iterator(directory))
{
	std::cout << "File: " << p;

	if (p.path().extension() == ".png") {
			std::cout << "found a texture" << std::endl;

			auto newpath = p.path();
			newpath.replace_extension(".tx");
			convert_image(p.path(), newpath);
	}
	if (p.path().extension() == ".obj") {
			std::cout << "found a mesh" << std::endl;

			auto newpath = p.path();
			newpath.replace_extension(".mesh");
			convert_mesh(p.path(), newpath);
		}
	}
}
```

We are using the Cpp17 Filesystem library. This is so we get an easy way of iterate a folder contents. If you cant use Cpp17 then you will have to use a platform API for that.

We begin by storing argv[1] into a filesystem::path. We will then iterate the contents at that path using `directory_iterator`.
For each file in the folder, we check if the extension is `.png`, and convert it as a texture. We convert into a mesh if it's a `.obj` file.
Textures will be stored as `.tx`, and meshes as `.mesh`.

The code inside the conversion function is copypasted from the code we used to have in the main engine. It works exactly the same, except instead of loading it into a buffer for the gpu, we use the asset library to store it to disk. We will be looking at the texture one, as again, the mesh is similar and you can look at the implementation.


```cpp
bool convert_image(const fs::path& input, const fs::path& output)
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(input.u8string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		std::cout << "Failed to load texture file " << input << std::endl;
		return false;
	}

	int texture_size = texWidth * texHeight * 4;

	TextureInfo texinfo;
	texinfo.textureSize = texture_size;
	texinfo.pixelsize[0] = texWidth;
	texinfo.pixelsize[1] = texHeight;
	texinfo.textureFormat = TextureFormat::RGBA8;
	texinfo.originalFile = input.string();
	assets::AssetFile newImage = assets::pack_texture(&texinfo, pixels);


	stbi_image_free(pixels);

	save_binaryfile(output.string().c_str(), newImage);

	return true;
}
```

We will use stb_image library to load the pixel data and the texture format/sizes.
We then fill a TextureInfo struct with the size and format information of the texture.

We then pack the pixel data of the texture into a new AssetFile, and save the asset file.

With that, we can now convert `.png` files into a directly-loadable pixel format. The speed gain from this is quite big, as `.png` format is very slow to load.


## Loading
With the files converted, last step is to be able to load this sort of asset to the engine.

```cpp

bool vkutil::load_image_from_asset(VulkanEngine& engine, const char* filename, AllocatedImage& outImage)
{
	assets::AssetFile file;
	bool loaded = assets::load_binaryfile(filename, file);

	if (!loaded) {
		std::cout << "Error when loading image\n";
		return false;
	}

	assets::TextureInfo textureInfo = assets::read_texture_info(&file);

	VkDeviceSize imageSize = textureInfo.textureSize;
	VkFormat image_format;
	switch (textureInfo.textureFormat) {
	case assets::TextureFormat::RGBA8:
		image_format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	default:
		return false;
	}

	AllocatedBuffer stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);

	assets::unpack_texture(&textureInfo, file.binaryBlob.data(), file.binaryBlob.size(), (char*)data);

	vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);

	outImage = upload_image(textureInfo.pixelsize[0], textureInfo.pixelsize[1], image_format, engine, stagingBuffer);

	vmaDestroyBuffer(engine._allocator, stagingBuffer._buffer, stagingBuffer._allocation);

	return true;
}
```

This is all we need to load from the file. We begin by loading the asset file itself, then we parse texture info, and then we unpack the texture pixels directly into the staging buffer that will be used to upload the texture. You can find this code on https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/vk_textures.cpp

For mesh data, go look at the source code. The general flow is the same as the texture loading, but with the slightly different data. This asset system is very easy to expand, as you can keep creating new types of assets, and adding more formats to mesh/texture asset themselves.

Another thing that would be possible to do in a system like this is to make it able to load pure-json files, that are on text format. They could point into another file for the blob data or just hold no blob data. This way they would be easier to edit by people.


{% include comments.html term="Asset System Comments" %}







