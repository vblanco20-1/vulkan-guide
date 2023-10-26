/*
 * Copyright (C) 2022 - 2023 spnda
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

#pragma once

#include <memory>
#include <tuple>

#include "types.hpp"

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
	enum class Error : std::uint64_t;

	template <typename T>
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
    class GltfDataBuffer;

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
    };

	inline std::string_view getErrorName(Error error) {
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
			default: FASTGLTF_UNREACHABLE
		}
	}

	inline std::string_view getErrorMessage(Error error) {
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
			default: FASTGLTF_UNREACHABLE
		}
	}

	// clang-format off
    enum class Extensions : std::uint64_t {
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

#if FASTGLTF_ENABLE_DEPRECATED_EXT
        // See https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md
        KHR_materials_pbrSpecularGlossiness = 1 << 20,
#endif
    };
    // clang-format on

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Extensions, Extensions, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Extensions, Extensions, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Extensions, Extensions, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Extensions, Extensions, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Extensions, ~)

    // clang-format off
    enum class Options : std::uint64_t {
        None                            = 0,
        /**
         * This allows 5130 as an accessor component type. 5130 is the OpenGL constant GL_DOUBLE,
         * which is by default not listed as an allowed component type in the glTF spec.
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
        LoadGLBBuffers                  = 1 << 3,

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
         * This option makes fastgltf minimise the JSON file before parsing. In most cases,
         * minimising it beforehand actually reduces the time spent. However, there are plenty
         * of cases where this option slows down parsing drastically, which from my testing seem
         * to all be glTFs which contain embedded buffers and/or are already minimised. Note that
         * fastgltf only minimises the string if the data was loaded using GltfDataBuffer::loadFromFile
         * or GltfDataBuffer::copyBytes, and that the bytes will also be overwritten.
         */
        MinimiseJsonBeforeParsing       = 1 << 6,

        /**
         * Loads all external images into CPU memory. It does not decode any texture data. Complementary
         * to LoadExternalBuffers.
         */
        LoadExternalImages              = 1 << 7,
    };
    // clang-format on

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Options, Options, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Options, Options, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Options, Options, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Options, Options, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Options, ~)

    // String representations of glTF 2.0 extension identifiers.
    namespace extensions {
        constexpr std::string_view EXT_mesh_gpu_instancing = "EXT_mesh_gpu_instancing";
        constexpr std::string_view EXT_meshopt_compression = "EXT_meshopt_compression";
        constexpr std::string_view EXT_texture_webp = "EXT_texture_webp";
        constexpr std::string_view KHR_lights_punctual = "KHR_lights_punctual";
		constexpr std::string_view KHR_materials_anisotropy = "KHR_materials_anisotropy";
        constexpr std::string_view KHR_materials_clearcoat = "KHR_materials_clearcoat";
        constexpr std::string_view KHR_materials_emissive_strength = "KHR_materials_emissive_strength";
        constexpr std::string_view KHR_materials_ior = "KHR_materials_ior";
        constexpr std::string_view KHR_materials_iridescence = "KHR_materials_iridescence";
        constexpr std::string_view KHR_materials_sheen = "KHR_materials_sheen";
        constexpr std::string_view KHR_materials_specular = "KHR_materials_specular";
        constexpr std::string_view KHR_materials_transmission = "KHR_materials_transmission";
        constexpr std::string_view KHR_materials_unlit = "KHR_materials_unlit";
        constexpr std::string_view KHR_materials_volume = "KHR_materials_volume";
        constexpr std::string_view KHR_mesh_quantization = "KHR_mesh_quantization";
        constexpr std::string_view KHR_texture_basisu = "KHR_texture_basisu";
        constexpr std::string_view KHR_texture_transform = "KHR_texture_transform";
        constexpr std::string_view MSFT_texture_dds = "MSFT_texture_dds";

#if FASTGLTF_ENABLE_DEPRECATED_EXT
        constexpr std::string_view KHR_materials_pbrSpecularGlossiness = "KHR_materials_pbrSpecularGlossiness";
#endif
    } // namespace extensions

	// clang-format off
	// An array of pairs of string representations of extension identifiers and their respective enum
	// value used for enabling/disabling the loading of it. This also represents all extensions that
	// fastgltf supports and understands.
#if FASTGLTF_ENABLE_DEPRECATED_EXT
	static constexpr size_t SUPPORTED_EXTENSION_COUNT = 19;
#else
	static constexpr size_t SUPPORTED_EXTENSION_COUNT = 18;
