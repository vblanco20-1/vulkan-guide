# Tests

These tests cover the glTF parser and other crucial parts of the library like custom containers
and other tools functionality. The tests are written in C++17 using the
[Catch2](https://github.com/catchorg/Catch2) framework.

## Required files

The tests use various real-world assets from the  [glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models/)
repository to test if the parser correctly maps data to the structs. The tests
expect a copy of the aforementioned repository to be in the `tests/gltf/glTF-Sample-Models`
folder. This can be a simple clone or a symlink to another copy you have locally.

The tests for KHR_implicit_shapes and KHR_physics_rigid_body use various assets from the 
[glTF_Physics](https://github.com/eoineoineoin/glTF_Physics) repository. The 
tests expect a copy of that repo to be in `tests/gltf/glTF_Physics` - as with the other sample
assets, you may use a symlink if you so choose

You can also directly change the paths where the test application searches by modifying
`gltf_path.hpp` in this directory. It uses source-file-relative paths by default,
regardless of where the binary is stored.

## Optional benchmark-specific assets

When running the benchmarks there are additional files you need to run all of the benchmarks.
Specifically, [Intel's Sponza](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html)
and [Amazon's Bistro](https://developer.nvidia.com/orca/amazon-lumberyard-bistro), which need to be
placed in `tests/gltf/intel_sponza` and `tests/gltf/bistro`, respectively.
The same rules mentioned in the previous paragraph apply to these.

## Building & running the tests

To build the tests, one first needs to configure the CMake project with `FASTGLTF_ENABLE_TESTS` set
to `ON`. The CMake script will automatically fetch `Catch2` and other required libraries. Finally,
simply calling the following command will build the target with all tests:
```
cmake --build . --target fastgltf_tests
```

If you're using Visual Studio you may need to use this command instead:
```
cmake --build . --target tests/fastgltf_tests
```

**fastgltf** uses the `Catch2` test framework, which can take various command-line parameters when running.
To simply run all tests, one can simply run the following command from the build directory.
```
tests/fastgltf_tests -d yes --order lex ~[gltf-benchmark]
```

This will run all tests except those tagged as `gltf-benchmark`. You can fine-grain your tests using the
various tags the tests use. More information on running Catch2 tests can be found [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md).
