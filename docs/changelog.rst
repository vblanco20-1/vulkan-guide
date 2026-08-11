*********
Changelog
*********

.. contents:: Table of contents

To view the full changelogs for each release please see the `GitHub releases <https://github.com/spnda/fastgltf/releases>`_.

0.9.0
=====

- An incredible amount of validation fixes, internal parsing/exporting fixes, and general code improvements
- Add support for more extensions
    - KHR_draco_mesh_compression (`#66 <https://github.com/spnda/fastgltf/issues/66>`_)
    - KHR_physics_rigid_bodies (`#95 <https://github.com/spnda/fastgltf/pull/95>`_)
    - GODOT_single_root (`#100 <https://github.com/spnda/fastgltf/pull/100>`_)
    - KHR_materials_diffuse_transmission
- Add: More support for DXMath vectors/matrices
- Add: C++23 monadic operators on custom OptionalWithFlagValue
- Add: ``IterableAccessor`` satisfies ``ranges::input_range``
- Add: Custom accessor bound type for compile-time type checks
- Fix: Guarantee floating point round-trip precision while parsing/exporting (`#88 <https://github.com/spnda/fastgltf/issues/88>`_)
- Fix: Detect armv8 crc32 programmatically on Apple platforms
- Fix: Use std::byte correctly in Android implementation
- Fix: Make vectors and matrices trivially copyable
- Fix: Validate GLB file/chunk byte length

... and quite a few more various minor improvements and fixes!

0.8.0
=====

- Add: GltfFileStream and rewritten GltfDataBuffer (`#49 <https://github.com/spnda/fastgltf/pull/49>`_)
    - The interface now uses factory constructors, and allows for custom methods for providing the glTF. See documentation for more details.
    - There is also a ``GltfFileStream`` class which wraps a ``std::ifstream`` which can potentially increase memory loading speeds.
    - This allows support for memory mapped files on platforms which provide this functionality using ``MappedGltfFile``.
- Add: Integrated math library (@spnda)
    - All colors and the node transform properties now use vectors/matrices from the built-in library. This library only covers basic maths, and just about everything one needs to work with glTF. There will be additions in the near future for adding more functionality. I've tried not to include too much bloat, but the header might still be a little template-heavy.
    - This change does not require any changes to old code, since the types are essentially just wrappers around ``std::array``. And code such as ``glm::make_vec3(pTranslation->data())`` will continue to work as expected.
