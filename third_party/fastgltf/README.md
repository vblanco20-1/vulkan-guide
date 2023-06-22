# fastgltf

A superfast SIMD powered glTF 2.0 parser written in C++17 with minimal dependencies. Unlike other
glTF parsers, it does not automatically load textures and external buffers to allow the user to
optimise to their liking. It does, however, load embedded data and also decodes base64 encoded
buffers using high speed SIMD algorithms.

By utilising simdjson, this library can take advantage of SSE4, AVX2, AVX512, and ARM Neon.

## Features

fastgltf supports glTF 2.0:
- [x] glTF JSON files
- [x] GLB binary files
- [x] Scenes, nodes, meshes
- [x] Accessors, buffer views, buffers
- [x] Materials, textures, images, samplers
- [x] Animations
- [x] Skins
- [x] Cameras
- [x] Morph targets, sparse accessors
- [ ] Extra data

fastgltf supports a number of glTF extensions:
- [x] EXT_meshopt_compression
- [x] EXT_texture_webp
- [x] KHR_lights_punctual
- [x] KHR_materials_clearcoat
- [x] KHR_materials_emissive_strength
- [x] KHR_materials_ior
- [x] KHR_materials_iridescence
- [x] KHR_materials_sheen
- [x] KHR_materials_specular
- [x] KHR_materials_transmission
- [x] KHR_materials_unlit
- [x] KHR_materials_volume
- [x] KHR_texture_basisu
- [x] KHR_texture_transform
- [x] KHR_mesh_quantization
- [x] MSFT_texture_dds

fastgltf brings many utilities:
- [x] SIMD powered base64 buffer decoder
- [x] Can decompose transform matrices into independent translation, rotation, and scale factors
- [x] Allows writing directly to mapped GPU buffers for direct uploads, reducing copies
- [x] Advanced utilities to read, copy and iterate over accessor data

## Usage

