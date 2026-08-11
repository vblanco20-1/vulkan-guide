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

#include <simdjson.h>

#include <fastgltf/core.hpp>

#if defined(__APPLE__) || defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace fs = std::filesystem;
namespace fg = fastgltf;

#pragma region glTF file loading
fg::GltfDataBuffer::GltfDataBuffer(const fs::path& path) noexcept {
	std::error_code ec;
	dataSize = static_cast<std::streamsize>(fs::file_size(path, ec));
	if (ec) {
		error = Error::InvalidPath;
		return;
	}

	// Open the file and determine the size.
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open() || file.bad()) {
		error = Error::InvalidPath;
		return;
	}

	allocatedSize = dataSize + simdjson::SIMDJSON_PADDING;
	buffer = decltype(buffer)(new(std::nothrow) std::byte[allocatedSize]); // To mimic std::make_unique_for_overwrite (C++20)

	if (buffer != nullptr) {
		// Copy the data and fill the padding region with zeros.
		file.read(reinterpret_cast<std::ifstream::char_type*>(buffer.get()), static_cast<std::streamsize>(dataSize));
		std::memset(buffer.get() + dataSize, 0, allocatedSize - dataSize);
	} else {
		error = Error::FileBufferAllocationFailed;
	}
}

fg::GltfDataBuffer::GltfDataBuffer(const std::byte *bytes, const std::size_t count) noexcept {
	assert(bytes != nullptr && "Passing nullptr is not allowed.");
	dataSize = count;
	allocateAndCopy(bytes);
}

#if FASTGLTF_CPP_20
fg::GltfDataBuffer::GltfDataBuffer(std::span<std::byte> span) noexcept {
	assert(span.data() != nullptr && "Passing nullptr is not allowed");
	dataSize = span.size_bytes();
	allocateAndCopy(span.data());
}
#endif

void fg::GltfDataBuffer::allocateAndCopy(const std::byte *bytes) noexcept {
	allocatedSize = dataSize + simdjson::SIMDJSON_PADDING;
	buffer = decltype(buffer)(new(std::nothrow) std::byte[allocatedSize]);

	if (buffer != nullptr) {
		std::memcpy(buffer.get(), bytes, dataSize);
		std::memset(buffer.get() + dataSize, 0, allocatedSize - dataSize);
	} else {
		error = Error::FileBufferAllocationFailed;
	}
}

void fg::GltfDataBuffer::read(void *ptr, std::size_t count) {
	std::memcpy(ptr, buffer.get() + idx, count);
	idx += count;
}

fg::span<std::byte> fg::GltfDataBuffer::read(std::size_t count, [[maybe_unused]] std::size_t padding) {
	span<std::byte> sub(buffer.get() + idx, count);
	idx += count;
	return sub;
}

void fg::GltfDataBuffer::reset() {
	idx = 0;
}

std::size_t fg::GltfDataBuffer::bytesRead() {
	return idx;
}

std::size_t fg::GltfDataBuffer::totalSize() {
	return dataSize;
}

fg::GltfFileStream::GltfFileStream(const fs::path& path) : fileStream(path, std::ios::binary) {
	fileSize = fs::file_size(path);
}

bool fg::GltfFileStream::isOpen() const {
	return fileStream.is_open();
}

void fg::GltfFileStream::read(void *ptr, std::size_t count) {
	fileStream.read(
			static_cast<char*>(ptr),
			static_cast<std::streamsize>(count));
}

fg::span<std::byte> fg::GltfFileStream::read(std::size_t count, std::size_t padding) {
	static_assert(sizeof(decltype(buf)::value_type) == sizeof(std::byte));

	buf.resize(count + padding);
	fileStream.read(
			reinterpret_cast<char*>(buf.data()),
			static_cast<std::streamsize>(count));

	return span<std::byte>(reinterpret_cast<std::byte*>(buf.data()), buf.size());
}

void fg::GltfFileStream::reset() {
	fileStream.seekg(0, std::ifstream::beg);
}

std::size_t fg::GltfFileStream::bytesRead() {
	return fileStream.tellg();
}

std::size_t fg::GltfFileStream::totalSize() {
	return fileSize;
}