- Add: Support C++20 modules (@stripe2933, @spnda, `#61 <https://github.com/spnda/fastgltf/pull/61>`_)
    - Initial revision of a fastgltf.ixx [named module](https://en.cppreference.com/w/cpp/language/modules), which is accessible through CMake with the ``fastgltf::module`` target. This feature is still somewhat experimental, so please report any issues you find.
- Add: Load external files from an APK (`#15 <https://github.com/spnda/fastgltf/pull/15>`_)
    - The interface for letting fastgltf interact with Android assets has fully changed. See documentation for more details.
- Add: Documentation on how to use specific features
    - The 'Guides' subsection now includes short documentation about various features which might not be directly obvious how to use. This also covers the recent changes to ``GltfDataBuffer`` and the Android interface.
- Add: Support for ``KHR_accessor_float64``
- Add: Support for std::float{32,64}_t with accessor tools
- Add: Predefined element traits for DirectXMath types
- Change: Only pass buffer view index to data adapter in tools
    - The ``BufferDataAdapter`` interface now takes a buffer view index instead of the ``Buffer`` reference. Older custom functors will need upgrading.
- Change: Reduce ComponentType to 16 bits & AccessorType to 8 bits.
- Change: Use ``std::invoke`` in the tools header to be compatible with more function wrappers.
- Change: Make Buffer data ``std::byte`` instead of ``std::uint8_t``
- Fix: The ``EXT_meshopt_compression`` wasn't parsed correctly.
    - The test suite around extensions has been expanded to cover more edge cases, and bugs have been fixed accordingly.
- Various fixes and improvements to the Exporter interface
    - Fix #51: Normalize resource paths & always use forward slashes
    - Fix #55: Cache positionCount & use correct buffer size
    - Fix #54: Use correct field name for primitive topology
    - Fix #53: Let FileExporter create target directories
    - Fix #52: Use const references to GLB buffer sources
    - Fix: Typo in wrapT field name (`#56 <https://github.com/spnda/fastgltf/pull/56>`_)
    - Fix: Bad chunk padding for GLB export (`#57 <https://github.com/spnda/fastgltf/pull/57>`_)
    - Add: Animation support to Exporter (`#64 <https://github.com/spnda/fastgltf/pull/64>`_)
- Fix: Guard usages of throw (`#50 <https://github.com/spnda/fastgltf/pull/50>`_)
- Fix: Accessor's component type is forced (`#59 <https://github.com/spnda/fastgltf/pull/59>`_)
- Fix: Validate UTF-8 for exported JSON strings
- Fix: Always use exceptionless ``std::filesystem`` API
- Fix: Always write/read GLB data in little-endian
- Fix *many* different compiler and clang-tidy warnings

0.7.2
=====

*This release only contains backported fixes from the v0.8 release.*

0.7.1
=====
- Add: Support for **glTF extras**
- Add: ``KHR_materials_variants``
- Fix: Inline ``deserializeComponent`` template specializations (#47)
- Fix #48: Allow exporting ``ByteView`` in GLB & catch file write errors
- Fix: Support ``KHR_materials_dispersion`` when exporting
- Fix: Decode percents when loading local files
- Fix: Set ``MimeType::None`` in all sources
- Fix: Use correct error variable when parsing gpu instancing extension
- Fix: Make sure gltf buffer is valid before determining type
- Fix: Allow the GLB BIN chunk to be empty

0.7.0
=====
- Add: glTF Exporter (`#33 <https://github.com/spnda/fastgltf/pull/33>`_)
- Add: ``MSFT_packing`` texture extensions
- Add: ``KHR_materials_dispersion`` extension
- Add: More validation around byteOffsets & byteStride
- Add `#42 <https://github.com/spnda/fastgltf/issues/42>`_: Set default values in struct initializers
- Add: Validation for correctly enabled material extensions
- Change: Combine ``loadGLTF`` and ``loadBinaryGLTF``
- Change: Move ``TRS`` struct out of Node
- Change: Use custom ``StaticVector`` for large (buffer) allocations
- Change `#35 <https://github.com/spnda/fastgltf/issues/35>`_: Remove fastgltf_simdjson target
- Change `#45 <https://github.com/spnda/fastgltf/issues/45>`_: Assert when using accessor tools with unloaded buffers
- Fix `#38 <https://github.com/spnda/fastgltf/issues/38>`_: Switch documentation of rotation and uvOffset
- Fix: ``EXT_meshopt_compression`` used wrong json object
- Fix: Correctly load iridescence texture infos
- Fix `#46 <https://github.com/spnda/fastgltf/issues/46>`_: Make an animation channels' node index optional
- Fix: Always read accessor data as little-endian
- Fix: Support padded matrix accessor data
- Fix: Remove MeshoptCompressionMode::None
- Fix: Also use unreachable macro on Clang
- Fix: Use prefixed CMake variables
- Fix: Only enable ``-Og`` fix for MINGW
- Fix: Don't needlessly parse entire data URIs

0.6.1
=====

- Add: Option to disable polymorphic allocator
- Add: Option to use 64-bit floats for parsing
- Fix #34: Support for fallback buffers with ``EXT_meshopt_compression``
- Fix: Support old GCC ABI
- Fix: Automatically disable polymorphic allocators on non-supporting compilers

0.6.0
=====

- Add: Doxygen & Sphinx online documentation (https://spnda.github.io/fastgltf/)
- Add: Custom optimised ``Optional`` class
- Add: Accessor iterators
- Add: Header with ElementTraits for glm types
- Add: ``GenerateMeshIndices`` option
- Added support for ``EXT_mesh_gpu_instancing`` by @DragonJoker in `#30 <https://github.com/spnda/fastgltf/pull/30>`_
- Added support for ``KHR_materials_pbrSpecularGlossiness`` by @DragonJoker in `#31 <https://github.com/spnda/fastgltf/pull/31>`_
- Add: ``KHR_materials_anisotropy``
- Add: Error-to-string functions
- Add: ``iterateAccessorWithIndex``
- Add: Support normalized accessors in accessor tools
- Add: Polymorphic allocator support for SmallVector
- Change: Combine ``Parser`` and ``glTF`` class
- Change: Use individual image indices for each texture extension
- Change: Use linear polymorphic allocator
- Change: Replaced unordered_map with small_vector for primitive attributes
- Fix: Mark findSparseIndex as inline
- Fix: Properly supported UTF-8 strings & URIs
- Fix: Add missing 'strength' field for occlusion textures
- Fixed various issues with URI parsing & introduced URIView
- Fixed various issues with ``SmallVector``
- Fixed spot light cone angles not being loaded by @JuanDiegoMontoya in `#27 <https://github.com/spnda/fastgltf/pull/27>`_
- Silenced unused parameter warning by @JuanDiegoMontoya in `#23 <https://github.com/spnda/fastgltf/pull/23>`_
- Fixed narrowing conversion warnings by @JuanDiegoMontoya in `#24 <https://github.com/spnda/fastgltf/pull/24>`_
- Fixed multiple typos in documentation

0.5.0
=====

- Add: Android file utilities by @DethRaid in `#14 <https://github.com/spnda/fastgltf/pull/14>`_
- Add: Utilities to extract data from accessors by @forenoonwatch in `#20 <https://github.com/spnda/fastgltf/pull/20>`_
- Add: URI parsing
- Add: Accessor min/max
- Add: ``KHR_materials_specular``, ``KHR_materials_ior``, ``KHR_materials_iridescence``, ``KHR_materials_volume``, ``KHR_materials_transmission``, ``KHR_materials_clearcoat``, ``KHR_materials_emissive_strength``, ``KHR_materials_sheen``, ``KHR_materials_unlit``
- Change: Add ``LoadExternalImages``
- Change: Move headers to dedicated include folder
- Change: Represent GLB buffers using a span
- Change: Rename SparseAccessor fields
- And many other various fixes

0.4.0
=====

- Add: Custom base64 decode callbacks
- Change: Rework DataSource to use std::variant
- Change: Remove ``Options::DontUseSIMD``
- Change: Don't always check if the given directory is valid
- Change: Use std:: prefixed integer types
- Fix: Avoid segfault with MinGW
- Fix: Rework Category enum due to parsing bug
- Fix: More C++ version checks
- Fix: Avoid dangling pointer to GLB bytes
- Fix: Rare overflow warning due to signed-by-default char
- Fix: Various minor CMake issues
- Bump simdjson to 3.1.6
- Const-ify ``GltfBufferData::copyBytes`` by @Eearslya in `#10 <https://github.com/spnda/fastgltf/pull/10>`_

0.3.0
=====

- Add: ``KHR_lights_punctual``
- Add: ``EXT_texture_webp``
- Add: glTF and GLB detection
- Add: Optional ``SmallVector`` optimisation
- Add: Asset information
- Add: Morph targets & sparse accessors
- Add: Optionally minimise JSON before parsing
- Change: Cache cpuid calls for base64 decoding
- Fix: Invalid C++20 concept
- Fix: Loosen ARM64 detection for more platforms
- Bump simdjson from 3.0.0 to 3.1.0

0.2.0
=====

- Add ``KHR_mesh_quantization`` by @Eearslya in `#2 <https://github.com/spnda/fastgltf/pull/2>`_
- Add material parameters and fix defaults by @Eearslya in `#3 <https://github.com/spnda/fastgltf/pull/3>`_
- Add: ``glTF::validate`` function
- Add: Custom buffer memory allocator callbacks
- Add: Ability to decompose transformation matrices
- Fix: Set dataLocation for images with buffer view
- Fix Base64 decoding for + and / by @Eearslya in `#4 <https://github.com/spnda/fastgltf/pull/4>`_
- Perf: Avoid (large) allocations in base64 decoders
- Perf: Always move vectors if possible
- Change: Expose more raw base64 decode functions
- Change: New parse API
- Change: Use ``std::variant`` over raw unions
- Change: Use of concepts in headers if C++20 is used
