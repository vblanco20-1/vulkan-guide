/*
 * Copyright (C) 2022 - 2026 Sean Apeler
 * This file is part of fastgltf <https://github.com/spnda/fastgltf>.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef FASTGLTF_CORE_HPP
#define FASTGLTF_CORE_HPP

#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <fstream>
#include <memory>
#include <tuple>
#endif

#include <fastgltf/types.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

// fwd
#if defined(__ANDROID__)
struct AAssetManager;
#endif

namespace simdjson::dom {
    class array;
    class object;
    class parser;
} // namespace simdjson::dom

namespace fastgltf {
	FASTGLTF_EXPORT enum class Error : std::uint64_t;

	FASTGLTF_EXPORT template <typename T>
	class Expected;
} // namespace fastgltf

namespace std {
	template <typename T>
	struct tuple_size<fastgltf::Expected<T>> : std::integral_constant<std::size_t, 2> {};

	template <typename T>
	struct tuple_element<0, fastgltf::Expected<T>> { using type = fastgltf::Error; };
	template <typename T>
	struct tuple_element<1, fastgltf::Expected<T>> { using type = T; };
} // namespace std

namespace fastgltf {
    struct BinaryGltfChunk;

    enum class Error : std::uint64_t {
		None = 0,
		InvalidPath = 1, ///< The glTF directory passed to load*GLTF is invalid.
		MissingExtensions = 2, ///< One or more extensions are required by the glTF but not enabled in the Parser.
		UnknownRequiredExtension = 3, ///< An extension required by the glTF is not supported by fastgltf.
		InvalidJson = 4, ///< An error occurred while parsing the JSON.
		InvalidGltf = 5, ///< The glTF is either missing something or has invalid data.
		InvalidOrMissingAssetField = 6, ///< The glTF asset object is missing or invalid.
		InvalidGLB = 7, ///< The GLB container is invalid.
		/**
		 * A field is missing in the JSON.
		 * @note This is only used internally.
		 */
		MissingField = 8,
		MissingExternalBuffer = 9, ///< With Options::LoadExternalBuffers, an external buffer was not found.
		UnsupportedVersion = 10, ///< The glTF version is not supported by fastgltf.
		InvalidURI = 11, ///< A URI from a buffer or image failed to be parsed.
		InvalidFileData = 12, ///< The file data is invalid, or the file type could not be determined.
		FailedWritingFiles = 13, ///< The exporter failed to write some files (buffers/images) to disk.
		FileBufferAllocationFailed = 14, ///< The constructor of GltfDataBuffer failed to allocate a sufficiently large buffer.
    };

	FASTGLTF_EXPORT constexpr std::string_view getErrorName(Error error) {
		switch (error) {
			case Error::None: return "None";
			case Error::InvalidPath: return "InvalidPath";
			case Error::MissingExtensions: return "MissingExtensions";
			case Error::UnknownRequiredExtension: return "UnknownRequiredExtension";
			case Error::InvalidJson: return "InvalidJson";
			case Error::InvalidGltf: return "InvalidGltf";
			case Error::InvalidOrMissingAssetField: return "InvalidOrMissingAssetField";
			case Error::InvalidGLB: return "InvalidGLB";
			case Error::MissingField: return "MissingField";
			case Error::MissingExternalBuffer: return "MissingExternalBuffer";
			case Error::UnsupportedVersion: return "UnsupportedVersion";
			case Error::InvalidURI: return "InvalidURI";
            case Error::InvalidFileData: return "InvalidFileData";
            case Error::FailedWritingFiles: return "FailedWritingFiles";
			case Error::FileBufferAllocationFailed: return "FileBufferAllocationFailed";
			default: FASTGLTF_UNREACHABLE
		}
	}

	FASTGLTF_EXPORT constexpr std::string_view getErrorMessage(Error error) {
		switch (error) {
			case Error::None: return "";
			case Error::InvalidPath: return "The glTF directory passed to load*GLTF is invalid";
			case Error::MissingExtensions: return "One or more extensions are required by the glTF but not enabled in the Parser.";
			case Error::UnknownRequiredExtension: return "An extension required by the glTF is not supported by fastgltf.";
			case Error::InvalidJson: return "An error occurred while parsing the JSON.";
			case Error::InvalidGltf: return "The glTF is either missing something or has invalid data.";
			case Error::InvalidOrMissingAssetField: return "The glTF asset object is missing or invalid.";
			case Error::InvalidGLB: return "The GLB container is invalid.";
			case Error::MissingField: return "";
			case Error::MissingExternalBuffer: return "An external buffer was not found.";
			case Error::UnsupportedVersion: return "The glTF version is not supported by fastgltf.";
			case Error::InvalidURI: return "A URI from a buffer or image failed to be parsed.";
            case Error::InvalidFileData: return "The file data is invalid, or the file type could not be determined.";
            case Error::FailedWritingFiles: return "The exporter failed to write some files (buffers/images) to disk.";
			case Error::FileBufferAllocationFailed: return "The constructor of GltfDataBuffer failed to allocate a sufficiently large buffer.";
			default: FASTGLTF_UNREACHABLE
		}
	}

	// clang-format off
    FASTGLTF_EXPORT enum class Extensions : std::uint64_t {
        None = 0,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_texture_transform/README.md
        KHR_texture_transform = 1 << 1,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_texture_basisu/README.md
        KHR_texture_basisu = 1 << 2,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Vendor/MSFT_texture_dds/README.md
        MSFT_texture_dds = 1 << 3,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_mesh_quantization/README.md
        KHR_mesh_quantization = 1 << 4,

        // See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/EXT_meshopt_compression/README.md
        EXT_meshopt_compression = 1 << 5,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
        KHR_lights_punctual = 1 << 6,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Vendor/EXT_texture_webp/README.md
        EXT_texture_webp = 1 << 8,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_specular/README.md
        KHR_materials_specular = 1 << 9,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_ior/README.md
        KHR_materials_ior = 1 << 10,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_iridescence/README.md
        KHR_materials_iridescence = 1 << 11,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_volume/README.md
        KHR_materials_volume = 1 << 12,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_transmission/README.md
        KHR_materials_transmission = 1 << 13,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_clearcoat/README.md
        KHR_materials_clearcoat = 1 << 14,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_emissive_strength/README.md
        KHR_materials_emissive_strength = 1 << 15,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_sheen/README.md
        KHR_materials_sheen = 1 << 16,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_unlit/README.md
        KHR_materials_unlit = 1 << 17,

        // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_anisotropy/README.md
        KHR_materials_anisotropy = 1 << 18,

        // See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/EXT_mesh_gpu_instancing/README.md
        EXT_mesh_gpu_instancing = 1 << 19,

        // See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md
        KHR_materials_pbrSpecularGlossiness = 1 << 20,

		// See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Vendor/MSFT_packing_normalRoughnessMetallic/README.md
		MSFT_packing_normalRoughnessMetallic = 1 << 21,

		// See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md
		MSFT_packing_occlusionRoughnessMetallic = 1 << 22,

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_dispersion
		KHR_materials_dispersion = 1 << 23,

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_variants
		KHR_materials_variants = 1 << 24,

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_accessor_float64
		KHR_accessor_float64 = 1 << 25,

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_draco_mesh_compression
		KHR_draco_mesh_compression = 1 << 26,

#if FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES
		// See https://github.com/KhronosGroup/glTF/pull/2370
		KHR_implicit_shapes = 1 << 27,
#endif

#if FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES
		// See https://github.com/KhronosGroup/glTF/pull/2424
		KHR_physics_rigid_bodies = 1 << 28,
#endif

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/GODOT_single_root
		GODOT_single_root = 1 << 29,

		// See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_diffuse_transmission
		KHR_materials_diffuse_transmission = 1 << 30,

		// See https://github.com/KhronosGroup/glTF/pull/2410
		KHR_node_visibility = 1ULL << 31,

		// See https://github.com/KhronosGroup/glTF/pull/2422
		KHR_node_selectability = 1ULL << 32,

		// See https://github.com/KhronosGroup/glTF/pull/2426
		KHR_node_hoverability = 1ULL << 33,
    };
    // clang-format on

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Extensions, Extensions, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Extensions, Extensions, &)
	FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Extensions, Extensions, -)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Extensions, Extensions, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Extensions, Extensions, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Extensions, ~)

	FASTGLTF_EXPORT constexpr Extensions operator-(const Extensions& a, const std::underlying_type_t<Extensions>& b) noexcept {
		static_assert(std::is_enum_v<Extensions>);
		return static_cast<Extensions>(to_underlying(a) - b);
	}

    // clang-format off
    FASTGLTF_EXPORT enum class Options : std::uint64_t {
        None                            = 0,
        /**
         * This allows 5130 as an accessor component type. 5130 is the OpenGL constant GL_DOUBLE,
         * which is by default not listed as an allowed component type in the glTF spec.
         * The KHR_accessor_float64 extension offers the same functionality, so if your assets
         * require double precision floats, using that extension instead is encouraged.
         *
         * The glTF normally only allows these component types:
         * https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types
         */
        AllowDouble                     = 1 << 0,

        /**
         * This skips validating the asset field, as it is usually there and not used anyway.
         */
        DontRequireValidAssetMember     = 1 << 1,

        /**
         * Loads all the GLB buffers into CPU memory. If disabled, fastgltf will only provide
         * a byte offset and length into the GLB file, which can be useful when using APIs like
         * DirectStorage or Metal IO.
         */
		LoadGLBBuffers [[deprecated("This is now default behaviour")]]   = 1 << 3,

        /**
         * Loads all external buffers into CPU memory. If disabled, fastgltf will only provide
         * a full file path to the file holding the buffer, which can be useful when using APIs
         * like DirectStorage or Metal IO. For images, LoadExternalImages has to be explicitly
         * specified, too, if required.
         */
        LoadExternalBuffers             = 1 << 4,

        /**
         * This option makes fastgltf automatically decompose the transformation matrices of nodes
         * into the translation, rotation, and scale components. This might be useful to have only
         * TRS components, instead of matrices or TRS, which should simplify working with nodes,
         * especially with animations.
         */
        DecomposeNodeMatrices           = 1 << 5,

        /**
         * Loads all external images into CPU memory. It does not decode any texture data. Complementary
         * to LoadExternalBuffers.
         */
        LoadExternalImages              = 1 << 7,

		/**
		 * Lets fastgltf generate indices for all mesh primitives without indices. This currently
		 * does not de-duplicate the vertices. This is entirely for compatibility and simplifying the
		 * loading process.
		 */
		GenerateMeshIndices             = 1 << 8,
    };

    FASTGLTF_EXPORT enum class ExportOptions : std::uint64_t {
        None                            = 0,

        /**
         * Calls fastgltf::validate for the passed asset before writing.
         */
        ValidateAsset                   = 1 << 1,

        /**
         * Pretty-prints the outputted JSON. This option is ignored for binary glTFs.
         */
        PrettyPrintJson                 = 1 << 2,
    };
    // clang-format on

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Options, Options, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Options, Options, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Options, Options, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Options, Options, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Options, ~)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(ExportOptions, ExportOptions, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(ExportOptions, ExportOptions, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(ExportOptions, ExportOptions, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(ExportOptions, ExportOptions, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(ExportOptions, ~)

    // String representations of glTF 2.0 extension identifiers.
    namespace extensions {
        constexpr std::string_view EXT_mesh_gpu_instancing = "EXT_mesh_gpu_instancing";
        constexpr std::string_view EXT_meshopt_compression = "EXT_meshopt_compression";
        constexpr std::string_view EXT_texture_webp = "EXT_texture_webp";
		constexpr std::string_view GODOT_single_root = "GODOT_single_root";
		constexpr std::string_view KHR_accessor_float64 = "KHR_accessor_float64";
		constexpr std::string_view KHR_draco_mesh_compression = "KHR_draco_mesh_compression";
        constexpr std::string_view KHR_lights_punctual = "KHR_lights_punctual";
		constexpr std::string_view KHR_materials_anisotropy = "KHR_materials_anisotropy";
        constexpr std::string_view KHR_materials_clearcoat = "KHR_materials_clearcoat";
		constexpr std::string_view KHR_materials_diffuse_transmission = "KHR_materials_diffuse_transmission";
		constexpr std::string_view KHR_materials_dispersion = "KHR_materials_dispersion";
        constexpr std::string_view KHR_materials_emissive_strength = "KHR_materials_emissive_strength";
        constexpr std::string_view KHR_materials_ior = "KHR_materials_ior";
        constexpr std::string_view KHR_materials_iridescence = "KHR_materials_iridescence";
        constexpr std::string_view KHR_materials_sheen = "KHR_materials_sheen";
        constexpr std::string_view KHR_materials_specular = "KHR_materials_specular";
        constexpr std::string_view KHR_materials_transmission = "KHR_materials_transmission";
        constexpr std::string_view KHR_materials_unlit = "KHR_materials_unlit";
		constexpr std::string_view KHR_materials_variants = "KHR_materials_variants";
        constexpr std::string_view KHR_materials_volume = "KHR_materials_volume";
        constexpr std::string_view KHR_mesh_quantization = "KHR_mesh_quantization";
        constexpr std::string_view KHR_node_visibility = "KHR_node_visibility";
        constexpr std::string_view KHR_node_selectability = "KHR_node_selectability";
        constexpr std::string_view KHR_node_hoverability = "KHR_node_hoverability";
        constexpr std::string_view KHR_texture_basisu = "KHR_texture_basisu";
        constexpr std::string_view KHR_texture_transform = "KHR_texture_transform";
	    constexpr std::string_view MSFT_packing_normalRoughnessMetallic = "MSFT_packing_normalRoughnessMetallic";
	    constexpr std::string_view MSFT_packing_occlusionRoughnessMetallic = "MSFT_packing_occlusionRoughnessMetallic";
        constexpr std::string_view MSFT_texture_dds = "MSFT_texture_dds";
        constexpr std::string_view KHR_materials_pbrSpecularGlossiness = "KHR_materials_pbrSpecularGlossiness";

#if FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES
		constexpr std::string_view KHR_implicit_shapes = "KHR_implicit_shapes";
#endif

#if FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES
		constexpr std::string_view KHR_physics_rigid_bodies = "KHR_physics_rigid_bodies";
#endif
    } // namespace extensions

	// clang-format off
	// An array of pairs of string representations of extension identifiers and their respective enum
	// value used for enabling/disabling the loading of it. This also represents all extensions that
	// fastgltf supports and understands.
	static constexpr auto extensionStrings = to_array<std::pair<std::string_view, Extensions>>({
		{ extensions::EXT_mesh_gpu_instancing,                  Extensions::EXT_mesh_gpu_instancing },
		{ extensions::EXT_meshopt_compression,                  Extensions::EXT_meshopt_compression },
		{ extensions::EXT_texture_webp,                         Extensions::EXT_texture_webp },
		{ extensions::GODOT_single_root,                        Extensions::GODOT_single_root },
		{ extensions::KHR_accessor_float64,                     Extensions::KHR_accessor_float64 },
		{ extensions::KHR_draco_mesh_compression,               Extensions::KHR_draco_mesh_compression },
		{ extensions::KHR_lights_punctual,                      Extensions::KHR_lights_punctual },
		{ extensions::KHR_materials_anisotropy,                 Extensions::KHR_materials_anisotropy },
		{ extensions::KHR_materials_clearcoat,                  Extensions::KHR_materials_clearcoat },
		{ extensions::KHR_materials_diffuse_transmission,       Extensions::KHR_materials_diffuse_transmission },
		{ extensions::KHR_materials_dispersion,                 Extensions::KHR_materials_dispersion },
		{ extensions::KHR_materials_emissive_strength,          Extensions::KHR_materials_emissive_strength },
		{ extensions::KHR_materials_ior,                        Extensions::KHR_materials_ior },
		{ extensions::KHR_materials_iridescence,                Extensions::KHR_materials_iridescence },
		{ extensions::KHR_materials_sheen,                      Extensions::KHR_materials_sheen },
		{ extensions::KHR_materials_specular,                   Extensions::KHR_materials_specular },
		{ extensions::KHR_materials_transmission,               Extensions::KHR_materials_transmission },
		{ extensions::KHR_materials_unlit,                      Extensions::KHR_materials_unlit },
		{ extensions::KHR_materials_variants,                   Extensions::KHR_materials_variants },
		{ extensions::KHR_materials_volume,                     Extensions::KHR_materials_volume },
		{ extensions::KHR_mesh_quantization,                    Extensions::KHR_mesh_quantization },
		{ extensions::KHR_node_visibility,                      Extensions::KHR_node_visibility },
		{ extensions::KHR_node_selectability,                   Extensions::KHR_node_selectability },
		{ extensions::KHR_node_hoverability,                    Extensions::KHR_node_hoverability },
		{ extensions::KHR_texture_basisu,                       Extensions::KHR_texture_basisu },
		{ extensions::KHR_texture_transform,                    Extensions::KHR_texture_transform },
		{ extensions::MSFT_packing_normalRoughnessMetallic,     Extensions::MSFT_packing_normalRoughnessMetallic },
		{ extensions::MSFT_packing_occlusionRoughnessMetallic,  Extensions::MSFT_packing_occlusionRoughnessMetallic },
		{ extensions::MSFT_texture_dds,                         Extensions::MSFT_texture_dds },
		{ extensions::KHR_materials_pbrSpecularGlossiness,      Extensions::KHR_materials_pbrSpecularGlossiness },

#if FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES
        {extensions::KHR_implicit_shapes,						Extensions::KHR_implicit_shapes},
#endif

#if FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES
		{ extensions::KHR_physics_rigid_bodies,					Extensions::KHR_physics_rigid_bodies },
#endif
	});
	// clang-format on
	static constexpr std::size_t SUPPORTED_EXTENSION_COUNT = extensionStrings.size();

	/**
	 * Returns the name of the passed glTF extension.
	 *
	 * @note If \p extensions has more than one bit set (multiple extensions), this
	 * will return the name of the first set bit.
	 */
	FASTGLTF_EXPORT
#if FASTGLTF_CPP_20
	constexpr
#else
	inline
#endif
	std::string_view stringifyExtension(Extensions extensions) {
		// Remove everything but the rightmost bit
		extensions = extensions - (extensions & (extensions - 1));

		for (const auto& [string, value] : extensionStrings)
			if (value == extensions)
				return string;
		return "";
	}

	/**
	 * Returns a list of extension names based on the given extension flags.
	 */
	FASTGLTF_EXPORT inline auto stringifyExtensionBits(const Extensions extensions) -> decltype(Asset::extensionsRequired) {
		decltype(Asset::extensionsRequired) stringified;
		for (std::uint8_t i = 0; i < std::numeric_limits<std::underlying_type_t<Extensions>>::digits; ++i) {
			// The 1 has to be cast to the underlying type as uint8_t(1) << 9 will overflow and be effectively the same as uint8_t(1).
			auto curExtension = static_cast<Extensions>(
				static_cast<std::underlying_type_t<Extensions>>(1) << i);
			if ((extensions & curExtension) == Extensions::None)
				continue;

			// Find the stringified extension name
			for (const auto& [name, ext] : extensionStrings) {
				if (ext == curExtension) {
					stringified.emplace_back(name);
					break;
				}
			}
		}
		return stringified;
	}

	/**
	 * A type that stores an error together with an expected value.
	 * To use this type, first call error() to inspect if any errors have occurred.
	 * If error() is not fastgltf::Error::None,
	 * calling get(), operator->(), and operator*() is undefined behaviour.
	 */
	template <typename T>
	class Expected {
		static_assert(std::is_default_constructible_v<T>);
		static_assert(!std::is_same_v<Error, T>);

		Error err;
		T value;

	public:
		Expected(Error error) : err(error) {}
		Expected(T&& value) : err(Error::None), value(std::forward<T>(value)) {}

		Expected(const Expected& other) = delete;
		Expected(Expected&& other) noexcept : err(other.err), value(std::move(other.value)) {}

		Expected& operator=(const Expected& other) = delete;
		Expected& operator=(Expected&& other) noexcept {
			err = other.err;
			value = std::move(other.value);
			return *this;
		}

		[[nodiscard]] Error error() const noexcept {
			return err;
		}

		/**
		 * Returns a reference to the value of T.
		 * When error() returns anything but Error::None, the returned value is undefined.
		 */
		[[nodiscard]] T& get() noexcept {
			assert(err == Error::None);
			return value;
		}

		/**
		 * Returns the address of the value of T, or nullptr if error() returns anything but Error::None.
		 */
		[[nodiscard]] T* get_if() noexcept {
			if (err != Error::None)
				return nullptr;
			return std::addressof(value);
		}

		template <std::size_t I>
		[[nodiscard]] auto& get() noexcept {
			if constexpr (I == 0) return err;
			else if constexpr (I == 1) return value;
		}

		template <std::size_t I>
		[[nodiscard]] const auto& get() const noexcept {
			if constexpr (I == 0) return err;
			else if constexpr (I == 1) return value;
		}

		/**
		 * Returns the address of the value of T.
		 * When error() returns anything but Error::None, the returned value is undefined.
		 */
		[[nodiscard]] T* operator->() noexcept {
			assert(err == Error::None);
			return std::addressof(value);
		}

		/**
		 * Returns the address of the const value of T.
		 * When error() returns anything but Error::None, the returned value is undefined.
		 */
		[[nodiscard]] const T* operator->() const noexcept {
			assert(err == Error::None);
			return std::addressof(value);
		}

		[[nodiscard]] T&& operator*() && noexcept {
			assert(err == Error::None);
			return std::move(value);
		}

		[[nodiscard]] operator bool() const noexcept {
			return err == Error::None;
		}
	};

    FASTGLTF_EXPORT struct BufferInfo {
        void* mappedMemory;
        CustomBufferId customId;
    };

    FASTGLTF_EXPORT using BufferMapCallback = BufferInfo(std::uint64_t bufferSize, void* userPointer);
    FASTGLTF_EXPORT using BufferUnmapCallback = void(BufferInfo* bufferInfo, void* userPointer);
    FASTGLTF_EXPORT using Base64DecodeCallback = void(std::string_view base64, std::uint8_t* dataOutput, std::size_t padding, std::size_t dataOutputSize, void* userPointer);
	FASTGLTF_EXPORT using ExtrasParseCallback = void(simdjson::dom::object* extras, std::size_t objectIndex, Category objectType, void* userPointer);
	FASTGLTF_EXPORT using ExtrasWriteCallback = std::optional<std::string>(std::size_t objectIndex, Category objectType, void* userPointer);

	/**
	 * This interface defines how the parser can read the bytes making up a glTF or GLB file.
	 */
	FASTGLTF_EXPORT class GltfDataGetter {
	public:
		virtual ~GltfDataGetter() noexcept = default;

		/**
		 * The read functions expect the implementation to store an offset from the start
		 * of the buffer/file to the current position. The parse process will always linearly
		 * access the memory, meaning it will go through the memory once from start to finish.
		 */
		virtual void read(void* ptr, std::size_t count) = 0;
		/**
		 * Reads a chunk of memory from the current offset, with some amount of padding.
		 * This padding is necessary for the simdjson parser, but can be initialized to anything.
		 * The memory pointed to by the span only needs to live until the next call to read().
		 */
		[[nodiscard]] virtual span<std::byte> read(std::size_t count, std::size_t padding) = 0;

		/**
		 * Reset is used to put the offset index back to the start of the buffer/file.
		 * This is only necessary for functionality like determineGltfFileType. However, reset()
		 * will be called at the beginning of every parse process.
		 */
		virtual void reset() = 0;

		[[nodiscard]] virtual std::size_t bytesRead() = 0;
		[[nodiscard]] virtual std::size_t totalSize() = 0;
	};

	FASTGLTF_EXPORT class GltfDataBuffer : public GltfDataGetter {
	protected:
		std::unique_ptr<std::byte[]> buffer;

		std::size_t allocatedSize = 0;
		std::size_t dataSize = 0;

		std::size_t idx = 0;

		Error error = Error::None;

		void allocateAndCopy(const std::byte* bytes) noexcept;

		explicit GltfDataBuffer(const std::filesystem::path& path) noexcept;
		explicit GltfDataBuffer(const std::byte* bytes, std::size_t count) noexcept;
#if FASTGLTF_CPP_20
		explicit GltfDataBuffer(std::span<std::byte> span) noexcept;
#endif

	public:
		explicit GltfDataBuffer() noexcept = default;
		GltfDataBuffer(const GltfDataBuffer& other) = delete;
		GltfDataBuffer& operator=(const GltfDataBuffer& other) = delete;
		GltfDataBuffer(GltfDataBuffer&& other) noexcept = default;
		GltfDataBuffer& operator=(GltfDataBuffer&& other) noexcept = default;
		~GltfDataBuffer() noexcept override = default;

		static Expected<GltfDataBuffer> FromPath(const std::filesystem::path& path) noexcept {
			GltfDataBuffer buffer(path);
			if (buffer.error != Error::None) {
				return buffer.error;
			}
			return buffer;
		}

		static Expected<GltfDataBuffer> FromBytes(const std::byte* bytes, std::size_t count) noexcept {
			GltfDataBuffer buffer(bytes, count);
			if (buffer.error != Error::None) {
				return buffer.error;
			}
			return buffer;
		}

#if FASTGLTF_CPP_20
		static Expected<GltfDataBuffer> FromSpan(std::span<std::byte> data) noexcept {
			GltfDataBuffer buffer(data);
			if (buffer.buffer.get() == nullptr) {
				return buffer.error;
			}
			return buffer;
		}
#endif

		void read(void* ptr, std::size_t count) override;

		[[nodiscard]] span<std::byte> read(std::size_t count, std::size_t padding) override;

		void reset() override;

		[[nodiscard]] std::size_t bytesRead() override;

		[[nodiscard]] std::size_t totalSize() override;

		[[nodiscard]] explicit operator span<std::byte>() const {
			return span(buffer.get(), dataSize);
		}
	};

#if defined(_WIN32)
#include <winapifamily.h>
#endif

#if defined(__APPLE__) || defined(__linux__) || (defined(_WIN32) && !(defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)))
#define FASTGLTF_HAS_MEMORY_MAPPED_FILE 1
	/**
	 * Memory-maps a file. This uses mmap on macOS and Linux, and MapViewOfFile on Windows, and is not available elsewhere.
	 * You should check for FASTGLTF_HAS_MEMORY_MAPPED_FILE before using this class.
	 */
	FASTGLTF_EXPORT class MappedGltfFile : public GltfDataGetter {
		void* mappedFile = nullptr;
#if defined(_WIN32)
		// Windows requires us to keep the file handle alive. Win32 HANDLE is a void*.
		void* fileHandle = nullptr;
		void* fileMapping = nullptr;
#endif
		std::uint64_t fileSize = 0;

		std::size_t idx = 0;

		Error error = Error::None;

		explicit MappedGltfFile(const std::filesystem::path& path) noexcept;

	public:
		explicit MappedGltfFile() = default;
		MappedGltfFile(const MappedGltfFile& other) = delete;
		MappedGltfFile& operator=(const MappedGltfFile& other) = delete;
		MappedGltfFile(MappedGltfFile&& other) noexcept;
		MappedGltfFile& operator=(MappedGltfFile&& other) noexcept;
		~MappedGltfFile() noexcept override;

		/** Memory maps a file. If this fails, you can check std::strerror for a more exact error. */
		static Expected<MappedGltfFile> FromPath(const std::filesystem::path& path) noexcept {
			MappedGltfFile buffer(path);
			if (buffer.error != Error::None) {
				return buffer.error;
			}
			return buffer;
		}

		void read(void* ptr, std::size_t count) override;

		[[nodiscard]] span<std::byte> read(std::size_t count, std::size_t padding) override;

		void reset() override;

		[[nodiscard]] std::size_t bytesRead() override;

		[[nodiscard]] std::size_t totalSize() override;

		[[nodiscard]] explicit operator span<std::byte>() const {
			return span(static_cast<std::byte*>(mappedFile), fileSize);
		}
	};
#endif

	FASTGLTF_EXPORT class GltfFileStream : public GltfDataGetter {
		std::ifstream fileStream;
		std::vector<std::ifstream::char_type> buf;

		std::size_t fileSize;

	public:
		explicit GltfFileStream(const std::filesystem::path& path);
		~GltfFileStream() noexcept override = default;

		[[nodiscard]] bool isOpen() const;

		void read(void* ptr, std::size_t count) override;

		[[nodiscard]] span<std::byte> read(std::size_t count, std::size_t padding) override;

		void reset() override;

		[[nodiscard]] std::size_t bytesRead() override;

		[[nodiscard]] std::size_t totalSize() override;
	};

    #if defined(__ANDROID__)
	FASTGLTF_EXPORT void setAndroidAssetManager(AAssetManager* assetManager) noexcept;

    FASTGLTF_EXPORT class AndroidGltfDataBuffer : public GltfDataBuffer {
		explicit AndroidGltfDataBuffer(const std::filesystem::path& path, std::uint64_t byteOffset) noexcept;

    public:
        explicit AndroidGltfDataBuffer() noexcept = default;
        AndroidGltfDataBuffer(const AndroidGltfDataBuffer& other) = delete;
        AndroidGltfDataBuffer& operator=(const AndroidGltfDataBuffer& other) = delete;
        AndroidGltfDataBuffer(AndroidGltfDataBuffer&& other) noexcept = default;
        AndroidGltfDataBuffer& operator=(AndroidGltfDataBuffer&& other) noexcept = default;
        ~AndroidGltfDataBuffer() noexcept override = default;

		static Expected<AndroidGltfDataBuffer> FromAsset(const std::filesystem::path& path, std::uint64_t byteOffset = 0) noexcept {
			AndroidGltfDataBuffer buffer(path, byteOffset);
			if (buffer.buffer.get() == nullptr) {
				return buffer.error;
			}
			return buffer;
		}
	};
	#endif

	/**
	 * Enum to represent the type of a glTF file. glTFs can either be the standard JSON file with
	 * paths to buffers or with a base64 embedded buffers, or they can be in a so called GLB
	 * container format which has two or more chunks of binary data, where one represents buffers
	 * and the other contains the JSON string.
	 */
	FASTGLTF_EXPORT enum class GltfType : std::uint8_t {
		glTF,
		GLB,
		Invalid,
	};

	/**
	 * This function starts reading into the buffer and tries to determine what type of glTF container it is.
	 * This should be used to know whether to call Parser::loadGltfJson or Parser::loadGltfBinary.
	 *
	 * @note Usually, you'll want to just use Parser::loadGltf, which will call this itself.
	 *
	 * @return The type of the glTF file, either glTF, GLB, or Invalid if it was not determinable. If this function
	 * returns Invalid it is highly likely that the buffer does not actually represent a valid glTF file.
	 */
	FASTGLTF_EXPORT GltfType determineGltfFileType(GltfDataGetter& data);

	/**
	 * This function further validates all the input more strictly that is parsed from the glTF.
	 * Realistically, this should not be necessary in Release applications, but could be helpful
	 * when debugging an asset related issue.
	*/
	FASTGLTF_EXPORT [[nodiscard]] Error validate(const Asset& asset);

    /**
     * Some internals the parser passes on to each glTF instance.
     */
    struct ParserInternalConfig {
        BufferMapCallback* mapCallback = nullptr;
        BufferUnmapCallback* unmapCallback = nullptr;
        Base64DecodeCallback* decodeCallback = nullptr;
		ExtrasParseCallback* extrasCallback = nullptr;

        void* userPointer = nullptr;
        Extensions extensions = Extensions::None;
    };

    /**
     * A parser for one or more glTF files. It uses a SIMD based JSON parser to maximize efficiency
     * and performance at runtime.
     *
     * @note This class is not thread-safe.
     */
    class Parser {
        // The simdjson parser object. We want to share it between runs, so it does not need to
        // reallocate over and over again. We're hiding it here to not leak the simdjson header.
        std::unique_ptr<simdjson::dom::parser> jsonParser;

		ParserInternalConfig config = {};
		DataSource glbBuffer;
#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
		std::shared_ptr<std::pmr::monotonic_buffer_resource> resourceAllocator;
#endif
		std::filesystem::path directory;
		Options options = Options::None;

		static auto getMimeTypeFromString(std::string_view mime) -> MimeType;
		static void fillCategories(Category& inputCategories) noexcept;

		template <typename T>
		Error parseAttributes(simdjson::dom::object& object, T& attributes);

		[[nodiscard]] auto decodeDataUri(const URIView& uri) const noexcept -> Expected<DataSource>;
		[[nodiscard]] auto loadFileFromUri(URIView& uri) const noexcept -> Expected<DataSource>;
#if defined(__ANDROID__)
		[[nodiscard]] auto loadFileFromApk(const std::filesystem::path& filepath) const noexcept -> Expected<DataSource>;
#endif

		Error generateMeshIndices(Asset& asset) const;

		Error parseAccessors(const simdjson::dom::array& array, Asset& asset);
		Error parseAnimations(simdjson::dom::array& array, Asset& asset);
		Error parseBuffers(simdjson::dom::array& array, Asset& asset);
		Error parseBufferViews(const simdjson::dom::array& array, Asset& asset);
		Error parseCameras(simdjson::dom::array& array, Asset& asset);
		Error parseExtensions(const simdjson::dom::object& extensionsObject, Asset& asset);
		Error parseImages(simdjson::dom::array& array, Asset& asset);
		Error parseLights(const simdjson::dom::array& array, Asset& asset);
		Error parseMaterialExtensions(simdjson::dom::object& object, Material& material);
		Error parseMaterials(simdjson::dom::array& array, Asset& asset);
		Error parsePrimitiveExtensions(const simdjson::dom::object& object, Primitive& primitive);
		Error parseMeshes(simdjson::dom::array& array, Asset& asset);
		Error parseNodes(simdjson::dom::array& array, Asset& asset);
		Error parseSamplers(const simdjson::dom::array& array, Asset& asset);
		Error parseScenes(const simdjson::dom::array& array, Asset& asset);
		Error parseSkins(const simdjson::dom::array& array, Asset& asset);
		Error parseTextures(const simdjson::dom::array& array, Asset& asset);
#if FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES
		Error parseShapes(const simdjson::dom::array& shapes, Asset& asset);
#endif
#if FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES
		Error parsePhysicsMaterials(const simdjson::dom::array& physicsMaterials, Asset& asset);
		Error parseCollisionFilters(const simdjson::dom::array& collisionFilters, Asset& asset);
		Error parsePhysicsJoints(const simdjson::dom::array& physicsJoints, Asset& asset);

		Error parsePhysicsRigidBody(simdjson::dom::object& khr_physics_rigid_bodies, Node& node);
#endif
		Expected<Asset> parse(simdjson::dom::object root, Category categories);

    public:
        explicit Parser(Extensions extensionsToLoad = Extensions::None) noexcept;
        explicit Parser(const Parser& parser) = delete;
        Parser(Parser&& parser) noexcept;
        Parser& operator=(const Parser& parser) = delete;
        Parser& operator=(Parser&& other) noexcept;

        ~Parser();

        /**
         * Loads a glTF file from pre-loaded bytes.
         *
         * This function tries to detect wether the bytes represent a standard JSON glTF or a binary glTF.
         *
         * @return An Asset wrapped in an Expected type, which may contain an error if one occurred.
         */
        [[nodiscard]] Expected<Asset> loadGltf(GltfDataGetter& buffer, std::filesystem::path directory, Options options = Options::None, Category categories = Category::All);

        /**
         * Loads a glTF file from pre-loaded bytes representing a JSON file.
         *
         * @return An Asset wrapped in an Expected type, which may contain an error if one occurred.
         */
        [[nodiscard]] Expected<Asset> loadGltfJson(GltfDataGetter& buffer, std::filesystem::path directory, Options options = Options::None, Category categories = Category::All);

		/**
		 * Loads a glTF file embedded within a GLB container, which may contain the first buffer of the glTF asset.
		 *
         * @return An Asset wrapped in an Expected type, which may contain an error if one occurred.
		 */
		[[nodiscard]] Expected<Asset> loadGltfBinary(GltfDataGetter& buffer, std::filesystem::path directory, Options options = Options::None, Category categories = Category::All);

        /**
         * This function can be used to set callbacks so that you can control memory allocation for
         * large buffers and images that are loaded from a glTF file. For example, one could use
         * the callbacks to map a GPU buffer through Vulkan or DirectX so that fastgltf can write
         * the buffer directly to the GPU to avoid a copy into RAM first. To remove the callbacks
         * for a specific load, call this method with both parameters as nullptr before load*GLTF.
         * Using Parser::setUserPointer you can also set a user pointer to access your
         * own class or other data you may need.
         *
         * @param mapCallback function called when the parser requires a buffer to write data
         * embedded in a GLB file or decoded from a base64 URI, cannot be nullptr.
         * @param unmapCallback function called when the parser is done with writing into a
         * buffer, can be nullptr.
         * @note This is likely only useful for advanced users who know what they're doing.
         */
        void setBufferAllocationCallback(BufferMapCallback* mapCallback, BufferUnmapCallback* unmapCallback = nullptr) noexcept;

        /**
         * Allows setting callbacks for base64 decoding.
         * This can be useful if you have another base64 decoder optimised for a certain platform or architecture,
         * or want to use your own scheduler to schedule multiple threads for working on decoding individual chunks of the data.
         * Using Parser::setUserPointer you can also set a user pointer to access your own class or other data you may need.
         *
         * It is still recommended to use fastgltf's base64 decoding features as they're highly optimised
         * for SSE4, AVX2, and ARM Neon.
         *
         * @param decodeCallback function called when the parser tries to decode a base64 buffer
         */
        void setBase64DecodeCallback(Base64DecodeCallback* decodeCallback) noexcept;

		void setExtrasParseCallback(ExtrasParseCallback* extrasCallback) noexcept;

        void setUserPointer(void* pointer) noexcept;
    };

    /**
     * This converts a compacted JSON string into a more readable pretty format.
     */
    void prettyPrintJson(std::string& json);

    /**
     * Escapes a string for use in JSON.
     */
    std::string escapeString(std::string_view string);

    FASTGLTF_EXPORT template <typename T>
    struct ExportResult {
        T output;

        std::vector<std::optional<std::filesystem::path>> bufferPaths;
        std::vector<std::optional<std::filesystem::path>> imagePaths;
    };

    /**
     * A exporter for serializing one or more glTF assets into JSON and GLB forms.
     *
     * @note This does not write anything to any files. This class only serializes data
     * into memory structures, which can then be used to manually write them to disk.
     * If you want to let fastgltf handle the file writing too, use fastgltf::FileExporter.
     */
    FASTGLTF_EXPORT class Exporter {
    protected:
        Error errorCode = Error::None;
        ExportOptions options = ExportOptions::None;
		bool exportingBinary = false;

        std::filesystem::path bufferFolder = "";
        std::filesystem::path imageFolder = "";

		void* userPointer = nullptr;
		ExtrasWriteCallback* extrasWriteCallback = nullptr;

        std::vector<std::optional<std::filesystem::path>> bufferPaths;
        std::vector<std::optional<std::filesystem::path>> imagePaths;

        void writeAccessors(const Asset& asset, std::string& json);
        void writeAnimations(const Asset& asset, std::string& json);
        void writeBuffers(const Asset& asset, std::string& json);
        void writeBufferViews(const Asset& asset, std::string& json);
        void writeCameras(const Asset& asset, std::string& json);
        void writeImages(const Asset& asset, std::string& json);
        void writeLights(const Asset& asset, std::string& json);
        void writeMaterials(const Asset& asset, std::string& json);
        void writeMeshes(const Asset& asset, std::string& json);
        void writeNodes(const Asset& asset, std::string& json);
        void writeSamplers(const Asset& asset, std::string& json);
        void writeScenes(const Asset& asset, std::string& json);
        void writeSkins(const Asset& asset, std::string& json);
        void writeTextures(const Asset& asset, std::string& json);
#if FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES
		void writeShapes(const Asset& asset, std::string& json);
#endif
#if FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES
		void writePhysicsMaterials(const Asset& asset, std::string& json);
		void writeCollisionFilters(const Asset& asset, std::string& json);
		void writePhysicsJoints(const Asset& asset, std::string& json);
#endif
        void writeExtensions(const Asset& asset, std::string& json);

        std::filesystem::path getBufferFilePath(const Asset& asset, std::size_t index);
        std::filesystem::path getImageFilePath(const Asset& asset, std::size_t index, MimeType mimeType);

        std::string writeJson(const Asset& asset);

    public:
        /**
         * Sets the relative base path for buffer URIs.
         *
         * If folder.is_relative() returns false, this has no effect.
         */
        void setBufferPath(std::filesystem::path folder);
        /**
         * Sets the relative base path for image URIs.
         *
         * If folder.is_relative() returns false, this has no effect.
         */
        void setImagePath(std::filesystem::path folder);

		void setExtrasWriteCallback(ExtrasWriteCallback* callback) noexcept;

		void setUserPointer(void* pointer) noexcept;

        /**
         * Generates a glTF JSON string from the given asset.
         */
        Expected<ExportResult<std::string>> writeGltfJson(const Asset& asset, ExportOptions options = ExportOptions::None);

        /**
         * Generates a glTF binary (GLB) blob from the given asset.
         *
         * If the first buffer holds a sources::Vector, a sources::Array, a or sources::ByteView and the byte length is smaller than 2^32 (4.2GB),
         * it will be embedded into the binary. Note that the returned vector might therefore get quite large.
         */
        Expected<ExportResult<std::vector<std::byte>>> writeGltfBinary(const Asset& asset, ExportOptions options = ExportOptions::None);
    };

	/**
	 * A exporter for serializing one or more glTF files into JSON and GLB forms.
	 * This exporter builds upon Exporter by writing all files automatically to the
	 * given paths.
	 */
	FASTGLTF_EXPORT class FileExporter : public Exporter {
        using Exporter::writeGltfJson;
        using Exporter::writeGltfBinary;

	public:
        /**
         * Writes a glTF JSON string generated from the given asset to the specified target file. This will also write
         * all buffers and textures to disk using the buffer and image paths set using Exporter::setBufferPath and
         * Exporter::setImagePath.
         */
		Error writeGltfJson(const Asset& asset, const std::filesystem::path& target, ExportOptions options = ExportOptions::None);

		/**
		 * Writes a glTF binary (GLB) blob from the given asset to the specified target file. This will also write
         * all buffers and textures to disk using the buffer and image paths set using Exporter::setBufferPath and
         * Exporter::setImagePath.
         *
		 * If the first buffer holds a sources::Vector, a sources::Array, a or sources::ByteView and the byte length is smaller than 2^32 (4.2GB),
         * it will be embedded into the binary.
         *
		 * \see Exporter::writeGltfBinary
		 */
        Error writeGltfBinary(const Asset& asset, const std::filesystem::path& target, ExportOptions options = ExportOptions::None);
	};
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