#if defined(FASTGLTF_HAS_MEMORY_MAPPED_FILE)
#if defined(_WIN32)
fg::MappedGltfFile::MappedGltfFile(const fs::path& path) noexcept {
	// Create file with FILE_FLAG_SEQUENTIAL_SCAN flag, to match the Parser behaviour.
	auto* file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
							 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if (file == nullptr) {
		error = Error::InvalidPath;
		return;
	}
	fileHandle = file;

	LARGE_INTEGER result;
	if (GetFileSizeEx(fileHandle, &result) == FALSE) {
		error = Error::InvalidPath;
		return;
	}
	fileSize = static_cast<std::uint64_t>(result.QuadPart);;

	// Create the file mapping
	auto* mapping = CreateFileMapping(fileHandle, nullptr, PAGE_READONLY,
									  0, 0, nullptr);
	if (mapping == nullptr) {
		error = Error::FileBufferAllocationFailed;
		return;
	}
	fileMapping = mapping;

	// Map the view
	auto* map = MapViewOfFile(fileMapping, FILE_MAP_READ,
							 0, 0, fileSize);
	if (map == nullptr) {
		error = Error::FileBufferAllocationFailed;
		return;
	}
	mappedFile = map;
#else
fg::MappedGltfFile::MappedGltfFile(const fs::path& path) noexcept : mappedFile(MAP_FAILED) {
	// Open the file
	const auto fd = open(path.c_str(), O_RDONLY, 0);
	if (fd == -1) {
		// TODO: Cover actual error messages using std::strerror(errno)?
		error = Error::InvalidPath;
		return;
	}

	// Get the file size
	struct stat statInfo {};
	if (fstat(fd, &statInfo) != 0) {
		error = Error::InvalidPath;
		return;
	}

	// Map the file
	mappedFile = mmap(nullptr,
					  statInfo.st_size,
					  PROT_READ,
					  MAP_PRIVATE,
					  fd,
					  0);
	if (mappedFile != MAP_FAILED) {
		fileSize = static_cast<std::uint64_t>(statInfo.st_size);

		// Hint about map access
		madvise(mappedFile, fileSize, MADV_SEQUENTIAL);
	} else {
		error = Error::FileBufferAllocationFailed;
	}

	// The file descriptor is not used for the memory mapped and is not required anymore.
	close(fd);
#endif
}

fg::MappedGltfFile::MappedGltfFile(fastgltf::MappedGltfFile &&other) noexcept {
#if defined(_WIN32)
	mappedFile = std::exchange(other.mappedFile, nullptr);
	fileMapping = std::exchange(other.fileMapping, nullptr);
	fileHandle = std::exchange(other.fileHandle, nullptr);
#else
	// Make sure that munmap is never called when other gets destructed.
	mappedFile = std::exchange(other.mappedFile, MAP_FAILED);
#endif
	fileSize = other.fileSize;
	idx = other.idx;
	error = other.error;
}

fg::MappedGltfFile& fg::MappedGltfFile::operator=(fastgltf::MappedGltfFile &&other) noexcept {
#if defined(_WIN32)
	if (mappedFile != nullptr) {
		UnmapViewOfFile(mappedFile);
	}
	mappedFile = std::exchange(other.mappedFile, nullptr);

	if (fileMapping != nullptr) {
		CloseHandle(fileMapping);
	}
	fileMapping = std::exchange(other.fileMapping, nullptr);

	if (fileHandle != nullptr) {
		CloseHandle(fileHandle);
	}
	fileHandle = std::exchange(other.fileHandle, nullptr);
#else
	if (mappedFile != MAP_FAILED) {
		munmap(mappedFile, fileSize);
	}
	mappedFile = std::exchange(other.mappedFile, MAP_FAILED);
#endif
	fileSize = other.fileSize;
	idx = other.idx;
	error = other.error;
	return *this;
}

fg::MappedGltfFile::~MappedGltfFile() noexcept {
#if defined(_WIN32)
	if (mappedFile != nullptr) {
		UnmapViewOfFile(mappedFile);
	}
	if (fileMapping != nullptr) {
		CloseHandle(fileMapping);
	}
	if (fileHandle != nullptr) {
		CloseHandle(fileHandle);
	}
#else
	if (mappedFile != MAP_FAILED) {
		munmap(mappedFile, fileSize);
	}
#endif
}

void fg::MappedGltfFile::read(void *ptr, std::size_t count) {
	std::memcpy(ptr, static_cast<std::byte*>(mappedFile) + idx, count);
	idx += count;
}

fg::span<std::byte> fg::MappedGltfFile::read(std::size_t count, [[maybe_unused]] std::size_t padding) {
	span<std::byte> sub(static_cast<std::byte*>(mappedFile) + idx, count);
	idx += count;
	return sub;
}

void fg::MappedGltfFile::reset() {
	idx = 0;
}

std::size_t fg::MappedGltfFile::bytesRead() {
	return idx;
}

std::size_t fg::MappedGltfFile::totalSize() {
	return fileSize;
}
#endif // FASTGLTF_HAS_MEMORY_MAPPED_FILE

#pragma region AndroidGltfDataBuffer
#if defined(__ANDROID__)
#include <android/asset_manager.h>

namespace fastgltf {
	/**
	 * Global asset manager that can be accessed freely.
	 * The value of this global should only be set by fastgltf::setAndroidAssetManager.
	 */
	static AAssetManager* androidAssetManager = nullptr;
}

void fg::setAndroidAssetManager(AAssetManager* assetManager) noexcept {
	androidAssetManager = assetManager;
}

fg::AndroidGltfDataBuffer::AndroidGltfDataBuffer(const fs::path& path, std::uint64_t byteOffset) noexcept {
	if (androidAssetManager == nullptr) {
		error = Error::InvalidPath;
		return;
	}

	const auto filenameString = path.string();
	auto file = deletable_unique_ptr<AAsset, AAsset_close>(
		AAssetManager_open(androidAssetManager, filenameString.c_str(), AASSET_MODE_BUFFER));
	if (file == nullptr) {
		error = Error::InvalidPath;
		return;
	}

	const auto length = AAsset_getLength(file.get());
	if (length == 0) {
		error = Error::InvalidPath;
		return;
	}

	dataSize = length - byteOffset;
	allocatedSize = dataSize + simdjson::SIMDJSON_PADDING;
	buffer = decltype(buffer)(new(std::nothrow) std::byte[allocatedSize]);

	if (buffer == nullptr) {
		error = Error::FileBufferAllocationFailed;
	} else {
		if (byteOffset > 0)
			AAsset_seek64(file.get(), byteOffset, SEEK_SET);

		// Copy the data and fill the padding region with zeros.
		AAsset_read(file.get(), buffer.get(), dataSize);
		std::memset(buffer.get() + dataSize, 0, allocatedSize - dataSize);
	}
}
#endif
#pragma endregion
#pragma endregion

#pragma region Parser I/O
#if defined(__ANDROID__)
fg::Expected<fg::DataSource> fg::Parser::loadFileFromApk(const fs::path& path) const noexcept {
	auto file = deletable_unique_ptr<AAsset, AAsset_close>(
		AAssetManager_open(androidAssetManager, path.c_str(), AASSET_MODE_BUFFER));
	if (file == nullptr) {
		return Error::MissingExternalBuffer;
	}

	const auto length = AAsset_getLength(file.get());
	if (length == 0) {
		return Error::MissingExternalBuffer;
	}

	if (config.mapCallback != nullptr) {
		auto info = config.mapCallback(static_cast<std::uint64_t>(length), config.userPointer);
		if (info.mappedMemory != nullptr) {
			const sources::CustomBuffer customBufferSource = { info.customId, MimeType::None };
			AAsset_read(file.get(), info.mappedMemory, length);
			if (config.unmapCallback != nullptr) {
				config.unmapCallback(&info, config.userPointer);
			}

			return { customBufferSource };
		}
	}

	StaticVector<std::byte> data(static_cast<std::size_t>(length));
	AAsset_read(file.get(), data.data(), length);
	sources::Array arraySource {
		std::move(data),
	};
	return { std::move(arraySource) };
}
#endif

fg::Expected<fg::DataSource> fg::Parser::loadFileFromUri(URIView& uri) const noexcept {
	URI decodedUri(uri.path()); // Re-allocate so we can decode potential characters.
	// JSON strings are always in UTF-8, so we can safely always use u8path here.
	// Since u8path is deprecated with C++20 and newer, u8path is deprecated.
	// As there is no other proper solution that doesn't do something illegal,
	// we'll just disable related warnings here.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
	auto path = directory / fs::u8path(decodedUri.path());
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(__ANDROID__)
	if (androidAssetManager != nullptr) {
		// Try to load external buffers from the APK. If they're not there, fall through to the file case
		if (auto androidResult = loadFileFromApk(path); androidResult.error() == Error::None) {
			return std::move(androidResult.get());
		}
	}
#endif

	// If we were instructed to load external buffers and the files don't exist, we'll return an error.
	std::error_code error;
	if (!fs::exists(path, error) || error) {
		return Error::MissingExternalBuffer;
	}

	auto length = static_cast<std::streamsize>(fs::file_size(path, error));
	if (error) {
		return Error::InvalidURI;
	}

	std::ifstream file(path, std::ios::binary);

	if (config.mapCallback != nullptr) {
		auto info = config.mapCallback(static_cast<std::uint64_t>(length), config.userPointer);
		if (info.mappedMemory != nullptr) {
			const sources::CustomBuffer customBufferSource = { info.customId };
			file.read(static_cast<char*>(info.mappedMemory), length);
			if (config.unmapCallback != nullptr) {
				config.unmapCallback(&info, config.userPointer);
			}

			return { customBufferSource };
		}
	}

	StaticVector<std::byte> data(static_cast<std::size_t>(length));
	file.read(reinterpret_cast<char*>(data.data()), length);
	sources::Array arraySource {
		std::move(data),
	};
	return { std::move(arraySource) };
}
#pragma endregion
