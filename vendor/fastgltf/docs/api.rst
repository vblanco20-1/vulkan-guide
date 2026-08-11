***
API
***

.. contents:: Table of Contents

glTF enumerations
=================

AccessorType
------------

.. doxygenenum:: fastgltf::AccessorType

.. doxygenfunction:: fastgltf::getNumComponents

.. doxygenfunction:: fastgltf::getElementRowCount

.. doxygenfunction:: fastgltf::isMatrix

.. doxygenfunction:: fastgltf::getElementByteSize

.. doxygenfunction:: fastgltf::getAccessorType

.. doxygenfunction:: fastgltf::getAccessorTypeName

ComponentType
-------------

.. doxygenenum:: fastgltf::ComponentType

.. doxygenfunction:: fastgltf::getComponentByteSize

.. doxygenfunction:: fastgltf::getComponentBitSize

.. doxygenfunction:: fastgltf::getGLComponentType

.. doxygenfunction:: fastgltf::getComponentType

PrimitiveType
-------------

.. doxygenenum:: fastgltf::PrimitiveType

Filter
------

.. doxygenenum:: fastgltf::Filter

Wrap
----

.. doxygenenum:: fastgltf::Wrap

BufferTarget
------------

.. doxygenenum:: fastgltf::BufferTarget

MimeType
--------

.. doxygenenum:: fastgltf::MimeType

AnimationInterpolation
----------------------

.. doxygenenum:: fastgltf::AnimationInterpolation

AnimationPath
-------------

.. doxygenenum:: fastgltf::AnimationPath

AlphaMode
---------

.. doxygenenum:: fastgltf::AlphaMode

MeshoptCompressionMode
----------------------

.. doxygenenum:: fastgltf::MeshoptCompressionMode

MeshoptCompressionFilter
------------------------

.. doxygenenum:: fastgltf::MeshoptCompressionFilter

LightType
---------

.. doxygenenum:: fastgltf::LightType


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


Reading & Writing
=================

This section contains all types that one requires to read or write a glTF file using fastgltf.
This includes the Parser class, Exporter class, options, and data buffers.


Error
-----

.. doxygenenum:: fastgltf::Error


Extensions
----------

.. doxygenenum:: fastgltf::Extensions

.. doxygenfunction:: fastgltf::stringifyExtension

.. doxygenfunction:: fastgltf::stringifyExtensionBits

Category
--------

.. doxygenenum:: fastgltf::Category


.. _options:

Options
-------

.. doxygenenum:: fastgltf::Options


.. _exportoptions:

ExportOptions
-------------

.. doxygenenum:: fastgltf::ExportOptions


Expected
--------

.. doxygenclass:: fastgltf::Expected
   :members:
   :undoc-members:


GltfDataGetter
--------------

.. doxygenclass:: fastgltf::GltfDataGetter
   :members:
   :undoc-members:


GltfDataBuffer
--------------

.. doxygenclass:: fastgltf::GltfDataBuffer
   :members:
   :undoc-members:


GltfFileStream
--------------

.. doxygenclass:: fastgltf::GltfFileStream
   :members:
   :undoc-members:


Parser
------

.. doxygenfunction:: fastgltf::determineGltfFileType

.. doxygenclass:: fastgltf::Parser
   :members:
   :undoc-members:

.. doxygenstruct:: fastgltf::BufferInfo
   :members:


Exporter
--------

.. doxygenclass:: fastgltf::Exporter
   :members:
   :undoc-members:

.. doxygenclass:: fastgltf::FileExporter
   :members:
   :undoc-members:


Math
====

**fastgltf** includes a small math library with everything necessary for working with glTF assets.

Vector
------

.. doxygenclass:: fastgltf::math::vec
   :members:
   :undoc-members:

.. doxygenfunction:: fastgltf::math::dot

.. doxygenfunction:: fastgltf::math::cross

.. doxygenfunction:: fastgltf::math::length

.. doxygenfunction:: fastgltf::math::normalize

Quaternion
----------

.. doxygenclass:: fastgltf::math::quat
   :members:
   :undoc-members:

.. doxygenfunction:: fastgltf::math::asMatrix

Matrix
------

.. doxygenclass:: fastgltf::math::mat
   :members:
   :undoc-members:

.. doxygenfunction:: fastgltf::math::translate

.. doxygenfunction:: fastgltf::math::scale

.. doxygenfunction:: fastgltf::math::rotate

.. doxygenfunction:: fastgltf::math::transpose

.. doxygenfunction:: fastgltf::math::decomposeTransformMatrix


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
