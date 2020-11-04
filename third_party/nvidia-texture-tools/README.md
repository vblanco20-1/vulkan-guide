# NVIDIA Texture Tools [![Actions Status](https://github.com/castano/nvidia-texture-tools/workflows/build/badge.svg)](https://github.com/castano/nvidia-texture-tools/actions) ![MIT](https://img.shields.io/badge/license-MIT-blue.svg) [![GitHub](https://img.shields.io/badge/repo-github-green.svg)](https://github.com/castano/nvidia-texture-tools)

The NVIDIA Texture Tools is a collection of image processing and texture 
manipulation tools, designed to be integrated in game tools and asset 
processing pipelines.

The primary features of the library are mipmap and normal map generation, format 
conversion, and DXT compression.


### How to build (Windows)

Use the provided Visual Studio 2017 solution `project/vc2017/thekla.sln`.


### How to build (Linux/OSX)

Use [cmake](http://www.cmake.org/) and the provided configure script:

```bash
$ ./configure
$ make
$ sudo make install
```


### Using NVIDIA Texture Tools

To use the NVIDIA Texture Tools in your own applications you just have to
include the following header file:

[src/nvtt/nvtt.h](https://github.com/castano/nvidia-texture-tools/blob/master/src/nvtt/nvtt.h)

And include the nvtt library in your projects. 

The following file contains a simple example that shows how to use the library:

[src/nvtt/tools/compress.cpp](https://github.com/castano/nvidia-texture-tools/blob/master/src/nvtt/tools/compress.cpp)

Detailed documentation of the API can be found at:

https://github.com/castano/nvidia-texture-tools/wiki/ApiDocumentation