#endif
	static constexpr std::array<std::pair<std::string_view, Extensions>, SUPPORTED_EXTENSION_COUNT> extensionStrings = {{
		{ extensions::EXT_mesh_gpu_instancing,            Extensions::EXT_mesh_gpu_instancing },
		{ extensions::EXT_meshopt_compression,            Extensions::EXT_meshopt_compression },
		{ extensions::EXT_texture_webp,                   Extensions::EXT_texture_webp },
		{ extensions::KHR_lights_punctual,                Extensions::KHR_lights_punctual },
		{ extensions::KHR_materials_anisotropy,           Extensions::KHR_materials_anisotropy },
		{ extensions::KHR_materials_clearcoat,            Extensions::KHR_materials_clearcoat },
		{ extensions::KHR_materials_emissive_strength,    Extensions::KHR_materials_emissive_strength },
		{ extensions::KHR_materials_ior,                  Extensions::KHR_materials_ior },
		{ extensions::KHR_materials_iridescence,          Extensions::KHR_materials_iridescence },
		{ extensions::KHR_materials_sheen,                Extensions::KHR_materials_sheen },
		{ extensions::KHR_materials_specular,             Extensions::KHR_materials_specular },
		{ extensions::KHR_materials_transmission,         Extensions::KHR_materials_transmission },
		{ extensions::KHR_materials_unlit,                Extensions::KHR_materials_unlit },
		{ extensions::KHR_materials_volume,               Extensions::KHR_materials_volume },
		{ extensions::KHR_mesh_quantization,              Extensions::KHR_mesh_quantization },
		{ extensions::KHR_texture_basisu,                 Extensions::KHR_texture_basisu },
		{ extensions::KHR_texture_transform,              Extensions::KHR_texture_transform },
		{ extensions::MSFT_texture_dds,                   Extensions::MSFT_texture_dds },

#if FASTGLTF_ENABLE_DEPRECATED_EXT
		{ extensions::KHR_materials_pbrSpecularGlossiness,Extensions::KHR_materials_pbrSpecularGlossiness },
#endif
	}};
	// clang-format on

	/**
	 * Returns the name of the passed glTF extension.
	 *
	 * @note If \p extensions has more than one bit set (multiple extensions), this
	 * will return the name of the first set bit.
	 */
#if FASTGLTF_CPP_20
	constexpr
#else
	inline