fastgltf is built using C++17 and only depends on simdjson, which is downloaded automatically in
the CMake script. The library is tested on GCC 9, GCC 10, Clang 12, and MSVC 14 (Visual Studio 2022)
using CI. The project uses a simple CMake 3.11 script, and can be simply used by adding fastgltf as
a subdirectory. Also, fastgltf is available from [vcpkg](https://github.com/microsoft/vcpkg).

Tests and examples are also available and can be built by enabling the respective options, `FASTGLTF_ENABLE_TESTS`
and `FASTGLTF_ENABLE_EXAMPLES`. fastgltf will then require a few extra dependencies for the test
framework, and OpenGL tools, which have to be downloaded by running `fetch_test_deps.py`.

When you use `simdjson` yourself or want to target a specific option, you can set `FASTGLTF_DOWNLOAD_SIMDJSON`
to `OFF`. fastgltf will now not try to download simdjson and will instead only try to link to the
`simdjson::simdjson` target.

It can sometimes also be useful to set `FASTGLTF_USE_CUSTOM_SMALLVECTOR` to `ON`. fastgltf comes
with a custom `SmallVector`. This has the same API as a `std::vector` but can store up to N elements
(the exact amount is a hardcoded template parameter) on the stack to avoid possibly expensive
allocations for only a few bytes.

```cpp
#include <fastgltf_parser.hpp>
#include <fastgltf_types.hpp>

void load(std::filesystem::path path) {
    // Creates a Parser instance. Optimally, you should reuse this across loads, but don't use it
    // across threads. To enable extensions you have to pass them into the parser's constructor.
    fastgltf::Parser parser;

    // The GltfDataBuffer class is designed for re-usability of the same JSON string. It contains
    // utility functions to load data from a std::filesystem::path, copy from an existing buffer,
    // or re-use an already existing allocation. Note that it has to outlive the process of every
    // parsing function you call.
    fastgltf::GltfDataBuffer data;
    data.loadFromFile(path);

    // This loads the glTF file into the gltf object and parses the JSON. For GLB files, use
    // fastgltf::Parser::loadBinaryGLTF instead.
    auto gltf = parser.loadGLTF(&data, path.parent_path(), fastgltf::Options::None);
    if (parser.getError() != fastgltf::Error::None) {
        // File doesn't exist, couldn't be read, or is not a valid JSON document.
    }

    // With this call to parse you let fastgltf serialize the whole JSON document into the
    // glTF data structures. If desired, you can pass OR'd category enums that will exclude
    // certain glTF aspects from being loaded.
    if (gltf->parse() != fastgltf::Error::None) {
        // Most likely the asset does not follow the glTF spec. Though perhaps fastgltf doesn't
        // handle something correctly, so please let me know.
    }
    
    // Optionally, you can now also call the glTF::validate method. This will more strictly
    // enforce the glTF spec and is not needed most of the time, though I would certainly
    // recommend it in a development environment or when debugging to avoid mishaps.

    // You obtain the asset with this call. This can only be done once, and all successive
    // calls will return a nullptr. The parser and glTF object can be destroyed safely after
    // this call.
    std::unique_ptr<fastgltf::Asset> asset = gltf->getParsedAsset();
}
```

All the nodes, meshes, buffers, textures, ... can now be accessed through the `fastgltf::Asset`
type. References in between objects are done with a single `size_t`, which is used to index into
the various vectors in the asset. fastgltf heavily utilizes standard containers like std::variant
and std::optional to enforce safety for the user.

## Performance

[spreadsheet-link]: https://docs.google.com/spreadsheets/d/1ocdHGoty-rF0N46ZlAlswzcPHVRsqG_tncy8paD3iMY/edit?usp=sharing

In the following chapter I'll show some graphs on how fastgltf compares to the two most used glTF
libraries, cgltf and tinygltf. I've disabled loading of images and buffers to only compare the
JSON parsing and serialization of the glTF data. I create these graphs using a spreadsheet that you
can find [here][spreadsheet-link].
These numbers were benchmarked using Catch2's benchmark tool on a Ryzen 5800X (with AVX2) with 32GB
of RAM using Clang 16, as Clang showed a significant performance improvement over MSVC in every test.
I would recommend every Windows developer to use Clang. Visual Studio offers ClangCL as one of the
downloadable options.

First of I compared the performance with embedded buffers that are encoded with base64. This uses
the [2CylinderEngine asset](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/2CylinderEngine)
which contains a 1.7MB embedded buffer. fastgltf includes an optimised base64 decoding algorithm
that can take advantage of AVX2, SSE4, and ARM Neon. With this asset, fastgltf is **20.56 times faster**
than tinygltf using RapidJSON and **6.5 times faster** than cgltf.

[![](https://cdn.discordapp.com/attachments/442748131898032138/1088470860333060207/Mean_time_parsing_2CylinderEngine_ms_8.png)][spreadsheet-link]

[Amazon's Bistro](https://developer.nvidia.com/orca/amazon-lumberyard-bistro) (converted to glTF
2.0 using Blender) is another excellent test subject, as it's a 148k line long JSON. This shows
the raw serialization speed of all the parsers. For this benchmark I used the `MinimiseJsonBeforeParsing`
option that lets fastgltf minimise the JSON before parsing (note that the time it takes to minimise
is measured here too). In this case fastgltf is **2.1 times faster** than tinygltf and **5.6 times
faster** than cgltf.

[![](https://cdn.discordapp.com/attachments/442748131898032138/1088470983024840754/Bistro_load_from_memory_without_images_and_buffer_load_1.png)][spreadsheet-link]

## Acknowledgments

**The major speedup comes from the awesome [`simdjson`](https://github.com/simdjson/simdjson) JSON
library which can use a multitude of different SIMD intrinsics to increase JSON decoding speed. The
library really shows off at massive JSON files, by parsing around 8GB/s.**

The SIMD-based base64 decoding algorithms come from
[Wojciech Muła](http://0x80.pl/notesen/2016-01-17-sse-base64-decoding.html#avx2-version), which can
usually nearly triple the decoding speed with AVX2.

Thanks to https://github.com/komrad36/CRC for some insight into optimising the CRC32-C algorithm and
using the SSE4 CRC instructions.

## License

The **fastgltf** library is licensed under the MIT License.

----

Libraries embedded in fastgltf:
- [simdjson](https://github.com/simdjson/simdjson): Licensed under Apache 2.0.

Libraries used in examples and tests:
- [Catch2](https://github.com/catchorg/Catch2): Licensed under BSL-1.0.
- [glad](https://github.com/Dav1dde/glad): Licensed under MIT.
- [glfw](https://github.com/glfw/glfw): Licensed under Zlib.
- [glm](https://github.com/g-truc/glm): Licensed under MIT.
