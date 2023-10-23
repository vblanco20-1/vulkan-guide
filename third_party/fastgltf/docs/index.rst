fastgltf
========

**fastgltf** is a speed and usability focused glTF 2.0 parser written in modern C++17 with minimal dependencies.
It uses SIMD in various areas to decrease the time the application spends parsing and loading glTF data.
By taking advantage of modern C++17 (and optionally C++20) it also provides easy and safe access to the properties and data.

The parser supports the entirety of glTF 2.0 specification, including many extensions.
By default, fastgltf will only do the absolute minimum to work with a glTF model.
However, it brings many additional features to ease working with the data,
including accessor tools, the ability to directly write to mapped GPU buffers, and decomposing transform matrices.


Indices and tables
------------------

* :doc:`overview`
* :doc:`tools`
* :doc:`options`
* :doc:`api`


.. toctree::
   :caption: Documentation
   :hidden:
   :maxdepth: 2

   overview
   tools
   options
   api
