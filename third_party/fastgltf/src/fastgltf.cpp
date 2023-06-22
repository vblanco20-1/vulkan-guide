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

#if !defined(__cplusplus) || (!defined(_MSVC_LANG) && __cplusplus < 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#error "fastgltf requires C++17"
#endif

#include <array>
#include <cmath>
#include <fstream>
#include <functional>
#include <mutex>
#include <utility>

#if __ANDROID__
#include <android/asset_manager.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

#include <simdjson.h>

#ifdef SIMDJSON_TARGET_VERSION
// Make sure that SIMDJSON_TARGET_VERSION is equal to SIMDJSON_VERSION.
static_assert(std::string_view { SIMDJSON_TARGET_VERSION } == SIMDJSON_VERSION, "Outdated version of simdjson. Reconfigure project to update.");
#endif

#include <fastgltf/parser.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/base64.hpp>

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
#include <smmintrin.h>
#endif

namespace fg = fastgltf;
namespace fs = std::filesystem;

namespace fastgltf {
    constexpr std::string_view mimeTypeJpeg = "image/jpeg";
    constexpr std::string_view mimeTypePng = "image/png";
    constexpr std::string_view mimeTypeKtx = "image/ktx2";
    constexpr std::string_view mimeTypeDds = "image/vnd-ms.dds";
    constexpr std::string_view mimeTypeGltfBuffer = "application/gltf-buffer";
    constexpr std::string_view mimeTypeOctetStream = "application/octet-stream";

    struct ParserData {
        simdjson::dom::document doc;
        simdjson::dom::object root;
        ParserInternalConfig config;

        explicit ParserData() = default;
        explicit ParserData(const ParserData& other) = delete;
        ParserData& operator=(const ParserData& other) = delete;
    };

    constexpr std::uint32_t binaryGltfHeaderMagic = 0x46546C67; // ASCII for "glTF".
    constexpr std::uint32_t binaryGltfJsonChunkMagic = 0x4E4F534A;
    constexpr std::uint32_t binaryGltfDataChunkMagic = 0x004E4942;

    struct BinaryGltfHeader {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t length;
    };
    static_assert(sizeof(BinaryGltfHeader) == 12, "Binary gltf header must be 12 bytes");

    struct BinaryGltfChunk {
        std::uint32_t chunkLength;
        std::uint32_t chunkType;
    };

    using CRCFunction = std::uint32_t(*)(const std::uint8_t*, std::size_t);
    using CRCStringFunction = std::uint32_t(*)(std::string_view str);

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
    [[gnu::hot, gnu::const, gnu::target("sse4.2")]] std::uint32_t hwcrc32c(std::string_view str) noexcept {
        return hwcrc32c(reinterpret_cast<const std::uint8_t*>(str.data()), str.size());
    }

    [[gnu::hot, gnu::const, gnu::target("sse4.2")]] std::uint32_t hwcrc32c(const std::uint8_t* d, std::size_t len) noexcept {
        std::uint32_t crc = 0;

        // Try to advance forwards until the address is aligned to 4 bytes.
        auto address = reinterpret_cast<std::uintptr_t>(d);
        std::size_t i = 0;
        if (address % 2 != 0 && i < len) {
            crc = _mm_crc32_u8(crc, d[i++]);
        }

        // We might be 4 byte aligned, but if not we'll read 2 more bytes to get to 4 byte alignment.
        if ((address + i) % 4 != 0 && i < len) {
            crc = _mm_crc32_u16(crc, *reinterpret_cast<const std::uint16_t*>(&d[i]));
            i += 2;
        }

        // Now, try to decode as much as possible using 4 byte steps. We specifically don't use
        // the 8 byte instruction here because the strings used by glTF are usually very short.
        while (i < len && (len - i) >= 4) {
            crc = _mm_crc32_u32(crc, *reinterpret_cast<const std::uint32_t*>(&d[i]));
            i += 4;
        }

        if ((len - i) >= 2) {
            crc = _mm_crc32_u16(crc, *reinterpret_cast<const std::uint16_t*>(&d[i]));
            i += 2;
        }

        // Decode the rest
        if (i < len) {
            crc = _mm_crc32_u8(crc, d[i++]);
        }

        return crc;
    }
#endif

    /**
     * Points to the most 'optimal' CRC32-C encoding function. After initialiseCrc has been called,
     * this might also point to hwcrc32c. We only use this for runtime evaluation of hashes, and is
     * intended to work for any length of data.
     */
    static CRCFunction crcFunction = crc32c;
    static CRCStringFunction crcStringFunction = crc32c;

    std::once_flag crcInitialisation;

    /**
     * Checks if SSE4.2 is available to try and use the hardware accelerated version.
     */
    void initialiseCrc() {
#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
        const auto& impls = simdjson::get_available_implementations();
        if (const auto* sse4 = impls["westmere"]; sse4 != nullptr && sse4->supported_by_runtime_system()) {
            crcFunction = hwcrc32c;
            crcStringFunction = hwcrc32c;
        }
#endif
    }

	[[nodiscard, gnu::always_inline]] inline std::tuple<bool, bool, std::size_t> getImageIndexForExtension(simdjson::dom::object& object, std::string_view extension) {
		using namespace simdjson;

		dom::object sourceExtensionObject;
		if (object[extension].get_object().get(sourceExtensionObject) != SUCCESS) {
			return std::make_tuple(false, true, 0U);
		}

		std::uint64_t imageIndex;
		if (sourceExtensionObject["source"].get_uint64().get(imageIndex) != SUCCESS) {
			return std::make_tuple(true, false, 0U);
		}

		return std::make_tuple(false, false, imageIndex);
	}

	[[nodiscard, gnu::always_inline]] inline bool parseTextureExtensions(Texture& texture, simdjson::dom::object& extensions, Extensions extensionFlags) {
		if (hasBit(extensionFlags, Extensions::KHR_texture_basisu)) {
			auto [invalidGltf, extensionNotPresent, imageIndex] = getImageIndexForExtension(extensions, extensions::KHR_texture_basisu);
			if (invalidGltf) {
				return false;
			}

			if (!extensionNotPresent) {
				texture.imageIndex = imageIndex;
				return true;
			}
		}

		if (hasBit(extensionFlags, Extensions::MSFT_texture_dds)) {
			auto [invalidGltf, extensionNotPresent, imageIndex] = getImageIndexForExtension(extensions, extensions::MSFT_texture_dds);
			if (invalidGltf) {
				return false;
			}

			if (!extensionNotPresent) {
				texture.imageIndex = imageIndex;
				return true;
			}
		}

		if (hasBit(extensionFlags, Extensions::EXT_texture_webp)) {
			auto [invalidGltf, extensionNotPresent, imageIndex] = getImageIndexForExtension(extensions, extensions::EXT_texture_webp);
			if (invalidGltf) {
				return false;
			}

			if (!extensionNotPresent) {
				texture.imageIndex = imageIndex;
				return true;
			}
		}

		return false;
	}

	[[nodiscard, gnu::always_inline]] inline Error getJsonArray(simdjson::dom::object& parent, std::string_view arrayName, simdjson::dom::array* array) noexcept {
		using namespace simdjson;

		auto error = parent[arrayName].get_array().get(*array);
		if (error == NO_SUCH_FIELD) {
			return Error::MissingField;
		}
		if (error == SUCCESS) {
			return Error::None;
		}
		return Error::InvalidJson;
	}

	fg::Error parseTextureObject(simdjson::dom::object& object, std::string_view key, TextureInfo* info, Extensions extensions) noexcept {
		using namespace simdjson;

		dom::object child;
		const auto childErr = object[key].get_object().get(child);
		if (childErr == NO_SUCH_FIELD) {
			return Error::MissingField; // Don't set errorCode.
		} else if (childErr != SUCCESS) {
			return Error::InvalidGltf;
		}

		std::uint64_t index;
		if (child["index"].get_uint64().get(index) == SUCCESS) {
			info->textureIndex = static_cast<std::size_t>(index);
		} else {
			return Error::InvalidGltf;
		}

		if (child["texCoord"].get_uint64().get(index) == SUCCESS) {
			info->texCoordIndex = static_cast<std::size_t>(index);
		} else {
			info->texCoordIndex = 0;
		}

		// scale only applies to normal textures.
		double scale = 1.0F;
		if (child["scale"].get_double().get(scale) == SUCCESS) {
			info->scale = static_cast<float>(scale);
		} else {
			info->scale = 1.0F;
		}

		dom::object extensionsObject;
		if (child["extensions"].get_object().get(extensionsObject) == SUCCESS) {
			dom::object textureTransform;
			if (hasBit(extensions, Extensions::KHR_texture_transform) && extensionsObject[extensions::KHR_texture_transform].get_object().get(textureTransform) == SUCCESS) {
				auto transform = std::make_unique<TextureTransform>();
				transform->rotation = 0.0F;
				transform->uvOffset = {{ 0.0F, 0.0F }};
				transform->uvScale = {{ 1.0F, 1.0F }};

				if (textureTransform["texCoord"].get_uint64().get(index) == SUCCESS) {
					transform->texCoordIndex = index;
				}

				double rotation = 0.0F;
				if (textureTransform["rotation"].get_double().get(rotation) == SUCCESS) {
					transform->rotation = static_cast<float>(rotation);
				}

				dom::array array;
				if (textureTransform["offset"].get_array().get(array) == SUCCESS) {
					for (auto i = 0U; i < 2; ++i) {
						double val;
						if (array.at(i).get_double().get(val) != SUCCESS) {
							return Error::InvalidGltf;
						}
						transform->uvOffset[i] = static_cast<float>(val);
					}
				}

				if (textureTransform["scale"].get_array().get(array) == SUCCESS) {
					for (auto i = 0U; i < 2; ++i) {
						double val;
						if (array.at(i).get_double().get(val) != SUCCESS) {
							return Error::InvalidGltf;
						}
						transform->uvScale[i] = static_cast<float>(val);
					}
				}

				info->transform = std::move(transform);
			}
		}

		return Error::None;
	}
} // namespace fastgltf

#pragma region URI
fg::URI::URI() noexcept = default;

fg::URI::URI(std::string uri) noexcept : uri(std::move(uri)) {
    parse();
}

fg::URI::URI(std::string_view uri) noexcept : uri(uri) {
    parse();
}

// Some C++ stdlib implementations copy in some cases when moving strings, which invalidates the
// views stored in the URI struct. This function adjusts the views from the old string to the new
// string for safe copying.
fg::URI::URI(const URI& other) : uri(other.uri) {
    *this = other;
}

fg::URI::URI(URI&& other) noexcept {
    *this = other;
}

fg::URI& fg::URI::operator=(const URI& other) {
    uri = other.uri;
    readjustViews(other, other.uri);
    return *this;
}

fg::URI& fg::URI::operator=(URI&& other) noexcept {
    std::string_view view = other.uri;
    uri = std::move(other.uri);
    _valid = other._valid;
    other._valid = false;
    if (uri.data() == view.data()) {
        _scheme = other._scheme;
        _path = other._path;
        _userinfo = other._userinfo;
        _host = other._host;
        _port = other._port;
        _query = other._query;
        _fragment = other._fragment;
    } else {
        // A reallocation happened or stack memory was used.
        readjustViews(other, view);
    }
    return *this;
}

void fg::URI::readjustViews(const fastgltf::URI& other, std::string_view otherUri) {
    if (!other._scheme.empty()) {   _scheme     = std::string_view(uri.data() + (other._scheme.data()     - other.uri.data()), other._scheme.size()); }
    if (!other._path.empty()) {     _path       = std::string_view(uri.data() + (other._path.data()       - other.uri.data()), other._path.size()); }
    if (!other._userinfo.empty()) { _userinfo   = std::string_view(uri.data() + (other._userinfo.data()   - other.uri.data()), other._userinfo.size()); }
    if (!other._host.empty()) {     _host       = std::string_view(uri.data() + (other._host.data()       - other.uri.data()), other._host.size()); }
    if (!other._port.empty()) {     _port       = std::string_view(uri.data() + (other._port.data()       - other.uri.data()), other._port.size()); }
    if (!other._query.empty()) {    _query      = std::string_view(uri.data() + (other._query.data()      - other.uri.data()), other._query.size()); }
    if (!other._fragment.empty()) { _fragment   = std::string_view(uri.data() + (other._fragment.data()   - other.uri.data()), other._fragment.size()); }
}

void fg::URI::decodePercents(std::string& x) noexcept {
    for (auto it = x.begin(); it != x.end(); it++) {
        if (*it == '%') {
            // Read the next two chars and store them.
            std::array<char, 3> chars = {*(it + 1), *(it + 2), 0};
            *it = static_cast<char>(std::strtoul(chars.data(), nullptr, 16));
            x.erase(it + 1, it + 3);
        }
    }
}

