*******
Options
*******

.. contents:: Table of Contents

CMake options
=============

All of the following CMake options simply define the corresponding macro for compilation.
Therefore, when using another build system these compile definitions can simply be set to 0 or 1,
and the fastgltf headers also define a fallback if undefined.
However, you should not define any of those macros within your source files, since that would break compilation and/or linking.

``FASTGLTF_USE_64BIT_FLOAT``
----------------------------

Instead of using a normal 32-bit ``float`` for all of the types the glTF spec refers to as ``num``,
this option instead makes fastgltf use 64-bit ``double``.


``FASTGLTF_USE_CUSTOM_SMALLVECTOR``
-----------------------------------

While fastgltf uses its custom ``SmallVector`` class in various areas by default, it might be useful to enable it in more places.


``FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL``
---------------------------------------

fastgltf by default comes with a custom memory allocator which makes use of ``std::pmr`` functionality.
This allocator allocates fixed-size blocks of memory as needed and divides them up for all heap allocations fastgltf performs.
All of this functionality can be disabled using this flag.
All types will then be normal ``std`` containers and use standard heap allocation with new and malloc.

``FASTGLTF_COMPILE_AS_CPP20``
-----------------------------

This ``BOOL`` option controls the C++ standard the library is compiled as. When ``NO`` fastgltf is always compiled as C++17.
When ``YES`` fastgltf is compiled as C++20, including the tests. This might allow the compiler to perform certain optimisations,
since fastgltf then uses some specialized stdlib functions instead.

``FASTGLTF_ENABLE_CPP_MODULES``
-------------------------------

When this setting is ``YES`` the ``fastgltf::module`` target will be exposed, as long as the CMake script detects a compiler which supports modules.
Note that this is currently not supported by all compilers and generators.
See `the CMake docs <https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html>`_ for more details.

``FASTGLTF_USE_STD_MODULE``
---------------------------

This ``BOOL`` option makes the fastgltf C++ module import the std module, which is available since C++23.
This can have further compiler time benefits, but is only experimentally supported with CMake 3.30 or newer, and only with some toolchains.
Therefore, this option should be used with caution and issues are likely.


``FASTGLTF_ENABLE_TESTS``
-------------------------

To build and run the tests and benchmarks you need to set this ``BOOL`` option to ``YES``.
When this option is set, the ``fastgltf_tests`` target will be configured.
The tests target depends on various dependencies, which will need to be downloaded before configuring CMake using ``fetch_test_deps.py``.


``FASTGLTF_ENABLE_EXAMPLES``
----------------------------

To build and run the examples you need to this ``BOOL`` option to ``YES``.
When this option is set, all targets from examples will be configured.
The CMake targets depend on various dependencies, which will need to be downloaded before configuring CMake using ``fetch_test_deps.py``.


``FASTGLTF_ENABLE_DOCS``
------------------------

.. _doxygen: https://https://www.doxygen.nl/
.. _sphinx: https://github.com/sphinx-doc/sphinx
.. _breathe: https://github.com/breathe-doc/breathe

This ``BOOL`` option controls whether the targets and commands related to the documentation should be configured.
Setting this to ``YES`` requires `Doxygen`_, `Sphinx`_ and `breathe`_ to be installed.


``FASTGLTF_ENABLE_GLTF_RS``
---------------------------

.. _corrosion: https://github.com/corrosion-rs/corrosion/
.. _gltf-rs: https://github.com/gltf-rs/gltf

When this ``BOOL`` option is set to ``YES`` fastgltf will use `corrosion`_, which is downloaded using ``fetch_test_deps.py``,
to link against the `gltf-rs`_ Rust library for comparison within the benchmarks.
Note that this option has no effect when ``FASTGLTF_ENABLE_TESTS`` is set to ``NO``.

``FASTGLTF_ENABLE_IMPLICIT_SHAPES``
--------------------------------------------

This option enables support for the draft KHR_implicit_shapes <https://github.com/KhronosGroup/glTF/pull/2370> extension

``FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES``
--------------------------------------------

This option enables support for the draft KHR_physics_rigid_bodies <https://github.com/KhronosGroup/glTF/pull/2424> extension

KHR_physics_rigid_bodies relies on KHR_implicit_shapes, so this option implicitly enables `FASTGLTF_ENABLE_IMPLICIT_SHAPES`

Parsing options
===============

For more information about the options when parsing a file, see :ref:`the API reference<options>`.


Exporting options
=================

For more information about the options when exporting a file, see :ref:`the API reference<exportoptions>`.
