# fastgltf

**fastgltf** is a speed and usability focused glTF 2.0 parser written in modern C++17 with minimal dependencies.
It uses SIMD in various areas to decrease the time the application spends parsing and loading glTF data.
By taking advantage of modern C++17 (and optionally C++20) it also provides easy and safe access to the properties and data.

The parser supports the entirety of glTF 2.0 specification, including many extensions.
By default, fastgltf will only do the absolute minimum to work with a glTF model.
However, it brings many additional features to ease working with the data,
including accessor tools, the ability to directly write to mapped GPU buffers, and decomposing transform matrices.

To learn more about fastgltf, its features, performance and API you can read [the docs](https://spnda.github.io/fastgltf).

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
