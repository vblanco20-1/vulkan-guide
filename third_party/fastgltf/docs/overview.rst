********
Overview
********

.. contents:: Table of Contents

**fastgltf** is a speed and usability focused glTF 2.0 parser written in modern C++17 with minimal dependencies.
It uses SIMD in various areas to decrease the time the application spends parsing and loading glTF data.
By taking advantage of modern C++17 (and optionally C++20) it also provides easy and safe access to the properties and data.

The parser supports the entirety of glTF 2.0 specification, including many extensions.
By default, fastgltf will only do the absolute minimum to work with a glTF model.
However, it brings many additional features to ease working with the data,
including accessor tools, the ability to directly write to mapped GPU buffers, and decomposing transform matrices.


.. _why:

Why use fastgltf?
=================

There are many other options for working with glTF in C and C++, including the two most popular libraries tinygltf_ and cgltf_.
These have been around for years and support virtually everything you need, so why would you even switch?

.. _tinygltf: https://github.com/syoyo/tinygltf
.. _cgltf: https://github.com/jkuhlmann/cgltf

This table includes a quick overview of a comparison of the general quality-of-life features of the popular
glTF libraries.

.. list-table::
   :header-rows: 1

   * -
     - cgltf
     - tinygltf
     - fastgltf
   * - glTF 2.0 reading
     - ✔️
     - ✔️
     - ✔️
   * - glTF 2.0 writing
     - ✔️
     - ✔️
     - ❌
   * - Extension support
     - ✔️
     - 🟡¹
     - ✔️
   * - Image decoding (PNG, JPEG, ...)
     - ✔️
     - ✔️
     - ❌
   * - Built-in Draco decompression
     - ❌
     - ✔️
     - ❌
   * - Memory callbacks
     - ✔️
     - ❌
     - 🟡²
   * - Android asset functionality
     - ❌
     - ✔️
     - ✔️
   * - Accessor utilities
     - ✔️
     - ❌
     - ✔️
   * - Sparse accessor utilities
     - 🟡³
     - ❌
     - ✔️
   * - Matrix accessor utilities
     - 🟡³
     - ❌
     - ✔️
   * - Node transform utilities
     - ✔️
     - ❌
     - ✔️

¹ tinygltf does provide the JSON structure for extension data, but leaves the deserialization for you to do.
² fastgltf allows the user to allocate memory for buffers and images.
It does not provide any mechanism for controlling all the heap allocations the library performs.
³ cgltf supports sparse accessors and matrix data only with some accessor functions, but not all.

You can read more about the accessor utilities from fastgltf :ref:`here <accessor-tools>`.

fastgltf follows C++'s concept of "you don't pay for what you don't use" by only doing the absolute minimum by default.
Without specifying any options, fastgltf will only parse the specified parts of the glTF JSON.
For buffers and images, fastgltf will by default only either give you buffers,
when the buffer or image data is embedded within the glTF, or just the plain old URIs.
Still, fastgltf offers various options that will let the library load buffers and images into memory,
which can be controlled with the memory map/unmap callbacks.
These can also be used for mapping GPU buffers so that fastgltf will write or decode base64 data directly into GPU memory.

By using modern C++ features, the code that reads data and properties from the glTF becomes simpler and vastly more descriptive,
which is a big aspect of guaranteeing code-correctness.
A big factor for this improvement is the use of types which enforce certain properties about the data, like e.g. ``std::variant`` or ``std::optional``.
Compared with tinygltf, where, for example, optional values are simply represented by a boolean or a ``-1`` for indices, this is a big improvement.

The biggest difference, which may not be as relevant to everyone, is the drastic increase in deserialization speed.
In some cases, fastgltf is at least 2 times quicker than its competitors, while in others it can be as much as 20 times.
You can read more about fastgltf's performance in the :ref:`performance chapter <performance>`.


.. _usage:

Usage
=====

.. _vcpkg: https://github.com/microsoft/vcpkg
.. _conan: https://conan.io/

fastgltf is a pure C++17 library and only depends on simdjson.
By using the included CMake 3.11 script, simdjson is automatically downloaded while configuring by default.
The library is tested on GCC 9, GCC 10, Clang 13, and MSVC 14 (Visual Studio 2022) using CI.
fastgltf is also available from vcpkg_ and conan_.

The following snippet illustrates how to use fastgltf to load a glTF file.