void fg::URI::parse() {
    if (uri.empty()) {
        _valid = false;
        return;
    }

    auto uriView = std::string_view { uri };

    size_t idx = 0;
    auto firstColon = uri.find(':');
    if (firstColon != std::string::npos) {
        // URI has a scheme.
        if (firstColon == 0) {
            // Empty scheme is invalid
            _valid = false;
            return;
        }
        _scheme = uriView.substr(0, firstColon);
        idx = firstColon + 1;
    }

    if (startsWith(uriView.substr(idx), "//")) {
        // URI has an authority part.
        idx += 2;
        auto nextSlash = uriView.find('/', idx);
        auto userInfo = uriView.find('@', idx);
        if (userInfo != std::string::npos && userInfo < nextSlash) {
            _userinfo = uriView.substr(idx, userInfo - idx);
            idx += _userinfo.size() + 1;
        }

        auto hostEnd = nextSlash - 1;
        std::size_t portColon;
        if (uriView[idx] == '[') {
            hostEnd = uriView.find(']', idx);
            if (hostEnd == std::string::npos) {
                _valid = false;
                return;
            }
            // IPv6 addresses are made up of colons, so we need to search after its address.
            // This will just be hostEnd + 1 or std::string::npos.
            portColon = uriView.find(':', hostEnd);
        } else {
            portColon = uriView.find(':', idx);
        }

        if (portColon != std::string::npos) {
            _host = uriView.substr(idx, portColon - idx);
            ++portColon; // We don't want to include the colon in the port string.
            _port = uriView.substr(portColon, nextSlash - portColon);
        } else {
            ++idx;
            _host = uriView.substr(idx, hostEnd - idx);
        }

        idx = nextSlash; // Path includes this slash
    }

    // Parse the path.
    auto questionIdx = uriView.find("?", idx);
    auto hashIdx = uriView.find("#", idx);
    if (questionIdx != std::string::npos) {
        _path = uriView.substr(idx, questionIdx - idx);

        if (hashIdx == std::string::npos) {
            _query = uriView.substr(++questionIdx);
        } else {
            ++questionIdx;
            _query = uriView.substr(questionIdx, hashIdx - questionIdx);
            _fragment = uriView.substr(++hashIdx);
        }
    } else if (hashIdx != std::string::npos) {
        _path = uriView.substr(idx, hashIdx - idx);
        _fragment = uriView.substr(++hashIdx);
    } else {
        _path = uriView.substr(idx);
    }
}

std::string_view fg::URI::raw() const noexcept { return uri; }
std::string_view fg::URI::scheme() const noexcept { return _scheme; }
std::string_view fg::URI::userinfo() const noexcept { return _userinfo; }
std::string_view fg::URI::host() const noexcept { return _host; }
std::string_view fg::URI::port() const noexcept { return _port; }
std::string_view fg::URI::path() const noexcept { return _path; }
std::string_view fg::URI::query() const noexcept { return _query; }
std::string_view fg::URI::fragment() const noexcept { return _fragment; }

fs::path fg::URI::fspath() const {
    if (!isLocalPath())
        return {};
    return { path() };
}

bool fg::URI::valid() const noexcept {
    return _valid;
}

bool fg::URI::isLocalPath() const noexcept {
    return scheme().empty() || (scheme() == "file" && host().empty());
}

bool fg::URI::isDataUri() const noexcept {
    return scheme() == "data";
}
#pragma endregion

#pragma region glTF
fg::glTF::glTF(std::unique_ptr<ParserData> data, fs::path directory, Options options) : data(std::move(data)), directory(std::move(directory)), options(options) {
    parsedAsset = std::make_unique<Asset>();
}

// We define the destructor here as otherwise the definition would be generated in other cpp files
// in which the definition for ParserData is not available.
fg::glTF::~glTF() = default;

// clang-format off
// An array of pairs of string representations of extension identifiers and their respective enum
// value used for enabling/disabling the loading of it. This also represents all extensions that
// fastgltf supports and understands.
static constexpr std::array<std::pair<std::string_view, fastgltf::Extensions>, 16> extensionStrings = {{
    { fg::extensions::EXT_meshopt_compression,            fg::Extensions::EXT_meshopt_compression },
    { fg::extensions::EXT_texture_webp,                   fg::Extensions::EXT_texture_webp },
    { fg::extensions::KHR_lights_punctual,                fg::Extensions::KHR_lights_punctual },
    { fg::extensions::KHR_materials_clearcoat,            fg::Extensions::KHR_materials_clearcoat },
    { fg::extensions::KHR_materials_emissive_strength,    fg::Extensions::KHR_materials_emissive_strength },
    { fg::extensions::KHR_materials_ior,                  fg::Extensions::KHR_materials_ior },
    { fg::extensions::KHR_materials_iridescence,          fg::Extensions::KHR_materials_iridescence },
    { fg::extensions::KHR_materials_sheen,                fg::Extensions::KHR_materials_sheen },
    { fg::extensions::KHR_materials_specular,             fg::Extensions::KHR_materials_specular },
    { fg::extensions::KHR_materials_transmission,         fg::Extensions::KHR_materials_transmission },
    { fg::extensions::KHR_materials_unlit,                fg::Extensions::KHR_materials_unlit },
    { fg::extensions::KHR_materials_volume,               fg::Extensions::KHR_materials_volume },
    { fg::extensions::KHR_mesh_quantization,              fg::Extensions::KHR_mesh_quantization },
    { fg::extensions::KHR_texture_basisu,                 fg::Extensions::KHR_texture_basisu },
    { fg::extensions::KHR_texture_transform,              fg::Extensions::KHR_texture_transform },
    { fg::extensions::MSFT_texture_dds,                   fg::Extensions::MSFT_texture_dds },
}};
// clang-format on

#define SET_ERROR_RETURN(error) errorCode = error; \
    return;
#define SET_ERROR_RETURN_ERROR(error) errorCode = error; \
    return errorCode;

std::pair<fg::Error, fg::DataSource> fg::glTF::decodeDataUri(URI& uri) const noexcept {
    auto path = uri.path();
    auto mimeEnd = path.find(';');
    auto mime = path.substr(0, mimeEnd);

    auto encodingEnd = path.find(',');
    auto encoding = path.substr(mimeEnd + 1, encodingEnd - mimeEnd - 1);
    if (encoding != "base64") {
        return std::make_pair(Error::InvalidURI, std::monostate {});
    }

    auto encodedData = path.substr(encodingEnd + 1);
    if (data->config.mapCallback != nullptr) {
        // If a map callback is specified, we use a pointer to memory specified by it.
        auto padding = base64::getPadding(encodedData);
        auto size = base64::getOutputSize(encodedData.size(), padding);
        auto info = data->config.mapCallback(size, data->config.userPointer);
        if (info.mappedMemory != nullptr) {
            if (data->config.decodeCallback != nullptr) {
                data->config.decodeCallback(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding, size, data->config.userPointer);
            } else {
                base64::decode_inplace(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding);
            }

            if (data->config.unmapCallback != nullptr) {
                data->config.unmapCallback(&info, data->config.userPointer);
            }

            sources::CustomBuffer source = {};
            source.id = info.customId;
            source.mimeType = getMimeTypeFromString(mime);
            return std::make_pair(Error::None, source);
        }
    }

    // Decode the base64 data into a traditional vector
    std::vector<std::uint8_t> uriData;
    if (data->config.decodeCallback != nullptr) {
        auto padding = base64::getPadding(encodedData);
        uriData.resize(base64::getOutputSize(encodedData.size(), padding));
        data->config.decodeCallback(encodedData, uriData.data(), padding, uriData.size(), data->config.userPointer);
    } else {
        uriData = base64::decode(encodedData);
    }

    sources::Vector source = {};
    source.mimeType = getMimeTypeFromString(mime);
    source.bytes = std::move(uriData);
    return std::make_pair(Error::None, std::move(source));
}

std::pair<fg::Error, fg::DataSource> fg::glTF::loadFileFromUri(URI& uri) const noexcept {
    auto path = directory / uri.path();
    std::error_code error;
    // If we were instructed to load external buffers and the files don't exist, we'll return an error.
    if (!fs::exists(path, error) || error) {
        return std::make_pair(Error::MissingExternalBuffer, std::monostate {});
    }

    auto length = static_cast<std::streamsize>(std::filesystem::file_size(path, error));
    if (error) {
        return std::make_pair(Error::InvalidPath, std::monostate {});
    }

    std::ifstream file(path, std::ios::binary);

    if (data->config.mapCallback != nullptr) {
        auto info = data->config.mapCallback(static_cast<std::uint64_t>(length), data->config.userPointer);
        if (info.mappedMemory != nullptr) {
            const sources::CustomBuffer customBufferSource = { info.customId, MimeType::None };
            file.read(reinterpret_cast<char*>(info.mappedMemory), length);
            if (data->config.unmapCallback != nullptr) {
                data->config.unmapCallback(&info, data->config.userPointer);
            }

            return std::make_pair(Error::None, customBufferSource);
        }
    }

    sources::Vector vectorSource = {};
    vectorSource.mimeType = MimeType::GltfBuffer;
    vectorSource.bytes.resize(length);
    file.read(reinterpret_cast<char*>(vectorSource.bytes.data()), length);
    return std::make_pair(Error::None, std::move(vectorSource));
}

void fg::glTF::fillCategories(Category& inputCategories) noexcept {
    if (inputCategories == Category::All)
        return;

    // The Category enum used to already OR values together so that e.g. Scenes would also implicitly
    // have the Nodes bit set. This, however, caused some issues within the parse function as it tries
    // to bail out when all requested categories have been parsed, as now something that hasn't been
    // parsed could still be set. So, this has to exist...
    if (hasBit(inputCategories, Category::Scenes))
        inputCategories |= Category::Nodes;
    if (hasBit(inputCategories, Category::Nodes))
        inputCategories |= Category::Cameras | Category::Meshes | Category::Skins;
    if (hasBit(inputCategories, Category::Skins))
        // Skins needs nodes, nodes needs skins. To counter this circular dep we just redefine what we just wrote above.
        inputCategories |= Category::Accessors | (Category::Nodes | Category::Cameras | Category::Meshes | Category::Skins);
    if (hasBit(inputCategories, Category::Meshes))
        inputCategories |= Category::Accessors | Category::Materials;
    if (hasBit(inputCategories, Category::Materials))
        inputCategories |= Category::Textures;
    if (hasBit(inputCategories, Category::Animations))
        inputCategories |= Category::Accessors;
    if (hasBit(inputCategories, Category::Textures))
        inputCategories |= Category::Images | Category::Samplers;
    if (hasBit(inputCategories, Category::Images) || hasBit(inputCategories, Category::Accessors))
        inputCategories |= Category::BufferViews;
    if (hasBit(inputCategories, Category::BufferViews))
        inputCategories |= Category::Buffers;
}

fg::MimeType fg::glTF::getMimeTypeFromString(std::string_view mime) {
    const auto hash = crcStringFunction(mime);
    switch (hash) {
        case force_consteval<crc32c(mimeTypeJpeg)>: {
            return MimeType::JPEG;
        }
        case force_consteval<crc32c(mimeTypePng)>: {
            return MimeType::PNG;
        }
        case force_consteval<crc32c(mimeTypeKtx)>: {
            return MimeType::KTX2;
        }
        case force_consteval<crc32c(mimeTypeDds)>: {
            return MimeType::DDS;
        }
        case force_consteval<crc32c(mimeTypeGltfBuffer)>: {
            return MimeType::GltfBuffer;
        }
        case force_consteval<crc32c(mimeTypeOctetStream)>: {
            return MimeType::OctetStream;
        }
        default: {
            return MimeType::None;
        }
    }
}

std::unique_ptr<fg::Asset> fg::glTF::getParsedAsset() {
    // If there has been any errors we don't want the caller to get the partially parsed asset.
    if (errorCode != Error::None) {
        return nullptr;
    }
    return std::move(parsedAsset);
}

