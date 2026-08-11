**************
Accessor tools
**************

.. contents:: Table of Contents

**fastgltf** provides utility functionality for working with accessors, found in the ``fastgltf/tools.hpp`` header.
The header contains various functions and utilities for reading, copying, and converting accessor data.
All of these tools also directly support sparse accessors to help add support for these without having to understand how they work.
This functionality was written by `forenoonwatch <https://github.com/forenoonwatch>`_ with the help of `Eearslya <https://github.com/Eearslya>`_ and me.

.. warning::

   By default, these functions will only be able to load from buffers where the source is either a ``sources::ByteView``, a ``sources::Array``, or a ``sources::Vector``.
   For other data sources, you'll need to provide a functor similar to the already provided ``DefaultBufferDataAdapter`` to the last parameter of each function.
   For more detailed documentation about this see :ref:`this section <bufferdataadapter>`.

.. warning::

   When an accessor contains, for example, Vec3 data, the functions will assert when you try to specify a type which is not a vector of 3.
   That means that the type you specify in the template for ``T`` needs to have ``AccessorType::Vec3`` specified for its ``ElementTraits``.

ElementTraits
=============

All related functions are templated and take ``T`` as an argument, which allows the functions to read data directly into your data types,
while converting the data correctly, if necessary.
This type has to have a ``ElementTraits`` specialization, which provides information about the vector properties and data properties.
Using this information, **fastgltf** can convert the accessor data into your preferred format.
For example, if you had a custom vector type like ``MyVec3``, you'd just need to specialize the template for it like this:

.. code:: c++

   template <>
   struct fastgltf::ElementTraits<MyVec3> : fastgltf::ElementTraitsBase<MyVec3, AccessorType::Vec3, float> {};

.. note::

   **fastgltf** includes these definitions for types from other math libraries. Currently, **fastgltf** supports
   DirectXMath with ``fastgltf/dxmath_element_traits.hpp`` and glm with  ``fastgltf/glm_element_traits.hpp``.
   Simply including these headers will allow the tools to convert to your preferred types directly.
   These headers could also be used as a reference on how to add support for other types.

.. note::

   **fastgltf** also includes its own vector and matrix types, which can be used instead.
   These also have a ``ElementTraits`` specialization included in the standard header.

getAccessorElement
==================

This function can be used to retrieve a single element from an accessor using an index.
It handles sparse accessors and can properly convert the type.

.. doxygenfunction:: fastgltf::getAccessorElement


iterateAccessor
===============

Using ``iterateAccessor`` you can iterate over the data of an accessor using a lambda, similarly to ``std::for_each``.

.. doxygenfunction:: iterateAccessor(const Asset &asset, const Accessor &accessor, Functor &&func, const BufferDataAdapter &adapter) -> void

.. code:: c++

   fastgltf::Primitive& primitive = ...;

   std::vector<std::uint32_t> indices;
   if (primitive.indicesAccessor.has_value()) {
       auto& accessor = asset->accessors[primitive.indicesAccessor.value()];
       indices.resize(accessor.count);

       std::size_t idx = 0;
       fastgltf::iterateAccessor<std::uint32_t>(asset.get(), accessor, [&](std::uint32_t index) {
           indices[idx++] = index;
       });
   }

iterateAccessorWithIndex
========================

Functionally identical to ``iterateAccessor``, but provides you with the current index as the second parameter to the lambda.

.. doxygenfunction:: fastgltf::iterateAccessorWithIndex


copyFromAccessor
================

This function essentially does a ``memcpy`` on the contents of the accessor data.
In cases where the `ElementType` is default-constructible, and the accessor type allows direct copying, this performs a direct ``memcpy``.
Otherwise, this function properly respects normalization and sparse accessors while copying and converting the data.

.. doxygenfunction:: fastgltf::copyFromAccessor


Accessor iterators
==================

**fastgltf** also provides C++ iterators over accessor data to support the syntactic sugar of C++11's range-based for-loops.
These iterators can be obtained using ``iterateAccessor``, and can be used like so:

.. doxygenfunction:: iterateAccessor(const Asset& asset, const Accessor& accessor, const BufferDataAdapter& adapter = {}) -> IterableAccessor<ElementType, BufferDataAdapter>

.. code:: c++

   std::size_t idx = 0;
   for (auto element : fastgltf::iterateAccessor(asset.get(), accessor)) {
       array[idx++] = element;
   }


.. _bufferdataadapter:

BufferDataAdapter interface
===========================

The accessor tools acquire the binary data through this functional interface.
By default, **fastgltf** provides a ``DefaultBufferDataAdapter`` struct.
The accessor functions also default to using this class,
however it is important to note that this default interface only works with buffers or images that have a ``sources::ByteView``, a ``sources::Array``, or a ``sources::Vector`` in the ``DataSource`` member.

.. doxygenstruct:: fastgltf::DefaultBufferDataAdapter
   :members:
   :undoc-members:

If you do not provide Options::LoadExternalBuffers to the Parser while loading the glTF,
external buffers will be available as ``sources::URI`` and will not work with the ``DefaultBufferDataAdapter``.
Therefore, you'll either have to set that option or provide a custom functional interface that properly returns a pointer to the memory.

As this is a functional interface it is possible to also use lambdas for this:

.. code:: c++

   std::vector<std::byte> fileBytes;
   std::vector<std::uint8_t> accessorData(accessor.count);
   fastgltf::copyFromAccessor(asset.get(), accessor, accessorData.data(), [&](const Asset& asset, const std::size_t bufferViewIdx) const {
       const auto& bufferView = asset.bufferViews[bufferViewIdx];
       return span(fileBytes).subspan(bufferView.byteOffset, bufferView.byteLength);
   });


Example: Loading primitive positions
====================================

The following snippet illustrates how one could potentially load vertex positions for a primitive into a OpenGL buffer using the accessor tools.

.. code:: c++

   fastgltf::Primitive* primitive = ...;

   // findAttribute returns a iterator into the underlying vector of primitive attributes.
   // Note that the glTF spec requires every primitive to have a POSITION,
   // so it's perfectly valid to assert that positionIt is never nullptr.
   auto* positionIt = primitive->findAttribute("POSITION");
   auto& positionAccessor = asset.accessors[positionIt->second];
   if (!positionAccessor.bufferViewIndex.has_value())
      continue;

   // Create the vertex buffer for this primitive,
   // and use the accessor tools to copy directly into the mapped buffer.
   glCreateBuffers(1, &primitive.vertexBuffer);
   glNamedBufferData(primitive.vertexBuffer,
                     positionAccessor.count * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
   auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
   
   // Iterates over the accessor (potentially handling any sparse accessors),
   // and gives each vertex UV a default value, which need to be loaded separately.
   fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
         asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
      vertices[idx].position = pos;
      vertices[idx].uv = fastgltf::math::fvec2();
   });