.. code:: c++

   #include <fastgltf/parser.hpp>
   #include <fastgltf/types.hpp>

   void load(std::filesystem::path path) {
       // Creates a Parser instance. Optimally, you should reuse this across loads, but don't use it
       // across threads. To enable extensions, you have to pass them into the parser's constructor.
       fastgltf::Parser parser;

       // The GltfDataBuffer class is designed for re-usability of the same JSON string. It contains
       // utility functions to load data from a std::filesystem::path, copy from an existing buffer,
       // or re-use an already existing allocation. Note that it has to outlive the process of every
       // parsing function you call.
       fastgltf::GltfDataBuffer data;
       data.loadFromFile(path);

       // This loads the glTF file into the gltf object and parses the JSON. For GLB files, use
       // Parser::loadBinaryGLTF instead.
       // You can detect the type of glTF using fastgltf::determineGltfFileType.
       auto asset = parser.loadGLTF(&data, path.parent_path(), fastgltf::Options::None);
       if (auto error = asset.error(); error != fastgltf::Error::None) {
           // Some error occurred while reading the buffer, parsing the JSON, or validating the data.
       }

       // The glTF 2.0 asset is now ready to be used. Simply call asset.get(), asset.get_if() or
       // asset-> to get a direct reference to the Asset class. You can then access the glTF data
       // structures, like, for example, with buffers:
       for (auto& buffer : asset->buffers) {
           // Process the buffers.
       }

       // Optionally, you can now also call the fastgltf::validate method. This will more strictly
       // enforce the glTF spec and is not needed most of the time, though I would certainly
       // recommend it in a development environment or when debugging to avoid mishaps.

       // fastgltf::validate(asset.get());
   }


All the nodes, meshes, buffers, textures, ... can now be accessed through the ``fastgltf::Asset`` type.
References in between objects are done with a single ``size_t``,
which is used to index into the various vectors in the asset.


.. _accessor-tools:

Accessor tools
==============

fastgltf provides a utility header for working with accessors.
The header contains various functions and utilities for reading, copying, and converting accessor data.
All of these tools also directly support sparse accessors to help add support for these without having to understand how they work.
These utilities are meant to drastically simplify using glTF accessors and buffers.

You can learn more about this feature of fastgltf in the dedicated chapter: :doc:`tools`.
However, to give a quick overview this is a simple example of how to load the indices of a primitive:

.. code:: c++

   fastgltf::Primitive& primitive = ...;

   std::vector<std::uint32_t> indices;
   if (primitive.indicesAccessor.has_value()) {
       auto& accessor = asset->accessors[primitive.indicesAccessor.value()];
       indices.resize(accessor.count);

       fastgltf::iterateAccessorWithIndex<std::uint32_t>(
               asset.get(), accessor, [&](std::uint32_t index, std::size_t idx) {
           indices[idx] = index;
       });
   }

.. _performance:

Performance
===========

.. _spreadsheet-link: https://docs.google.com/spreadsheets/d/1ocdHGoty-rF0N46ZlAlswzcPHVRsqG_tncy8paD3iMY/edit?usp=sharing
.. _two-cylinder-engine: https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/2CylinderEngine
.. _bistro: https://developer.nvidia.com/orca/amazon-lumberyard-bistro

In this chapter, I'll show some graphs on how fastgltf compares to the two most used glTF libraries, cgltf and tinygltf.
I've disabled loading of images and buffers to only compare the JSON parsing and deserialization of the glTF data.
The values and the graphs themselves can be found in `this spreadsheet <spreadsheet-link>`_.
These numbers were benchmarked using Catch2's benchmark tool on a Ryzen 5800X (with AVX2) with 32GB of RAM using Clang 16,
as Clang showed a significant performance improvement over MSVC in every test.

First, I compared the performance with embedded buffers that are encoded with base64.
This uses the `2CylinderEngine asset <two-cylinder-engine>`_ which contains a 1.7MB embedded buffer.
fastgltf includes an optimised base64 decoding algorithm that can take advantage of AVX2, SSE4, and ARM Neon.
With this asset, fastgltf is **20.56 times faster** than tinygltf using RapidJSON and **6.5 times faster** than cgltf.

.. image:: https://cdn.discordapp.com/attachments/442748131898032138/1088470860333060207/Mean_time_parsing_2CylinderEngine_ms_8.png

`Amazon's Bistro <bistro>`_ (converted to glTF 2.0 using Blender) is another excellent test subject, as it's a 148k line long JSON.
This shows the raw deserialization speed of all the parsers.
In this case fastgltf is **2.1 times faster** than tinygltf and **5.6 times faster** than cgltf.

.. image:: https://cdn.discordapp.com/attachments/442748131898032138/1088470983024840754/Bistro_load_from_memory_without_images_and_buffer_load_1.png
