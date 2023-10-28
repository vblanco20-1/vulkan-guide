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

#if defined(FASTGLTF_IS_X86)
#include <nmmintrin.h> // SSE4.2 for the CRC-32C instructions
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

	enum class TextureInfoType {
		Standard = 0,
		NormalTexture = 1,
		OcclusionTexture = 2,
	};

	fg::Error parseTextureInfo(simdjson::dom::object& object, std::string_view key, TextureInfo* info, Extensions extensions, TextureInfoType type = TextureInfoType::Standard) noexcept {
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

		if (type == TextureInfoType::NormalTexture) {
			double scale = 1.0F;
			if (child["scale"].get_double().get(scale) == SUCCESS) {
				reinterpret_cast<NormalTextureInfo*>(info)->scale = static_cast<float>(scale);
			} else {
				reinterpret_cast<NormalTextureInfo*>(info)->scale = 1.0F;
			}
		} else if (type == TextureInfoType::OcclusionTexture) {
			double strength = 1.0F;
			if (child["strength"].get_double().get(strength) == SUCCESS) {
				reinterpret_cast<OcclusionTextureInfo*>(info)->strength = static_cast<float>(strength);
			} else {
				reinterpret_cast<OcclusionTextureInfo*>(info)->strength = 1.0F;
			}
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
fg::URIView::URIView() noexcept = default;

fg::URIView::URIView(std::string_view uri) noexcept : view(uri) {
	parse();
}

fg::URIView::URIView(const URIView& other) noexcept {
	*this = other;
}

fg::URIView& fg::URIView::operator=(const URIView& other) {
	view = other.view;
	_scheme = other._scheme;
	_path = other._path;
	_userinfo = other._userinfo;
	_host = other._host;
	_port = other._port;
	_query = other._query;
	_fragment = other._fragment;
	return *this;
}

fg::URIView& fg::URIView::operator=(std::string_view other) {
	view = other;
	parse();
	return *this;
}

void fg::URIView::parse() {
	if (view.empty()) {
		_valid = false;
		return;
	}

	size_t idx = 0;
	auto firstColon = view.find(':');
	if (firstColon != std::string::npos) {
		// URI has a scheme.
		if (firstColon == 0) {
			// Empty scheme is invalid
			_valid = false;
			return;
		}
		_scheme = view.substr(0, firstColon);
		idx = firstColon + 1;
	}

	if (startsWith(view.substr(idx), "//")) {
		// URI has an authority part.
		idx += 2;
		auto nextSlash = view.find('/', idx);
		auto userInfo = view.find('@', idx);
		if (userInfo != std::string::npos && userInfo < nextSlash) {
			_userinfo = view.substr(idx, userInfo - idx);
			idx += _userinfo.size() + 1;
		}

		auto hostEnd = nextSlash - 1;
		std::size_t portColon;
		if (view[idx] == '[') {
			hostEnd = view.find(']', idx);
			if (hostEnd == std::string::npos) {
				_valid = false;
				return;
			}
			// IPv6 addresses are made up of colons, so we need to search after its address.
			// This will just be hostEnd + 1 or std::string::npos.
			portColon = view.find(':', hostEnd);
		} else {
			portColon = view.find(':', idx);
		}

		if (portColon != std::string::npos) {
			_host = view.substr(idx, portColon - idx);
			++portColon; // We don't want to include the colon in the port string.
			_port = view.substr(portColon, nextSlash - portColon);
		} else {
			++idx;
			_host = view.substr(idx, hostEnd - idx);
		}

		idx = nextSlash; // Path includes this slash
	}

	// Parse the path.
	auto questionIdx = view.find("?", idx);
	auto hashIdx = view.find("#", idx);
	if (questionIdx != std::string::npos) {
		_path = view.substr(idx, questionIdx - idx);

		if (hashIdx == std::string::npos) {
			_query = view.substr(++questionIdx);
		} else {
			++questionIdx;
			_query = view.substr(questionIdx, hashIdx - questionIdx);
			_fragment = view.substr(++hashIdx);
		}
	} else if (hashIdx != std::string::npos) {
		_path = view.substr(idx, hashIdx - idx);
		_fragment = view.substr(++hashIdx);
	} else {
		_path = view.substr(idx);
	}
}

const char* fg::URIView::data() const noexcept {
	return view.data();
}

std::string_view fg::URIView::string() const noexcept { return view; }
std::string_view fg::URIView::scheme() const noexcept { return _scheme; }
std::string_view fg::URIView::userinfo() const noexcept { return _userinfo; }
std::string_view fg::URIView::host() const noexcept { return _host; }
std::string_view fg::URIView::port() const noexcept { return _port; }
std::string_view fg::URIView::path() const noexcept { return _path; }
std::string_view fg::URIView::query() const noexcept { return _query; }
std::string_view fg::URIView::fragment() const noexcept { return _fragment; }

fs::path fg::URIView::fspath() const {
	if (!isLocalPath())
		return {};
	return { path() };
}

bool fg::URIView::valid() const noexcept {
	return _valid;
}

bool fg::URIView::isLocalPath() const noexcept {
	return scheme().empty() || (scheme() == "file" && host().empty());
}

bool fg::URIView::isDataUri() const noexcept {
	return scheme() == "data";
}

fg::URI::URI() noexcept = default;

fg::URI::URI(std::string uri) noexcept : uri(uri) {
	decodePercents(this->uri);
	view = this->uri; // Also parses.
}

fg::URI::URI(std::pmr::string uri) noexcept : uri(std::move(uri)) {
	decodePercents(this->uri);
	view = this->uri; // Also parses.
}

fg::URI::URI(std::string_view uri) noexcept : uri(uri) {
	decodePercents(this->uri);
	view = this->uri; // Also parses.
}

fg::URI::URI(fastgltf::URIView view) noexcept : uri(view.view) {
	readjustViews(view);
}

// Some C++ stdlib implementations copy in some cases when moving strings, which invalidates the
// views stored in the URI struct. This function adjusts the views from the old string to the new
// string for safe copying.
fg::URI::URI(const URI& other) {
	*this = other;
}

fg::URI::URI(URI&& other) noexcept {
	*this = other;
}

fg::URI& fg::URI::operator=(const URI& other) {
	uri = other.uri;
	// We'll assume that with copying the string will always have to reallocate.
	readjustViews(other.view);
	return *this;
}

fg::URI& fg::URI::operator=(const URIView& other) {
	uri = other.view;
	auto oldSize = uri.size();
	decodePercents(uri);
	if (uri.size() == oldSize) {
		readjustViews(other);
	} else {
		// We removed some encoded chars, which have now invalidated all the string views.
		// Therefore, the URI needs to be parsed again.
		view = this->uri;
	}
	return *this;
}

fg::URI& fg::URI::operator=(URI&& other) noexcept {
	auto* oldData = other.uri.data();
	uri = std::move(other.uri);

	// Invalidate the previous URI's view.
	view._valid = other.view._valid;
	other.view._valid = false;

	if (uri.data() != oldData) {
		// Allocation changed, we need to readjust views
		readjustViews(other.view);
	} else {
		// No reallocation happened, we can safely copy the view.
		view = other.view;
	}
	return *this;
}

fg::URI::operator fg::URIView() const noexcept {
	return view;
}

void fg::URI::readjustViews(const URIView& other) {
	if (!other._scheme.empty())   { view._scheme     = std::string_view(uri.data() + (other._scheme.data()     - other.view.data()), other._scheme.size()); }
	if (!other._path.empty())     { view._path       = std::string_view(uri.data() + (other._path.data()       - other.view.data()), other._path.size()); }
	if (!other._userinfo.empty()) { view._userinfo   = std::string_view(uri.data() + (other._userinfo.data()   - other.view.data()), other._userinfo.size()); }
	if (!other._host.empty())     { view._host       = std::string_view(uri.data() + (other._host.data()       - other.view.data()), other._host.size()); }
	if (!other._port.empty())     { view._port       = std::string_view(uri.data() + (other._port.data()       - other.view.data()), other._port.size()); }
	if (!other._query.empty())    { view._query      = std::string_view(uri.data() + (other._query.data()      - other.view.data()), other._query.size()); }
	if (!other._fragment.empty()) { view._fragment   = std::string_view(uri.data() + (other._fragment.data()   - other.view.data()), other._fragment.size()); }

	view.view = uri;
}

void fg::URI::decodePercents(std::pmr::string& x) noexcept {
	for (auto it = x.begin(); it != x.end(); ++it) {
		if (*it == '%') {
			// Read the next two chars and store them.
			std::array<char, 3> chars = {*(it + 1), *(it + 2), 0};
			*it = static_cast<char>(std::strtoul(chars.data(), nullptr, 16));
			x.erase(it + 1, it + 3);
		}
	}
}

std::string_view fg::URI::string() const noexcept { return uri; }
std::string_view fg::URI::scheme() const noexcept { return view.scheme(); }
std::string_view fg::URI::userinfo() const noexcept { return view.userinfo(); }
std::string_view fg::URI::host() const noexcept { return view.host(); }
std::string_view fg::URI::port() const noexcept { return view.port(); }
std::string_view fg::URI::path() const noexcept { return view.path(); }
std::string_view fg::URI::query() const noexcept { return view.query(); }
std::string_view fg::URI::fragment() const noexcept { return view.fragment(); }

fs::path fg::URI::fspath() const {
	return view.fspath();
}

bool fg::URI::valid() const noexcept {
	return view.valid();
}

bool fg::URI::isLocalPath() const noexcept {
	return view.isLocalPath();
}

bool fg::URI::isDataUri() const noexcept {
	return view.isDataUri();
}
#pragma endregion

#pragma region glTF parsing
fg::Expected<fg::DataSource> fg::Parser::decodeDataUri(URIView& uri) const noexcept {
    auto path = uri.path();
    auto mimeEnd = path.find(';');
    auto mime = path.substr(0, mimeEnd);

    auto encodingEnd = path.find(',');
    auto encoding = path.substr(mimeEnd + 1, encodingEnd - mimeEnd - 1);
    if (encoding != "base64") {
		return Expected<DataSource> { Error::InvalidURI };
    }

    auto encodedData = path.substr(encodingEnd + 1);
    if (config.mapCallback != nullptr) {
        // If a map callback is specified, we use a pointer to memory specified by it.
        auto padding = base64::getPadding(encodedData);
        auto size = base64::getOutputSize(encodedData.size(), padding);
        auto info = config.mapCallback(size, config.userPointer);
        if (info.mappedMemory != nullptr) {
            if (config.decodeCallback != nullptr) {
                config.decodeCallback(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding, size, config.userPointer);
            } else {
                base64::decode_inplace(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding);
            }

            if (config.unmapCallback != nullptr) {
                config.unmapCallback(&info, config.userPointer);
            }

            sources::CustomBuffer source = {};
            source.id = info.customId;
            source.mimeType = getMimeTypeFromString(mime);
			return Expected<DataSource> { source };
        }
    }

    // Decode the base64 data into a traditional vector
    std::vector<std::uint8_t> uriData;
    if (config.decodeCallback != nullptr) {
        auto padding = base64::getPadding(encodedData);
        uriData.resize(base64::getOutputSize(encodedData.size(), padding));
        config.decodeCallback(encodedData, uriData.data(), padding, uriData.size(), config.userPointer);
    } else {
        uriData = base64::decode(encodedData);
    }

    sources::Vector source = {};
    source.mimeType = getMimeTypeFromString(mime);
    source.bytes = std::move(uriData);
	return Expected<DataSource> { std::move(source) };
}

fg::Expected<fg::DataSource> fg::Parser::loadFileFromUri(URIView& uri) const noexcept {
    auto path = directory / fs::u8path(uri.path());
    std::error_code error;
    // If we were instructed to load external buffers and the files don't exist, we'll return an error.
    if (!fs::exists(path, error) || error) {
	    return Expected<DataSource> { Error::MissingExternalBuffer };
    }

    auto length = static_cast<std::streamsize>(std::filesystem::file_size(path, error));
    if (error) {
	    return Expected<DataSource> { Error::InvalidURI };
    }

    std::ifstream file(path, std::ios::binary);

    if (config.mapCallback != nullptr) {
        auto info = config.mapCallback(static_cast<std::uint64_t>(length), config.userPointer);
        if (info.mappedMemory != nullptr) {
            const sources::CustomBuffer customBufferSource = { info.customId, MimeType::None };
            file.read(reinterpret_cast<char*>(info.mappedMemory), length);
            if (config.unmapCallback != nullptr) {
                config.unmapCallback(&info, config.userPointer);
            }

	        return Expected<DataSource> { customBufferSource };
        }
    }

    sources::Vector vectorSource = {};
    vectorSource.mimeType = MimeType::GltfBuffer;
    vectorSource.bytes.resize(length);
    file.read(reinterpret_cast<char*>(vectorSource.bytes.data()), length);
	return Expected<DataSource> { std::move(vectorSource) };
}

void fg::Parser::fillCategories(Category& inputCategories) noexcept {
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

fg::MimeType fg::Parser::getMimeTypeFromString(std::string_view mime) {
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

fg::Error fg::validate(const fastgltf::Asset& asset) {
	auto isExtensionUsed = [&used = asset.extensionsUsed](std::string_view extension) {
		for (const auto& extensionUsed : used) {
			if (extension == extensionUsed) {
				return true;
			}
		}
		return false;
	};

	for (const auto& accessor : asset.accessors) {
		if (accessor.type == AccessorType::Invalid)
			return Error::InvalidGltf;
		if (accessor.componentType == ComponentType::Invalid)
			return Error::InvalidGltf;
		if (accessor.count < 1)
			return Error::InvalidGltf;
		if (accessor.bufferViewIndex.has_value() &&
		    accessor.bufferViewIndex.value() >= asset.bufferViews.size())
			return Error::InvalidGltf;

		if (!std::holds_alternative<std::monostate>(accessor.max)) {
			if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
			    && !std::holds_alternative<std::pmr::vector<double>>(accessor.max))
				return Error::InvalidGltf;
		}
		if (!std::holds_alternative<std::monostate>(accessor.min)) {
			if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
			    && !std::holds_alternative<std::pmr::vector<double>>(accessor.min))
				return Error::InvalidGltf;
		}
	}

	for (const auto& animation : asset.animations) {
		if (animation.channels.empty())
			return Error::InvalidGltf;
		if (animation.samplers.empty())
			return Error::InvalidGltf;
	}

	for (const auto& buffer : asset.buffers) {
		if (buffer.byteLength < 1)
			return Error::InvalidGltf;
	}

	for (const auto& bufferView : asset.bufferViews) {
		if (bufferView.byteLength < 1)
			return Error::InvalidGltf;
		if (bufferView.byteStride.has_value() && (bufferView.byteStride < 4U || bufferView.byteStride > 252U))
			return Error::InvalidGltf;
		if (bufferView.bufferIndex >= asset.buffers.size())
			return Error::InvalidGltf;

		if (bufferView.meshoptCompression != nullptr && isExtensionUsed(extensions::EXT_meshopt_compression))
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

	for (const auto& camera : asset.cameras) {
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

	for (const auto& image : asset.images) {
		if (const auto* view = std::get_if<sources::BufferView>(&image.data); view != nullptr) {
			if (view->bufferViewIndex >= asset.bufferViews.size()) {
				return Error::InvalidGltf;
			}
		}
	}

	for (const auto& material : asset.materials) {
		auto isInvalidTexture = [&textures = asset.textures](std::optional<std::size_t> textureIndex) {
			return textureIndex.has_value() && textureIndex.value() >= textures.size();
		};
		if (material.normalTexture.has_value() && isInvalidTexture(material.normalTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.emissiveTexture.has_value() && isInvalidTexture(material.emissiveTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.occlusionTexture.has_value() && isInvalidTexture(material.occlusionTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.pbrData.baseColorTexture.has_value() &&
		    isInvalidTexture(material.pbrData.baseColorTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.pbrData.metallicRoughnessTexture.has_value() &&
		    isInvalidTexture(material.pbrData.metallicRoughnessTexture->textureIndex))
			return Error::InvalidGltf;
	}

	for (const auto& mesh : asset.meshes) {
		for (const auto& primitives : mesh.primitives) {
			for (auto [name, index] : primitives.attributes) {
				if (asset.accessors.size() <= index)
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
				const auto& accessor = asset.accessors[index];
				if (name == "POSITION") {
					if (accessor.type != AccessorType::Vec3)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
						if (accessor.componentType != ComponentType::Float)
							return Error::InvalidGltf;
					} else {
						if (accessor.componentType == ComponentType::Double || accessor.componentType == ComponentType::UnsignedInt)
							return Error::InvalidGltf;
					}
				} else if (name == "NORMAL") {
					if (accessor.type != AccessorType::Vec3)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
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
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
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
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
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
				} else if (startsWith(name, "_")) {
					// Application-specific attribute semantics MUST start with an underscore, e.g., _TEMPERATURE.
					// Application-specific attribute semantics MUST NOT use unsigned int component type.
					if (accessor.componentType == ComponentType::UnsignedInt) {
						return Error::InvalidGltf;
					}
				}
			}
		}
	}

	for (const auto& node : asset.nodes) {
		if (node.cameraIndex.has_value() && asset.cameras.size() <= node.cameraIndex.value())
			return Error::InvalidGltf;
		if (node.skinIndex.has_value() && asset.skins.size() <= node.skinIndex.value())
			return Error::InvalidGltf;
		if (node.meshIndex.has_value() && asset.meshes.size() <= node.meshIndex.value())
			return Error::InvalidGltf;

		if (const auto* pTRS = std::get_if<Node::TRS>(&node.transform)) {
			for (const auto& x : pTRS->rotation)
				if (x > 1.0 || x < -1.0)
					return Error::InvalidGltf;
		}

		if (node.skinIndex.has_value() && node.meshIndex.has_value()) {
			// "When the node contains skin, all mesh.primitives MUST contain JOINTS_0 and WEIGHTS_0 attributes."
			const auto& mesh = asset.meshes[node.meshIndex.value()];
			for (const auto& primitive : mesh.primitives) {
				const auto* joints0 = primitive.findAttribute("JOINTS_0");
				const auto* weights0 = primitive.findAttribute("WEIGHTS_0");
				if (joints0 == primitive.attributes.end() || weights0 == primitive.attributes.end())
					return Error::InvalidGltf;
			}
		}
	}

	for (const auto& scene : asset.scenes) {
		for (const auto& node : scene.nodeIndices) {
			if (node >= asset.nodes.size())
				return Error::InvalidGltf;
		}
	}

	for (const auto& skin : asset.skins) {
		if (skin.joints.empty())
			return Error::InvalidGltf;
		if (skin.skeleton.has_value() && skin.skeleton.value() >= asset.nodes.size())
			return Error::InvalidGltf;
		if (skin.inverseBindMatrices.has_value() && skin.inverseBindMatrices.value() >= asset.accessors.size())
			return Error::InvalidGltf;
	}

	for (const auto& texture : asset.textures) {
		if (texture.samplerIndex.has_value() && texture.samplerIndex.value() >= asset.samplers.size())
			return Error::InvalidGltf;
		if (texture.imageIndex.has_value() && texture.imageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
		if (texture.fallbackImageIndex.has_value() && texture.fallbackImageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
	}

	return Error::None;
}

fg::Expected<fg::Asset> fg::Parser::parse(simdjson::dom::object root, Category categories) {
	using namespace simdjson;
	fillCategories(categories);

	Asset asset {};

	// Create a new chunk memory resource for each asset we parse.
	asset.memoryResource = resourceAllocator = std::make_shared<ChunkMemoryResource>();

	if (!hasBit(options, Options::DontRequireValidAssetMember)) {
		dom::object assetInfo;
		AssetInfo info = {};
		auto error = root["asset"].get_object().get(assetInfo);
		if (error == NO_SUCH_FIELD) {
			return Expected<Asset>(Error::InvalidOrMissingAssetField);
		}
		if (error != SUCCESS) {
			return Expected<Asset>(Error::InvalidJson);
		}

		std::string_view version;
		if (assetInfo["version"].get_string().get(version) != SUCCESS) {
			return Expected<Asset>(Error::InvalidOrMissingAssetField);
		}

		const auto major = static_cast<std::uint32_t>(version.substr(0, 1)[0] - '0');
		// std::uint32_t minor = version.substr(2, 3)[0] - '0';
		if (major != 2) {
			return Expected<Asset>(Error::UnsupportedVersion);
		}
		info.gltfVersion = std::string { version };

		std::string_view copyright;
		if (assetInfo["copyright"].get_string().get(copyright) == SUCCESS) {
			info.copyright = std::string { copyright };
		}

		std::string_view generator;
		if (assetInfo["generator"].get_string().get(generator) == SUCCESS) {
			info.generator = std::string { generator };
		}

		asset.assetInfo = std::move(info);
	}

	dom::array extensionsRequired;
	if (root["extensionsRequired"].get_array().get(extensionsRequired) == SUCCESS) {
		for (auto extension : extensionsRequired) {
			std::string_view string;
			if (extension.get_string().get(string) != SUCCESS) {
				return Expected<Asset>(Error::InvalidGltf);
			}

			bool known = false;
			for (const auto& [extensionString, extensionEnum] : extensionStrings) {
				if (extensionString == string) {
					known = true;
					if (!hasBit(config.extensions, extensionEnum)) {
						// The extension is required, but not enabled by the user.
						return Expected<Asset>(Error::MissingExtensions);
					}
					break;
				}
			}
			if (!known) {
				return Expected<Asset>(Error::UnknownRequiredExtension);
			}
		}
	}

	Category readCategories = Category::None;
	for (const auto& object : root) {
		auto hashedKey = crcStringFunction(object.key);
		if (hashedKey == force_consteval<crc32c("scene")>) {
			std::uint64_t defaultScene;
			if (object.value.get_uint64().get(defaultScene) != SUCCESS) {
				return Expected<Asset>(Error::InvalidGltf);
			}
			asset.defaultScene = static_cast<std::size_t>(defaultScene);
			continue;
		}

		if (hashedKey == force_consteval<crc32c("extensions")>) {
			dom::object extensionsObject;
			if (object.value.get_object().get(extensionsObject) != SUCCESS) {
				return Expected<Asset>(Error::InvalidGltf);
			}

			if (auto error = parseExtensions(extensionsObject, asset); error != Error::None)
				return Expected<Asset>(error);
			continue;
		}

		if (hashedKey == force_consteval<crc32c("asset")> || hashedKey == force_consteval<crc32c("extras")>) {
			continue;
		}

		dom::array array;
		if (object.value.get_array().get(array) != SUCCESS) {
			return Expected<Asset>(Error::InvalidGltf);
		}

#define KEY_SWITCH_CASE(name, id) case force_consteval<crc32c(FASTGLTF_QUOTE(id))>:       \
                if (hasBit(categories, Category::name))   \
                    error = parse##name(array, asset);                     \
                readCategories |= Category::name;         \
                break;

		Error error = Error::None;
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
			case force_consteval<crc32c("extensionsUsed")>: {
				for (auto usedValue : array) {
					std::string_view usedString;
					if (auto eError = usedValue.get_string().get(usedString); eError == SUCCESS) {
						std::pmr::string string(usedString, resourceAllocator.get());
						asset.extensionsUsed.emplace_back(std::move(string));
					} else {
						error = Error::InvalidGltf;
					}
				}
				break;
			}
			case force_consteval<crc32c("extensionsRequired")>: {
				for (auto requiredValue : array) {
					std::string_view requiredString;
					if (auto eError = requiredValue.get_string().get(requiredString); eError == SUCCESS) {
						std::pmr::string string(requiredString, resourceAllocator.get());
						asset.extensionsRequired.emplace_back(std::move(string));
					} else {
						error = Error::InvalidGltf;
					}
				}
				break;
			}
			default:
				break;
		}

		if (error != Error::None)
			return Expected<Asset>(error);

#undef KEY_SWITCH_CASE
	}

	asset.availableCategories = readCategories;

	return Expected(std::move(asset));
}

fg::Error fg::Parser::parseAccessors(simdjson::dom::array& accessors, Asset& asset) {
    using namespace simdjson;

	asset.accessors.reserve(accessors.size());
    for (auto accessorValue : accessors) {
        // Required fields: "componentType", "count"
        Accessor accessor = {};
        dom::object accessorObject;
        if (accessorValue.get_object().get(accessorObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::uint64_t componentType;
        if (accessorObject["componentType"].get_uint64().get(componentType) != SUCCESS) {
            return Error::InvalidGltf;
        }
		accessor.componentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(componentType));
        if (accessor.componentType == ComponentType::Double && !hasBit(options, Options::AllowDouble)) {
            return Error::InvalidGltf;
        }

        std::string_view accessorType;
        if (accessorObject["type"].get_string().get(accessorType) != SUCCESS) {
            return Error::InvalidGltf;
        }
		accessor.type = getAccessorType(accessorType);

        std::uint64_t accessorCount;
        if (accessorObject["count"].get_uint64().get(accessorCount) != SUCCESS) {
            return Error::InvalidGltf;
        }
		accessor.count = static_cast<std::size_t>(accessorCount);


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

				using double_vec = std::variant_alternative_t<1, decltype(Accessor::max)>;
				using int64_vec = std::variant_alternative_t<2, decltype(Accessor::max)>;

				auto num = getNumComponents(accessor.type);
                if (accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double) {
                    variant = double_vec(num, resourceAllocator.get());
                } else {
                    variant = int64_vec(num, resourceAllocator.get());
                }

				std::size_t idx = 0;
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
                            if (!std::holds_alternative<double_vec>(variant)) {
                                return Error::InvalidGltf;
                            }
                            std::get<double_vec>(variant)[idx++] = value;
                            break;
                        }
                        case dom::element_type::INT64: {
                            std::int64_t value;
                            if (element.get_int64().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }

							if (auto* doubles = std::get_if<double_vec>(&variant); doubles != nullptr) {
								(*doubles)[idx++] = static_cast<double>(value);
							} else if (auto* ints = std::get_if<int64_vec>(&variant); ints != nullptr) {
								(*ints)[idx++] = static_cast<std::int64_t>(value);
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

							if (auto* doubles = std::get_if<double_vec>(&variant); doubles != nullptr) {
								(*doubles)[idx++] = static_cast<double>(value);
							} else if (auto* ints = std::get_if<int64_vec>(&variant); ints != nullptr) {
								(*ints)[idx++] = static_cast<std::int64_t>(value);
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
            return error;
        }
        if (auto error = parseMinMax("min", accessor.min); error != Error::None) {
            return error;
        }

        if (accessorObject["normalized"].get_bool().get(accessor.normalized) != SUCCESS) {
            accessor.normalized = false;
        }

		// This property MUST NOT be set to true for accessors with FLOAT or UNSIGNED_INT component type.
		if (accessor.normalized && (accessor.componentType == ComponentType::UnsignedInt || accessor.componentType == ComponentType::Float)) {
			return Error::InvalidGltf;
		}

        dom::object sparseAccessorObject;
        if (accessorObject["sparse"].get_object().get(sparseAccessorObject) == SUCCESS) {
            SparseAccessor sparse = {};
            std::uint64_t value;
            dom::object child;
            if (sparseAccessorObject["count"].get_uint64().get(value) != SUCCESS) {
                return Error::InvalidGltf;
            }
            sparse.count = static_cast<std::size_t>(value);

            // Accessor Sparce Indices
            if (sparseAccessorObject["indices"].get_object().get(child) != SUCCESS) {
                return Error::InvalidGltf;
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) {
                return Error::InvalidGltf;
            }
            sparse.indicesBufferView = static_cast<std::size_t>(value);

            if (child["byteOffset"].get_uint64().get(value) != SUCCESS) {
                sparse.indicesByteOffset = 0;
            } else {
                sparse.indicesByteOffset = static_cast<std::size_t>(value);
            }

            if (child["componentType"].get_uint64().get(value) != SUCCESS) {
                return Error::InvalidGltf;
            }
            sparse.indexComponentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(value));

            // Accessor Sparse Values
            if (sparseAccessorObject["values"].get_object().get(child) != SUCCESS) {
                return Error::InvalidGltf;
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) {
                return Error::InvalidGltf;
            }
            sparse.valuesBufferView = static_cast<std::size_t>(value);

            if (child["byteOffset"].get_uint64().get(value) != SUCCESS) {
                sparse.valuesByteOffset = 0;
            } else {
                sparse.valuesByteOffset = static_cast<std::size_t>(value);
            }

            accessor.sparse = sparse;
        }

        std::string_view name;
        if (accessorObject["name"].get_string().get(name) == SUCCESS) {
            accessor.name = std::pmr::string(name, resourceAllocator.get());
        }

	    asset.accessors.emplace_back(std::move(accessor));
    }

	return Error::None;
}

fg::Error fg::Parser::parseAnimations(simdjson::dom::array& animations, Asset& asset) {
    using namespace simdjson;

	asset.animations.reserve(animations.size());
    for (auto animationValue : animations) {
        dom::object animationObject;
        Animation animation = {};
        if (animationValue.get_object().get(animationObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        dom::array channels;
        auto channelError = getJsonArray(animationObject, "channels", &channels);
        if (channelError != Error::None) {
            return Error::InvalidGltf;
        }

		animation.channels = decltype(animation.channels)(0, resourceAllocator.get());
        animation.channels.reserve(channels.size());
        for (auto channelValue : channels) {
            dom::object channelObject;
            AnimationChannel channel = {};
            if (channelValue.get_object().get(channelObject) != SUCCESS) {
                return Error::InvalidGltf;
            }

            std::uint64_t sampler;
            if (channelObject["sampler"].get_uint64().get(sampler) != SUCCESS) {
                return Error::InvalidGltf;
            }
            channel.samplerIndex = static_cast<std::size_t>(sampler);

            dom::object targetObject;
            if (channelObject["target"].get_object().get(targetObject) != SUCCESS) {
                return Error::InvalidGltf;
            } else {
                std::uint64_t node;
                if (targetObject["node"].get_uint64().get(node) != SUCCESS) {
                    // We don't support any extensions for animations, so it is required.
                    return Error::InvalidGltf;
                }
                channel.nodeIndex = static_cast<std::size_t>(node);

                std::string_view path;
                if (targetObject["path"].get_string().get(path) != SUCCESS) {
                    return Error::InvalidGltf;
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
            return Error::InvalidGltf;
        }

		animation.samplers = decltype(animation.samplers)(0, resourceAllocator.get());
        animation.samplers.reserve(samplers.size());
        for (auto samplerValue : samplers) {
            dom::object samplerObject;
            AnimationSampler sampler = {};
            if (samplerValue.get_object().get(samplerObject) != SUCCESS) {
                return Error::InvalidGltf;
            }

            std::uint64_t input;
            if (samplerObject["input"].get_uint64().get(input) != SUCCESS) {
                return Error::InvalidGltf;
            }
            sampler.inputAccessor = static_cast<std::size_t>(input);

            std::uint64_t output;
            if (samplerObject["output"].get_uint64().get(output) != SUCCESS) {
                return Error::InvalidGltf;
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
                    return Error::InvalidGltf;
                }
            }

            animation.samplers.emplace_back(sampler);
        }

        std::string_view name;
        if (animationObject["name"].get_string().get(name) == SUCCESS) {
            animation.name = std::pmr::string(name, resourceAllocator.get());
        }

	    asset.animations.emplace_back(std::move(animation));
    }

	return Error::None;
}

fg::Error fg::Parser::parseBuffers(simdjson::dom::array& buffers, Asset& asset) {
    using namespace simdjson;

	asset.buffers.reserve(buffers.size());
    std::size_t bufferIndex = 0;
    for (auto bufferValue : buffers) {
        // Required fields: "byteLength"
        Buffer buffer = {};
        dom::object bufferObject;
        if (bufferValue.get_object().get(bufferObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::uint64_t byteLength;
        if (bufferObject["byteLength"].get_uint64().get(byteLength) != SUCCESS) {
            return Error::InvalidGltf;
        }
		buffer.byteLength = static_cast<std::size_t>(byteLength);

        // When parsing GLB, there's a buffer object that will point to the BUF chunk in the
        // file. Otherwise, data must be specified in the "uri" field.
        std::string_view uriString;
        if (bufferObject["uri"].get_string().get(uriString) == SUCCESS) {
			URIView uriView(uriString);

            if (!uriView.valid()) {
                return Error::InvalidURI;
            }

            if (uriView.isDataUri()) {
                auto [error, source] = decodeDataUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                buffer.data = std::move(source);
            } else if (uriView.isLocalPath() && hasBit(options, Options::LoadExternalBuffers)) {
	            auto [error, source] = loadFileFromUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                buffer.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = uriView;
                buffer.data = std::move(filePath);
            }
        } else if (bufferIndex == 0 && !std::holds_alternative<std::monostate>(glbBuffer)) {
            buffer.data = std::move(glbBuffer);
        } else {
            // All other buffers have to contain an uri field.
            return Error::InvalidGltf;
        }

        if (std::holds_alternative<std::monostate>(buffer.data)) {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (bufferObject["name"].get_string().get(name) == SUCCESS) {
			buffer.name = std::pmr::string(name, resourceAllocator.get());
        }

        ++bufferIndex;
	    asset.buffers.emplace_back(std::move(buffer));
    }

	return Error::None;
}

fg::Error fg::Parser::parseBufferViews(simdjson::dom::array& bufferViews, Asset& asset) {
    using namespace simdjson;

	asset.bufferViews.reserve(bufferViews.size());
    for (auto bufferViewValue : bufferViews) {
        dom::object bufferViewObject;
        if (bufferViewValue.get_object().get(bufferViewObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::uint64_t number;
        BufferView view;
        if (auto error = bufferViewObject["buffer"].get_uint64().get(number); error != SUCCESS) {
            return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
        }
        view.bufferIndex = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteOffset"].get_uint64().get(number); error == SUCCESS) {
            view.byteOffset = static_cast<std::size_t>(number);
        } else if (error == NO_SUCH_FIELD) {
            view.byteOffset = 0;
        } else {
            return Error::InvalidJson;
        }

        if (auto error = bufferViewObject["byteLength"].get_uint64().get(number); error != SUCCESS) {
            return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
        }
        view.byteLength = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteStride"].get_uint64().get(number); error == SUCCESS) {
            view.byteStride = static_cast<std::size_t>(number);
        } else if (error != NO_SUCH_FIELD) {
            return Error::InvalidJson;
        }

        if (auto error = bufferViewObject["target"].get_uint64().get(number); error == SUCCESS) {
            view.target = static_cast<BufferTarget>(number);
        } else if (error != NO_SUCH_FIELD) {
            return Error::InvalidJson;
        }

        std::string_view string;
        if (auto error = bufferViewObject["name"].get_string().get(string); error == SUCCESS) {
            view.name = std::pmr::string(string, resourceAllocator.get());
        } else if (error != NO_SUCH_FIELD) {
            return Error::InvalidJson;
        }

        dom::object extensionObject;
        if (bufferViewObject["extensions"].get_object().get(extensionObject) == SUCCESS) {
            dom::object meshoptCompression;
            if (hasBit(config.extensions, Extensions::EXT_meshopt_compression) && bufferViewObject[extensions::EXT_meshopt_compression].get_object().get(meshoptCompression) == SUCCESS) {
                auto compression = std::make_unique<CompressedBufferView>();

                if (auto error = bufferViewObject["buffer"].get_uint64().get(number); error != SUCCESS) {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->bufferIndex = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["byteOffset"].get_uint64().get(number); error == SUCCESS) {
                    compression->byteOffset = static_cast<std::size_t>(number);
                } else if (error == NO_SUCH_FIELD) {
                    compression->byteOffset = 0;
                } else {
                    return Error::InvalidJson;
                }

                if (auto error = bufferViewObject["byteLength"].get_uint64().get(number); error != SUCCESS) {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->byteLength = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["byteStride"].get_uint64().get(number); error != SUCCESS) {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->byteStride = static_cast<std::size_t>(number);

                if (auto error = bufferViewObject["count"].get_uint64().get(number); error != SUCCESS) {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->count = number;

                if (auto error = bufferViewObject["mode"].get_string().get(string); error != SUCCESS) {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
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
                        return Error::InvalidGltf;
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
                            return Error::InvalidGltf;
                        }
                    }
                } else if (error == NO_SUCH_FIELD) {
                    compression->filter = MeshoptCompressionFilter::None;
                } else {
                    return Error::InvalidJson;
                }

                view.meshoptCompression = std::move(compression);
            }
        }

	    asset.bufferViews.emplace_back(std::move(view));
    }

	return Error::None;
}

fg::Error fg::Parser::parseCameras(simdjson::dom::array& cameras, Asset& asset) {
    using namespace simdjson;

	asset.cameras.reserve(cameras.size());
    for (auto cameraValue : cameras) {
        Camera camera = {};
        dom::object cameraObject;
        if (cameraValue.get_object().get(cameraObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (cameraObject["name"].get_string().get(name) == SUCCESS) {
            camera.name = std::pmr::string(name, resourceAllocator.get());
        }

        std::string_view type;
        if (cameraObject["type"].get_string().get(type) != SUCCESS) {
            return Error::InvalidGltf;
        }

        if (type == "perspective") {
            dom::object perspectiveCamera;
            if (cameraObject["perspective"].get_object().get(perspectiveCamera) != SUCCESS) {
                return Error::InvalidGltf;
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
                return Error::InvalidGltf;
            }

            if (perspectiveCamera["znear"].get_double().get(value) == SUCCESS) {
                perspective.znear = static_cast<float>(value);
            } else {
                return Error::InvalidGltf;
            }

            camera.camera = perspective;
        } else if (type == "orthographic") {
            dom::object orthographicCamera;
            if (cameraObject["orthographic"].get_object().get(orthographicCamera) != SUCCESS) {
                return Error::InvalidGltf;
            }

            Camera::Orthographic orthographic = {};
            double value;
            if (orthographicCamera["xmag"].get_double().get(value) == SUCCESS) {
                orthographic.xmag = static_cast<float>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["ymag"].get_double().get(value) == SUCCESS) {
                orthographic.ymag = static_cast<float>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["zfar"].get_double().get(value) == SUCCESS) {
                orthographic.zfar = static_cast<float>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["znear"].get_double().get(value) == SUCCESS) {
                orthographic.znear = static_cast<float>(value);
            } else {
                return Error::InvalidGltf;
            }

            camera.camera = orthographic;
        } else {
            return Error::InvalidGltf;
        }

	    asset.cameras.emplace_back(std::move(camera));
    }

	return Error::None;
}

fg::Error fg::Parser::parseExtensions(simdjson::dom::object& extensionsObject, Asset& asset) {
    using namespace simdjson;

    for (auto extensionValue : extensionsObject) {
        dom::object extensionObject;
        if (auto error = extensionValue.value.get_object().get(extensionObject); error != SUCCESS) {
            if (error == INCORRECT_TYPE) {
                continue; // We want to ignore
            }
            return Error::InvalidGltf;
        }

        auto hash = crcStringFunction(extensionValue.key);
        switch (hash) {
            case force_consteval<crc32c(extensions::KHR_lights_punctual)>: {
                if (!hasBit(config.extensions, Extensions::KHR_lights_punctual))
                    break;

                dom::array lightsArray;
                if (auto error = extensionObject["lights"].get_array().get(lightsArray); error == SUCCESS) {
                    if (auto lightsError = parseLights(lightsArray, asset); lightsError != Error::None)
						return lightsError;
                } else if (error != NO_SUCH_FIELD) {
                    return Error::InvalidGltf;
                }
                break;
            }
        }
    }

	return Error::None;
}

fg::Error fg::Parser::parseImages(simdjson::dom::array& images, Asset& asset) {
    using namespace simdjson;

	asset.images.reserve(images.size());
    for (auto imageValue : images) {
        Image image = {};
        dom::object imageObject;
        if (imageValue.get_object().get(imageObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::string_view uriString;
        if (imageObject["uri"].get_string().get(uriString) == SUCCESS) {
            if (imageObject["bufferView"].error() == SUCCESS) {
                // If uri is declared, bufferView cannot be declared.
                return Error::InvalidGltf;
            }

            URIView uriView(uriString);
            if (!uriView.valid()) {
                return Error::InvalidURI;
            }

            if (uriView.isDataUri()) {
                auto [error, source] = decodeDataUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                image.data = std::move(source);
            } else if (uriView.isLocalPath() && hasBit(options, Options::LoadExternalImages)) {
	            auto [error, source] = loadFileFromUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                image.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = uriView;
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
                return Error::InvalidGltf;
            }

            image.data = sources::BufferView {
                static_cast<std::size_t>(bufferViewIndex),
                getMimeTypeFromString(mimeType),
            };
        }

        if (std::holds_alternative<std::monostate>(image.data)) {
            return Error::InvalidGltf;
        }

        // name is optional.
        std::string_view name;
        if (imageObject["name"].get_string().get(name) == SUCCESS) {
            image.name = std::pmr::string(name, resourceAllocator.get());
        }

        asset.images.emplace_back(std::move(image));
    }

	return Error::None;
}

fg::Error fg::Parser::parseLights(simdjson::dom::array& lights, Asset& asset) {
    using namespace simdjson;

    asset.lights.reserve(lights.size());
    for (auto lightValue : lights) {
        dom::object lightObject;
        if (lightValue.get_object().get(lightObject) != SUCCESS) {
            return Error::InvalidGltf;
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
                    return Error::InvalidGltf;
                }
            }
        } else {
            return Error::InvalidGltf;
        }

        if (light.type == LightType::Spot) {
            dom::object spotObject;
            if (lightObject["spot"].get_object().get(spotObject) != SUCCESS) {
                return Error::InvalidGltf;
            }

            double innerConeAngle;
            if (auto error = spotObject["innerConeAngle"].get_double().get(innerConeAngle); error == SUCCESS) {
                light.innerConeAngle = static_cast<float>(innerConeAngle);
            } else if (error == NO_SUCH_FIELD) {
                light.innerConeAngle = 0.0f;
            } else {
                return Error::InvalidGltf;
            }

            double outerConeAngle;
            if (auto error = spotObject["outerConeAngle"].get_double().get(outerConeAngle); error == SUCCESS) {
                light.outerConeAngle = static_cast<float>(outerConeAngle);
            } else if (error == NO_SUCH_FIELD) {
                static constexpr double pi = 3.141592653589793116;
                light.outerConeAngle = static_cast<float>(pi / 4.0);
            } else {
                return Error::InvalidGltf;
            }
        }

        dom::array colorArray;
        if (auto error = lightObject["color"].get_array().get(colorArray); error == SUCCESS) {
            if (colorArray.size() != 3U) {
                return Error::InvalidGltf;
            }
            for (std::size_t i = 0U; i < colorArray.size(); ++i) {
                double color;
                if (colorArray.at(i).get_double().get(color) == SUCCESS) {
                    light.color[i] = static_cast<float>(color);
                } else {
                    return Error::InvalidGltf;
                }
            }
        } else if (error == NO_SUCH_FIELD) {
            light.color = std::array<float, 3>{{1.0f, 1.0f, 1.0f}};
        } else {
            return Error::InvalidGltf;
        }

        double intensity;
        if (lightObject["intensity"].get_double().get(intensity) == SUCCESS) {
            light.intensity = static_cast<float>(intensity);
        } else {
            light.intensity = 0.0f;
        }

        double range;
        if (lightObject["range"].get_double().get(range) == SUCCESS) {
            light.range = static_cast<float>(range);
        }

        std::string_view name;
        if (lightObject["name"].get_string().get(name) == SUCCESS) {
            light.name = std::pmr::string(name, resourceAllocator.get());
        }

        asset.lights.emplace_back(std::move(light));
    }

	return Error::None;
}

fg::Error fg::Parser::parseMaterials(simdjson::dom::array& materials, Asset& asset) {
    using namespace simdjson;

    asset.materials.reserve(materials.size());
    for (auto materialValue : materials) {
        dom::object materialObject;
        if (materialValue.get_object().get(materialObject) != SUCCESS) {
            return Error::InvalidGltf;
        }
        Material material = {};

        dom::array emissiveFactor;
        if (materialObject["emissiveFactor"].get_array().get(emissiveFactor) == SUCCESS) {
            if (emissiveFactor.size() != 3) {
                return Error::InvalidGltf;
            }
            for (auto i = 0U; i < 3; ++i) {
                double val;
                if (emissiveFactor.at(i).get_double().get(val) != SUCCESS) {
                    return Error::InvalidGltf;
                }
                material.emissiveFactor[i] = static_cast<float>(val);
            }
        } else {
            material.emissiveFactor = {{ 0, 0, 0 }};
        }

	    {
		    NormalTextureInfo normalTextureInfo = {};
		    if (auto error = parseTextureInfo(materialObject, "normalTexture", &normalTextureInfo, config.extensions, TextureInfoType::NormalTexture); error == Error::None) {
			    material.normalTexture = std::move(normalTextureInfo);
		    } else if (error != Error::MissingField) {
			    return error;
		    }
	    }

	    {
			OcclusionTextureInfo occlusionTextureInfo = {};
	        if (auto error = parseTextureInfo(materialObject, "occlusionTexture", &occlusionTextureInfo, config.extensions, TextureInfoType::OcclusionTexture); error == Error::None) {
	            material.occlusionTexture = std::move(occlusionTextureInfo);
	        } else if (error != Error::MissingField) {
	            return error;
	        }
	    }

	    {
		    TextureInfo textureInfo = {};
	        if (auto error = parseTextureInfo(materialObject, "emissiveTexture", &textureInfo, config.extensions); error == Error::None) {
	            material.emissiveTexture = std::move(textureInfo);
	        } else if (error != Error::MissingField) {
	            return error;
	        }
	    }

        dom::object pbrMetallicRoughness;
        if (materialObject["pbrMetallicRoughness"].get_object().get(pbrMetallicRoughness) == SUCCESS) {
            PBRData pbr = {};

            dom::array baseColorFactor;
            if (pbrMetallicRoughness["baseColorFactor"].get_array().get(baseColorFactor) == SUCCESS) {
                for (auto i = 0U; i < 4; ++i) {
                    double val;
                    if (baseColorFactor.at(i).get_double().get(val) != SUCCESS) {
                        return Error::InvalidGltf;
                    }
                    pbr.baseColorFactor[i] = static_cast<float>(val);
                }
            }

            double factor;
            if (pbrMetallicRoughness["metallicFactor"].get_double().get(factor) == SUCCESS) {
                pbr.metallicFactor = static_cast<float>(factor);
            }
            if (pbrMetallicRoughness["roughnessFactor"].get_double().get(factor) == SUCCESS) {
                pbr.roughnessFactor = static_cast<float>(factor);
            }

	        TextureInfo textureInfo;
            if (auto error = parseTextureInfo(pbrMetallicRoughness, "baseColorTexture", &textureInfo, config.extensions); error == Error::None) {
                pbr.baseColorTexture = std::move(textureInfo);
            } else if (error != Error::MissingField) {
                return error;
            }

            if (auto error = parseTextureInfo(pbrMetallicRoughness, "metallicRoughnessTexture", &textureInfo, config.extensions); error == Error::None) {
                pbr.metallicRoughnessTexture = std::move(textureInfo);
            } else if (error != Error::MissingField) {
                return error;
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
                return Error::InvalidGltf;
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
            material.name = std::pmr::string(name, resourceAllocator.get());
        }

        material.unlit = false;
        dom::object extensionsObject;
        if (auto extensionError = materialObject["extensions"].get_object().get(extensionsObject); extensionError == SUCCESS) {
			if (hasBit(config.extensions, Extensions::KHR_materials_anisotropy)) {
				dom::object anisotropyObject;
				auto anisotropyError = extensionsObject[extensions::KHR_materials_anisotropy].get_object().get(anisotropyObject);
				if (anisotropyError == SUCCESS) {
					auto anisotropy = std::make_unique<MaterialAnisotropy>();

					double anisotropyStrength;
					if (auto error = anisotropyObject["anisotropyStrength"].get_double().get(anisotropyStrength); error == SUCCESS) {
						anisotropy->anisotropyStrength = static_cast<float>(anisotropyStrength);
					} else if (error == NO_SUCH_FIELD) {
						anisotropy->anisotropyStrength = 0.0f;
					} else {
						return Error::InvalidJson;
					}

					double anisotropyRotation;
					if (auto error = anisotropyObject["anisotropyRotation"].get_double().get(anisotropyRotation); error == SUCCESS) {
						anisotropy->anisotropyRotation = static_cast<float>(anisotropyRotation);
					} else if (error == NO_SUCH_FIELD) {
						anisotropy->anisotropyRotation = 0.0f;
					} else {
						return Error::InvalidJson;
					}

					TextureInfo anisotropyTexture;
					if (auto error = parseTextureInfo(anisotropyObject, "anisotropyTexture", &anisotropyTexture, config.extensions); error == Error::None) {
						anisotropy->anisotropyTexture = std::move(anisotropyTexture);
					} else if (error != Error::MissingField) {
						return error;
					}
				}
			}

            if (hasBit(config.extensions, Extensions::KHR_materials_clearcoat)) {
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
                        return Error::InvalidJson;
                    }

                    TextureInfo clearcoatTexture;
                    if (auto error = parseTextureInfo(clearcoatObject, "clearcoatTexture", &clearcoatTexture, config.extensions); error == Error::None) {
                        clearcoat->clearcoatTexture = std::move(clearcoatTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    double clearcoatRoughnessFactor;
                    if (auto error = clearcoatObject["clearcoatRoughnessFactor"].get_double().get(clearcoatRoughnessFactor); error == SUCCESS) {
                        clearcoat->clearcoatRoughnessFactor = static_cast<float>(clearcoatRoughnessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        clearcoat->clearcoatRoughnessFactor = 0.0f;
                    } else {
                        return Error::InvalidJson;
                    }

                    TextureInfo clearcoatRoughnessTexture;
                    if (auto error = parseTextureInfo(clearcoatObject, "clearcoatRoughnessTexture", &clearcoatRoughnessTexture, config.extensions); error == Error::None) {
                        clearcoat->clearcoatRoughnessTexture = std::move(clearcoatRoughnessTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    TextureInfo clearcoatNormalTexture;
                    if (auto error = parseTextureInfo(clearcoatObject, "clearcoatNormalTexture", &clearcoatNormalTexture, config.extensions); error == Error::None) {
                        clearcoat->clearcoatNormalTexture = std::move(clearcoatNormalTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.clearcoat = std::move(clearcoat);
                } else if (clearcoatError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_emissive_strength)) {
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
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_ior)) {
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
                        return Error::InvalidJson;
                    }
                } else if (iorError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_iridescence)) {
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
                        return Error::InvalidGltf;
                    }

                    TextureInfo iridescenceTexture;
                    if (auto error = parseTextureInfo(iridescenceObject, "specularTexture", &iridescenceTexture, config.extensions); error == Error::None) {
                        iridescence->iridescenceTexture = std::move(iridescenceTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    double iridescenceIor;
                    if (auto error = iridescenceObject["iridescenceIor"].get_double().get(iridescenceIor); error == SUCCESS) {
                        iridescence->iridescenceIor = static_cast<float>(iridescenceIor);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceIor = 1.3f;
                    } else {
                        return Error::InvalidGltf;
                    }

                    double iridescenceThicknessMinimum;
                    if (auto error = iridescenceObject["iridescenceThicknessMinimum"].get_double().get(iridescenceThicknessMinimum); error == SUCCESS) {
                        iridescence->iridescenceThicknessMinimum = static_cast<float>(iridescenceThicknessMinimum);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceThicknessMinimum = 100.0f;
                    } else {
                        return Error::InvalidGltf;
                    }

                    double iridescenceThicknessMaximum;
                    if (auto error = iridescenceObject["iridescenceThicknessMaximum"].get_double().get(iridescenceThicknessMaximum); error == SUCCESS) {
                        iridescence->iridescenceThicknessMaximum = static_cast<float>(iridescenceThicknessMaximum);
                    } else if (error == NO_SUCH_FIELD) {
                        iridescence->iridescenceThicknessMaximum = 400.0f;
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo iridescenceThicknessTexture;
                    if (auto error = parseTextureInfo(iridescenceObject, "specularTexture", &iridescenceThicknessTexture, config.extensions); error == Error::None) {
                        iridescence->iridescenceThicknessTexture = std::move(iridescenceThicknessTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.iridescence = std::move(iridescence);
                } else if (iridescenceError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_sheen)) {
                dom::object sheenObject;
                auto sheenError = extensionsObject[extensions::KHR_materials_sheen].get_object().get(sheenObject);
                if (sheenError == SUCCESS) {
                    auto sheen = std::make_unique<MaterialSheen>();

                    dom::array sheenColorFactor;
                    if (auto error = sheenObject["sheenColorFactor"].get_array().get(sheenColorFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : sheenColorFactor) {
                            if (i >= sheen->sheenColorFactor.size()) {
                                return Error::InvalidGltf;
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            sheen->sheenColorFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        sheen->sheenColorFactor = std::array<float, 3>{{ 0.0f, 0.0f, 0.0f }};
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo sheenColorTexture;
                    if (auto error = parseTextureInfo(sheenObject, "sheenColorTexture", &sheenColorTexture, config.extensions); error == Error::None) {
                        sheen->sheenColorTexture = std::move(sheenColorTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    double sheenRoughnessFactor;
                    if (auto error = sheenObject["sheenRoughnessFactor"].get_double().get(sheenRoughnessFactor); error == SUCCESS) {
                        sheen->sheenRoughnessFactor = static_cast<float>(sheenRoughnessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        sheen->sheenRoughnessFactor = 0.0f;
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo sheenRoughnessTexture;
                    if (auto error = parseTextureInfo(sheenObject, "sheenRoughnessTexture", &sheenRoughnessTexture, config.extensions); error == Error::None) {
                        sheen->sheenRoughnessTexture = std::move(sheenRoughnessTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.sheen = std::move(sheen);
                } else if (sheenError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_specular)) {
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
                        return Error::InvalidGltf;
                    }

                    TextureInfo specularTexture;
                    if (auto error = parseTextureInfo(specularObject, "specularTexture", &specularTexture, config.extensions); error == Error::None) {
                        specular->specularTexture = std::move(specularTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    dom::array specularColorFactor;
                    if (auto error = specularObject["specularColorFactor"].get_array().get(specularColorFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : specularColorFactor) {
                            if (i >= specular->specularColorFactor.size()) {
                                return Error::InvalidGltf;
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            specular->specularColorFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        specular->specularColorFactor = std::array<float, 3>{{ 1.0f, 1.0f, 1.0f }};
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo specularColorTexture;
                    if (auto error = parseTextureInfo(specularObject, "specularColorTexture", &specularColorTexture, config.extensions); error == Error::None) {
                        specular->specularColorTexture = std::move(specularColorTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.specular = std::move(specular);
                } else if (specularError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_transmission)) {
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
                        return Error::InvalidGltf;
                    }

                    TextureInfo transmissionTexture;
                    if (auto error = parseTextureInfo(transmissionObject, "transmissionTexture", &transmissionTexture, config.extensions); error == Error::None) {
                        transmission->transmissionTexture = std::move(transmissionTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.transmission = std::move(transmission);
                } else if (transmissionError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_unlit)) {
                dom::object unlitObject;
                auto unlitError = extensionsObject[extensions::KHR_materials_unlit].get_object().get(unlitObject);
                if (unlitError == SUCCESS) {
                    material.unlit = true;
                } else if (unlitError != NO_SUCH_FIELD) {
                    return Error::InvalidGltf;
                }
            }

            if (hasBit(config.extensions, Extensions::KHR_materials_volume)) {
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
                        return Error::InvalidGltf;
                    }

                    TextureInfo thicknessTexture;
                    if (auto error = parseTextureInfo(volumeObject, "thicknessTexture", &thicknessTexture, config.extensions); error == Error::None) {
                        volume->thicknessTexture = std::move(thicknessTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    double attenuationDistance;
                    if (auto error = volumeObject["attenuationDistance"].get_double().get(attenuationDistance); error == SUCCESS) {
                        volume->attenuationDistance = static_cast<float>(attenuationDistance);
                    } else if (error == NO_SUCH_FIELD) {
                        volume->attenuationDistance = +std::numeric_limits<float>::infinity();
                    } else {
                        return Error::InvalidGltf;
                    }

                    dom::array attenuationColor;
                    if (auto error = volumeObject["attenuationColor"].get_array().get(attenuationColor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : attenuationColor) {
                            if (i >= volume->attenuationColor.size()) {
                                return Error::InvalidGltf;
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            (volume->attenuationColor)[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        volume->attenuationColor = std::array<float, 3>{{1.0f, 1.0f, 1.0f}};
                    } else {
                        return Error::InvalidGltf;
                    }

                    material.volume = std::move(volume);
                } else if (volumeError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }

#if FASTGLTF_ENABLE_DEPRECATED_EXT
            if (hasBit(config.extensions, Extensions::KHR_materials_pbrSpecularGlossiness)) {
                dom::object specularGlossinessObject;
                auto specularGlossinessError = extensionsObject[extensions::KHR_materials_pbrSpecularGlossiness].get_object().get(specularGlossinessObject);
                if (specularGlossinessError == SUCCESS) {
                    auto specularGlossiness = std::make_unique<MaterialSpecularGlossiness>();

                    dom::array diffuseFactor;
                    if (auto error = specularGlossinessObject["diffuseFactor"].get_array().get(diffuseFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : diffuseFactor) {
                            if (i >= specularGlossiness->diffuseFactor.size()) {
                                return Error::InvalidGltf;
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            specularGlossiness->diffuseFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        specularGlossiness->diffuseFactor = std::array<float, 4>{{ 1.0f, 1.0f, 1.0f, 1.0f }};
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo diffuseTexture;
                    if (auto error = parseTextureInfo(specularGlossinessObject, "diffuseTexture", &diffuseTexture, config.extensions); error == Error::None) {
                        specularGlossiness->diffuseTexture = std::move(diffuseTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    dom::array specularFactor;
                    if (auto error = specularGlossinessObject["specularFactor"].get_array().get(specularFactor); error == SUCCESS) {
                        std::size_t i = 0;
                        for (auto factor : specularFactor) {
                            if (i >= specularGlossiness->specularFactor.size()) {
                                return Error::InvalidGltf;
                            }
                            double value;
                            if (factor.get_double().get(value) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            specularGlossiness->specularFactor[i++] = static_cast<float>(value);
                        }
                    } else if (error == NO_SUCH_FIELD) {
                        specularGlossiness->specularFactor = std::array<float, 3>{{ 1.0f, 1.0f, 1.0f }};
                    } else {
                        return Error::InvalidGltf;
                    }

                    double glossinessFactor;
                    if (auto error = specularGlossinessObject["glossinessFactor"].get_double().get(glossinessFactor); error == SUCCESS) {
                        specularGlossiness->glossinessFactor = static_cast<float>(glossinessFactor);
                    } else if (error == NO_SUCH_FIELD) {
                        specularGlossiness->glossinessFactor = 1.0f;
                    } else {
                        return Error::InvalidGltf;
                    }

                    TextureInfo specularGlossinessTexture;
                    if (auto error = parseTextureInfo(specularGlossinessObject, "specularGlossinessTexture", &specularGlossinessTexture, config.extensions); error == Error::None) {
                        specularGlossiness->specularGlossinessTexture = std::move(specularGlossinessTexture);
                    } else if (error != Error::MissingField) {
                        return error;
                    }

                    material.specularGlossiness = std::move(specularGlossiness);
                } else if (specularGlossinessError != NO_SUCH_FIELD) {
                    return Error::InvalidJson;
                }
            }
#endif
        } else if (extensionError != NO_SUCH_FIELD) {
            return Error::InvalidJson;
        }

        asset.materials.emplace_back(std::move(material));
    }

	return Error::None;
}

fg::Error fg::Parser::parseMeshes(simdjson::dom::array& meshes, Asset& asset) {
    using namespace simdjson;

    asset.meshes.reserve(meshes.size());
    for (auto meshValue : meshes) {
        // Required fields: "primitives"
        dom::object meshObject;
        if (meshValue.get_object().get(meshObject) != SUCCESS) {
            return Error::InvalidGltf;
        }
        Mesh mesh = {};

        dom::array array;
        auto meshError = getJsonArray(meshObject, "primitives", &array);
        if (meshError == Error::MissingField) {
            return Error::InvalidGltf;
        } else if (meshError != Error::None) {
            return meshError;
        } else {
	        mesh.primitives = decltype(mesh.primitives)(0, resourceAllocator.get());
            mesh.primitives.reserve(array.size());
            for (auto primitiveValue : array) {
                // Required fields: "attributes"
                Primitive primitive = {};
                dom::object primitiveObject;
                if (primitiveValue.get_object().get(primitiveObject) != SUCCESS) {
                    return Error::InvalidGltf;
                }

                auto parseAttributes = [this](dom::object& object, decltype(primitive.attributes)& attributes) -> auto {
                    // We iterate through the JSON object and write each key/pair value into the
                    // attribute map. The keys are only validated in the validate() method.
					attributes = decltype(primitive.attributes)(0, resourceAllocator.get());
					attributes.reserve(object.size());
                    for (const auto& field : object) {
                        const auto key = field.key;

                        std::uint64_t attributeIndex;
                        if (field.value.get_uint64().get(attributeIndex) != SUCCESS) {
                            return Error::InvalidGltf;
                        }
						attributes.emplace_back(
							std::make_pair(std::pmr::string(key, resourceAllocator.get()), static_cast<std::size_t>(attributeIndex)));
                    }
                    return Error::None;
                };

                dom::object attributesObject;
                if (primitiveObject["attributes"].get_object().get(attributesObject) != SUCCESS) {
                    return Error::InvalidGltf;
                }
                parseAttributes(attributesObject, primitive.attributes);

                dom::array targets;
                if (primitiveObject["targets"].get_array().get(targets) == SUCCESS) {
					primitive.targets = decltype(primitive.targets)(0, resourceAllocator.get());
					primitive.targets.reserve(targets.size());
                    for (auto targetValue : targets) {
                        if (targetValue.get_object().get(attributesObject) != SUCCESS) {
                            return Error::InvalidGltf;
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
	        mesh.weights = decltype(mesh.weights)(0, resourceAllocator.get());
            mesh.weights.reserve(array.size());
            for (auto weightValue : array) {
                double val;
                if (weightValue.get_double().get(val) != SUCCESS) {
                    return Error::InvalidGltf;
                }
                mesh.weights.emplace_back(static_cast<float>(val));
            }
        } else if (meshError != Error::MissingField && meshError != Error::None) {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (meshObject["name"].get_string().get(name) == SUCCESS) {
            mesh.name = std::pmr::string(name, resourceAllocator.get());
        }

        asset.meshes.emplace_back(std::move(mesh));
    }

	return Error::None;
}

fg::Error fg::Parser::parseNodes(simdjson::dom::array& nodes, Asset& asset) {
    using namespace simdjson;

    asset.nodes.reserve(nodes.size());
    for (auto nodeValue : nodes) {
        Node node = {};
        dom::object nodeObject;
        if (nodeValue.get_object().get(nodeObject) != SUCCESS) {
            return Error::InvalidGltf;
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
			node.children = decltype(node.children)(0, resourceAllocator.get());
			node.children.reserve(array.size());
            for (auto childValue : array) {
                if (childValue.get_uint64().get(index) != SUCCESS) {
                    return Error::InvalidGltf;
                }

                node.children.emplace_back(static_cast<std::size_t>(index));
            }
        } else if (childError != Error::MissingField) {
            return childError;
        }

        auto weightsError = getJsonArray(nodeObject, "weights", &array);
        if (weightsError != Error::MissingField) {
            if (weightsError != Error::None) {
	            node.weights = decltype(node.weights)(0, resourceAllocator.get());
                node.weights.reserve(array.size());
                for (auto weightValue : array) {
                    double val;
                    if (weightValue.get_double().get(val) != SUCCESS) {
                        return Error::InvalidGltf;
                    }
                    node.weights.emplace_back(static_cast<float>(val));
                }
            } else {
                return Error::InvalidGltf;
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
                        return Error::InvalidGltf;
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
                        return Error::InvalidGltf;
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
                        return Error::InvalidGltf;
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
                    node.lightIndex = static_cast<std::size_t>(light);
                }
            }

            dom::object gpuInstancingObject;
            if (extensionsObject[extensions::EXT_mesh_gpu_instancing].get_object().get(gpuInstancingObject) == SUCCESS) {
                dom::object attributesObject;
                if (gpuInstancingObject["attributes"].get_object().get(attributesObject) == SUCCESS) {
                    auto parseAttributes = [this](dom::object& object, decltype(node.instancingAttributes)& attributes) -> auto {
                        // We iterate through the JSON object and write each key/pair value into the
                        // attribute map. The keys are only validated in the validate() method.
                        attributes = decltype(node.instancingAttributes)(0, resourceAllocator.get());
                        attributes.reserve(object.size());
                        for (const auto& field : object) {
                            const auto key = field.key;

                            std::uint64_t attributeIndex;
                            if (field.value.get_uint64().get(attributeIndex) != SUCCESS) {
                                return Error::InvalidGltf;
                            }
                            attributes.emplace_back(
                                std::make_pair(std::pmr::string(key, resourceAllocator.get()), static_cast<std::size_t>(attributeIndex)));
                        }
                        return Error::None;
                    };
                    parseAttributes(attributesObject, node.instancingAttributes);
                }
            }
        }

        std::string_view name;
        if (nodeObject["name"].get_string().get(name) == SUCCESS) {
            node.name = std::pmr::string(name, resourceAllocator.get());
        }

        asset.nodes.emplace_back(std::move(node));
    }

	return Error::None;
}

fg::Error fg::Parser::parseSamplers(simdjson::dom::array& samplers, Asset& asset) {
    using namespace simdjson;

    std::uint64_t number;
    asset.samplers.reserve(samplers.size());
    for (auto samplerValue : samplers) {
        Sampler sampler = {};
        dom::object samplerObject;
        if (samplerValue.get_object().get(samplerObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (samplerObject["name"].get_string().get(name) == SUCCESS) {
            sampler.name = std::pmr::string(name, resourceAllocator.get());
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

        asset.samplers.emplace_back(std::move(sampler));
    }

	return Error::None;
}

fg::Error fg::Parser::parseScenes(simdjson::dom::array& scenes, Asset& asset) {
    using namespace simdjson;

    asset.scenes.reserve(scenes.size());
    for (auto sceneValue : scenes) {
        // The scene object can be completely empty
        Scene scene = {};
        dom::object sceneObject;
        if (sceneValue.get_object().get(sceneObject) != SUCCESS) {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (sceneObject["name"].get_string().get(name) == SUCCESS) {
            scene.name = std::pmr::string(name, resourceAllocator.get());
        }

        // Parse the array of nodes.
        dom::array nodes;
        auto nodeError = getJsonArray(sceneObject, "nodes", &nodes);
        if (nodeError == Error::None) {
            scene.nodeIndices = decltype(scene.nodeIndices)(0, resourceAllocator.get());
			scene.nodeIndices.reserve(nodes.size());
            for (auto nodeValue : nodes) {
                std::uint64_t index;
                if (nodeValue.get_uint64().get(index) != SUCCESS) {
                    return Error::InvalidGltf;
                }

                scene.nodeIndices.emplace_back(static_cast<std::size_t>(index));
            }

            asset.scenes.emplace_back(std::move(scene));
        } else if (nodeError != Error::MissingField) {
            return nodeError;
        }
    }

	return Error::None;
}

fg::Error fg::Parser::parseSkins(simdjson::dom::array& skins, Asset& asset) {
    using namespace simdjson;

    asset.skins.reserve(skins.size());
    for (auto skinValue : skins) {
        Skin skin = {};
        dom::object skinObject;
        if (skinValue.get_object().get(skinObject) != SUCCESS) {
            return Error::InvalidGltf;
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
            return Error::InvalidGltf;
        }
	    skin.joints = decltype(skin.joints)(0, resourceAllocator.get());
        skin.joints.reserve(jointsArray.size());
        for (auto jointValue : jointsArray) {
            if (jointValue.get_uint64().get(index) != SUCCESS) {
                return Error::InvalidGltf;
            }
            skin.joints.emplace_back(index);
        }

        std::string_view name;
        if (skinObject["name"].get_string().get(name) == SUCCESS) {
            skin.name = std::pmr::string(name, resourceAllocator.get());
        }
        asset.skins.emplace_back(std::move(skin));
    }

	return Error::None;
}

fg::Error fg::Parser::parseTextures(simdjson::dom::array& textures, Asset& asset) {
    using namespace simdjson;

    asset.textures.reserve(textures.size());
    for (auto textureValue : textures) {
        Texture texture;
        dom::object textureObject;
        if (textureValue.get_object().get(textureObject) != SUCCESS) {
            return Error::InvalidGltf;
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
            if (!parseTextureExtensions(texture, extensionsObject, config.extensions)) {
                return Error::InvalidGltf;
            }
        }

        // The index of the sampler used by this texture. When undefined, a sampler with
        // repeat wrapping and auto filtering SHOULD be used.
        std::uint64_t samplerIndex;
        if (textureObject["sampler"].get_uint64().get(samplerIndex) == SUCCESS) {
            texture.samplerIndex = static_cast<std::size_t>(samplerIndex);
        }

        std::string_view name;
        if (textureObject["name"].get_string().get(name) == SUCCESS) {
            texture.name = std::pmr::string(name, resourceAllocator.get());
        }

        asset.textures.emplace_back(std::move(texture));
    }

	return Error::None;
}

#pragma endregion

#pragma region GltfDataBuffer
std::size_t fg::getGltfBufferPadding() noexcept {
    return simdjson::SIMDJSON_PADDING;
}

fg::GltfDataBuffer::GltfDataBuffer() noexcept = default;
fg::GltfDataBuffer::~GltfDataBuffer() noexcept = default;

fg::GltfDataBuffer::GltfDataBuffer(span<std::byte> data) noexcept {
	dataSize = data.size();

	allocatedSize = data.size() + getGltfBufferPadding();
	buffer = decltype(buffer)(new std::byte[allocatedSize]);
	auto* ptr = buffer.get();

	std::memcpy(ptr, data.data(), dataSize);
	std::memset(ptr + dataSize, 0, allocatedSize - dataSize);

	bufferPointer = ptr;
}

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
    allocatedSize = byteCount + getGltfBufferPadding();
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
    allocatedSize = dataSize + getGltfBufferPadding();
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

fg::Expected<fg::Asset> fg::Parser::loadGLTF(GltfDataBuffer* buffer, fs::path directory, Options options, Category categories) {
    using namespace simdjson;

    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (hasBit(options, Options::LoadExternalBuffers) && !fs::is_directory(directory)) {
        return Expected<Asset>(Error::InvalidPath);
    }

	this->options = options;
	this->directory = directory;

    // If we own the allocation of the JSON data, we'll try to minify the JSON, which, in most cases,
    // will speed up the parsing by a small amount.
    std::size_t jsonLength = buffer->getBufferSize();
    if (buffer->buffer != nullptr && hasBit(options, Options::MinimiseJsonBeforeParsing)) {
        std::size_t newLength = 0;
        auto result = simdjson::minify(reinterpret_cast<const char*>(buffer->bufferPointer), buffer->getBufferSize(),
                                       reinterpret_cast<char*>(buffer->bufferPointer), newLength);
        if (result != SUCCESS || newLength == 0) {
            return Expected<Asset>(Error::InvalidJson);
        }
        buffer->dataSize = jsonLength = newLength;
    }

    auto view = padded_string_view(reinterpret_cast<const std::uint8_t*>(buffer->bufferPointer), jsonLength, buffer->allocatedSize);
	simdjson::dom::object root;
    if (auto error = jsonParser->parse(view).get(root); error != SUCCESS) {
	    return Expected<Asset>(Error::InvalidJson);
    }

	return parse(root, categories);
}

fg::Expected<fg::Asset> fg::Parser::loadBinaryGLTF(GltfDataBuffer* buffer, fs::path directory, Options options, Category categories) {
    using namespace simdjson;

    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (hasBit(options, Options::LoadExternalBuffers) && !fs::is_directory(directory)) {
	    return Expected<Asset>(Error::InvalidPath);
    }

	this->options = options;
	this->directory = directory;

	std::size_t offset = 0UL;
    auto read = [&buffer, &offset](void* dst, std::size_t size) mutable {
        std::memcpy(dst, buffer->bufferPointer + offset, size);
        offset += size;
    };

    BinaryGltfHeader header = {};
    read(&header, sizeof header);
    if (header.magic != binaryGltfHeaderMagic || header.version != 2) {
	    return Expected<Asset>(Error::InvalidGLB);
    }
    if (header.length >= buffer->allocatedSize) {
	    return Expected<Asset>(Error::InvalidGLB);
    }

    // The glTF 2 spec specifies that in GLB files the order of chunks is predefined. Specifically,
    //  1. JSON chunk
    //  2. BIN chunk (optional)
    BinaryGltfChunk jsonChunk = {};
    read(&jsonChunk, sizeof jsonChunk);
    if (jsonChunk.chunkType != binaryGltfJsonChunkMagic) {
	    return Expected<Asset>(Error::InvalidGLB);
    }

    // Create a string view of the JSON chunk in the GLB data buffer. The documentation of parse()
    // says the padding can be initialised to anything, apparently. Therefore, this should work.
    simdjson::padded_string_view jsonChunkView(reinterpret_cast<const std::uint8_t*>(buffer->bufferPointer) + offset,
                                               jsonChunk.chunkLength,
                                               jsonChunk.chunkLength + SIMDJSON_PADDING);
    offset += jsonChunk.chunkLength;

	simdjson::dom::object root;
    if (jsonParser->parse(jsonChunkView).get(root) != SUCCESS) {
	    return Expected<Asset>(Error::InvalidJson);
    }

    // Is there enough room for another chunk header?
    if (header.length > (offset + sizeof(BinaryGltfChunk))) {
        BinaryGltfChunk binaryChunk = {};
        read(&binaryChunk, sizeof binaryChunk);

        if (binaryChunk.chunkType != binaryGltfDataChunkMagic) {
	        return Expected<Asset>(Error::InvalidGLB);
        }

        if (hasBit(options, Options::LoadGLBBuffers)) {
            if (config.mapCallback != nullptr) {
                auto info = config.mapCallback(binaryChunk.chunkLength, config.userPointer);
                if (info.mappedMemory != nullptr) {
                    read(info.mappedMemory, binaryChunk.chunkLength);
                    if (config.unmapCallback != nullptr) {
                        config.unmapCallback(&info, config.userPointer);
                    }
                    glbBuffer = sources::CustomBuffer { info.customId, MimeType::None };
                }
            } else {
                sources::Vector vectorData = {};
                vectorData.bytes.resize(binaryChunk.chunkLength);
                read(vectorData.bytes.data(), binaryChunk.chunkLength);
                vectorData.mimeType = MimeType::GltfBuffer;
                glbBuffer = std::move(vectorData);
            }
        } else {
            const span<const std::byte> glbBytes(reinterpret_cast<std::byte*>(buffer->bufferPointer + offset), binaryChunk.chunkLength);
            sources::ByteView glbByteView = {};
            glbByteView.bytes = glbBytes;
            glbByteView.mimeType = MimeType::GltfBuffer;
            glbBuffer = glbByteView;
        }
    }

	return parse(root, categories);
}

void fg::Parser::setBufferAllocationCallback(BufferMapCallback* mapCallback, BufferUnmapCallback* unmapCallback) noexcept {
	if (mapCallback == nullptr)
		unmapCallback = nullptr;
	config.mapCallback = mapCallback;
	config.unmapCallback = unmapCallback;
}

void fg::Parser::setBase64DecodeCallback(Base64DecodeCallback* decodeCallback) noexcept {
    config.decodeCallback = decodeCallback;
}

void fg::Parser::setUserPointer(void* pointer) noexcept {
    config.userPointer = pointer;
}
#pragma endregion

#ifdef _MSC_VER
#pragma warning(pop)
#endif