fg::Error fg::glTF::validate() {
    if (parsedAsset == nullptr) {
        // This would indicate that the asset has already been moved.
        return Error::None;
    }
    if (errorCode != Error::None) {
        return errorCode;
    }

    for (const auto& accessor : parsedAsset->accessors) {
        if (accessor.type == AccessorType::Invalid)
            return Error::InvalidGltf;
        if (accessor.componentType == ComponentType::Invalid)
            return Error::InvalidGltf;
        if (accessor.count < 1)
            return Error::InvalidGltf;
        if (accessor.bufferViewIndex.has_value() &&
            accessor.bufferViewIndex.value() >= parsedAsset->bufferViews.size())
            return Error::InvalidGltf;

        if (!std::holds_alternative<std::monostate>(accessor.max)) {
            if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
                && !std::holds_alternative<std::vector<double>>(accessor.max))
                return Error::InvalidGltf;
        }
        if (!std::holds_alternative<std::monostate>(accessor.min)) {
            if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
                && !std::holds_alternative<std::vector<double>>(accessor.min))
                return Error::InvalidGltf;
        }
    }

    for (const auto& animation : parsedAsset->animations) {
        if (animation.channels.empty())
            return Error::InvalidGltf;
        if (animation.samplers.empty())
            return Error::InvalidGltf;
    }

    for (const auto& buffer : parsedAsset->buffers) {
        if (buffer.byteLength < 1)
            return Error::InvalidGltf;
    }

    for (const auto& bufferView : parsedAsset->bufferViews) {
        if (bufferView.byteLength < 1)
            return Error::InvalidGltf;
        if (bufferView.byteStride.has_value() && (bufferView.byteStride < 4U || bufferView.byteStride > 252U))
            return Error::InvalidGltf;
        if (bufferView.bufferIndex >= parsedAsset->buffers.size())
            return Error::InvalidGltf;

        if (bufferView.meshoptCompression != nullptr && hasBit(data->config.extensions, Extensions::EXT_meshopt_compression))
            return Error::InvalidGltf;

        if (bufferView.meshoptCompression) {
            const auto& compression = bufferView.meshoptCompression;
            switch (compression->mode) {
                case MeshoptCompressionMode::Attributes:
                    if (compression->byteStride % 4 != 0 || compression->byteStride > 256)
                        return Error::InvalidGltf;
                    break;
                case MeshoptCompressionMode::Triangles:
                    if (compression->count % 3 != 0)
                        return Error::InvalidGltf;
                    [[fallthrough]];
                case MeshoptCompressionMode::Indices:
                    if (compression->byteStride != 2 && compression->byteStride != 4)
                        return Error::InvalidGltf;
                    break;
                case MeshoptCompressionMode::None:
                    break;
            }
        }
    }

    for (const auto& camera : parsedAsset->cameras) {
        if (const auto* pOrthographic = std::get_if<Camera::Orthographic>(&camera.camera)) {
            if (pOrthographic->zfar == 0)
                return Error::InvalidGltf;
        } else if (const auto* pPerspective = std::get_if<Camera::Perspective>(&camera.camera)) {
            if (pPerspective->aspectRatio.has_value() && pPerspective->aspectRatio == .0f)
                return Error::InvalidGltf;
            if (pPerspective->yfov == 0)
                return Error::InvalidGltf;
            if (pPerspective->zfar.has_value() && pPerspective->zfar == .0f)
                return Error::InvalidGltf;
            if (pPerspective->znear == 0.0F)
                return Error::InvalidGltf;
        }
    }

    for (const auto& image : parsedAsset->images) {
        if (const auto* view = std::get_if<sources::BufferView>(&image.data); view != nullptr) {
            if (view->bufferViewIndex >= parsedAsset->bufferViews.size()) {
                return Error::InvalidGltf;
            }
        }
    }

    for (const auto& material : parsedAsset->materials) {
        auto isInvalidTexture = [&textures = parsedAsset->textures](std::optional<std::size_t> textureIndex) {
            return textureIndex.has_value() && textureIndex.value() >= textures.size();
        };
        if (material.normalTexture.has_value() && isInvalidTexture(material.normalTexture->textureIndex))
            return Error::InvalidGltf;
        if (material.emissiveTexture.has_value() && isInvalidTexture(material.emissiveTexture->textureIndex))
            return Error::InvalidGltf;
        if (material.occlusionTexture.has_value() && isInvalidTexture(material.occlusionTexture->textureIndex))
            return Error::InvalidGltf;
        if (material.pbrData.has_value()) {
            if (material.pbrData->baseColorTexture.has_value() &&
                isInvalidTexture(material.pbrData->baseColorTexture->textureIndex))
                return Error::InvalidGltf;
            if (material.pbrData->metallicRoughnessTexture.has_value() &&
                isInvalidTexture(material.pbrData->metallicRoughnessTexture->textureIndex))
                return Error::InvalidGltf;
        }
    }

    for (const auto& mesh : parsedAsset->meshes) {
        for (const auto& primitives : mesh.primitives) {
            for (auto [name, index] : primitives.attributes) {
                if (parsedAsset->accessors.size() <= index)
                    return Error::InvalidGltf;

                // The spec provides a list of attributes that it accepts and mentions that all
                // custom attributes have to start with an underscore. We'll enforce this.
                if (!startsWith(name, "_")) {
                    if (name != "POSITION" && name != "NORMAL" && name != "TANGENT" &&
                        !startsWith(name, "TEXCOORD_") && !startsWith(name, "COLOR_") &&
                        !startsWith(name, "JOINTS_") && !startsWith(name, "WEIGHTS_")) {
                        return Error::InvalidGltf;
                    }
                }

                // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
                const auto& accessor = parsedAsset->accessors[index];
                if (name == "POSITION") {
                    if (accessor.type != AccessorType::Vec3)
                        return Error::InvalidGltf;
                    if (!hasBit(data->config.extensions, Extensions::KHR_mesh_quantization)) {
                        if (accessor.componentType != ComponentType::Float)
                            return Error::InvalidGltf;
                    } else {
                        if (accessor.componentType == ComponentType::Double || accessor.componentType == ComponentType::UnsignedInt)
                            return Error::InvalidGltf;
                    }
                } else if (name == "NORMAL") {
                    if (accessor.type != AccessorType::Vec3)
                        return Error::InvalidGltf;
                    if (!hasBit(data->config.extensions, Extensions::KHR_mesh_quantization)) {
                        if (accessor.componentType != ComponentType::Float)
                            return Error::InvalidGltf;
                    } else {
                        if (accessor.componentType != ComponentType::Float &&
                            accessor.componentType != ComponentType::Short &&
                            accessor.componentType != ComponentType::Byte)
                            return Error::InvalidGltf;
                    }
                } else if (name == "TANGENT") {
                    if (accessor.type != AccessorType::Vec4)
                        return Error::InvalidGltf;
                    if (!hasBit(data->config.extensions, Extensions::KHR_mesh_quantization)) {
                        if (accessor.componentType != ComponentType::Float)
                            return Error::InvalidGltf;
                    } else {
                        if (accessor.componentType != ComponentType::Float &&
                            accessor.componentType != ComponentType::Short &&
                            accessor.componentType != ComponentType::Byte)
                            return Error::InvalidGltf;
                    }
                } else if (startsWith(name, "TEXCOORD_")) {
                    if (accessor.type != AccessorType::Vec2)
                        return Error::InvalidGltf;
                    if (!hasBit(data->config.extensions, Extensions::KHR_mesh_quantization)) {
                        if (accessor.componentType != ComponentType::Float &&
                            accessor.componentType != ComponentType::UnsignedByte &&
                            accessor.componentType != ComponentType::UnsignedShort) {
                            return Error::InvalidGltf;
                        }
                    } else {
                        if (accessor.componentType == ComponentType::Double ||
                            accessor.componentType == ComponentType::UnsignedInt) {
                            return Error::InvalidGltf;
                        }
                    }
                } else if (startsWith(name, "COLOR_")) {
                    if (accessor.type != AccessorType::Vec3 && accessor.type != AccessorType::Vec4)
                        return Error::InvalidGltf;
                    if (accessor.componentType != ComponentType::Float &&
                        accessor.componentType != ComponentType::UnsignedByte &&
                        accessor.componentType != ComponentType::UnsignedShort) {
                        return Error::InvalidGltf;
                    }
                } else if (startsWith(name, "JOINTS_")) {
                    if (accessor.type != AccessorType::Vec4)
                        return Error::InvalidGltf;
                    if (accessor.componentType != ComponentType::UnsignedByte &&
                        accessor.componentType != ComponentType::UnsignedShort) {
                        return Error::InvalidGltf;
                    }
                } else if (startsWith(name, "WEIGHTS_")) {
                    if (accessor.type != AccessorType::Vec4)
                        return Error::InvalidGltf;
                    if (accessor.componentType != ComponentType::Float &&
                        accessor.componentType != ComponentType::UnsignedByte &&
                        accessor.componentType != ComponentType::UnsignedShort) {
                        return Error::InvalidGltf;
                    }
                }
            }
        }
    }

    for (const auto& node : parsedAsset->nodes) {
        if (node.cameraIndex.has_value() && parsedAsset->cameras.size() <= node.cameraIndex.value())
            return Error::InvalidGltf;
        if (node.skinIndex.has_value() && parsedAsset->skins.size() <= node.skinIndex.value())
            return Error::InvalidGltf;
        if (node.meshIndex.has_value() && parsedAsset->meshes.size() <= node.meshIndex.value())
            return Error::InvalidGltf;

        if (const auto* pTRS = std::get_if<Node::TRS>(&node.transform)) {
            for (const auto& x : pTRS->rotation)
                if (x > 1.0 || x < -1.0)
                    return Error::InvalidGltf;
        }

        if (node.skinIndex.has_value() && node.meshIndex.has_value()) {
            // "When the node contains skin, all mesh.primitives MUST contain JOINTS_0 and WEIGHTS_0 attributes."
            auto& mesh = parsedAsset->meshes[node.meshIndex.value()];
            for (auto& primitive : mesh.primitives) {
                auto end = primitive.attributes.end();
                if (primitive.attributes.find("JOINTS_0") == end || primitive.attributes.find("WEIGHTS_0") == end) {
                    return Error::InvalidGltf;
                }
            }
        }
    }

    for (const auto& scene : parsedAsset->scenes) {
        for (const auto& node : scene.nodeIndices) {
            if (node >= parsedAsset->nodes.size())
                return Error::InvalidGltf;
        }
    }

    for (const auto& skin : parsedAsset->skins) {
        if (skin.joints.empty())
            return Error::InvalidGltf;
        if (skin.skeleton.has_value() && skin.skeleton.value() >= parsedAsset->nodes.size())
            return Error::InvalidGltf;
        if (skin.inverseBindMatrices.has_value() && skin.inverseBindMatrices.value() >= parsedAsset->accessors.size())
            return Error::InvalidGltf;
    }

    for (const auto& texture : parsedAsset->textures) {
        if (texture.samplerIndex.has_value() && texture.samplerIndex.value() >= parsedAsset->samplers.size())
            return Error::InvalidGltf;
        if (texture.imageIndex.has_value() && texture.imageIndex.value() >= parsedAsset->images.size())
            return Error::InvalidGltf;
        if (texture.fallbackImageIndex.has_value() && texture.fallbackImageIndex.value() >= parsedAsset->images.size())
            return Error::InvalidGltf;
    }

    return Error::None;
}

fg::Error fg::glTF::parse(Category categories) {
    using namespace simdjson;
    fillCategories(categories);

    if (!hasBit(options, Options::DontRequireValidAssetMember)) {
        dom::object asset;
        AssetInfo info = {};
        auto error = data->root["asset"].get_object().get(asset);
        if (error == NO_SUCH_FIELD) {
            SET_ERROR_RETURN_ERROR(Error::InvalidOrMissingAssetField)
        } else if (error != SUCCESS) {
            SET_ERROR_RETURN_ERROR(Error::InvalidJson)
        }

        std::string_view version;
        if (asset["version"].get_string().get(version) != SUCCESS) {
            SET_ERROR_RETURN_ERROR(Error::InvalidOrMissingAssetField)
        } else {
            const auto major = static_cast<std::uint32_t>(version.substr(0, 1)[0] - '0');
            // std::uint32_t minor = version.substr(2, 3)[0] - '0';
            if (major != 2) {
                SET_ERROR_RETURN_ERROR(Error::UnsupportedVersion)
            }
        }
        info.gltfVersion = std::string { version };

        std::string_view copyright;
        if (asset["copyright"].get_string().get(copyright) == SUCCESS) {
            info.copyright = std::string { copyright };
        }

        std::string_view generator;
        if (asset["generator"].get_string().get(generator) == SUCCESS) {
            info.generator = std::string { generator };
        }

        parsedAsset->assetInfo = std::move(info);
    }

    dom::array extensionsRequired;
    if (data->root["extensionsRequired"].get_array().get(extensionsRequired) == SUCCESS) {
        for (auto extension : extensionsRequired) {
            std::string_view string;
            if (extension.get_string().get(string) != SUCCESS) {
                SET_ERROR_RETURN_ERROR(Error::InvalidGltf)
            }

            bool known = false;
            for (const auto& [extensionString, extensionEnum] : extensionStrings) {
                if (extensionString == string) {
                    known = true;
                    if (!hasBit(data->config.extensions, extensionEnum)) {
                        // The extension is required, but not enabled by the user.
                        SET_ERROR_RETURN_ERROR(Error::MissingExtensions)
                    }
                    break;
                }
            }
            if (!known) {
                SET_ERROR_RETURN_ERROR(Error::UnknownRequiredExtension)
            }
        }
    }

    Category readCategories = Category::None;
    for (const auto& object : data->root) {
        // We've read everything the user asked for, we can safely exit the loop.
        if (readCategories == categories) {
            break;
        }

        auto hashedKey = crcStringFunction(object.key);
        if (hashedKey == force_consteval<crc32c("scene")>) {
            std::uint64_t defaultScene;
            if (object.value.get_uint64().get(defaultScene) != SUCCESS) {
                errorCode = Error::InvalidGltf;
            }
            parsedAsset->defaultScene = static_cast<std::size_t>(defaultScene);
            continue;
        }

        if (hashedKey == force_consteval<crc32c("extensions")>) {
            dom::object extensionsObject;
            if (object.value.get_object().get(extensionsObject) != SUCCESS) {
                errorCode = Error::InvalidGltf;
                return errorCode;
            }

            parseExtensions(extensionsObject);
            continue;
        }

        if (hashedKey == force_consteval<crc32c("asset")> || hashedKey == force_consteval<crc32c("extras")>) {
            continue;
        }

        dom::array array;
        if (object.value.get_array().get(array) != SUCCESS) {
            errorCode = Error::InvalidGltf;
            return errorCode;
        }

#define KEY_SWITCH_CASE(name, id) case force_consteval<crc32c(FASTGLTF_QUOTE(id))>:       \
                if (hasBit(categories, Category::name))   \
                    parse##name(array);                     \
                readCategories |= Category::name;         \
                break;

        switch (hashedKey) {
            KEY_SWITCH_CASE(Accessors, accessors)
            KEY_SWITCH_CASE(Animations, animations)
            KEY_SWITCH_CASE(Buffers, buffers)
            KEY_SWITCH_CASE(BufferViews, bufferViews)
            KEY_SWITCH_CASE(Cameras, cameras)
            KEY_SWITCH_CASE(Images, images)
            KEY_SWITCH_CASE(Materials, materials)
            KEY_SWITCH_CASE(Meshes, meshes)
            KEY_SWITCH_CASE(Nodes, nodes)
            KEY_SWITCH_CASE(Samplers, samplers)
            KEY_SWITCH_CASE(Scenes, scenes)
            KEY_SWITCH_CASE(Skins, skins)
            KEY_SWITCH_CASE(Textures, textures)
            default:
                break;
        }

#undef KEY_SWITCH_CASE
    }

    parsedAsset->availableCategories = readCategories;

    return errorCode;
}