#endif
	std::string_view stringifyExtension(Extensions extensions) {
		// Find the first set bit and mask the value to that
		std::uint8_t position = 0;
		while (position < std::numeric_limits<std::underlying_type_t<Extensions>>::digits) {
			if (((to_underlying(extensions) >> position) & 1) != 0) {
				extensions &= static_cast<Extensions>(1 << position);
				break;
			}
			++position;
		}

		for (const auto& extensionString : extensionStrings)
			if (extensionString.second == extensions)
				return extensionString.first;
		return "";
	}

	class ChunkMemoryResource : public std::pmr::memory_resource {
		/**
		 * The default size of the individual blocks we allocate.
		 */
		constexpr static std::size_t blockSize = 2048;

		struct Block {
			std::unique_ptr<std::byte[]> data;
			std::size_t size;

			std::byte* dataPointer;
		};
		SmallVector<Block, 4> blocks;
		std::size_t blockIdx = 0;

	public:
		explicit ChunkMemoryResource() {
			allocateNewBlock();
		}

		void allocateNewBlock() {
			auto& block = blocks.emplace_back();
			block.data = std::unique_ptr<std::byte[]>(new std::byte[blockSize]);
			block.dataPointer = block.data.get();
			block.size = blockSize;
		}

		[[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override {
			auto& block = blocks[blockIdx];
			auto availableSize = static_cast<std::size_t>(block.dataPointer - block.data.get());
			if ((availableSize + bytes) > block.size) {
				// The block can't fit the new allocation. We'll just create a new block and use that.
				allocateNewBlock();
				++blockIdx;
				return do_allocate(bytes, alignment);
			}

			void* alloc = block.dataPointer;
			std::size_t space = availableSize;
			if (std::align(alignment, availableSize, alloc, space) == nullptr) {
				// Not enough space after alignment
				allocateNewBlock();
				++blockIdx;
				return do_allocate(bytes, alignment);
			}

			// Get the number of bytes used for padding, and calculate the new offset using that
			block.dataPointer = block.dataPointer + (availableSize - space) + bytes;
			return alloc;
		}

		void do_deallocate([[maybe_unused]] void* p, [[maybe_unused]] std::size_t bytes, [[maybe_unused]] std::size_t alignment) override {
			// We currently do nothing, as we don't keep track of what portions of the blocks are still used.
			// Therefore, we keep all blocks alive until the destruction of this resource (parser).
		}

		[[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
			return this == std::addressof(other);
		}
	};

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
		explicit Expected(Error error) : err(error) {}
		explicit Expected(T&& value) : err(Error::None), value(std::move(value)) {}

		Expected(const Expected<T>& other) = delete;
		Expected(Expected<T>&& other) noexcept : err(other.err), value(std::move(other.value)) {}

		Expected<T>& operator=(const Expected<T>& other) = delete;
		Expected<T>& operator=(Expected<T>&& other) noexcept {
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
		auto& get() noexcept {
			if constexpr (I == 0) return err;
			else if constexpr (I == 1) return value;
		}

		template <std::size_t I>
		const auto& get() const noexcept {
			if constexpr (I == 0) return err;
			else if constexpr (I == 1) return value;
		}

		/**
		 * Returns the address of the value of T.
		 * When error() returns anything but Error::None, the returned value is undefined.
		 */
		T* operator->() noexcept {
			assert(err == Error::None);
			return std::addressof(value);
		}

		/**
		 * Returns the address of the const value of T.
		 * When error() returns anything but Error::None, the returned value is undefined.
		 */
		const T* operator->() const noexcept {
			assert(err == Error::None);
			return std::addressof(value);
		}

		T&& operator*() && noexcept {
			assert(err == Error::None);
			return std::move(value);
		}

		operator bool() const noexcept {
			return err == Error::None;
		}
	};

    struct BufferInfo {
        void* mappedMemory;
        CustomBufferId customId;
    };

    using BufferMapCallback = BufferInfo(std::uint64_t bufferSize, void* userPointer);
    using BufferUnmapCallback = void(BufferInfo* bufferInfo, void* userPointer);
    using Base64DecodeCallback = void(std::string_view base64, std::uint8_t* dataOutput, std::size_t padding, std::size_t dataOutputSize, void* userPointer);

    /**
     * Enum to represent the type of a glTF file. glTFs can either be the standard JSON file with
     * paths to buffers or with a base64 embedded buffers, or they can be in a so called GLB
     * container format which has two or more chunks of binary data, where one represents buffers
     * and the other contains the JSON string.
     */
    enum class GltfType {
        glTF,
        GLB,
        Invalid,
    };

	/**
	 * This function starts reading into the buffer and tries to determine what type of glTF container it is.
	 * This should be used to know whether to call Parser::loadGLTF or Parser::loadBinaryGLTF.
	 *
	 * @return The type of the glTF file, either glTF, GLB, or Invalid if it was not determinable. If this function
	 * returns Invalid it is highly likely that the buffer does not actually represent a valid glTF file.
	 */
    GltfType determineGltfFileType(GltfDataBuffer* buffer);

    /**
     * Gets the amount of byte padding required on the GltfDataBuffer, as simdjson requires to be
     * able to overflow as it uses SIMD to load N bytes at a time.
     */
    std::size_t getGltfBufferPadding() noexcept;

    /**
     * This class holds a chunk of data that makes up a JSON string that the glTF parser will use
     * and read from.
     */
    class GltfDataBuffer {
        friend class Parser;
        friend GltfType determineGltfFileType(GltfDataBuffer* buffer);

    protected:
        std::size_t allocatedSize = 0;
        std::size_t dataSize = 0;
        std::byte* bufferPointer = nullptr;

        std::unique_ptr<std::byte[]> buffer;

        std::filesystem::path filePath = {};

    public:
        explicit GltfDataBuffer() noexcept;

		/**
		 * Constructs a new GltfDataBuffer from a span object, copying its data as there
		 * is no guarantee for the allocation size to have the adequate padding.
		 */
		explicit GltfDataBuffer(span<std::byte> data) noexcept;

        virtual ~GltfDataBuffer() noexcept;

        /**
         * Saves the pointer including its range. Does not copy any data. This requires the
         * original allocation to outlive the parsing of the glTF, so after the last relevant
         * call to fastgltf::Parser::loadGLTF. However, this function asks for a capacity size, as
         * the JSON parsing requires some padding. See fastgltf::getGltfBufferPadding for more information.
         * If the capacity does not have enough padding, the function will instead copy the bytes
         * with the copyBytes method. Also, it will set the padding bytes all to 0, so be sure to
         * not use that for any other data.
         */
        bool fromByteView(std::uint8_t* bytes, std::size_t byteCount, std::size_t capacity) noexcept;

        /**
         * This will create a copy of the passed bytes and allocate an adequately sized buffer.
         */
        bool copyBytes(const std::uint8_t* bytes, std::size_t byteCount) noexcept;

        /**
         * Loads the file with a optional byte offset into a memory buffer.
         */
        bool loadFromFile(const std::filesystem::path& path, std::uint64_t byteOffset = 0) noexcept;

        /**
         * Returns the size, in bytes,
         * @return
         */
        [[nodiscard]] inline std::size_t getBufferSize() const noexcept;

        [[nodiscard]] explicit operator span<std::byte>() {
            return span<std::byte>(bufferPointer, dataSize);
        }
    };

    #if defined(__ANDROID__)
    class AndroidGltfDataBuffer : public GltfDataBuffer {
        AAssetManager* assetManager;

    public:
        explicit AndroidGltfDataBuffer(AAssetManager* assetManager) noexcept;
        ~AndroidGltfDataBuffer() noexcept = default;

        /**
         * Loads a file from within an Android APK
         */
        bool loadFromAndroidAsset(const std::filesystem::path& path, std::uint64_t byteOffset = 0) noexcept;
    };
	#endif

	/**
	 * This function further validates all the input more strictly that is parsed from the glTF.
	 * Realistically, this should not be necessary in Release applications, but could be helpful
	 * when debugging an asset related issue.
	*/
	[[nodiscard]] Error validate(const Asset& asset);

    /**
     * Some internals the parser passes on to each glTF instance.
     */
    struct ParserInternalConfig {
        BufferMapCallback* mapCallback = nullptr;
        BufferUnmapCallback* unmapCallback = nullptr;
        Base64DecodeCallback* decodeCallback = nullptr;

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
		std::shared_ptr<ChunkMemoryResource> resourceAllocator;
		std::filesystem::path directory;
		Options options;

		static auto getMimeTypeFromString(std::string_view mime) -> MimeType;
		static void fillCategories(Category& inputCategories) noexcept;

		[[nodiscard]] auto decodeDataUri(URIView& uri) const noexcept -> Expected<DataSource>;
		[[nodiscard]] auto loadFileFromUri(URIView& uri) const noexcept -> Expected<DataSource>;

		Error parseAccessors(simdjson::dom::array& array, Asset& asset);
		Error parseAnimations(simdjson::dom::array& array, Asset& asset);
		Error parseBuffers(simdjson::dom::array& array, Asset& asset);
		Error parseBufferViews(simdjson::dom::array& array, Asset& asset);
		Error parseCameras(simdjson::dom::array& array, Asset& asset);
		Error parseExtensions(simdjson::dom::object& extensionsObject, Asset& asset);
		Error parseImages(simdjson::dom::array& array, Asset& asset);
		Error parseLights(simdjson::dom::array& array, Asset& asset);
		Error parseMaterials(simdjson::dom::array& array, Asset& asset);
		Error parseMeshes(simdjson::dom::array& array, Asset& asset);
		Error parseNodes(simdjson::dom::array& array, Asset& asset);
		Error parseSamplers(simdjson::dom::array& array, Asset& asset);
		Error parseScenes(simdjson::dom::array& array, Asset& asset);
		Error parseSkins(simdjson::dom::array& array, Asset& asset);
		Error parseTextures(simdjson::dom::array& array, Asset& asset);
		Expected<Asset> parse(simdjson::dom::object root, Category categories);

    public:
        explicit Parser(Extensions extensionsToLoad = Extensions::None) noexcept;
        explicit Parser(const Parser& parser) = delete;
        Parser(Parser&& parser) noexcept;
        Parser& operator=(const Parser& parser) = delete;
        Parser& operator=(Parser&& other) noexcept;

        ~Parser();

        /**
         * Loads a glTF file from pre-loaded bytes representing a JSON file.
         *
         * @return An Asset wrapped in an Expected type, which may contain an error if one occurred.
         */
        [[nodiscard]] Expected<Asset> loadGLTF(GltfDataBuffer* buffer, std::filesystem::path directory, Options options = Options::None, Category categories = Category::All);

		/**
		 * Loads a glTF file embedded within a GLB container, which may contain the first buffer of the glTF asset.
		 *
         * @return An Asset wrapped in an Expected type, which may contain an error if one occurred.
		 */
		[[nodiscard]] Expected<Asset> loadBinaryGLTF(GltfDataBuffer* buffer, std::filesystem::path directory, Options options = Options::None, Category categories = Category::All);

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
        void setUserPointer(void* pointer) noexcept;
    };
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif
