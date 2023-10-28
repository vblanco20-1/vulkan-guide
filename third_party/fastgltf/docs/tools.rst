**************
Accessor tools
**************

.. contents:: Table of Contents

fastgltf provides a utility header for working with accessors. The header contains various functions
and utilities for reading, copying, and converting accessor data. All of these tools also directly
support sparse accessors to help add support for these without having to understand how they work.
This header was written by `forenoonwatch <https://github.com/forenoonwatch>`_ with the help of
`Eearslya <https://github.com/Eearslya>`_ and me.

All related functions are templated and take ``T`` as an argument.
This type has to be  have a ``ElementTraits`` specialization, which provides information about the
vector properties and data properties.
Using this information, fastgltf can convert the accessor data into your preferred format.
For example, ``glm::vec3`` would be a vector of 3 floats, which would be defined like this:

.. code:: c++

   template <>
   struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};

Note that, for glm types, there is a header with all pre-defined types shipped with fastgltf: ``fastgltf/glm_element_traits.hpp``.
This header includes the ElementTraits definition for all relevant glm types.


.. warning::

   Note that, by default, these functions will only be able to load from buffers where the source is either a ``sources::ByteView`` or a ``sources::Vector``.
   For other data sources, you'll need to provide a functor similar to the already provided ``DefaultBufferDataAdapter`` to the last parameter of each function.
   For more detailed documentation about this see :ref:`this section <bufferdataadapter>`.

getAccessorElement
==================

This function can be used to retrieve a single element from an accessor using an index.
It handles sparse accessors and can properly convert the type.

.. doxygenfunction:: getAccessorElement(const Asset& asset, const Accessor& accessor, size_t index, const BufferDataAdapter& adapter) -> ElementType


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

.. doxygenfunction:: iterateAccessorWithIndex(const Asset &asset, const Accessor &accessor, Functor &&func, const BufferDataAdapter &adapter) -> void


copyFromAccessor
================

This function essentially does a ``memcpy`` on the contents of the accessor data.
In cases where the `ElementType` is default-constructible, and the accessor type allows direct copying, this performs a direct ``memcpy``.
Otherwise, this function properly respects normalization and sparse accessors while copying and converting the data.

.. doxygenfunction:: copyFromAccessor(const Asset &asset, const Accessor &accessor, void *dest, const BufferDataAdapter &adapter = {}) -> void


Accessor iterators
==================

fastgltf also provides C++ iterators over accessor data to support the syntactic sugar of C++11's range-based for-loops.
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
By default, fastgltf provides a ``DefaultBufferDataAdapter`` struct.
The accessor functions also default to using this class,
however it is important to note that this default interface only works with buffers or images that have a ``sources::Vectors`` or a ``sources::ByteView`` in the ``DataSource`` member.

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
   fastgltf::copyFromAccessor(asset.get(), accessor, accessorData.data(), [&](const fastgltf::Buffer& buffer) const {
       return fileBytes.data();
   });