void fg::glTF::parseAccessors(simdjson::dom::array& accessors) {
    using namespace simdjson;

    parsedAsset->accessors.reserve(accessors.size());
    for (auto accessorValue : accessors) {
        // Required fields: "componentType", "count"
        Accessor accessor = {};
        dom::object accessorObject;
        if (accessorValue.get_object().get(accessorObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t componentType;
        if (accessorObject["componentType"].get_uint64().get(componentType) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        } else {
            accessor.componentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(componentType));
            if (accessor.componentType == ComponentType::Double && !hasBit(options, Options::AllowDouble)) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
        }

        std::string_view accessorType;
        if (accessorObject["type"].get_string().get(accessorType) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        } else {
            accessor.type = getAccessorType(accessorType);
        }

        std::uint64_t accessorCount;
        if (accessorObject["count"].get_uint64().get(accessorCount) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        } else {
            accessor.count = static_cast<std::size_t>(accessorCount);
        }

        std::uint64_t bufferView;
        if (accessorObject["bufferView"].get_uint64().get(bufferView) == SUCCESS) {
            accessor.bufferViewIndex = static_cast<std::size_t>(bufferView);
        }

        // byteOffset is optional, but defaults to 0
        std::uint64_t byteOffset;
        if (accessorObject["byteOffset"].get_uint64().get(byteOffset) != SUCCESS) {
            accessor.byteOffset = 0U;
        } else {
            accessor.byteOffset = static_cast<std::size_t>(byteOffset);
        }

        // Type of min and max should always be the same.
        auto parseMinMax = [&](std::string_view key, decltype(Accessor::max)& ref) -> fastgltf::Error {
            dom::array elements;
            if (accessorObject[key].get_array().get(elements) == SUCCESS) {
                decltype(Accessor::max) variant;
                if (accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double) {
                    auto vec = std::vector<double> {};
                    vec.reserve(getNumComponents(accessor.type));
                    variant = std::move(vec);
                } else {
                    auto vec = std::vector<std::int64_t> {};
                    vec.reserve(getNumComponents(accessor.type));
                    variant = std::move(vec);
                }

                for (auto element : elements) {
                    auto type = element.type();
                    switch (type) {
                        case dom::element_type::DOUBLE: {
                            // We can't safely promote double to ints. Therefore, if the element is a double,
                            // but our component type is not a floating point, that's invalid.
                            if (accessor.componentType != ComponentType::Float && accessor.componentType != ComponentType::Double) {
                                return Error::InvalidGltf;
                            }

                            double value;
                            if (element.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            if (!std::holds_alternative<std::vector<double>>(variant)) {
                                return Error::InvalidGltf;
                            }
                            std::get<std::vector<double>>(variant).emplace_back(value);
                            break;
                        }
                        case dom::element_type::INT64: {
                            std::int64_t value;
                            if (element.get_int64().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }

                            if (std::holds_alternative<std::vector<double>>(variant)) {
                                std::get<std::vector<double>>(variant).emplace_back(static_cast<double>(value));
                            } else if (std::holds_alternative<std::vector<std::int64_t>>(variant)) {
                                std::get<std::vector<std::int64_t>>(variant).emplace_back(static_cast<std::int64_t>(value));
                            } else {
                                return Error::InvalidGltf;
                            }
                            break;
                        }
                        case dom::element_type::UINT64: {
                            // Note that the glTF spec doesn't care about any integer larger than 32-bits, so
                            // truncating uint64 to int64 wouldn't make any difference, as those large values
                            // aren't allowed anyway.
                            std::uint64_t value;
                            if (element.get_uint64().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }

                            if (std::holds_alternative<std::vector<double>>(variant)) {
                                std::get<std::vector<double>>(variant).emplace_back(static_cast<double>(value));
                            } else if (std::holds_alternative<std::vector<std::int64_t>>(variant)) {
                                std::get<std::vector<std::int64_t>>(variant).emplace_back(static_cast<std::int64_t>(value));
                            } else {
                                return Error::InvalidGltf;
                            }
                            break;
                        }
                        default: return Error::InvalidGltf;
                    }
                }
                ref = std::move(variant);
            }
            return Error::None;
        };

        if (auto error = parseMinMax("max", accessor.max); error != Error::None) {
            SET_ERROR_RETURN(error)
        }
        if (auto error = parseMinMax("min", accessor.min); error != Error::None) {
            SET_ERROR_RETURN(error)
        }

        if (accessorObject["normalized"].get_bool().get(accessor.normalized) != SUCCESS) {
            accessor.normalized = false;
        }

        dom::object sparseAccessorObject;
        if (accessorObject["sparse"].get_object().get(sparseAccessorObject) == SUCCESS) {
            SparseAccessor sparse = {};
            std::uint64_t value;
            dom::object child;
            if (sparseAccessorObject["count"].get_uint64().get(value) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sparse.count = static_cast<std::size_t>(value);

            // Accessor Sparce Indices
            if (sparseAccessorObject["indices"].get_object().get(child) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sparse.indicesBufferView = static_cast<std::size_t>(value);

            if (child["byteOffset"].get_uint64().get(value) != SUCCESS) {
                sparse.indicesByteOffset = 0;
            } else {
                sparse.indicesByteOffset = static_cast<std::size_t>(value);
            }

            if (child["componentType"].get_uint64().get(value) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sparse.indexComponentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(value));

            // Accessor Sparse Values
            if (sparseAccessorObject["values"].get_object().get(child) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sparse.valuesBufferView = static_cast<std::size_t>(value);

            if (child["byteOffset"].get_uint64().get(value) != SUCCESS) {
                sparse.valuesByteOffset = 0;
            } else {
                sparse.valuesByteOffset = static_cast<std::size_t>(value);
            }

            accessor.sparse = sparse;
        }

        // name is optional.
        std::string_view name;
        if (accessorObject["name"].get_string().get(name) == SUCCESS) {
            accessor.name = std::string { name };
        }

        parsedAsset->accessors.emplace_back(std::move(accessor));
    }
}

void fg::glTF::parseAnimations(simdjson::dom::array& animations) {
    using namespace simdjson;

    parsedAsset->animations.reserve(animations.size());
    for (auto animationValue : animations) {
        dom::object animationObject;
        Animation animation = {};
        if (animationValue.get_object().get(animationObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        dom::array channels;
        auto channelError = getJsonArray(animationObject, "channels", &channels);
        if (channelError != Error::None) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        animation.channels.reserve(channels.size());
        for (auto channelValue : channels) {
            dom::object channelObject;
            AnimationChannel channel = {};
            if (channelValue.get_object().get(channelObject) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            std::uint64_t sampler;
            if (channelObject["sampler"].get_uint64().get(sampler) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            channel.samplerIndex = static_cast<std::size_t>(sampler);

            dom::object targetObject;
            if (channelObject["target"].get_object().get(targetObject) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            } else {
                std::uint64_t node;
                if (targetObject["node"].get_uint64().get(node) != SUCCESS) {
                    // We don't support any extensions for animations, so it is required.
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
                channel.nodeIndex = static_cast<std::size_t>(node);

                std::string_view path;
                if (targetObject["path"].get_string().get(path) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }

                if (path == "translation") {
                    channel.path = AnimationPath::Translation;
                } else if (path == "rotation") {
                    channel.path = AnimationPath::Rotation;
                } else if (path == "scale") {
                    channel.path = AnimationPath::Scale;
                } else if (path == "weights") {
                    channel.path = AnimationPath::Weights;
                }
            }

            animation.channels.emplace_back(channel);
        }

        dom::array samplers;
        auto samplerError = getJsonArray(animationObject, "samplers", &samplers);
        if (samplerError != Error::None) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        animation.samplers.reserve(samplers.size());
        for (auto samplerValue : samplers) {
            dom::object samplerObject;
            AnimationSampler sampler = {};
            if (samplerValue.get_object().get(samplerObject) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            std::uint64_t input;
            if (samplerObject["input"].get_uint64().get(input) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sampler.inputAccessor = static_cast<std::size_t>(input);

            std::uint64_t output;
            if (samplerObject["output"].get_uint64().get(output) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            sampler.outputAccessor = static_cast<std::size_t>(output);

            std::string_view interpolation;
            if (samplerObject["interpolation"].get_string().get(interpolation) != SUCCESS) {
                sampler.interpolation = AnimationInterpolation::Linear;
            } else {
                if (interpolation == "LINEAR") {
                    sampler.interpolation = AnimationInterpolation::Linear;
                } else if (interpolation == "STEP") {
                    sampler.interpolation = AnimationInterpolation::Step;
                } else if (interpolation == "CUBICSPLINE") {
                    sampler.interpolation = AnimationInterpolation::CubicSpline;
                } else {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
            }

            animation.samplers.emplace_back(sampler);
        }

        // name is optional.
        {
            std::string_view name;
            if (animationObject["name"].get_string().get(name) == SUCCESS) {
                animation.name = std::string { name };
            }
        }

        parsedAsset->animations.emplace_back(std::move(animation));
    }
}

void fg::glTF::parseBuffers(simdjson::dom::array& buffers) {
    using namespace simdjson;

    parsedAsset->buffers.reserve(buffers.size());
    std::size_t bufferIndex = 0;
    for (auto bufferValue : buffers) {
        // Required fields: "byteLength"
        Buffer buffer = {};
        dom::object bufferObject;
        if (bufferValue.get_object().get(bufferObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t byteLength;
        if (bufferObject["byteLength"].get_uint64().get(byteLength) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        } else {
            buffer.byteLength = static_cast<std::size_t>(byteLength);
        }

        // When parsing GLB, there's a buffer object that will point to the BUF chunk in the
        // file. Otherwise, data must be specified in the "uri" field.
        std::string_view uriString;
        if (bufferObject["uri"].get_string().get(uriString) == SUCCESS) {
            URI uri(uriString);
            if (!uri.valid()) {
                SET_ERROR_RETURN(Error::InvalidURI)
            }

            if (uri.isDataUri()) {
                auto [error, source] = decodeDataUri(uri);
                if (error != Error::None) {
                    SET_ERROR_RETURN(error)
                }

                buffer.data = std::move(source);
            } else if (uri.isLocalPath() && hasBit(options, Options::LoadExternalBuffers)) {
                auto [error, source] = loadFileFromUri(uri);
                if (error != Error::None) {
                    SET_ERROR_RETURN(error)
                }

                buffer.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = std::move(uri);
                buffer.data = std::move(filePath);
            }
        } else if (bufferIndex == 0 && !std::holds_alternative<std::monostate>(glbBuffer)) {
            buffer.data = std::move(glbBuffer);
        } else {
            // All other buffers have to contain an uri field.
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        if (std::holds_alternative<std::monostate>(buffer.data)) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        // name is optional.
        std::string_view name;
        if (bufferObject["name"].get_string().get(name) == SUCCESS) {
            buffer.name = std::string { name };
        }

        ++bufferIndex;
        parsedAsset->buffers.emplace_back(std::move(buffer));
    }
}

void fg::glTF::parseBufferViews(simdjson::dom::array& bufferViews) {
    using namespace simdjson;

    parsedAsset->bufferViews.reserve(bufferViews.size());
    for (auto bufferViewValue : bufferViews) {
        dom::object bufferViewObject;
        if (bufferViewValue.get_object().get(bufferViewObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t number;
        BufferView view;
        if (auto error = bufferViewObject["buffer"].get_uint64().get(number); error != SUCCESS) {
            SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
        }
        view.bufferIndex = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteOffset"].get_uint64().get(number); error == SUCCESS) {
            view.byteOffset = static_cast<std::size_t>(number);
        } else if (error == NO_SUCH_FIELD) {
            view.byteOffset = 0;
        } else {
            SET_ERROR_RETURN(Error::InvalidJson)
        }

        if (auto error = bufferViewObject["byteLength"].get_uint64().get(number); error != SUCCESS) {
            SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
        }
        view.byteLength = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteStride"].get_uint64().get(number); error == SUCCESS) {
            view.byteStride = static_cast<std::size_t>(number);
        } else if (error != NO_SUCH_FIELD) {
            SET_ERROR_RETURN(Error::InvalidJson)
        }

        if (auto error = bufferViewObject["target"].get_uint64().get(number); error == SUCCESS) {
            view.target = static_cast<BufferTarget>(number);
        } else if (error != NO_SUCH_FIELD) {
            SET_ERROR_RETURN(Error::InvalidJson)
        }

        std::string_view string;
        if (auto error = bufferViewObject["name"].get_string().get(string); error == SUCCESS) {
            view.name = std::string { string };
        } else if (error != NO_SUCH_FIELD) {
            SET_ERROR_RETURN(Error::InvalidJson)
        }

        dom::object extensionObject;
        if (bufferViewObject["extensions"].get_object().get(extensionObject) == SUCCESS) {
            dom::object meshoptCompression;
            if (hasBit(data->config.extensions, Extensions::EXT_meshopt_compression) && bufferViewObject[extensions::EXT_meshopt_compression].get_object().get(meshoptCompression) == SUCCESS) {
                auto compression = std::make_unique<CompressedBufferView>();

                if (auto error = bufferViewObject["buffer"].get_uint64().get(number); error != SUCCESS) {
                    SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
                }
                compression->bufferIndex = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["byteOffset"].get_uint64().get(number); error == SUCCESS) {
                    compression->byteOffset = static_cast<std::size_t>(number);
                } else if (error == NO_SUCH_FIELD) {
                    compression->byteOffset = 0;
                } else {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }

                if (auto error = bufferViewObject["byteLength"].get_uint64().get(number); error != SUCCESS) {
                    SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
                }
                compression->byteLength = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["byteStride"].get_uint64().get(number); error != SUCCESS) {
                    SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
                }
                compression->byteStride = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["count"].get_uint64().get(number); error != SUCCESS) {
                    SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
                }
                compression->count = number;

                if (auto error = bufferViewObject["mode"].get_string().get(string); error != SUCCESS) {
                    SET_ERROR_RETURN(error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson)
                }
                switch (crcStringFunction(string)) {
                    case force_consteval<crc32c("ATTRIBUTES")>: {
                        compression->mode = MeshoptCompressionMode::Attributes;
                        break;
                    }
                    case force_consteval<crc32c("TRIANGLES")>: {
                        compression->mode = MeshoptCompressionMode::Triangles;
                        break;
                    }
                    case force_consteval<crc32c("INDICES")>: {
                        compression->mode = MeshoptCompressionMode::Indices;
                        break;
                    }
                    default: {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                }

                if (auto error = bufferViewObject["filter"].get_string().get(string); error == SUCCESS) {
                    switch (crcStringFunction(string)) {
                        case force_consteval<crc32c("NONE")>: {
                            compression->filter = MeshoptCompressionFilter::None;
                            break;
                        }
                        case force_consteval<crc32c("OCTAHEDRAL")>: {
                            compression->filter = MeshoptCompressionFilter::Octahedral;
                            break;
                        }
                        case force_consteval<crc32c("QUATERNION")>: {
                            compression->filter = MeshoptCompressionFilter::Quaternion;
                            break;
                        }
                        case force_consteval<crc32c("EXPONENTIAL")>: {
                            compression->filter = MeshoptCompressionFilter::Exponential;
                            break;
                        }
                        default: {
                            SET_ERROR_RETURN(Error::InvalidGltf)
                        }
                    }
                } else if (error == NO_SUCH_FIELD) {
                    compression->filter = MeshoptCompressionFilter::None;
                } else {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }

                view.meshoptCompression = std::move(compression);
            }
        }

        parsedAsset->bufferViews.emplace_back(std::move(view));
    }
}

void fg::glTF::parseCameras(simdjson::dom::array& cameras) {
    using namespace simdjson;

    parsedAsset->cameras.reserve(cameras.size());
    for (auto cameraValue : cameras) {
        Camera camera = {};
        dom::object cameraObject;
        if (cameraValue.get_object().get(cameraObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::string_view name;
        if (cameraObject["name"].get_string().get(name) == SUCCESS) {
            camera.name = std::string { name };
        }

        std::string_view type;
        if (cameraObject["type"].get_string().get(type) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        if (type == "perspective") {
            dom::object perspectiveCamera;
            if (cameraObject["perspective"].get_object().get(perspectiveCamera) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            Camera::Perspective perspective = {};
            double value;
            if (perspectiveCamera["aspectRatio"].get_double().get(value) == SUCCESS) {
                perspective.aspectRatio = static_cast<float>(value);
            }
            if (perspectiveCamera["zfar"].get_double().get(value) == SUCCESS) {
                perspective.zfar = static_cast<float>(value);
            }

            if (perspectiveCamera["yfov"].get_double().get(value) == SUCCESS) {
                perspective.yfov = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (perspectiveCamera["znear"].get_double().get(value) == SUCCESS) {
                perspective.znear = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            camera.camera = perspective;
        } else if (type == "orthographic") {
            dom::object orthographicCamera;
            if (cameraObject["orthographic"].get_object().get(orthographicCamera) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            Camera::Orthographic orthographic = {};
            double value;
            if (orthographicCamera["xmag"].get_double().get(value) == SUCCESS) {
                orthographic.xmag = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (orthographicCamera["ymag"].get_double().get(value) == SUCCESS) {
                orthographic.ymag = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (orthographicCamera["zfar"].get_double().get(value) == SUCCESS) {
                orthographic.zfar = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            if (orthographicCamera["znear"].get_double().get(value) == SUCCESS) {
                orthographic.znear = static_cast<float>(value);
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            camera.camera = orthographic;
        } else {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        parsedAsset->cameras.emplace_back(std::move(camera));
    }
}

void fg::glTF::parseExtensions(simdjson::dom::object& extensionsObject) {
    using namespace simdjson;

    for (auto extensionValue : extensionsObject) {
        dom::object extensionObject;
        if (auto error = extensionValue.value.get_object().get(extensionObject); error != SUCCESS) {
            if (error == INCORRECT_TYPE) {
                continue; // We want to ignore
            }
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        auto hash = crcStringFunction(extensionValue.key);
        switch (hash) {
            case force_consteval<crc32c(extensions::KHR_lights_punctual)>: {
                if (!hasBit(data->config.extensions, Extensions::KHR_lights_punctual))
                    break;

                dom::array lightsArray;
                if (auto error = extensionObject["lights"].get_array().get(lightsArray); error == SUCCESS) {
                    parseLights(lightsArray);
                } else if (error != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
                break;
            }
        }
    }
}

void fg::glTF::parseImages(simdjson::dom::array& images) {
    using namespace simdjson;

    parsedAsset->images.reserve(images.size());
    for (auto imageValue : images) {
        Image image = {};
        dom::object imageObject;
        if (imageValue.get_object().get(imageObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::string_view uriString;
        if (imageObject["uri"].get_string().get(uriString) == SUCCESS) {
            if (imageObject["bufferView"].error() == SUCCESS) {
                // If uri is declared, bufferView cannot be declared.
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            URI uri(uriString);
            if (!uri.valid()) {
                SET_ERROR_RETURN(Error::InvalidURI)
            }

            if (uri.isDataUri()) {
                auto [error, source] = decodeDataUri(uri);
                if (error != Error::None) {
                    SET_ERROR_RETURN(error)
                }

                image.data = std::move(source);
            } else if (uri.isLocalPath() && hasBit(options, Options::LoadExternalImages)) {
                auto [error, source] = loadFileFromUri(uri);
                if (error != Error::None) {
                    SET_ERROR_RETURN(error)
                }

                image.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = std::move(uri);
                image.data = std::move(filePath);
            }

            std::string_view mimeType;
            if (imageObject["mimeType"].get_string().get(mimeType) == SUCCESS) {
                std::visit([&](auto& arg) {
                    using T = std::decay_t<decltype(arg)>;

                    // This is kinda cursed
                    if constexpr (is_any<T, sources::CustomBuffer, sources::BufferView, sources::URI, sources::Vector>()) {
                        arg.mimeType = getMimeTypeFromString(mimeType);
                    }
                }, image.data);
            }
        }

        std::uint64_t bufferViewIndex;
        if (imageObject["bufferView"].get_uint64().get(bufferViewIndex) == SUCCESS) {
            std::string_view mimeType;
            if (imageObject["mimeType"].get_string().get(mimeType) != SUCCESS) {
                // If bufferView is defined, mimeType needs to also be defined.
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            image.data = sources::BufferView {
                static_cast<std::size_t>(bufferViewIndex),
                getMimeTypeFromString(mimeType),
            };
        }

        if (std::holds_alternative<std::monostate>(image.data)) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        // name is optional.
        std::string_view name;
        if (imageObject["name"].get_string().get(name) == SUCCESS) {
            image.name = std::string { name };
        }

        parsedAsset->images.emplace_back(std::move(image));
    }
}

void fg::glTF::parseLights(simdjson::dom::array& lights) {
    using namespace simdjson;

    parsedAsset->lights.reserve(lights.size());
    for (auto lightValue : lights) {
        dom::object lightObject;
        if (lightValue.get_object().get(lightObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }
        Light light = {};

        std::string_view type;
        if (lightObject["type"].get_string().get(type) == SUCCESS) {
            switch (crcStringFunction(type.data())) {
                case force_consteval<crc32c("directional")>: {
                    light.type = LightType::Directional;
                    break;
                }
                case force_consteval<crc32c("spot")>: {
                    light.type = LightType::Spot;
                    break;
                }
                case force_consteval<crc32c("point")>: {
                    light.type = LightType::Point;
                    break;
                }
                default: {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
            }
        } else {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        if (light.type == LightType::Spot) {
            dom::object spotObject;
            if (lightObject["spot"].get_object().get(spotObject) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }

            double innerConeAngle;
            if (lightObject["innerConeAngle"].get_double().get(innerConeAngle) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            light.innerConeAngle = static_cast<float>(innerConeAngle);

            double outerConeAngle;
            if (lightObject["outerConeAngle"].get_double().get(outerConeAngle) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            light.outerConeAngle = static_cast<float>(outerConeAngle);
        }

        dom::array colorArray;
        if (lightObject["color"].get_array().get(colorArray) == SUCCESS) {
            if (colorArray.size() != 3U) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            for (std::size_t i = 0U; i < colorArray.size(); ++i) {
                double color;
                if (colorArray.at(i).get_double().get(color) == SUCCESS) {
                    light.color[i] = static_cast<float>(color);
                } else {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
            }
        }

        double intensity;
        if (lightObject["intensity"].get_double().get(intensity) == SUCCESS) {
            light.intensity = static_cast<float>(intensity);
        } else {
            light.intensity = 0.0;
        }

        double range;
        if (lightObject["range"].get_double().get(range) == SUCCESS) {
            light.range = static_cast<float>(range);
        }

        std::string_view name;
        if (lightObject["name"].get_string().get(name) == SUCCESS) {
            light.name = std::string { name };
        }

        parsedAsset->lights.emplace_back(std::move(light));
    }
}

void fg::glTF::parseMaterials(simdjson::dom::array& materials) {
    using namespace simdjson;

    parsedAsset->materials.reserve(materials.size());
    for (auto materialValue : materials) {
        dom::object materialObject;
        if (materialValue.get_object().get(materialObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }
        Material material = {};

        dom::array emissiveFactor;
        if (materialObject["emissiveFactor"].get_array().get(emissiveFactor) == SUCCESS) {
            if (emissiveFactor.size() != 3) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            for (auto i = 0U; i < 3; ++i) {
                double val;
                if (emissiveFactor.at(i).get_double().get(val) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
                material.emissiveFactor[i] = static_cast<float>(val);
            }
        } else {
            material.emissiveFactor = {{ 0, 0, 0 }};
        }

        TextureInfo textureObject = {};
        if (auto error = parseTextureObject(materialObject, "normalTexture", &textureObject, data->config.extensions); error == Error::None) {
            material.normalTexture = std::move(textureObject);
        } else if (error != Error::MissingField) {
            SET_ERROR_RETURN(error)
        }

        if (auto error = parseTextureObject(materialObject, "occlusionTexture", &textureObject, data->config.extensions); error == Error::None) {
            material.occlusionTexture = std::move(textureObject);
        } else if (error != Error::MissingField) {
            SET_ERROR_RETURN(error)
        }

        if (auto error = parseTextureObject(materialObject, "emissiveTexture", &textureObject, data->config.extensions); error == Error::None) {
            material.emissiveTexture = std::move(textureObject);
        } else if (error != Error::MissingField) {
            SET_ERROR_RETURN(error)
        }

        dom::object pbrMetallicRoughness;
        if (materialObject["pbrMetallicRoughness"].get_object().get(pbrMetallicRoughness) == SUCCESS) {
            PBRData pbr = {};

            dom::array baseColorFactor;
            if (pbrMetallicRoughness["baseColorFactor"].get_array().get(baseColorFactor) == SUCCESS) {
                for (auto i = 0U; i < 4; ++i) {
                    double val;
                    if (baseColorFactor.at(i).get_double().get(val) != SUCCESS) {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                    pbr.baseColorFactor[i] = static_cast<float>(val);
                }
            } else {
                pbr.baseColorFactor = {{ 1, 1, 1, 1 }};
            }

            double factor;
            if (pbrMetallicRoughness["metallicFactor"].get_double().get(factor) == SUCCESS) {
                pbr.metallicFactor = static_cast<float>(factor);
            } else {
                pbr.metallicFactor = 1.0F;
            }
            if (pbrMetallicRoughness["roughnessFactor"].get_double().get(factor) == SUCCESS) {
                pbr.roughnessFactor = static_cast<float>(factor);
            } else {
                pbr.roughnessFactor = 1.0F;
            }

            if (auto error = parseTextureObject(pbrMetallicRoughness, "baseColorTexture", &textureObject, data->config.extensions); error == Error::None) {
                pbr.baseColorTexture = std::move(textureObject);
            } else if (error != Error::MissingField) {
                SET_ERROR_RETURN(error)
            }

            if (auto error = parseTextureObject(pbrMetallicRoughness, "metallicRoughnessTexture", &textureObject, data->config.extensions); error == Error::None) {
                pbr.metallicRoughnessTexture = std::move(textureObject);
            } else if (error != Error::MissingField) {
                SET_ERROR_RETURN(error)
            }

            material.pbrData = std::move(pbr);
        }

        std::string_view alphaMode;
        if (materialObject["alphaMode"].get_string().get(alphaMode) == SUCCESS) {
            if (alphaMode == "OPAQUE") {
                material.alphaMode = AlphaMode::Opaque;
            } else if (alphaMode == "MASK") {
                material.alphaMode = AlphaMode::Mask;
            } else if (alphaMode == "BLEND") {
                material.alphaMode = AlphaMode::Blend;
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
        } else {
            material.alphaMode = AlphaMode::Opaque;
        }

        double alphaCutoff = 0.5;
        if (materialObject["alphaCutoff"].get_double().get(alphaCutoff) == SUCCESS) {
            material.alphaCutoff = static_cast<float>(alphaCutoff);
        } else {
            material.alphaCutoff = 0.5F;
        }

        bool doubleSided = false;
        if (materialObject["doubleSided"].get_bool().get(doubleSided) == SUCCESS) {
            material.doubleSided = doubleSided;
        } else {
            material.doubleSided = false;
        }

        std::string_view name;
        if (materialObject["name"].get_string().get(name) == SUCCESS) {
            material.name = std::string { name };
        }

        material.unlit = false;
        dom::object extensionsObject;
        if (auto extensionError = materialObject["extensions"].get_object().get(extensionsObject); extensionError == SUCCESS) {
            if (hasBit(data->config.extensions, Extensions::KHR_materials_clearcoat)) {
                dom::object clearcoatObject;
                auto clearcoatError = extensionsObject[extensions::KHR_materials_clearcoat].get_object().get(clearcoatObject);
                if (clearcoatError == SUCCESS) {
                    auto clearcoat = std::make_unique<MaterialClearcoat>();

                    double clearcoatFactor;
                    if (auto error = clearcoatObject["clearcoatFactor"].get_double().get(clearcoatFactor); error == SUCCESS) {
                        clearcoat->clearcoatFactor = static_cast<float>(clearcoatFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        clearcoat->clearcoatFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo clearcoatTexture;
                    if (auto error = parseTextureObject(clearcoatObject, "clearcoatTexture", &clearcoatTexture, data->config.extensions); error == Error::None) {
                        clearcoat->clearcoatTexture = std::move(clearcoatTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    double clearcoatRoughnessFactor;
                    if (auto error = clearcoatObject["clearcoatRoughnessFactor"].get_double().get(clearcoatRoughnessFactor); error == SUCCESS) {
                        clearcoat->clearcoatRoughnessFactor = static_cast<float>(clearcoatRoughnessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        clearcoat->clearcoatRoughnessFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo clearcoatRoughnessTexture;
                    if (auto error = parseTextureObject(clearcoatObject, "clearcoatRoughnessTexture", &clearcoatRoughnessTexture, data->config.extensions); error == Error::None) {
                        clearcoat->clearcoatRoughnessTexture = std::move(clearcoatRoughnessTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    TextureInfo clearcoatNormalTexture;
                    if (auto error = parseTextureObject(clearcoatObject, "clearcoatNormalTexture", &clearcoatNormalTexture, data->config.extensions); error == Error::None) {
                        clearcoat->clearcoatNormalTexture = std::move(clearcoatNormalTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    material.clearcoat = std::move(clearcoat);
                } else if (clearcoatError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_emissive_strength)) {
                dom::object emissiveObject;
                auto emissiveError = extensionsObject[extensions::KHR_materials_emissive_strength].get_object().get(emissiveObject);
                if (emissiveError == SUCCESS) {
                    double emissiveStrength;
                    auto error = emissiveObject["emissiveStrength"].get_double().get(emissiveStrength);
                    if (error == SUCCESS) {
                        material.emissiveStrength = static_cast<float>(emissiveStrength);
                    } else if (error == NO_SUCH_FIELD) {
                        material.emissiveStrength = 1.0f;
                    }
                } else if (emissiveError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_ior)) {
                dom::object iorObject;
                auto iorError = extensionsObject[extensions::KHR_materials_ior].get_object().get(iorObject);
                if (iorError == SUCCESS) {
                    double ior;
                    auto error = iorObject["ior"].get_double().get(ior);
                    if (error == SUCCESS) {
                        material.ior = static_cast<float>(ior);
                    } else if (error == NO_SUCH_FIELD) {
                        material.ior = 1.5f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidJson)
                    }
                } else if (iorError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_iridescence)) {
                dom::object iridescenceObject;
                auto iridescenceError = extensionsObject[extensions::KHR_materials_iridescence].get_object().get(iridescenceObject);
                if (iridescenceError == SUCCESS) {
                    auto iridescence = std::make_unique<MaterialIridescence>();

                    double iridescenceFactor;
                    if (auto error = iridescenceObject["iridescenceFactor"].get_double().get(iridescenceFactor); error == SUCCESS) {
                        iridescence->iridescenceFactor = static_cast<float>(iridescenceFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo iridescenceTexture;
                    if (auto error = parseTextureObject(iridescenceObject, "specularTexture", &iridescenceTexture, data->config.extensions); error == Error::None) {
                        iridescence->iridescenceTexture = std::move(iridescenceTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    double iridescenceIor;
                    if (auto error = iridescenceObject["iridescenceIor"].get_double().get(iridescenceIor); error == SUCCESS) {
                        iridescence->iridescenceIor = static_cast<float>(iridescenceIor);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceIor = 1.3f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    double iridescenceThicknessMinimum;
                    if (auto error = iridescenceObject["iridescenceThicknessMinimum"].get_double().get(iridescenceThicknessMinimum); error == SUCCESS) {
                        iridescence->iridescenceThicknessMinimum = static_cast<float>(iridescenceThicknessMinimum);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceThicknessMinimum = 100.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    double iridescenceThicknessMaximum;
                    if (auto error = iridescenceObject["iridescenceThicknessMaximum"].get_double().get(iridescenceThicknessMaximum); error == SUCCESS) {
                        iridescence->iridescenceThicknessMaximum = static_cast<float>(iridescenceThicknessMaximum);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceThicknessMaximum = 400.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo iridescenceThicknessTexture;
                    if (auto error = parseTextureObject(iridescenceObject, "specularTexture", &iridescenceThicknessTexture, data->config.extensions); error == Error::None) {
                        iridescence->iridescenceThicknessTexture = std::move(iridescenceThicknessTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    material.iridescence = std::move(iridescence);
                } else if (iridescenceError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_sheen)) {
                dom::object sheenObject;
                auto sheenError = extensionsObject[extensions::KHR_materials_sheen].get_object().get(sheenObject);
                if (sheenError == SUCCESS) {
                    auto sheen = std::make_unique<MaterialSheen>();

                    dom::array sheenColorFactor;
                    if (auto error = sheenObject["sheenColorFactor"].get_array().get(sheenColorFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : sheenColorFactor) {
                            if (i >= sheen->sheenColorFactor.size()) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            sheen->sheenColorFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        sheen->sheenColorFactor = std::array<float, 3>{{ 0.0f, 0.0f, 0.0f }};
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo sheenColorTexture;
                    if (auto error = parseTextureObject(sheenObject, "sheenColorTexture", &sheenColorTexture, data->config.extensions); error == Error::None) {
                        sheen->sheenColorTexture = std::move(sheenColorTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    double sheenRoughnessFactor;
                    if (auto error = sheenObject["sheenRoughnessFactor"].get_double().get(sheenRoughnessFactor); error == SUCCESS) {
                        sheen->sheenRoughnessFactor = static_cast<float>(sheenRoughnessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        sheen->sheenRoughnessFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo sheenRoughnessTexture;
                    if (auto error = parseTextureObject(sheenObject, "sheenRoughnessTexture", &sheenRoughnessTexture, data->config.extensions); error == Error::None) {
                        sheen->sheenRoughnessTexture = std::move(sheenRoughnessTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    material.sheen = std::move(sheen);
                } else if (sheenError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_specular)) {
                dom::object specularObject;
                auto specularError = extensionsObject[extensions::KHR_materials_specular].get_object().get(specularObject);
                if (specularError == SUCCESS) {
                    auto specular = std::make_unique<MaterialSpecular>();

                    double specularFactor;
                    if (auto error = specularObject["specularFactor"].get_double().get(specularFactor); error == SUCCESS) {
                        specular->specularFactor = static_cast<float>(specularFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        specular->specularFactor = 1.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo specularTexture;
                    if (auto error = parseTextureObject(specularObject, "specularTexture", &specularTexture, data->config.extensions); error == Error::None) {
                        specular->specularTexture = std::move(specularTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    dom::array specularColorFactor;
                    if (auto error = specularObject["specularColorFactor"].get_array().get(specularColorFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : specularColorFactor) {
                            if (i >= specular->specularColorFactor.size()) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            specular->specularColorFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        specular->specularColorFactor = std::array<float, 3>{{ 1.0f, 1.0f, 1.0f }};
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo specularColorTexture;
                    if (auto error = parseTextureObject(specularObject, "specularColorTexture", &specularColorTexture, data->config.extensions); error == Error::None) {
                        specular->specularColorTexture = std::move(specularColorTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    material.specular = std::move(specular);
                } else if (specularError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_transmission)) {
                dom::object transmissionObject;
                auto transmissionError = extensionsObject[extensions::KHR_materials_transmission].get_object().get(transmissionObject);
                if (transmissionError == SUCCESS) {
                    auto transmission = std::make_unique<MaterialTransmission>();

                    double transmissionFactor;
                    if (auto error = transmissionObject["transmissionFactor"].get_double().get(transmissionFactor); error == SUCCESS) {
                        transmission->transmissionFactor = static_cast<float>(transmissionFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        transmission->transmissionFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo transmissionTexture;
                    if (auto error = parseTextureObject(transmissionObject, "transmissionTexture", &transmissionTexture, data->config.extensions); error == Error::None) {
                        transmission->transmissionTexture = std::move(transmissionTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    material.transmission = std::move(transmission);
                } else if (transmissionError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_unlit)) {
                dom::object unlitObject;
                auto unlitError = extensionsObject[extensions::KHR_materials_unlit].get_object().get(unlitObject);
                if (unlitError == SUCCESS) {
                    material.unlit = true;
                } else if (unlitError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
            }

            if (hasBit(data->config.extensions, Extensions::KHR_materials_volume)) {
                dom::object volumeObject;
                auto volumeError = extensionsObject[extensions::KHR_materials_volume].get_object().get(volumeObject);
                if (volumeError == SUCCESS) {
                    auto volume = std::make_unique<MaterialVolume>();

                    double thicknessFactor;
                    if (auto error = volumeObject["thicknessFactor"].get_double().get(thicknessFactor); error == SUCCESS) {
                        volume->thicknessFactor = static_cast<float>(thicknessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        volume->thicknessFactor = 0.0f;
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    TextureInfo thicknessTexture;
                    if (auto error = parseTextureObject(volumeObject, "thicknessTexture", &thicknessTexture, data->config.extensions); error == Error::None) {
                        volume->thicknessTexture = std::move(thicknessTexture);
                    } else if (error != Error::MissingField) {
                        SET_ERROR_RETURN(error)
                    }

                    double attenuationDistance;
                    if (auto error = volumeObject["attenuationDistance"].get_double().get(attenuationDistance); error == SUCCESS) {
                        volume->attenuationDistance = static_cast<float>(attenuationDistance);
                    } else if (error == NO_SUCH_FIELD) {
                        volume->attenuationDistance = +std::numeric_limits<float>::infinity();
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    dom::array attenuationColor;
                    if (auto error = volumeObject["attenuationColor"].get_array().get(attenuationColor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : attenuationColor) {
                            if (i >= volume->attenuationColor.size()) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                SET_ERROR_RETURN(Error::InvalidGltf)
                            }
                            (volume->attenuationColor)[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        volume->attenuationColor = std::array<float, 3>{{1.0f, 1.0f, 1.0f}};
                    } else {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }

                    material.volume = std::move(volume);
                } else if (volumeError != NO_SUCH_FIELD) {
                    SET_ERROR_RETURN(Error::InvalidJson)
                }
            }
        } else if (extensionError != NO_SUCH_FIELD) {
            SET_ERROR_RETURN(Error::InvalidJson)
        }

        parsedAsset->materials.emplace_back(std::move(material));
    }
}

void fg::glTF::parseMeshes(simdjson::dom::array& meshes) {
    using namespace simdjson;

    parsedAsset->meshes.reserve(meshes.size());
    for (auto meshValue : meshes) {
        // Required fields: "primitives"
        dom::object meshObject;
        if (meshValue.get_object().get(meshObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }
        Mesh mesh = {};

        dom::array array;
        auto meshError = getJsonArray(meshObject, "primitives", &array);
        if (meshError == Error::MissingField) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        } else if (meshError != Error::None) {
            SET_ERROR_RETURN(meshError)
        } else {
            mesh.primitives.reserve(array.size());
            for (auto primitiveValue : array) {
                // Required fields: "attributes"
                Primitive primitive = {};
                dom::object primitiveObject;
                if (primitiveValue.get_object().get(primitiveObject) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }

                auto parseAttributes = [](dom::object& object, std::unordered_map<std::string, std::size_t>& map) -> auto {
                    // We iterate through the JSON object and write each key/pair value into the
                    // attributes map. The keys are only validated in the validate() method.
                    for (const auto& field : object) {
                        const auto key = field.key;

                        std::uint64_t attributeIndex;
                        if (field.value.get_uint64().get(attributeIndex) != SUCCESS) {
                            return Error::InvalidGltf;
                        }
                        map[std::string { key }] = static_cast<std::size_t>(attributeIndex);
                    }
                    return Error::None;
                };

                dom::object attributesObject;
                if (primitiveObject["attributes"].get_object().get(attributesObject) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
                parseAttributes(attributesObject, primitive.attributes);

                dom::array targets;
                if (primitiveObject["targets"].get_array().get(targets) == SUCCESS) {
                    for (auto targetValue : targets) {
                        if (targetValue.get_object().get(attributesObject) != SUCCESS) {
                            SET_ERROR_RETURN(Error::InvalidGltf)
                        }
                        auto& map = primitive.targets.emplace_back();
                        parseAttributes(attributesObject, map);
                    }
                }

                // Mode shall default to 4.
                std::uint64_t value;
                if (primitiveObject["mode"].get_uint64().get(value) != SUCCESS) {
                    primitive.type = PrimitiveType::Triangles;
                } else {
                    primitive.type = static_cast<PrimitiveType>(value);
                }

                if (primitiveObject["indices"].get_uint64().get(value) == SUCCESS) {
                    primitive.indicesAccessor = static_cast<std::size_t>(value);
                }

                if (primitiveObject["material"].get_uint64().get(value) == SUCCESS) {
                    primitive.materialIndex = static_cast<std::size_t>(value);
                }

                mesh.primitives.emplace_back(std::move(primitive));
            }
        }

        if (meshError = getJsonArray(meshObject, "weights", &array); meshError == Error::None) {
            mesh.weights.reserve(array.size());
            for (auto weightValue : array) {
                double val;
                if (weightValue.get_double().get(val) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }
                mesh.weights.emplace_back(static_cast<float>(val));
            }
        } else if (meshError != Error::MissingField && meshError != Error::None) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        // name is optional.
        std::string_view name;
        if (meshObject["name"].get_string().get(name) == SUCCESS) {
            mesh.name = std::string { name };
        }

        parsedAsset->meshes.emplace_back(std::move(mesh));
    }
}

void fg::glTF::parseNodes(simdjson::dom::array& nodes) {
    using namespace simdjson;

    parsedAsset->nodes.reserve(nodes.size());
    for (auto nodeValue : nodes) {
        Node node = {};
        dom::object nodeObject;
        if (nodeValue.get_object().get(nodeObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t index;
        if (nodeObject["mesh"].get_uint64().get(index) == SUCCESS) {
            node.meshIndex = static_cast<std::size_t>(index);
        }
        if (nodeObject["skin"].get_uint64().get(index) == SUCCESS) {
            node.skinIndex = static_cast<std::size_t>(index);
        }
        if (nodeObject["camera"].get_uint64().get(index) == SUCCESS) {
            node.cameraIndex = static_cast<std::size_t>(index);
        }

        dom::array array;
        auto childError = getJsonArray(nodeObject, "children", &array);
        if (childError == Error::None) {
            node.children.reserve(array.size());
            for (auto childValue : array) {
                if (childValue.get_uint64().get(index) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }

                node.children.emplace_back(static_cast<std::size_t>(index));
            }
        } else if (childError != Error::MissingField) {
            SET_ERROR_RETURN(childError)
        }

        auto weightsError = getJsonArray(nodeObject, "weights", &array);
        if (weightsError != Error::MissingField) {
            if (weightsError != Error::None) {
                node.weights.reserve(array.size());
                for (auto weightValue : array) {
                    double val;
                    if (weightValue.get_double().get(val) != SUCCESS) {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                    node.weights.emplace_back(static_cast<float>(val));
                }
            } else {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
        }

        auto error = nodeObject["matrix"].get_array().get(array);
        if (error == SUCCESS) {
            Node::TransformMatrix transformMatrix = {};
            auto i = 0U;
            for (auto num : array) {
                double val;
                if (num.get_double().get(val) != SUCCESS) {
                    break;
                }
                transformMatrix[i] = static_cast<float>(val);
                ++i;
            }

            if (hasBit(options, Options::DecomposeNodeMatrices)) {
                Node::TRS trs = {};
                decomposeTransformMatrix(transformMatrix, trs.scale, trs.rotation, trs.translation);
                node.transform = trs;
            } else {
                node.transform = transformMatrix;
            }
        } else if (error == NO_SUCH_FIELD) {
            Node::TRS trs = {};

            // There's no matrix, let's see if there's scale, rotation, or rotation fields.
            if (nodeObject["scale"].get_array().get(array) == SUCCESS) {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                    trs.scale[i] = static_cast<float>(val);
                    ++i;
                }
            } else {
                trs.scale = {{ 1.0F, 1.0F, 1.0F }};
            }

            if (nodeObject["translation"].get_array().get(array) == SUCCESS) {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                    trs.translation[i] = static_cast<float>(val);
                    ++i;
                }
            } else {
                trs.translation = {{ 0.0F, 0.0F, 0.0F }};
            }

            if (nodeObject["rotation"].get_array().get(array) == SUCCESS) {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) {
                        SET_ERROR_RETURN(Error::InvalidGltf)
                    }
                    trs.rotation[i] = static_cast<float>(val);
                    ++i;
                }
            } else {
                trs.rotation = {{ 0.0F, 0.0F, 0.0F, 1.0F }};
            }

            node.transform = trs;
        }

        dom::object extensionsObject;
        if (nodeObject["extensions"].get_object().get(extensionsObject) == SUCCESS) {
            dom::object lightsObject;
            if (extensionsObject[extensions::KHR_lights_punctual].get_object().get(lightsObject) == SUCCESS) {
                std::uint64_t light;
                if (lightsObject["light"].get_uint64().get(light) == SUCCESS) {
                    node.lightsIndex = static_cast<std::size_t>(light);
                }
            }
        }

        std::string_view name;
        if (nodeObject["name"].get_string().get(name) == SUCCESS) {
            node.name = std::string { name };
        }

        parsedAsset->nodes.emplace_back(std::move(node));
    }
}

void fg::glTF::parseSamplers(simdjson::dom::array& samplers) {
    using namespace simdjson;

    std::uint64_t number;
    parsedAsset->samplers.reserve(samplers.size());
    for (auto samplerValue : samplers) {
        Sampler sampler = {};
        dom::object samplerObject;
        if (samplerValue.get_object().get(samplerObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        // name is optional.
        std::string_view name;
        if (samplerObject["name"].get_string().get(name) == SUCCESS) {
            sampler.name = std::string { name };
        }

        if (samplerObject["magFilter"].get_uint64().get(number) == SUCCESS) {
            sampler.magFilter = static_cast<Filter>(number);
        }
        if (samplerObject["minFilter"].get_uint64().get(number) == SUCCESS) {
            sampler.minFilter = static_cast<Filter>(number);
        }

        if (samplerObject["wrapS"].get_uint64().get(number) == SUCCESS) {
            sampler.wrapS = static_cast<Wrap>(number);
        } else {
            sampler.wrapS = Wrap::Repeat;
        }
        if (samplerObject["wrapT"].get_uint64().get(number) == SUCCESS) {
            sampler.wrapT = static_cast<Wrap>(number);
        } else {
            sampler.wrapT = Wrap::Repeat;
        }

        parsedAsset->samplers.emplace_back(std::move(sampler));
    }
}

void fg::glTF::parseScenes(simdjson::dom::array& scenes) {
    using namespace simdjson;

    parsedAsset->scenes.reserve(scenes.size());
    for (auto sceneValue : scenes) {
        // The scene object can be completely empty
        Scene scene = {};
        dom::object sceneObject;
        if (sceneValue.get_object().get(sceneObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        // name is optional.
        std::string_view name;
        if (sceneObject["name"].get_string().get(name) == SUCCESS) {
            scene.name = std::string { name };
        }

        // Parse the array of nodes.
        dom::array nodes;
        auto nodeError = getJsonArray(sceneObject, "nodes", &nodes);
        if (nodeError == Error::None) {
            scene.nodeIndices.reserve(nodes.size());
            for (auto nodeValue : nodes) {
                std::uint64_t index;
                if (nodeValue.get_uint64().get(index) != SUCCESS) {
                    SET_ERROR_RETURN(Error::InvalidGltf)
                }

                scene.nodeIndices.emplace_back(static_cast<std::size_t>(index));
            }

            parsedAsset->scenes.emplace_back(std::move(scene));
        } else if (nodeError != Error::MissingField) {
            SET_ERROR_RETURN(nodeError)
        }
    }
}

void fg::glTF::parseSkins(simdjson::dom::array& skins) {
    using namespace simdjson;

    parsedAsset->skins.reserve(skins.size());
    for (auto skinValue : skins) {
        Skin skin = {};
        dom::object skinObject;
        if (skinValue.get_object().get(skinObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t index;
        if (skinObject["inverseBindMatrices"].get_uint64().get(index) == SUCCESS) {
            skin.inverseBindMatrices = static_cast<std::size_t>(index);
        }
        if (skinObject["skeleton"].get_uint64().get(index) == SUCCESS) {
            skin.skeleton = static_cast<std::size_t>(index);
        }

        dom::array jointsArray;
        if (skinObject["joints"].get_array().get(jointsArray) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }
        skin.joints.reserve(jointsArray.size());
        for (auto jointValue : jointsArray) {
            if (jointValue.get_uint64().get(index) != SUCCESS) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
            skin.joints.emplace_back(index);
        }

        // name is optional.
        std::string_view name;
        if (skinObject["name"].get_string().get(name) == SUCCESS) {
            skin.name = std::string { name };
        }
        parsedAsset->skins.emplace_back(std::move(skin));
    }
}

void fg::glTF::parseTextures(simdjson::dom::array& textures) {
    using namespace simdjson;

    parsedAsset->textures.reserve(textures.size());
    for (auto textureValue : textures) {
        Texture texture;
        dom::object textureObject;
        if (textureValue.get_object().get(textureObject) != SUCCESS) {
            SET_ERROR_RETURN(Error::InvalidGltf)
        }

        std::uint64_t sourceIndex;
        if (textureObject["source"].get_uint64().get(sourceIndex) == SUCCESS) {
            texture.imageIndex = static_cast<std::size_t>(sourceIndex);
        }

        bool hasExtensions = false;
        dom::object extensionsObject;
        if (textureObject["extensions"].get_object().get(extensionsObject) == SUCCESS) {
            hasExtensions = true;
        }

        // If we have extensions, we'll use the normal "source" as the fallback and then parse
        // the extensions for any "source" field.
        if (hasExtensions) {
            if (texture.imageIndex.has_value()) {
                // If the source was specified we'll use that as a fallback.
                texture.fallbackImageIndex = texture.imageIndex;
            }
            if (!parseTextureExtensions(texture, extensionsObject, data->config.extensions)) {
                SET_ERROR_RETURN(Error::InvalidGltf)
            }
        }

        // The index of the sampler used by this texture. When undefined, a sampler with
        // repeat wrapping and auto filtering SHOULD be used.
        std::uint64_t samplerIndex;
        if (textureObject["sampler"].get_uint64().get(samplerIndex) == SUCCESS) {
            texture.samplerIndex = static_cast<std::size_t>(samplerIndex);
        }

        // name is optional.
        std::string_view name;
        if (textureObject["name"].get_string().get(name) == SUCCESS) {
            texture.name = std::string { name };
        }

        parsedAsset->textures.emplace_back(std::move(texture));
    }
}

#pragma endregion

#pragma region GltfDataBuffer
std::size_t fg::getGltfBufferPadding() noexcept {
    return simdjson::SIMDJSON_PADDING;
}

fg::GltfDataBuffer::GltfDataBuffer() noexcept = default;
fg::GltfDataBuffer::~GltfDataBuffer() noexcept = default;

bool fg::GltfDataBuffer::fromByteView(std::uint8_t* bytes, std::size_t byteCount, std::size_t capacity) noexcept {
    using namespace simdjson;
    if (bytes == nullptr || byteCount == 0 || capacity == 0)
        return false;

    if (capacity - byteCount < simdjson::SIMDJSON_PADDING)
        return copyBytes(bytes, byteCount);

    dataSize = byteCount;
    bufferPointer = reinterpret_cast<std::byte*>(bytes);
    allocatedSize = capacity;
    std::memset(bufferPointer + dataSize, 0, allocatedSize - dataSize);
    return true;
}

bool fg::GltfDataBuffer::copyBytes(const std::uint8_t* bytes, std::size_t byteCount) noexcept {
    using namespace simdjson;
    if (bytes == nullptr || byteCount == 0)
        return false;

    // Allocate a byte array with a bit of padding.
    dataSize = byteCount;
    allocatedSize = byteCount + simdjson::SIMDJSON_PADDING;
    buffer = decltype(buffer)(new std::byte[allocatedSize]); // To mimic std::make_unique_for_overwrite (C++20)
    bufferPointer = buffer.get();

    // Copy the data and fill the padding region with zeros.
    std::memcpy(bufferPointer, bytes, dataSize);
    std::memset(bufferPointer + dataSize, 0, allocatedSize - dataSize);
    return true;
}

bool fg::GltfDataBuffer::loadFromFile(const fs::path& path, std::uint64_t byteOffset) noexcept {
    using namespace simdjson;
    std::error_code ec;
    auto length = static_cast<std::streamsize>(std::filesystem::file_size(path, ec));
    if (ec) {
        return false;
    }

    // Open the file and determine the size.
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open() || file.bad())
        return false;

    filePath = path;

    file.seekg(static_cast<std::streamsize>(byteOffset), std::ifstream::beg);

    dataSize = static_cast<std::uint64_t>(length) - byteOffset;
    allocatedSize = dataSize + simdjson::SIMDJSON_PADDING;
    buffer = decltype(buffer)(new std::byte[allocatedSize]); // To mimic std::make_unique_for_overwrite (C++20)
    if (!buffer)
        return false;
    bufferPointer = buffer.get();

    // Copy the data and fill the padding region with zeros.
    file.read(reinterpret_cast<char*>(bufferPointer), static_cast<std::streamsize>(dataSize));
    std::memset(bufferPointer + dataSize, 0, allocatedSize - dataSize);
    return true;
}

std::size_t fg::GltfDataBuffer::getBufferSize() const noexcept {
    return dataSize;
}
#pragma endregion

#pragma region AndroidGltfDataBuffer
#if defined(__ANDROID__)
fg::AndroidGltfDataBuffer::AndroidGltfDataBuffer(AAssetManager* assetManager) noexcept : assetManager{assetManager} {}

bool fg::AndroidGltfDataBuffer::loadFromAndroidAsset(const fs::path& path, std::uint64_t byteOffset) noexcept {
    if (assetManager == nullptr) {
        return false;
    }
    
    using namespace simdjson;

    const auto filenameString = path.string();

    auto assetDeleter = [](AAsset* file) { AAsset_close(file); };
    auto file = std::unique_ptr<AAsset, decltype(assetDeleter)>(AAssetManager_open(assetManager, filenameString.c_str(), AASSET_MODE_BUFFER), assetDeleter);
    if (file == nullptr) {
        return false;
    }

    const auto length = AAsset_getLength(file.get());
    if (length == 0) {
        return false;
    }

    dataSize = length - byteOffset;
    allocatedSize = dataSize + simdjson::SIMDJSON_PADDING;
    buffer = decltype(buffer)(new std::byte[allocatedSize]);
    if (!buffer) {
        return false;
    }

    bufferPointer = buffer.get();

    if (byteOffset > 0) {
        AAsset_seek64(file.get(), byteOffset, SEEK_SET);
    }

    AAsset_read(file.get(), bufferPointer, dataSize);

    std::memset(bufferPointer + dataSize, 0, allocatedSize - dataSize);

    filePath = path;

    return true;
}
#endif
#pragma endregion

#pragma region Parser
fastgltf::GltfType fg::determineGltfFileType(GltfDataBuffer* buffer) {
    // First, check if any of the first four characters is a '{'.
    std::array<std::uint8_t, 4> begin = {};
    std::memcpy(begin.data(), buffer->bufferPointer, sizeof begin);
    for (const auto& i : begin) {
        if ((char)i == '{')
            return GltfType::glTF;
    }

    // We'll try and read a BinaryGltfHeader from the buffer to see if the magic is correct.
    BinaryGltfHeader header = {};
    std::memcpy(&header, buffer->bufferPointer, sizeof header);
    if (header.magic == binaryGltfHeaderMagic) {
        return GltfType::GLB;
    }

    return GltfType::Invalid;
}

fg::Parser::Parser(Extensions extensionsToLoad) noexcept {
    std::call_once(crcInitialisation, initialiseCrc);
    jsonParser = std::make_unique<simdjson::dom::parser>();
    config.extensions = extensionsToLoad;
}

fg::Parser::Parser(Parser&& other) noexcept : jsonParser(std::move(other.jsonParser)), config(other.config) {}

fg::Parser& fg::Parser::operator=(Parser&& other) noexcept {
    jsonParser = std::move(other.jsonParser);
    config = other.config;
    return *this;
}

fg::Parser::~Parser() = default;

fg::Error fg::Parser::getError() const {
    return errorCode;
}

std::unique_ptr<fg::glTF> fg::Parser::loadGLTF(GltfDataBuffer* buffer, fs::path directory, Options options) {
    using namespace simdjson;

    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (hasBit(options, Options::LoadExternalBuffers) && !fs::is_directory(directory)) {
        errorCode = Error::InvalidPath;
        return nullptr;
    }

    errorCode = Error::None;

    // If we own the allocation of the JSON data, we'll try to minify the JSON, which, in most cases,
    // will speed up the parsing by a small amount.
    std::size_t jsonLength = buffer->getBufferSize();
    if (buffer->buffer != nullptr && hasBit(options, Options::MinimiseJsonBeforeParsing)) {
        std::size_t newLength = 0;
        auto result = simdjson::minify(reinterpret_cast<const char*>(buffer->bufferPointer), buffer->getBufferSize(),
                                       reinterpret_cast<char*>(buffer->bufferPointer), newLength);
        if (result != SUCCESS || newLength == 0) {
            errorCode = Error::InvalidJson;
            return nullptr;
        }
        buffer->dataSize = jsonLength = newLength;
    }

    auto data = std::make_unique<ParserData>();
    auto view = padded_string_view(reinterpret_cast<const std::uint8_t*>(buffer->bufferPointer), jsonLength, buffer->allocatedSize);
    if (jsonParser->parse(view).get(data->root) != SUCCESS) {
        errorCode = Error::InvalidJson;
        return nullptr;
    }
    data->config = config;

    return std::unique_ptr<glTF>(new glTF(std::move(data), std::move(directory), options));
}

std::unique_ptr<fg::glTF> fg::Parser::loadBinaryGLTF(GltfDataBuffer* buffer, fs::path directory, Options options) {
    using namespace simdjson;

    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (hasBit(options, Options::LoadExternalBuffers) && !fs::is_directory(directory)) {
        errorCode = Error::InvalidPath;
        return nullptr;
    }

    errorCode = Error::None;

    std::size_t offset = 0UL;
    auto read = [&buffer, &offset](void* dst, std::size_t size) mutable {
        std::memcpy(dst, buffer->bufferPointer + offset, size);
        offset += size;
    };

    BinaryGltfHeader header = {};
    read(&header, sizeof header);
    if (header.magic != binaryGltfHeaderMagic || header.version != 2) {
        errorCode = Error::InvalidGLB;
        return nullptr;
    }
    if (header.length >= buffer->allocatedSize) {
        errorCode = Error::InvalidGLB;
        return nullptr;
    }

    // The glTF 2 spec specifies that in GLB files the order of chunks is predefined. Specifically,
    //  1. JSON chunk
    //  2. BIN chunk (optional)
    BinaryGltfChunk jsonChunk = {};
    read(&jsonChunk, sizeof jsonChunk);
    if (jsonChunk.chunkType != binaryGltfJsonChunkMagic) {
        errorCode = Error::InvalidGLB;
        return nullptr;
    }

    // Create a string view of the JSON chunk in the GLB data buffer. The documentation of parse()
    // says the padding can be initialised to anything, apparently. Therefore, this should work.
    simdjson::padded_string_view jsonChunkView(reinterpret_cast<const std::uint8_t*>(buffer->bufferPointer) + offset,
                                               jsonChunk.chunkLength,
                                               jsonChunk.chunkLength + SIMDJSON_PADDING);
    offset += jsonChunk.chunkLength;

    auto data = std::make_unique<ParserData>();
    if (jsonParser->parse(jsonChunkView).get(data->root) != SUCCESS) {
        errorCode = Error::InvalidJson;
        return nullptr;
    }
    data->config = config;

    auto gltf = std::unique_ptr<glTF>(new glTF(std::move(data), std::move(directory), options));

    // Is there enough room for another chunk header?
    if (header.length > (offset + sizeof(BinaryGltfChunk))) {
        BinaryGltfChunk binaryChunk = {};
        read(&binaryChunk, sizeof binaryChunk);

        if (binaryChunk.chunkType != binaryGltfDataChunkMagic) {
            errorCode = Error::InvalidGLB;
            return nullptr;
        }

        if (hasBit(options, Options::LoadGLBBuffers)) {
            const auto& gconfig = gltf->data->config;
            if (gconfig.mapCallback != nullptr) {
                auto info = gconfig.mapCallback(binaryChunk.chunkLength, gconfig.userPointer);
                if (info.mappedMemory != nullptr) {
                    read(info.mappedMemory, binaryChunk.chunkLength);
                    if (gconfig.unmapCallback != nullptr) {
                        gconfig.unmapCallback(&info, gconfig.userPointer);
                    }
                    gltf->glbBuffer = sources::CustomBuffer { info.customId, MimeType::None };
                }
            } else {
                sources::Vector vectorData = {};
                vectorData.bytes.resize(binaryChunk.chunkLength);
                read(vectorData.bytes.data(), binaryChunk.chunkLength);
                vectorData.mimeType = MimeType::GltfBuffer;
                gltf->glbBuffer = std::move(vectorData);
            }
        } else {
            const span<const std::byte> glbBytes(reinterpret_cast<std::byte*>(buffer->bufferPointer + offset), binaryChunk.chunkLength);
            sources::ByteView glbByteView = {};
            glbByteView.bytes = glbBytes;
            glbByteView.mimeType = MimeType::GltfBuffer;
            gltf->glbBuffer = glbByteView;
        }
    }

    return gltf;
}

void fg::Parser::setBufferAllocationCallback(BufferMapCallback* mapCallback, BufferUnmapCallback* unmapCallback) noexcept {
    if (mapCallback == nullptr)
        return;
    config.mapCallback = mapCallback;
    config.unmapCallback = unmapCallback;
}

void fg::Parser::setBase64DecodeCallback(Base64DecodeCallback* decodeCallback) noexcept {
    if (decodeCallback == nullptr)
        return;
    config.decodeCallback = decodeCallback;
}

void fg::Parser::setUserPointer(void* pointer) noexcept {
    config.userPointer = pointer;
}
#pragma endregion

#ifdef _MSC_VER
#pragma warning(pop)
#endif
