#pragma once
#include <filesystem>

// We need to use the __FILE__ macro so that we have access to test glTF files in this
// directory. As Clang does not yet fully support std::source_location, we cannot use that.
inline auto path = std::filesystem::path { __FILE__ }.parent_path() / "gltf";
inline auto sampleModels = std::filesystem::path { __FILE__ }.parent_path() / "gltf" / "glTF-Sample-Models";
inline auto intelSponza = std::filesystem::path { __FILE__ }.parent_path() / "gltf" / "intel_sponza";
inline auto bistroPath = std::filesystem::path {};
