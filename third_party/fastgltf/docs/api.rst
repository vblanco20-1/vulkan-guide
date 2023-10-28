***
API
***

.. contents:: Table of Contents

glTF structs
============

This section contains all types fastgltf provides to represent data from a glTF asset.

DataSource
----------

.. doxygentypedef:: fastgltf::DataSource

AssetInfo
---------

.. doxygenstruct:: fastgltf::AssetInfo
   :members:
   :undoc-members:


Accessor
---------

.. doxygenstruct:: fastgltf::Accessor
   :members:
   :undoc-members:


Animation
---------

.. doxygenstruct:: fastgltf::Animation
   :members:
   :undoc-members:


Buffer
------

.. doxygenstruct:: fastgltf::Buffer
   :members:
   :undoc-members:


BufferView
----------

.. doxygenstruct:: fastgltf::BufferView
   :members:
   :undoc-members:


Camera
------

.. doxygenstruct:: fastgltf::Camera
   :members:
   :undoc-members:


Image
-----

.. doxygenstruct:: fastgltf::Image
   :members:
   :undoc-members:


Light
-----

.. doxygenstruct:: fastgltf::Light
   :members:
   :undoc-members:


Material
--------

.. doxygenstruct:: fastgltf::Material
   :members:
   :undoc-members:


Mesh
----

.. doxygenstruct:: fastgltf::Mesh
   :members:
   :undoc-members:


Node
----

.. doxygenstruct:: fastgltf::Node
   :members:
   :undoc-members:


Sampler
-------

.. doxygenstruct:: fastgltf::Sampler
   :members:
   :undoc-members:


Scene
-----

.. doxygenstruct:: fastgltf::Scene
   :members:
   :undoc-members:


Skin
----

.. doxygenstruct:: fastgltf::Skin
   :members:
   :undoc-members:


Texture
-------

.. doxygenstruct:: fastgltf::Texture
   :members:
   :undoc-members:


Asset
-----

.. doxygenclass:: fastgltf::Asset
   :members:
   :undoc-members:


Parser
======

This section contains all types that one requires to load a glTF file using fastgltf.
This includes the Parser class, options, and data buffers.


Error
-----

.. doxygenenum:: fastgltf::Error


Extensions
----------

.. doxygenenum:: fastgltf::Extensions

.. doxygenfunction:: fastgltf::stringifyExtension


Category
--------

.. doxygenenum:: fastgltf::Category


.. _options:

Options
-------

.. doxygenenum:: fastgltf::Options


Expected
--------

.. doxygenclass:: fastgltf::Expected
   :members:
   :undoc-members:


GltfDataBuffer
--------------

.. doxygenfunction:: fastgltf::getGltfBufferPadding

.. doxygenclass:: fastgltf::GltfDataBuffer
   :members:
   :undoc-members:


Parser
------

.. doxygenclass:: fastgltf::Parser
   :members:
   :undoc-members:

.. doxygenfunction:: fastgltf::determineGltfFileType

.. doxygenstruct:: fastgltf::BufferInfo
   :members:


Utility
=======

This sections contains various types used by fastgltf to simplify & process glTF data,
and for types used to enhance performance & minimize memory usage.

URIView
-------

.. doxygenclass:: fastgltf::URIView
   :members:
   :undoc-members:


URI
---

.. doxygenclass:: fastgltf::URI
   :members:
   :undoc-members:


span
----

.. doxygenclass:: fastgltf::span
   :members:
   :undoc-members:


SmallVector
-----------

.. doxygenclass:: fastgltf::SmallVector
   :members:
   :undoc-members:


Optional
--------

.. doxygentypedef:: fastgltf::Optional


OptionalFlagValue
-----------------

.. doxygenstruct:: fastgltf::OptionalFlagValue
   :members:
   :undoc-members:


OptionalWithFlagValue
---------------------

.. doxygenclass:: fastgltf::OptionalWithFlagValue
   :members:
   :undoc-members:
