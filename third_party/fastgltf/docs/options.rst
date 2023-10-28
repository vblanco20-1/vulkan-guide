*******
Options
*******

.. contents:: Table of Contents

CMake options
=============

FASTGLTF_DOWNLOAD_SIMDJSON
--------------------------

A ``BOOL`` option that tells fastgltf's CMake script whether it should download the simdjson sources itself.

* If set to ``YES`` the script will automatically download the amalgamated simdjson header and source files and link them into fastgltf.
* If set to ``NO`` the CMake script expects a ``simdjson::simdjson`` target to exist against which it tries to link.
  This target needs to also use the amalgamated simdjson.h header.


FASTGLTF_USE_CUSTOM_SMALLVECTOR
-------------------------------

While fastgltf uses its custom ``SmallVector`` class in various areas by default, it might be useful to enable it in more places.


FASTGLTF_ENABLE_TESTS
---------------------

To build and run the tests and benchmarks you need to set this ``BOOL`` option to ``YES``.
When this option is set, the ``fastgltf_tests`` target will be configured.
The tests target depends on various dependencies, which will need to be downloaded before configuring CMake using ``fetch_test_deps.py``.


FASTGLTF_ENABLE_EXAMPLES
------------------------

To build and run the examples you need to this ``BOOL`` option to ``YES``.
When this option is set, all targets from examples will be configured.
The CMake targets depend on various dependencies, which will need to be downloaded before configuring CMake using ``fetch_test_deps.py``.


FASTGLTF_ENABLE_DOCS
--------------------

.. _doxygen: https://https://www.doxygen.nl/
.. _sphinx: https://github.com/sphinx-doc/sphinx
.. _breathe: https://github.com/breathe-doc/breathe

This ``BOOL`` option controls whether the targets and commands related to the documentation should be configured.
Setting this to ``YES`` requires `Doxygen`_, `Sphinx`_ and `breathe`_ to be installed.


FASTGLTF_ENABLE_GLTF_RS
-----------------------

.. _corrosion: https://github.com/corrosion-rs/corrosion/
.. _gltf-rs: https://github.com/gltf-rs/gltf

When this ``BOOL`` option is set to ``YES`` fastgltf will use `corrosion`_, which is downloaded using ``fetch_test_deps.py``,
to link against the `gltf-rs`_ Rust library for comparison within the benchmarks.
Note that this option has no effect when ``FASTGLTF_ENABLE_TESTS`` is set to ``NO``.


Parsing options
===============

For more information about the options when parsing a file, see :ref:`the API reference<options>`.
