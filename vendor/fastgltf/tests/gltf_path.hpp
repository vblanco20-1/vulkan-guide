#pragma once
#include <filesystem>

// We need to use the __FILE__ macro so that we have access to test glTF files in this
// directory. As Clang does not yet fully support std::source_location, we cannot use that.
inline auto path = std::filesystem::path { __FILE__ }.parent_path() / "gltf";
inline auto sampleAssets = std::filesystem::path { __FILE__ }.parent_path() / "gltf" / "glTF-Sample-Assets";
inline auto physicsSampleAssets = std::filesystem::path{ __FILE__ }.parent_path() / "gltf" / "glTF_Physics";
inline auto intelSponza = std::filesystem::path { __FILE__ }.parent_path() / "gltf" / "intel_sponza";
inline auto bistroPath = std::filesystem::path { __FILE__ }.parent_path() / "gltf" / "bistro";
