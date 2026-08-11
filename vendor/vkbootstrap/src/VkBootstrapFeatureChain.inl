/*
 * Copyright © 2025 Charles Giessen (charles@lunarg.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// This file is a part of VkBootstrap
// https://github.com/charles-lunarg/vk-bootstrap

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkb::detail {

void compare_VkPhysicalDeviceFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceFeatures const& supported, VkPhysicalDeviceFeatures const& requested) {
    if (requested.robustBufferAccess && !supported.robustBufferAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::robustBufferAccess");
    }
    if (requested.fullDrawIndexUint32 && !supported.fullDrawIndexUint32) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::fullDrawIndexUint32");
    }
    if (requested.imageCubeArray && !supported.imageCubeArray) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::imageCubeArray");
    }
    if (requested.independentBlend && !supported.independentBlend) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::independentBlend");
    }
    if (requested.geometryShader && !supported.geometryShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::geometryShader");
    }
    if (requested.tessellationShader && !supported.tessellationShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::tessellationShader");
    }
    if (requested.sampleRateShading && !supported.sampleRateShading) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sampleRateShading");
    }
    if (requested.dualSrcBlend && !supported.dualSrcBlend) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::dualSrcBlend");
    }
    if (requested.logicOp && !supported.logicOp) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::logicOp");
    }
    if (requested.multiDrawIndirect && !supported.multiDrawIndirect) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::multiDrawIndirect");
    }
    if (requested.drawIndirectFirstInstance && !supported.drawIndirectFirstInstance) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::drawIndirectFirstInstance");
    }
    if (requested.depthClamp && !supported.depthClamp) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::depthClamp");
    }
    if (requested.depthBiasClamp && !supported.depthBiasClamp) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::depthBiasClamp");
    }
    if (requested.fillModeNonSolid && !supported.fillModeNonSolid) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::fillModeNonSolid");
    }
    if (requested.depthBounds && !supported.depthBounds) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::depthBounds");
    }
    if (requested.wideLines && !supported.wideLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::wideLines");
    }
    if (requested.largePoints && !supported.largePoints) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::largePoints");
    }
    if (requested.alphaToOne && !supported.alphaToOne) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::alphaToOne");
    }
    if (requested.multiViewport && !supported.multiViewport) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::multiViewport");
    }
    if (requested.samplerAnisotropy && !supported.samplerAnisotropy) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::samplerAnisotropy");
    }
    if (requested.textureCompressionETC2 && !supported.textureCompressionETC2) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::textureCompressionETC2");
    }
    if (requested.textureCompressionASTC_LDR && !supported.textureCompressionASTC_LDR) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::textureCompressionASTC_LDR");
    }
    if (requested.textureCompressionBC && !supported.textureCompressionBC) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::textureCompressionBC");
    }
    if (requested.occlusionQueryPrecise && !supported.occlusionQueryPrecise) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::occlusionQueryPrecise");
    }
    if (requested.pipelineStatisticsQuery && !supported.pipelineStatisticsQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::pipelineStatisticsQuery");
    }
    if (requested.vertexPipelineStoresAndAtomics && !supported.vertexPipelineStoresAndAtomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::vertexPipelineStoresAndAtomics");
    }
    if (requested.fragmentStoresAndAtomics && !supported.fragmentStoresAndAtomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::fragmentStoresAndAtomics");
    }
    if (requested.shaderTessellationAndGeometryPointSize && !supported.shaderTessellationAndGeometryPointSize) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderTessellationAndGeometryPointSize");
    }
    if (requested.shaderImageGatherExtended && !supported.shaderImageGatherExtended) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderImageGatherExtended");
    }
    if (requested.shaderStorageImageExtendedFormats && !supported.shaderStorageImageExtendedFormats) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageImageExtendedFormats");
    }
    if (requested.shaderStorageImageMultisample && !supported.shaderStorageImageMultisample) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageImageMultisample");
    }
    if (requested.shaderStorageImageReadWithoutFormat && !supported.shaderStorageImageReadWithoutFormat) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageImageReadWithoutFormat");
    }
    if (requested.shaderStorageImageWriteWithoutFormat && !supported.shaderStorageImageWriteWithoutFormat) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageImageWriteWithoutFormat");
    }
    if (requested.shaderUniformBufferArrayDynamicIndexing && !supported.shaderUniformBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderUniformBufferArrayDynamicIndexing");
    }
    if (requested.shaderSampledImageArrayDynamicIndexing && !supported.shaderSampledImageArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderSampledImageArrayDynamicIndexing");
    }
    if (requested.shaderStorageBufferArrayDynamicIndexing && !supported.shaderStorageBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageBufferArrayDynamicIndexing");
    }
    if (requested.shaderStorageImageArrayDynamicIndexing && !supported.shaderStorageImageArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderStorageImageArrayDynamicIndexing");
    }
    if (requested.shaderClipDistance && !supported.shaderClipDistance) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderClipDistance");
    }
    if (requested.shaderCullDistance && !supported.shaderCullDistance) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderCullDistance");
    }
    if (requested.shaderFloat64 && !supported.shaderFloat64) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderFloat64");
    }
    if (requested.shaderInt64 && !supported.shaderInt64) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderInt64");
    }
    if (requested.shaderInt16 && !supported.shaderInt16) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderInt16");
    }
    if (requested.shaderResourceResidency && !supported.shaderResourceResidency) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderResourceResidency");
    }
    if (requested.shaderResourceMinLod && !supported.shaderResourceMinLod) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::shaderResourceMinLod");
    }
    if (requested.sparseBinding && !supported.sparseBinding) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseBinding");
    }
    if (requested.sparseResidencyBuffer && !supported.sparseResidencyBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidencyBuffer");
    }
    if (requested.sparseResidencyImage2D && !supported.sparseResidencyImage2D) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidencyImage2D");
    }
    if (requested.sparseResidencyImage3D && !supported.sparseResidencyImage3D) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidencyImage3D");
    }
    if (requested.sparseResidency2Samples && !supported.sparseResidency2Samples) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidency2Samples");
    }
    if (requested.sparseResidency4Samples && !supported.sparseResidency4Samples) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidency4Samples");
    }
    if (requested.sparseResidency8Samples && !supported.sparseResidency8Samples) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidency8Samples");
    }
    if (requested.sparseResidency16Samples && !supported.sparseResidency16Samples) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidency16Samples");
    }
    if (requested.sparseResidencyAliased && !supported.sparseResidencyAliased) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::sparseResidencyAliased");
    }
    if (requested.variableMultisampleRate && !supported.variableMultisampleRate) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::variableMultisampleRate");
    }
    if (requested.inheritedQueries && !supported.inheritedQueries) {
        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::inheritedQueries");
    }
}
void merge_VkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures & current, VkPhysicalDeviceFeatures const& merge_in) {
    current.robustBufferAccess = current.robustBufferAccess || merge_in.robustBufferAccess;
    current.fullDrawIndexUint32 = current.fullDrawIndexUint32 || merge_in.fullDrawIndexUint32;
    current.imageCubeArray = current.imageCubeArray || merge_in.imageCubeArray;
    current.independentBlend = current.independentBlend || merge_in.independentBlend;
    current.geometryShader = current.geometryShader || merge_in.geometryShader;
    current.tessellationShader = current.tessellationShader || merge_in.tessellationShader;
    current.sampleRateShading = current.sampleRateShading || merge_in.sampleRateShading;
    current.dualSrcBlend = current.dualSrcBlend || merge_in.dualSrcBlend;
    current.logicOp = current.logicOp || merge_in.logicOp;
    current.multiDrawIndirect = current.multiDrawIndirect || merge_in.multiDrawIndirect;
    current.drawIndirectFirstInstance = current.drawIndirectFirstInstance || merge_in.drawIndirectFirstInstance;
    current.depthClamp = current.depthClamp || merge_in.depthClamp;
    current.depthBiasClamp = current.depthBiasClamp || merge_in.depthBiasClamp;
    current.fillModeNonSolid = current.fillModeNonSolid || merge_in.fillModeNonSolid;
    current.depthBounds = current.depthBounds || merge_in.depthBounds;
    current.wideLines = current.wideLines || merge_in.wideLines;
    current.largePoints = current.largePoints || merge_in.largePoints;
    current.alphaToOne = current.alphaToOne || merge_in.alphaToOne;
    current.multiViewport = current.multiViewport || merge_in.multiViewport;
    current.samplerAnisotropy = current.samplerAnisotropy || merge_in.samplerAnisotropy;
    current.textureCompressionETC2 = current.textureCompressionETC2 || merge_in.textureCompressionETC2;
    current.textureCompressionASTC_LDR = current.textureCompressionASTC_LDR || merge_in.textureCompressionASTC_LDR;
    current.textureCompressionBC = current.textureCompressionBC || merge_in.textureCompressionBC;
    current.occlusionQueryPrecise = current.occlusionQueryPrecise || merge_in.occlusionQueryPrecise;
    current.pipelineStatisticsQuery = current.pipelineStatisticsQuery || merge_in.pipelineStatisticsQuery;
    current.vertexPipelineStoresAndAtomics = current.vertexPipelineStoresAndAtomics || merge_in.vertexPipelineStoresAndAtomics;
    current.fragmentStoresAndAtomics = current.fragmentStoresAndAtomics || merge_in.fragmentStoresAndAtomics;
    current.shaderTessellationAndGeometryPointSize = current.shaderTessellationAndGeometryPointSize || merge_in.shaderTessellationAndGeometryPointSize;
    current.shaderImageGatherExtended = current.shaderImageGatherExtended || merge_in.shaderImageGatherExtended;
    current.shaderStorageImageExtendedFormats = current.shaderStorageImageExtendedFormats || merge_in.shaderStorageImageExtendedFormats;
    current.shaderStorageImageMultisample = current.shaderStorageImageMultisample || merge_in.shaderStorageImageMultisample;
    current.shaderStorageImageReadWithoutFormat = current.shaderStorageImageReadWithoutFormat || merge_in.shaderStorageImageReadWithoutFormat;
    current.shaderStorageImageWriteWithoutFormat = current.shaderStorageImageWriteWithoutFormat || merge_in.shaderStorageImageWriteWithoutFormat;
    current.shaderUniformBufferArrayDynamicIndexing = current.shaderUniformBufferArrayDynamicIndexing || merge_in.shaderUniformBufferArrayDynamicIndexing;
    current.shaderSampledImageArrayDynamicIndexing = current.shaderSampledImageArrayDynamicIndexing || merge_in.shaderSampledImageArrayDynamicIndexing;
    current.shaderStorageBufferArrayDynamicIndexing = current.shaderStorageBufferArrayDynamicIndexing || merge_in.shaderStorageBufferArrayDynamicIndexing;
    current.shaderStorageImageArrayDynamicIndexing = current.shaderStorageImageArrayDynamicIndexing || merge_in.shaderStorageImageArrayDynamicIndexing;
    current.shaderClipDistance = current.shaderClipDistance || merge_in.shaderClipDistance;
    current.shaderCullDistance = current.shaderCullDistance || merge_in.shaderCullDistance;
    current.shaderFloat64 = current.shaderFloat64 || merge_in.shaderFloat64;
    current.shaderInt64 = current.shaderInt64 || merge_in.shaderInt64;
    current.shaderInt16 = current.shaderInt16 || merge_in.shaderInt16;
    current.shaderResourceResidency = current.shaderResourceResidency || merge_in.shaderResourceResidency;
    current.shaderResourceMinLod = current.shaderResourceMinLod || merge_in.shaderResourceMinLod;
    current.sparseBinding = current.sparseBinding || merge_in.sparseBinding;
    current.sparseResidencyBuffer = current.sparseResidencyBuffer || merge_in.sparseResidencyBuffer;
    current.sparseResidencyImage2D = current.sparseResidencyImage2D || merge_in.sparseResidencyImage2D;
    current.sparseResidencyImage3D = current.sparseResidencyImage3D || merge_in.sparseResidencyImage3D;
    current.sparseResidency2Samples = current.sparseResidency2Samples || merge_in.sparseResidency2Samples;
    current.sparseResidency4Samples = current.sparseResidency4Samples || merge_in.sparseResidency4Samples;
    current.sparseResidency8Samples = current.sparseResidency8Samples || merge_in.sparseResidency8Samples;
    current.sparseResidency16Samples = current.sparseResidency16Samples || merge_in.sparseResidency16Samples;
    current.sparseResidencyAliased = current.sparseResidencyAliased || merge_in.sparseResidencyAliased;
    current.variableMultisampleRate = current.variableMultisampleRate || merge_in.variableMultisampleRate;
    current.inheritedQueries = current.inheritedQueries || merge_in.inheritedQueries;
}
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceProtectedMemoryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceProtectedMemoryFeatures const& supported, VkPhysicalDeviceProtectedMemoryFeatures const& requested) {
    if (requested.protectedMemory && !supported.protectedMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceProtectedMemoryFeatures::protectedMemory");
    }
}
void merge_VkPhysicalDeviceProtectedMemoryFeatures(VkPhysicalDeviceProtectedMemoryFeatures & current, VkPhysicalDeviceProtectedMemoryFeatures const& merge_in) {
    current.protectedMemory = current.protectedMemory || merge_in.protectedMemory;
}
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
void compare_VkPhysicalDevice16BitStorageFeatures(std::vector<std::string> & error_list, VkPhysicalDevice16BitStorageFeatures const& supported, VkPhysicalDevice16BitStorageFeatures const& requested) {
    if (requested.storageBuffer16BitAccess && !supported.storageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeatures::storageBuffer16BitAccess");
    }
    if (requested.uniformAndStorageBuffer16BitAccess && !supported.uniformAndStorageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeatures::uniformAndStorageBuffer16BitAccess");
    }
    if (requested.storagePushConstant16 && !supported.storagePushConstant16) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeatures::storagePushConstant16");
    }
    if (requested.storageInputOutput16 && !supported.storageInputOutput16) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeatures::storageInputOutput16");
    }
}
void merge_VkPhysicalDevice16BitStorageFeatures(VkPhysicalDevice16BitStorageFeatures & current, VkPhysicalDevice16BitStorageFeatures const& merge_in) {
    current.storageBuffer16BitAccess = current.storageBuffer16BitAccess || merge_in.storageBuffer16BitAccess;
    current.uniformAndStorageBuffer16BitAccess = current.uniformAndStorageBuffer16BitAccess || merge_in.uniformAndStorageBuffer16BitAccess;
    current.storagePushConstant16 = current.storagePushConstant16 || merge_in.storagePushConstant16;
    current.storageInputOutput16 = current.storageInputOutput16 || merge_in.storageInputOutput16;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
void compare_VkPhysicalDevice16BitStorageFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevice16BitStorageFeaturesKHR const& supported, VkPhysicalDevice16BitStorageFeaturesKHR const& requested) {
    if (requested.storageBuffer16BitAccess && !supported.storageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeaturesKHR::storageBuffer16BitAccess");
    }
    if (requested.uniformAndStorageBuffer16BitAccess && !supported.uniformAndStorageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeaturesKHR::uniformAndStorageBuffer16BitAccess");
    }
    if (requested.storagePushConstant16 && !supported.storagePushConstant16) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeaturesKHR::storagePushConstant16");
    }
    if (requested.storageInputOutput16 && !supported.storageInputOutput16) {
        error_list.push_back("Missing feature VkPhysicalDevice16BitStorageFeaturesKHR::storageInputOutput16");
    }
}
void merge_VkPhysicalDevice16BitStorageFeaturesKHR(VkPhysicalDevice16BitStorageFeaturesKHR & current, VkPhysicalDevice16BitStorageFeaturesKHR const& merge_in) {
    current.storageBuffer16BitAccess = current.storageBuffer16BitAccess || merge_in.storageBuffer16BitAccess;
    current.uniformAndStorageBuffer16BitAccess = current.uniformAndStorageBuffer16BitAccess || merge_in.uniformAndStorageBuffer16BitAccess;
    current.storagePushConstant16 = current.storagePushConstant16 || merge_in.storagePushConstant16;
    current.storageInputOutput16 = current.storageInputOutput16 || merge_in.storageInputOutput16;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointersFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointersFeatures const& supported, VkPhysicalDeviceVariablePointersFeatures const& requested) {
    if (requested.variablePointersStorageBuffer && !supported.variablePointersStorageBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointersFeatures::variablePointersStorageBuffer");
    }
    if (requested.variablePointers && !supported.variablePointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointersFeatures::variablePointers");
    }
}
void merge_VkPhysicalDeviceVariablePointersFeatures(VkPhysicalDeviceVariablePointersFeatures & current, VkPhysicalDeviceVariablePointersFeatures const& merge_in) {
    current.variablePointersStorageBuffer = current.variablePointersStorageBuffer || merge_in.variablePointersStorageBuffer;
    current.variablePointers = current.variablePointers || merge_in.variablePointers;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointerFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointerFeatures const& supported, VkPhysicalDeviceVariablePointerFeatures const& requested) {
    if (requested.variablePointersStorageBuffer && !supported.variablePointersStorageBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointerFeatures::variablePointersStorageBuffer");
    }
    if (requested.variablePointers && !supported.variablePointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointerFeatures::variablePointers");
    }
}
void merge_VkPhysicalDeviceVariablePointerFeatures(VkPhysicalDeviceVariablePointerFeatures & current, VkPhysicalDeviceVariablePointerFeatures const& merge_in) {
    current.variablePointersStorageBuffer = current.variablePointersStorageBuffer || merge_in.variablePointersStorageBuffer;
    current.variablePointers = current.variablePointers || merge_in.variablePointers;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointerFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointerFeaturesKHR const& supported, VkPhysicalDeviceVariablePointerFeaturesKHR const& requested) {
    if (requested.variablePointersStorageBuffer && !supported.variablePointersStorageBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointerFeaturesKHR::variablePointersStorageBuffer");
    }
    if (requested.variablePointers && !supported.variablePointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointerFeaturesKHR::variablePointers");
    }
}
void merge_VkPhysicalDeviceVariablePointerFeaturesKHR(VkPhysicalDeviceVariablePointerFeaturesKHR & current, VkPhysicalDeviceVariablePointerFeaturesKHR const& merge_in) {
    current.variablePointersStorageBuffer = current.variablePointersStorageBuffer || merge_in.variablePointersStorageBuffer;
    current.variablePointers = current.variablePointers || merge_in.variablePointers;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointersFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointersFeaturesKHR const& supported, VkPhysicalDeviceVariablePointersFeaturesKHR const& requested) {
    if (requested.variablePointersStorageBuffer && !supported.variablePointersStorageBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointersFeaturesKHR::variablePointersStorageBuffer");
    }
    if (requested.variablePointers && !supported.variablePointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceVariablePointersFeaturesKHR::variablePointers");
    }
}
void merge_VkPhysicalDeviceVariablePointersFeaturesKHR(VkPhysicalDeviceVariablePointersFeaturesKHR & current, VkPhysicalDeviceVariablePointersFeaturesKHR const& merge_in) {
    current.variablePointersStorageBuffer = current.variablePointersStorageBuffer || merge_in.variablePointersStorageBuffer;
    current.variablePointers = current.variablePointers || merge_in.variablePointers;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
void compare_VkPhysicalDeviceSamplerYcbcrConversionFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& supported, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& requested) {
    if (requested.samplerYcbcrConversion && !supported.samplerYcbcrConversion) {
        error_list.push_back("Missing feature VkPhysicalDeviceSamplerYcbcrConversionFeatures::samplerYcbcrConversion");
    }
}
void merge_VkPhysicalDeviceSamplerYcbcrConversionFeatures(VkPhysicalDeviceSamplerYcbcrConversionFeatures & current, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& merge_in) {
    current.samplerYcbcrConversion = current.samplerYcbcrConversion || merge_in.samplerYcbcrConversion;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
void compare_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& supported, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& requested) {
    if (requested.samplerYcbcrConversion && !supported.samplerYcbcrConversion) {
        error_list.push_back("Missing feature VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR::samplerYcbcrConversion");
    }
}
void merge_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR & current, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& merge_in) {
    current.samplerYcbcrConversion = current.samplerYcbcrConversion || merge_in.samplerYcbcrConversion;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
void compare_VkPhysicalDeviceMultiviewFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewFeatures const& supported, VkPhysicalDeviceMultiviewFeatures const& requested) {
    if (requested.multiview && !supported.multiview) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeatures::multiview");
    }
    if (requested.multiviewGeometryShader && !supported.multiviewGeometryShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeatures::multiviewGeometryShader");
    }
    if (requested.multiviewTessellationShader && !supported.multiviewTessellationShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeatures::multiviewTessellationShader");
    }
}
void merge_VkPhysicalDeviceMultiviewFeatures(VkPhysicalDeviceMultiviewFeatures & current, VkPhysicalDeviceMultiviewFeatures const& merge_in) {
    current.multiview = current.multiview || merge_in.multiview;
    current.multiviewGeometryShader = current.multiviewGeometryShader || merge_in.multiviewGeometryShader;
    current.multiviewTessellationShader = current.multiviewTessellationShader || merge_in.multiviewTessellationShader;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
void compare_VkPhysicalDeviceMultiviewFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewFeaturesKHR const& supported, VkPhysicalDeviceMultiviewFeaturesKHR const& requested) {
    if (requested.multiview && !supported.multiview) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeaturesKHR::multiview");
    }
    if (requested.multiviewGeometryShader && !supported.multiviewGeometryShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeaturesKHR::multiviewGeometryShader");
    }
    if (requested.multiviewTessellationShader && !supported.multiviewTessellationShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewFeaturesKHR::multiviewTessellationShader");
    }
}
void merge_VkPhysicalDeviceMultiviewFeaturesKHR(VkPhysicalDeviceMultiviewFeaturesKHR & current, VkPhysicalDeviceMultiviewFeaturesKHR const& merge_in) {
    current.multiview = current.multiview || merge_in.multiview;
    current.multiviewGeometryShader = current.multiviewGeometryShader || merge_in.multiviewGeometryShader;
    current.multiviewTessellationShader = current.multiviewTessellationShader || merge_in.multiviewTessellationShader;
}
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceShaderDrawParametersFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDrawParametersFeatures const& supported, VkPhysicalDeviceShaderDrawParametersFeatures const& requested) {
    if (requested.shaderDrawParameters && !supported.shaderDrawParameters) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderDrawParametersFeatures::shaderDrawParameters");
    }
}
void merge_VkPhysicalDeviceShaderDrawParametersFeatures(VkPhysicalDeviceShaderDrawParametersFeatures & current, VkPhysicalDeviceShaderDrawParametersFeatures const& merge_in) {
    current.shaderDrawParameters = current.shaderDrawParameters || merge_in.shaderDrawParameters;
}
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceShaderDrawParameterFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDrawParameterFeatures const& supported, VkPhysicalDeviceShaderDrawParameterFeatures const& requested) {
    if (requested.shaderDrawParameters && !supported.shaderDrawParameters) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderDrawParameterFeatures::shaderDrawParameters");
    }
}
void merge_VkPhysicalDeviceShaderDrawParameterFeatures(VkPhysicalDeviceShaderDrawParameterFeatures & current, VkPhysicalDeviceShaderDrawParameterFeatures const& merge_in) {
    current.shaderDrawParameters = current.shaderDrawParameters || merge_in.shaderDrawParameters;
}
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_2))
void compare_VkPhysicalDeviceVulkan11Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan11Features const& supported, VkPhysicalDeviceVulkan11Features const& requested) {
    if (requested.storageBuffer16BitAccess && !supported.storageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::storageBuffer16BitAccess");
    }
    if (requested.uniformAndStorageBuffer16BitAccess && !supported.uniformAndStorageBuffer16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::uniformAndStorageBuffer16BitAccess");
    }
    if (requested.storagePushConstant16 && !supported.storagePushConstant16) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::storagePushConstant16");
    }
    if (requested.storageInputOutput16 && !supported.storageInputOutput16) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::storageInputOutput16");
    }
    if (requested.multiview && !supported.multiview) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::multiview");
    }
    if (requested.multiviewGeometryShader && !supported.multiviewGeometryShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::multiviewGeometryShader");
    }
    if (requested.multiviewTessellationShader && !supported.multiviewTessellationShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::multiviewTessellationShader");
    }
    if (requested.variablePointersStorageBuffer && !supported.variablePointersStorageBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::variablePointersStorageBuffer");
    }
    if (requested.variablePointers && !supported.variablePointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::variablePointers");
    }
    if (requested.protectedMemory && !supported.protectedMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::protectedMemory");
    }
    if (requested.samplerYcbcrConversion && !supported.samplerYcbcrConversion) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::samplerYcbcrConversion");
    }
    if (requested.shaderDrawParameters && !supported.shaderDrawParameters) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan11Features::shaderDrawParameters");
    }
}
void merge_VkPhysicalDeviceVulkan11Features(VkPhysicalDeviceVulkan11Features & current, VkPhysicalDeviceVulkan11Features const& merge_in) {
    current.storageBuffer16BitAccess = current.storageBuffer16BitAccess || merge_in.storageBuffer16BitAccess;
    current.uniformAndStorageBuffer16BitAccess = current.uniformAndStorageBuffer16BitAccess || merge_in.uniformAndStorageBuffer16BitAccess;
    current.storagePushConstant16 = current.storagePushConstant16 || merge_in.storagePushConstant16;
    current.storageInputOutput16 = current.storageInputOutput16 || merge_in.storageInputOutput16;
    current.multiview = current.multiview || merge_in.multiview;
    current.multiviewGeometryShader = current.multiviewGeometryShader || merge_in.multiviewGeometryShader;
    current.multiviewTessellationShader = current.multiviewTessellationShader || merge_in.multiviewTessellationShader;
    current.variablePointersStorageBuffer = current.variablePointersStorageBuffer || merge_in.variablePointersStorageBuffer;
    current.variablePointers = current.variablePointers || merge_in.variablePointers;
    current.protectedMemory = current.protectedMemory || merge_in.protectedMemory;
    current.samplerYcbcrConversion = current.samplerYcbcrConversion || merge_in.samplerYcbcrConversion;
    current.shaderDrawParameters = current.shaderDrawParameters || merge_in.shaderDrawParameters;
}
#endif //(defined(VK_VERSION_1_2))
#if (defined(VK_VERSION_1_2))
void compare_VkPhysicalDeviceVulkan12Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan12Features const& supported, VkPhysicalDeviceVulkan12Features const& requested) {
    if (requested.samplerMirrorClampToEdge && !supported.samplerMirrorClampToEdge) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::samplerMirrorClampToEdge");
    }
    if (requested.drawIndirectCount && !supported.drawIndirectCount) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::drawIndirectCount");
    }
    if (requested.storageBuffer8BitAccess && !supported.storageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::storageBuffer8BitAccess");
    }
    if (requested.uniformAndStorageBuffer8BitAccess && !supported.uniformAndStorageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::uniformAndStorageBuffer8BitAccess");
    }
    if (requested.storagePushConstant8 && !supported.storagePushConstant8) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::storagePushConstant8");
    }
    if (requested.shaderBufferInt64Atomics && !supported.shaderBufferInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderBufferInt64Atomics");
    }
    if (requested.shaderSharedInt64Atomics && !supported.shaderSharedInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderSharedInt64Atomics");
    }
    if (requested.shaderFloat16 && !supported.shaderFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderFloat16");
    }
    if (requested.shaderInt8 && !supported.shaderInt8) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderInt8");
    }
    if (requested.descriptorIndexing && !supported.descriptorIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorIndexing");
    }
    if (requested.shaderInputAttachmentArrayDynamicIndexing && !supported.shaderInputAttachmentArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderInputAttachmentArrayDynamicIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayDynamicIndexing && !supported.shaderUniformTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderUniformTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayDynamicIndexing && !supported.shaderStorageTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderStorageTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderUniformBufferArrayNonUniformIndexing && !supported.shaderUniformBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderUniformBufferArrayNonUniformIndexing");
    }
    if (requested.shaderSampledImageArrayNonUniformIndexing && !supported.shaderSampledImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderSampledImageArrayNonUniformIndexing");
    }
    if (requested.shaderStorageBufferArrayNonUniformIndexing && !supported.shaderStorageBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderStorageBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageImageArrayNonUniformIndexing && !supported.shaderStorageImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderStorageImageArrayNonUniformIndexing");
    }
    if (requested.shaderInputAttachmentArrayNonUniformIndexing && !supported.shaderInputAttachmentArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderInputAttachmentArrayNonUniformIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayNonUniformIndexing && !supported.shaderUniformTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderUniformTexelBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayNonUniformIndexing && !supported.shaderStorageTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderStorageTexelBufferArrayNonUniformIndexing");
    }
    if (requested.descriptorBindingUniformBufferUpdateAfterBind && !supported.descriptorBindingUniformBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingUniformBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingSampledImageUpdateAfterBind && !supported.descriptorBindingSampledImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingSampledImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageImageUpdateAfterBind && !supported.descriptorBindingStorageImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingStorageImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageBufferUpdateAfterBind && !supported.descriptorBindingStorageBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingStorageBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUniformTexelBufferUpdateAfterBind && !supported.descriptorBindingUniformTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingUniformTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageTexelBufferUpdateAfterBind && !supported.descriptorBindingStorageTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingStorageTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUpdateUnusedWhilePending && !supported.descriptorBindingUpdateUnusedWhilePending) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingUpdateUnusedWhilePending");
    }
    if (requested.descriptorBindingPartiallyBound && !supported.descriptorBindingPartiallyBound) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingPartiallyBound");
    }
    if (requested.descriptorBindingVariableDescriptorCount && !supported.descriptorBindingVariableDescriptorCount) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::descriptorBindingVariableDescriptorCount");
    }
    if (requested.runtimeDescriptorArray && !supported.runtimeDescriptorArray) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::runtimeDescriptorArray");
    }
    if (requested.samplerFilterMinmax && !supported.samplerFilterMinmax) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::samplerFilterMinmax");
    }
    if (requested.scalarBlockLayout && !supported.scalarBlockLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::scalarBlockLayout");
    }
    if (requested.imagelessFramebuffer && !supported.imagelessFramebuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::imagelessFramebuffer");
    }
    if (requested.uniformBufferStandardLayout && !supported.uniformBufferStandardLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::uniformBufferStandardLayout");
    }
    if (requested.shaderSubgroupExtendedTypes && !supported.shaderSubgroupExtendedTypes) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderSubgroupExtendedTypes");
    }
    if (requested.separateDepthStencilLayouts && !supported.separateDepthStencilLayouts) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::separateDepthStencilLayouts");
    }
    if (requested.hostQueryReset && !supported.hostQueryReset) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::hostQueryReset");
    }
    if (requested.timelineSemaphore && !supported.timelineSemaphore) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::timelineSemaphore");
    }
    if (requested.bufferDeviceAddress && !supported.bufferDeviceAddress) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::bufferDeviceAddress");
    }
    if (requested.bufferDeviceAddressCaptureReplay && !supported.bufferDeviceAddressCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::bufferDeviceAddressCaptureReplay");
    }
    if (requested.bufferDeviceAddressMultiDevice && !supported.bufferDeviceAddressMultiDevice) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::bufferDeviceAddressMultiDevice");
    }
    if (requested.vulkanMemoryModel && !supported.vulkanMemoryModel) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::vulkanMemoryModel");
    }
    if (requested.vulkanMemoryModelDeviceScope && !supported.vulkanMemoryModelDeviceScope) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::vulkanMemoryModelDeviceScope");
    }
    if (requested.vulkanMemoryModelAvailabilityVisibilityChains && !supported.vulkanMemoryModelAvailabilityVisibilityChains) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::vulkanMemoryModelAvailabilityVisibilityChains");
    }
    if (requested.shaderOutputViewportIndex && !supported.shaderOutputViewportIndex) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderOutputViewportIndex");
    }
    if (requested.shaderOutputLayer && !supported.shaderOutputLayer) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::shaderOutputLayer");
    }
    if (requested.subgroupBroadcastDynamicId && !supported.subgroupBroadcastDynamicId) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan12Features::subgroupBroadcastDynamicId");
    }
}
void merge_VkPhysicalDeviceVulkan12Features(VkPhysicalDeviceVulkan12Features & current, VkPhysicalDeviceVulkan12Features const& merge_in) {
    current.samplerMirrorClampToEdge = current.samplerMirrorClampToEdge || merge_in.samplerMirrorClampToEdge;
    current.drawIndirectCount = current.drawIndirectCount || merge_in.drawIndirectCount;
    current.storageBuffer8BitAccess = current.storageBuffer8BitAccess || merge_in.storageBuffer8BitAccess;
    current.uniformAndStorageBuffer8BitAccess = current.uniformAndStorageBuffer8BitAccess || merge_in.uniformAndStorageBuffer8BitAccess;
    current.storagePushConstant8 = current.storagePushConstant8 || merge_in.storagePushConstant8;
    current.shaderBufferInt64Atomics = current.shaderBufferInt64Atomics || merge_in.shaderBufferInt64Atomics;
    current.shaderSharedInt64Atomics = current.shaderSharedInt64Atomics || merge_in.shaderSharedInt64Atomics;
    current.shaderFloat16 = current.shaderFloat16 || merge_in.shaderFloat16;
    current.shaderInt8 = current.shaderInt8 || merge_in.shaderInt8;
    current.descriptorIndexing = current.descriptorIndexing || merge_in.descriptorIndexing;
    current.shaderInputAttachmentArrayDynamicIndexing = current.shaderInputAttachmentArrayDynamicIndexing || merge_in.shaderInputAttachmentArrayDynamicIndexing;
    current.shaderUniformTexelBufferArrayDynamicIndexing = current.shaderUniformTexelBufferArrayDynamicIndexing || merge_in.shaderUniformTexelBufferArrayDynamicIndexing;
    current.shaderStorageTexelBufferArrayDynamicIndexing = current.shaderStorageTexelBufferArrayDynamicIndexing || merge_in.shaderStorageTexelBufferArrayDynamicIndexing;
    current.shaderUniformBufferArrayNonUniformIndexing = current.shaderUniformBufferArrayNonUniformIndexing || merge_in.shaderUniformBufferArrayNonUniformIndexing;
    current.shaderSampledImageArrayNonUniformIndexing = current.shaderSampledImageArrayNonUniformIndexing || merge_in.shaderSampledImageArrayNonUniformIndexing;
    current.shaderStorageBufferArrayNonUniformIndexing = current.shaderStorageBufferArrayNonUniformIndexing || merge_in.shaderStorageBufferArrayNonUniformIndexing;
    current.shaderStorageImageArrayNonUniformIndexing = current.shaderStorageImageArrayNonUniformIndexing || merge_in.shaderStorageImageArrayNonUniformIndexing;
    current.shaderInputAttachmentArrayNonUniformIndexing = current.shaderInputAttachmentArrayNonUniformIndexing || merge_in.shaderInputAttachmentArrayNonUniformIndexing;
    current.shaderUniformTexelBufferArrayNonUniformIndexing = current.shaderUniformTexelBufferArrayNonUniformIndexing || merge_in.shaderUniformTexelBufferArrayNonUniformIndexing;
    current.shaderStorageTexelBufferArrayNonUniformIndexing = current.shaderStorageTexelBufferArrayNonUniformIndexing || merge_in.shaderStorageTexelBufferArrayNonUniformIndexing;
    current.descriptorBindingUniformBufferUpdateAfterBind = current.descriptorBindingUniformBufferUpdateAfterBind || merge_in.descriptorBindingUniformBufferUpdateAfterBind;
    current.descriptorBindingSampledImageUpdateAfterBind = current.descriptorBindingSampledImageUpdateAfterBind || merge_in.descriptorBindingSampledImageUpdateAfterBind;
    current.descriptorBindingStorageImageUpdateAfterBind = current.descriptorBindingStorageImageUpdateAfterBind || merge_in.descriptorBindingStorageImageUpdateAfterBind;
    current.descriptorBindingStorageBufferUpdateAfterBind = current.descriptorBindingStorageBufferUpdateAfterBind || merge_in.descriptorBindingStorageBufferUpdateAfterBind;
    current.descriptorBindingUniformTexelBufferUpdateAfterBind = current.descriptorBindingUniformTexelBufferUpdateAfterBind || merge_in.descriptorBindingUniformTexelBufferUpdateAfterBind;
    current.descriptorBindingStorageTexelBufferUpdateAfterBind = current.descriptorBindingStorageTexelBufferUpdateAfterBind || merge_in.descriptorBindingStorageTexelBufferUpdateAfterBind;
    current.descriptorBindingUpdateUnusedWhilePending = current.descriptorBindingUpdateUnusedWhilePending || merge_in.descriptorBindingUpdateUnusedWhilePending;
    current.descriptorBindingPartiallyBound = current.descriptorBindingPartiallyBound || merge_in.descriptorBindingPartiallyBound;
    current.descriptorBindingVariableDescriptorCount = current.descriptorBindingVariableDescriptorCount || merge_in.descriptorBindingVariableDescriptorCount;
    current.runtimeDescriptorArray = current.runtimeDescriptorArray || merge_in.runtimeDescriptorArray;
    current.samplerFilterMinmax = current.samplerFilterMinmax || merge_in.samplerFilterMinmax;
    current.scalarBlockLayout = current.scalarBlockLayout || merge_in.scalarBlockLayout;
    current.imagelessFramebuffer = current.imagelessFramebuffer || merge_in.imagelessFramebuffer;
    current.uniformBufferStandardLayout = current.uniformBufferStandardLayout || merge_in.uniformBufferStandardLayout;
    current.shaderSubgroupExtendedTypes = current.shaderSubgroupExtendedTypes || merge_in.shaderSubgroupExtendedTypes;
    current.separateDepthStencilLayouts = current.separateDepthStencilLayouts || merge_in.separateDepthStencilLayouts;
    current.hostQueryReset = current.hostQueryReset || merge_in.hostQueryReset;
    current.timelineSemaphore = current.timelineSemaphore || merge_in.timelineSemaphore;
    current.bufferDeviceAddress = current.bufferDeviceAddress || merge_in.bufferDeviceAddress;
    current.bufferDeviceAddressCaptureReplay = current.bufferDeviceAddressCaptureReplay || merge_in.bufferDeviceAddressCaptureReplay;
    current.bufferDeviceAddressMultiDevice = current.bufferDeviceAddressMultiDevice || merge_in.bufferDeviceAddressMultiDevice;
    current.vulkanMemoryModel = current.vulkanMemoryModel || merge_in.vulkanMemoryModel;
    current.vulkanMemoryModelDeviceScope = current.vulkanMemoryModelDeviceScope || merge_in.vulkanMemoryModelDeviceScope;
    current.vulkanMemoryModelAvailabilityVisibilityChains = current.vulkanMemoryModelAvailabilityVisibilityChains || merge_in.vulkanMemoryModelAvailabilityVisibilityChains;
    current.shaderOutputViewportIndex = current.shaderOutputViewportIndex || merge_in.shaderOutputViewportIndex;
    current.shaderOutputLayer = current.shaderOutputLayer || merge_in.shaderOutputLayer;
    current.subgroupBroadcastDynamicId = current.subgroupBroadcastDynamicId || merge_in.subgroupBroadcastDynamicId;
}
#endif //(defined(VK_VERSION_1_2))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
void compare_VkPhysicalDeviceVulkanMemoryModelFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVulkanMemoryModelFeatures const& supported, VkPhysicalDeviceVulkanMemoryModelFeatures const& requested) {
    if (requested.vulkanMemoryModel && !supported.vulkanMemoryModel) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModel");
    }
    if (requested.vulkanMemoryModelDeviceScope && !supported.vulkanMemoryModelDeviceScope) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModelDeviceScope");
    }
    if (requested.vulkanMemoryModelAvailabilityVisibilityChains && !supported.vulkanMemoryModelAvailabilityVisibilityChains) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModelAvailabilityVisibilityChains");
    }
}
void merge_VkPhysicalDeviceVulkanMemoryModelFeatures(VkPhysicalDeviceVulkanMemoryModelFeatures & current, VkPhysicalDeviceVulkanMemoryModelFeatures const& merge_in) {
    current.vulkanMemoryModel = current.vulkanMemoryModel || merge_in.vulkanMemoryModel;
    current.vulkanMemoryModelDeviceScope = current.vulkanMemoryModelDeviceScope || merge_in.vulkanMemoryModelDeviceScope;
    current.vulkanMemoryModelAvailabilityVisibilityChains = current.vulkanMemoryModelAvailabilityVisibilityChains || merge_in.vulkanMemoryModelAvailabilityVisibilityChains;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
void compare_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& supported, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& requested) {
    if (requested.vulkanMemoryModel && !supported.vulkanMemoryModel) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeaturesKHR::vulkanMemoryModel");
    }
    if (requested.vulkanMemoryModelDeviceScope && !supported.vulkanMemoryModelDeviceScope) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeaturesKHR::vulkanMemoryModelDeviceScope");
    }
    if (requested.vulkanMemoryModelAvailabilityVisibilityChains && !supported.vulkanMemoryModelAvailabilityVisibilityChains) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkanMemoryModelFeaturesKHR::vulkanMemoryModelAvailabilityVisibilityChains");
    }
}
void merge_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR & current, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& merge_in) {
    current.vulkanMemoryModel = current.vulkanMemoryModel || merge_in.vulkanMemoryModel;
    current.vulkanMemoryModelDeviceScope = current.vulkanMemoryModelDeviceScope || merge_in.vulkanMemoryModelDeviceScope;
    current.vulkanMemoryModelAvailabilityVisibilityChains = current.vulkanMemoryModelAvailabilityVisibilityChains || merge_in.vulkanMemoryModelAvailabilityVisibilityChains;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
void compare_VkPhysicalDeviceHostQueryResetFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceHostQueryResetFeatures const& supported, VkPhysicalDeviceHostQueryResetFeatures const& requested) {
    if (requested.hostQueryReset && !supported.hostQueryReset) {
        error_list.push_back("Missing feature VkPhysicalDeviceHostQueryResetFeatures::hostQueryReset");
    }
}
void merge_VkPhysicalDeviceHostQueryResetFeatures(VkPhysicalDeviceHostQueryResetFeatures & current, VkPhysicalDeviceHostQueryResetFeatures const& merge_in) {
    current.hostQueryReset = current.hostQueryReset || merge_in.hostQueryReset;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
void compare_VkPhysicalDeviceHostQueryResetFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceHostQueryResetFeaturesEXT const& supported, VkPhysicalDeviceHostQueryResetFeaturesEXT const& requested) {
    if (requested.hostQueryReset && !supported.hostQueryReset) {
        error_list.push_back("Missing feature VkPhysicalDeviceHostQueryResetFeaturesEXT::hostQueryReset");
    }
}
void merge_VkPhysicalDeviceHostQueryResetFeaturesEXT(VkPhysicalDeviceHostQueryResetFeaturesEXT & current, VkPhysicalDeviceHostQueryResetFeaturesEXT const& merge_in) {
    current.hostQueryReset = current.hostQueryReset || merge_in.hostQueryReset;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
void compare_VkPhysicalDeviceTimelineSemaphoreFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceTimelineSemaphoreFeatures const& supported, VkPhysicalDeviceTimelineSemaphoreFeatures const& requested) {
    if (requested.timelineSemaphore && !supported.timelineSemaphore) {
        error_list.push_back("Missing feature VkPhysicalDeviceTimelineSemaphoreFeatures::timelineSemaphore");
    }
}
void merge_VkPhysicalDeviceTimelineSemaphoreFeatures(VkPhysicalDeviceTimelineSemaphoreFeatures & current, VkPhysicalDeviceTimelineSemaphoreFeatures const& merge_in) {
    current.timelineSemaphore = current.timelineSemaphore || merge_in.timelineSemaphore;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
void compare_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& supported, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& requested) {
    if (requested.timelineSemaphore && !supported.timelineSemaphore) {
        error_list.push_back("Missing feature VkPhysicalDeviceTimelineSemaphoreFeaturesKHR::timelineSemaphore");
    }
}
void merge_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR & current, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& merge_in) {
    current.timelineSemaphore = current.timelineSemaphore || merge_in.timelineSemaphore;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeatures const& supported, VkPhysicalDeviceBufferDeviceAddressFeatures const& requested) {
    if (requested.bufferDeviceAddress && !supported.bufferDeviceAddress) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeatures::bufferDeviceAddress");
    }
    if (requested.bufferDeviceAddressCaptureReplay && !supported.bufferDeviceAddressCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeatures::bufferDeviceAddressCaptureReplay");
    }
    if (requested.bufferDeviceAddressMultiDevice && !supported.bufferDeviceAddressMultiDevice) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeatures::bufferDeviceAddressMultiDevice");
    }
}
void merge_VkPhysicalDeviceBufferDeviceAddressFeatures(VkPhysicalDeviceBufferDeviceAddressFeatures & current, VkPhysicalDeviceBufferDeviceAddressFeatures const& merge_in) {
    current.bufferDeviceAddress = current.bufferDeviceAddress || merge_in.bufferDeviceAddress;
    current.bufferDeviceAddressCaptureReplay = current.bufferDeviceAddressCaptureReplay || merge_in.bufferDeviceAddressCaptureReplay;
    current.bufferDeviceAddressMultiDevice = current.bufferDeviceAddressMultiDevice || merge_in.bufferDeviceAddressMultiDevice;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& supported, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& requested) {
    if (requested.bufferDeviceAddress && !supported.bufferDeviceAddress) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesKHR::bufferDeviceAddress");
    }
    if (requested.bufferDeviceAddressCaptureReplay && !supported.bufferDeviceAddressCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesKHR::bufferDeviceAddressCaptureReplay");
    }
    if (requested.bufferDeviceAddressMultiDevice && !supported.bufferDeviceAddressMultiDevice) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesKHR::bufferDeviceAddressMultiDevice");
    }
}
void merge_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(VkPhysicalDeviceBufferDeviceAddressFeaturesKHR & current, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& merge_in) {
    current.bufferDeviceAddress = current.bufferDeviceAddress || merge_in.bufferDeviceAddress;
    current.bufferDeviceAddressCaptureReplay = current.bufferDeviceAddressCaptureReplay || merge_in.bufferDeviceAddressCaptureReplay;
    current.bufferDeviceAddressMultiDevice = current.bufferDeviceAddressMultiDevice || merge_in.bufferDeviceAddressMultiDevice;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
void compare_VkPhysicalDevice8BitStorageFeatures(std::vector<std::string> & error_list, VkPhysicalDevice8BitStorageFeatures const& supported, VkPhysicalDevice8BitStorageFeatures const& requested) {
    if (requested.storageBuffer8BitAccess && !supported.storageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeatures::storageBuffer8BitAccess");
    }
    if (requested.uniformAndStorageBuffer8BitAccess && !supported.uniformAndStorageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeatures::uniformAndStorageBuffer8BitAccess");
    }
    if (requested.storagePushConstant8 && !supported.storagePushConstant8) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeatures::storagePushConstant8");
    }
}
void merge_VkPhysicalDevice8BitStorageFeatures(VkPhysicalDevice8BitStorageFeatures & current, VkPhysicalDevice8BitStorageFeatures const& merge_in) {
    current.storageBuffer8BitAccess = current.storageBuffer8BitAccess || merge_in.storageBuffer8BitAccess;
    current.uniformAndStorageBuffer8BitAccess = current.uniformAndStorageBuffer8BitAccess || merge_in.uniformAndStorageBuffer8BitAccess;
    current.storagePushConstant8 = current.storagePushConstant8 || merge_in.storagePushConstant8;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
void compare_VkPhysicalDevice8BitStorageFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevice8BitStorageFeaturesKHR const& supported, VkPhysicalDevice8BitStorageFeaturesKHR const& requested) {
    if (requested.storageBuffer8BitAccess && !supported.storageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeaturesKHR::storageBuffer8BitAccess");
    }
    if (requested.uniformAndStorageBuffer8BitAccess && !supported.uniformAndStorageBuffer8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeaturesKHR::uniformAndStorageBuffer8BitAccess");
    }
    if (requested.storagePushConstant8 && !supported.storagePushConstant8) {
        error_list.push_back("Missing feature VkPhysicalDevice8BitStorageFeaturesKHR::storagePushConstant8");
    }
}
void merge_VkPhysicalDevice8BitStorageFeaturesKHR(VkPhysicalDevice8BitStorageFeaturesKHR & current, VkPhysicalDevice8BitStorageFeaturesKHR const& merge_in) {
    current.storageBuffer8BitAccess = current.storageBuffer8BitAccess || merge_in.storageBuffer8BitAccess;
    current.uniformAndStorageBuffer8BitAccess = current.uniformAndStorageBuffer8BitAccess || merge_in.uniformAndStorageBuffer8BitAccess;
    current.storagePushConstant8 = current.storagePushConstant8 || merge_in.storagePushConstant8;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
void compare_VkPhysicalDeviceShaderAtomicInt64Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicInt64Features const& supported, VkPhysicalDeviceShaderAtomicInt64Features const& requested) {
    if (requested.shaderBufferInt64Atomics && !supported.shaderBufferInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicInt64Features::shaderBufferInt64Atomics");
    }
    if (requested.shaderSharedInt64Atomics && !supported.shaderSharedInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicInt64Features::shaderSharedInt64Atomics");
    }
}
void merge_VkPhysicalDeviceShaderAtomicInt64Features(VkPhysicalDeviceShaderAtomicInt64Features & current, VkPhysicalDeviceShaderAtomicInt64Features const& merge_in) {
    current.shaderBufferInt64Atomics = current.shaderBufferInt64Atomics || merge_in.shaderBufferInt64Atomics;
    current.shaderSharedInt64Atomics = current.shaderSharedInt64Atomics || merge_in.shaderSharedInt64Atomics;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
void compare_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& supported, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& requested) {
    if (requested.shaderBufferInt64Atomics && !supported.shaderBufferInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicInt64FeaturesKHR::shaderBufferInt64Atomics");
    }
    if (requested.shaderSharedInt64Atomics && !supported.shaderSharedInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicInt64FeaturesKHR::shaderSharedInt64Atomics");
    }
}
void merge_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR & current, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& merge_in) {
    current.shaderBufferInt64Atomics = current.shaderBufferInt64Atomics || merge_in.shaderBufferInt64Atomics;
    current.shaderSharedInt64Atomics = current.shaderSharedInt64Atomics || merge_in.shaderSharedInt64Atomics;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceShaderFloat16Int8Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat16Int8Features const& supported, VkPhysicalDeviceShaderFloat16Int8Features const& requested) {
    if (requested.shaderFloat16 && !supported.shaderFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat16Int8Features::shaderFloat16");
    }
    if (requested.shaderInt8 && !supported.shaderInt8) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat16Int8Features::shaderInt8");
    }
}
void merge_VkPhysicalDeviceShaderFloat16Int8Features(VkPhysicalDeviceShaderFloat16Int8Features & current, VkPhysicalDeviceShaderFloat16Int8Features const& merge_in) {
    current.shaderFloat16 = current.shaderFloat16 || merge_in.shaderFloat16;
    current.shaderInt8 = current.shaderInt8 || merge_in.shaderInt8;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& supported, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& requested) {
    if (requested.shaderFloat16 && !supported.shaderFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderFloat16");
    }
    if (requested.shaderInt8 && !supported.shaderInt8) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderInt8");
    }
}
void merge_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR & current, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& merge_in) {
    current.shaderFloat16 = current.shaderFloat16 || merge_in.shaderFloat16;
    current.shaderInt8 = current.shaderInt8 || merge_in.shaderInt8;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceFloat16Int8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFloat16Int8FeaturesKHR const& supported, VkPhysicalDeviceFloat16Int8FeaturesKHR const& requested) {
    if (requested.shaderFloat16 && !supported.shaderFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceFloat16Int8FeaturesKHR::shaderFloat16");
    }
    if (requested.shaderInt8 && !supported.shaderInt8) {
        error_list.push_back("Missing feature VkPhysicalDeviceFloat16Int8FeaturesKHR::shaderInt8");
    }
}
void merge_VkPhysicalDeviceFloat16Int8FeaturesKHR(VkPhysicalDeviceFloat16Int8FeaturesKHR & current, VkPhysicalDeviceFloat16Int8FeaturesKHR const& merge_in) {
    current.shaderFloat16 = current.shaderFloat16 || merge_in.shaderFloat16;
    current.shaderInt8 = current.shaderInt8 || merge_in.shaderInt8;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
void compare_VkPhysicalDeviceDescriptorIndexingFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorIndexingFeatures const& supported, VkPhysicalDeviceDescriptorIndexingFeatures const& requested) {
    if (requested.shaderInputAttachmentArrayDynamicIndexing && !supported.shaderInputAttachmentArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderInputAttachmentArrayDynamicIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayDynamicIndexing && !supported.shaderUniformTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayDynamicIndexing && !supported.shaderStorageTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderUniformBufferArrayNonUniformIndexing && !supported.shaderUniformBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformBufferArrayNonUniformIndexing");
    }
    if (requested.shaderSampledImageArrayNonUniformIndexing && !supported.shaderSampledImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderSampledImageArrayNonUniformIndexing");
    }
    if (requested.shaderStorageBufferArrayNonUniformIndexing && !supported.shaderStorageBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageImageArrayNonUniformIndexing && !supported.shaderStorageImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageImageArrayNonUniformIndexing");
    }
    if (requested.shaderInputAttachmentArrayNonUniformIndexing && !supported.shaderInputAttachmentArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderInputAttachmentArrayNonUniformIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayNonUniformIndexing && !supported.shaderUniformTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformTexelBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayNonUniformIndexing && !supported.shaderStorageTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageTexelBufferArrayNonUniformIndexing");
    }
    if (requested.descriptorBindingUniformBufferUpdateAfterBind && !supported.descriptorBindingUniformBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingUniformBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingSampledImageUpdateAfterBind && !supported.descriptorBindingSampledImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingSampledImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageImageUpdateAfterBind && !supported.descriptorBindingStorageImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingStorageImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageBufferUpdateAfterBind && !supported.descriptorBindingStorageBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingStorageBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUniformTexelBufferUpdateAfterBind && !supported.descriptorBindingUniformTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingUniformTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageTexelBufferUpdateAfterBind && !supported.descriptorBindingStorageTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingStorageTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUpdateUnusedWhilePending && !supported.descriptorBindingUpdateUnusedWhilePending) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingUpdateUnusedWhilePending");
    }
    if (requested.descriptorBindingPartiallyBound && !supported.descriptorBindingPartiallyBound) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingPartiallyBound");
    }
    if (requested.descriptorBindingVariableDescriptorCount && !supported.descriptorBindingVariableDescriptorCount) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingVariableDescriptorCount");
    }
    if (requested.runtimeDescriptorArray && !supported.runtimeDescriptorArray) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeatures::runtimeDescriptorArray");
    }
}
void merge_VkPhysicalDeviceDescriptorIndexingFeatures(VkPhysicalDeviceDescriptorIndexingFeatures & current, VkPhysicalDeviceDescriptorIndexingFeatures const& merge_in) {
    current.shaderInputAttachmentArrayDynamicIndexing = current.shaderInputAttachmentArrayDynamicIndexing || merge_in.shaderInputAttachmentArrayDynamicIndexing;
    current.shaderUniformTexelBufferArrayDynamicIndexing = current.shaderUniformTexelBufferArrayDynamicIndexing || merge_in.shaderUniformTexelBufferArrayDynamicIndexing;
    current.shaderStorageTexelBufferArrayDynamicIndexing = current.shaderStorageTexelBufferArrayDynamicIndexing || merge_in.shaderStorageTexelBufferArrayDynamicIndexing;
    current.shaderUniformBufferArrayNonUniformIndexing = current.shaderUniformBufferArrayNonUniformIndexing || merge_in.shaderUniformBufferArrayNonUniformIndexing;
    current.shaderSampledImageArrayNonUniformIndexing = current.shaderSampledImageArrayNonUniformIndexing || merge_in.shaderSampledImageArrayNonUniformIndexing;
    current.shaderStorageBufferArrayNonUniformIndexing = current.shaderStorageBufferArrayNonUniformIndexing || merge_in.shaderStorageBufferArrayNonUniformIndexing;
    current.shaderStorageImageArrayNonUniformIndexing = current.shaderStorageImageArrayNonUniformIndexing || merge_in.shaderStorageImageArrayNonUniformIndexing;
    current.shaderInputAttachmentArrayNonUniformIndexing = current.shaderInputAttachmentArrayNonUniformIndexing || merge_in.shaderInputAttachmentArrayNonUniformIndexing;
    current.shaderUniformTexelBufferArrayNonUniformIndexing = current.shaderUniformTexelBufferArrayNonUniformIndexing || merge_in.shaderUniformTexelBufferArrayNonUniformIndexing;
    current.shaderStorageTexelBufferArrayNonUniformIndexing = current.shaderStorageTexelBufferArrayNonUniformIndexing || merge_in.shaderStorageTexelBufferArrayNonUniformIndexing;
    current.descriptorBindingUniformBufferUpdateAfterBind = current.descriptorBindingUniformBufferUpdateAfterBind || merge_in.descriptorBindingUniformBufferUpdateAfterBind;
    current.descriptorBindingSampledImageUpdateAfterBind = current.descriptorBindingSampledImageUpdateAfterBind || merge_in.descriptorBindingSampledImageUpdateAfterBind;
    current.descriptorBindingStorageImageUpdateAfterBind = current.descriptorBindingStorageImageUpdateAfterBind || merge_in.descriptorBindingStorageImageUpdateAfterBind;
    current.descriptorBindingStorageBufferUpdateAfterBind = current.descriptorBindingStorageBufferUpdateAfterBind || merge_in.descriptorBindingStorageBufferUpdateAfterBind;
    current.descriptorBindingUniformTexelBufferUpdateAfterBind = current.descriptorBindingUniformTexelBufferUpdateAfterBind || merge_in.descriptorBindingUniformTexelBufferUpdateAfterBind;
    current.descriptorBindingStorageTexelBufferUpdateAfterBind = current.descriptorBindingStorageTexelBufferUpdateAfterBind || merge_in.descriptorBindingStorageTexelBufferUpdateAfterBind;
    current.descriptorBindingUpdateUnusedWhilePending = current.descriptorBindingUpdateUnusedWhilePending || merge_in.descriptorBindingUpdateUnusedWhilePending;
    current.descriptorBindingPartiallyBound = current.descriptorBindingPartiallyBound || merge_in.descriptorBindingPartiallyBound;
    current.descriptorBindingVariableDescriptorCount = current.descriptorBindingVariableDescriptorCount || merge_in.descriptorBindingVariableDescriptorCount;
    current.runtimeDescriptorArray = current.runtimeDescriptorArray || merge_in.runtimeDescriptorArray;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
void compare_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& supported, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& requested) {
    if (requested.shaderInputAttachmentArrayDynamicIndexing && !supported.shaderInputAttachmentArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderInputAttachmentArrayDynamicIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayDynamicIndexing && !supported.shaderUniformTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderUniformTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayDynamicIndexing && !supported.shaderStorageTexelBufferArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderStorageTexelBufferArrayDynamicIndexing");
    }
    if (requested.shaderUniformBufferArrayNonUniformIndexing && !supported.shaderUniformBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderUniformBufferArrayNonUniformIndexing");
    }
    if (requested.shaderSampledImageArrayNonUniformIndexing && !supported.shaderSampledImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderSampledImageArrayNonUniformIndexing");
    }
    if (requested.shaderStorageBufferArrayNonUniformIndexing && !supported.shaderStorageBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderStorageBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageImageArrayNonUniformIndexing && !supported.shaderStorageImageArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderStorageImageArrayNonUniformIndexing");
    }
    if (requested.shaderInputAttachmentArrayNonUniformIndexing && !supported.shaderInputAttachmentArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderInputAttachmentArrayNonUniformIndexing");
    }
    if (requested.shaderUniformTexelBufferArrayNonUniformIndexing && !supported.shaderUniformTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderUniformTexelBufferArrayNonUniformIndexing");
    }
    if (requested.shaderStorageTexelBufferArrayNonUniformIndexing && !supported.shaderStorageTexelBufferArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderStorageTexelBufferArrayNonUniformIndexing");
    }
    if (requested.descriptorBindingUniformBufferUpdateAfterBind && !supported.descriptorBindingUniformBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingUniformBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingSampledImageUpdateAfterBind && !supported.descriptorBindingSampledImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingSampledImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageImageUpdateAfterBind && !supported.descriptorBindingStorageImageUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingStorageImageUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageBufferUpdateAfterBind && !supported.descriptorBindingStorageBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingStorageBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUniformTexelBufferUpdateAfterBind && !supported.descriptorBindingUniformTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingUniformTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingStorageTexelBufferUpdateAfterBind && !supported.descriptorBindingStorageTexelBufferUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingStorageTexelBufferUpdateAfterBind");
    }
    if (requested.descriptorBindingUpdateUnusedWhilePending && !supported.descriptorBindingUpdateUnusedWhilePending) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingUpdateUnusedWhilePending");
    }
    if (requested.descriptorBindingPartiallyBound && !supported.descriptorBindingPartiallyBound) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingPartiallyBound");
    }
    if (requested.descriptorBindingVariableDescriptorCount && !supported.descriptorBindingVariableDescriptorCount) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::descriptorBindingVariableDescriptorCount");
    }
    if (requested.runtimeDescriptorArray && !supported.runtimeDescriptorArray) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorIndexingFeaturesEXT::runtimeDescriptorArray");
    }
}
void merge_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(VkPhysicalDeviceDescriptorIndexingFeaturesEXT & current, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& merge_in) {
    current.shaderInputAttachmentArrayDynamicIndexing = current.shaderInputAttachmentArrayDynamicIndexing || merge_in.shaderInputAttachmentArrayDynamicIndexing;
    current.shaderUniformTexelBufferArrayDynamicIndexing = current.shaderUniformTexelBufferArrayDynamicIndexing || merge_in.shaderUniformTexelBufferArrayDynamicIndexing;
    current.shaderStorageTexelBufferArrayDynamicIndexing = current.shaderStorageTexelBufferArrayDynamicIndexing || merge_in.shaderStorageTexelBufferArrayDynamicIndexing;
    current.shaderUniformBufferArrayNonUniformIndexing = current.shaderUniformBufferArrayNonUniformIndexing || merge_in.shaderUniformBufferArrayNonUniformIndexing;
    current.shaderSampledImageArrayNonUniformIndexing = current.shaderSampledImageArrayNonUniformIndexing || merge_in.shaderSampledImageArrayNonUniformIndexing;
    current.shaderStorageBufferArrayNonUniformIndexing = current.shaderStorageBufferArrayNonUniformIndexing || merge_in.shaderStorageBufferArrayNonUniformIndexing;
    current.shaderStorageImageArrayNonUniformIndexing = current.shaderStorageImageArrayNonUniformIndexing || merge_in.shaderStorageImageArrayNonUniformIndexing;
    current.shaderInputAttachmentArrayNonUniformIndexing = current.shaderInputAttachmentArrayNonUniformIndexing || merge_in.shaderInputAttachmentArrayNonUniformIndexing;
    current.shaderUniformTexelBufferArrayNonUniformIndexing = current.shaderUniformTexelBufferArrayNonUniformIndexing || merge_in.shaderUniformTexelBufferArrayNonUniformIndexing;
    current.shaderStorageTexelBufferArrayNonUniformIndexing = current.shaderStorageTexelBufferArrayNonUniformIndexing || merge_in.shaderStorageTexelBufferArrayNonUniformIndexing;
    current.descriptorBindingUniformBufferUpdateAfterBind = current.descriptorBindingUniformBufferUpdateAfterBind || merge_in.descriptorBindingUniformBufferUpdateAfterBind;
    current.descriptorBindingSampledImageUpdateAfterBind = current.descriptorBindingSampledImageUpdateAfterBind || merge_in.descriptorBindingSampledImageUpdateAfterBind;
    current.descriptorBindingStorageImageUpdateAfterBind = current.descriptorBindingStorageImageUpdateAfterBind || merge_in.descriptorBindingStorageImageUpdateAfterBind;
    current.descriptorBindingStorageBufferUpdateAfterBind = current.descriptorBindingStorageBufferUpdateAfterBind || merge_in.descriptorBindingStorageBufferUpdateAfterBind;
    current.descriptorBindingUniformTexelBufferUpdateAfterBind = current.descriptorBindingUniformTexelBufferUpdateAfterBind || merge_in.descriptorBindingUniformTexelBufferUpdateAfterBind;
    current.descriptorBindingStorageTexelBufferUpdateAfterBind = current.descriptorBindingStorageTexelBufferUpdateAfterBind || merge_in.descriptorBindingStorageTexelBufferUpdateAfterBind;
    current.descriptorBindingUpdateUnusedWhilePending = current.descriptorBindingUpdateUnusedWhilePending || merge_in.descriptorBindingUpdateUnusedWhilePending;
    current.descriptorBindingPartiallyBound = current.descriptorBindingPartiallyBound || merge_in.descriptorBindingPartiallyBound;
    current.descriptorBindingVariableDescriptorCount = current.descriptorBindingVariableDescriptorCount || merge_in.descriptorBindingVariableDescriptorCount;
    current.runtimeDescriptorArray = current.runtimeDescriptorArray || merge_in.runtimeDescriptorArray;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
void compare_VkPhysicalDeviceScalarBlockLayoutFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceScalarBlockLayoutFeatures const& supported, VkPhysicalDeviceScalarBlockLayoutFeatures const& requested) {
    if (requested.scalarBlockLayout && !supported.scalarBlockLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceScalarBlockLayoutFeatures::scalarBlockLayout");
    }
}
void merge_VkPhysicalDeviceScalarBlockLayoutFeatures(VkPhysicalDeviceScalarBlockLayoutFeatures & current, VkPhysicalDeviceScalarBlockLayoutFeatures const& merge_in) {
    current.scalarBlockLayout = current.scalarBlockLayout || merge_in.scalarBlockLayout;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
void compare_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& supported, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& requested) {
    if (requested.scalarBlockLayout && !supported.scalarBlockLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceScalarBlockLayoutFeaturesEXT::scalarBlockLayout");
    }
}
void merge_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT & current, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& merge_in) {
    current.scalarBlockLayout = current.scalarBlockLayout || merge_in.scalarBlockLayout;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
void compare_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& supported, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& requested) {
    if (requested.uniformBufferStandardLayout && !supported.uniformBufferStandardLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceUniformBufferStandardLayoutFeatures::uniformBufferStandardLayout");
    }
}
void merge_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(VkPhysicalDeviceUniformBufferStandardLayoutFeatures & current, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& merge_in) {
    current.uniformBufferStandardLayout = current.uniformBufferStandardLayout || merge_in.uniformBufferStandardLayout;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
void compare_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& supported, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& requested) {
    if (requested.uniformBufferStandardLayout && !supported.uniformBufferStandardLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR::uniformBufferStandardLayout");
    }
}
void merge_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR & current, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& merge_in) {
    current.uniformBufferStandardLayout = current.uniformBufferStandardLayout || merge_in.uniformBufferStandardLayout;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
void compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& supported, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& requested) {
    if (requested.shaderSubgroupExtendedTypes && !supported.shaderSubgroupExtendedTypes) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures::shaderSubgroupExtendedTypes");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures & current, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& merge_in) {
    current.shaderSubgroupExtendedTypes = current.shaderSubgroupExtendedTypes || merge_in.shaderSubgroupExtendedTypes;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
void compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& requested) {
    if (requested.shaderSubgroupExtendedTypes && !supported.shaderSubgroupExtendedTypes) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR::shaderSubgroupExtendedTypes");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& merge_in) {
    current.shaderSubgroupExtendedTypes = current.shaderSubgroupExtendedTypes || merge_in.shaderSubgroupExtendedTypes;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
void compare_VkPhysicalDeviceImagelessFramebufferFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceImagelessFramebufferFeatures const& supported, VkPhysicalDeviceImagelessFramebufferFeatures const& requested) {
    if (requested.imagelessFramebuffer && !supported.imagelessFramebuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceImagelessFramebufferFeatures::imagelessFramebuffer");
    }
}
void merge_VkPhysicalDeviceImagelessFramebufferFeatures(VkPhysicalDeviceImagelessFramebufferFeatures & current, VkPhysicalDeviceImagelessFramebufferFeatures const& merge_in) {
    current.imagelessFramebuffer = current.imagelessFramebuffer || merge_in.imagelessFramebuffer;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
void compare_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& supported, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& requested) {
    if (requested.imagelessFramebuffer && !supported.imagelessFramebuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceImagelessFramebufferFeaturesKHR::imagelessFramebuffer");
    }
}
void merge_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(VkPhysicalDeviceImagelessFramebufferFeaturesKHR & current, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& merge_in) {
    current.imagelessFramebuffer = current.imagelessFramebuffer || merge_in.imagelessFramebuffer;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
void compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& supported, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& requested) {
    if (requested.separateDepthStencilLayouts && !supported.separateDepthStencilLayouts) {
        error_list.push_back("Missing feature VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures::separateDepthStencilLayouts");
    }
}
void merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures & current, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& merge_in) {
    current.separateDepthStencilLayouts = current.separateDepthStencilLayouts || merge_in.separateDepthStencilLayouts;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
void compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& supported, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& requested) {
    if (requested.separateDepthStencilLayouts && !supported.separateDepthStencilLayouts) {
        error_list.push_back("Missing feature VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR::separateDepthStencilLayouts");
    }
}
void merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR & current, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& merge_in) {
    current.separateDepthStencilLayouts = current.separateDepthStencilLayouts || merge_in.separateDepthStencilLayouts;
}
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
#if (defined(VK_VERSION_1_3))
void compare_VkPhysicalDeviceVulkan13Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan13Features const& supported, VkPhysicalDeviceVulkan13Features const& requested) {
    if (requested.robustImageAccess && !supported.robustImageAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::robustImageAccess");
    }
    if (requested.inlineUniformBlock && !supported.inlineUniformBlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::inlineUniformBlock");
    }
    if (requested.descriptorBindingInlineUniformBlockUpdateAfterBind && !supported.descriptorBindingInlineUniformBlockUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::descriptorBindingInlineUniformBlockUpdateAfterBind");
    }
    if (requested.pipelineCreationCacheControl && !supported.pipelineCreationCacheControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::pipelineCreationCacheControl");
    }
    if (requested.privateData && !supported.privateData) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::privateData");
    }
    if (requested.shaderDemoteToHelperInvocation && !supported.shaderDemoteToHelperInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::shaderDemoteToHelperInvocation");
    }
    if (requested.shaderTerminateInvocation && !supported.shaderTerminateInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::shaderTerminateInvocation");
    }
    if (requested.subgroupSizeControl && !supported.subgroupSizeControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::subgroupSizeControl");
    }
    if (requested.computeFullSubgroups && !supported.computeFullSubgroups) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::computeFullSubgroups");
    }
    if (requested.synchronization2 && !supported.synchronization2) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::synchronization2");
    }
    if (requested.textureCompressionASTC_HDR && !supported.textureCompressionASTC_HDR) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::textureCompressionASTC_HDR");
    }
    if (requested.shaderZeroInitializeWorkgroupMemory && !supported.shaderZeroInitializeWorkgroupMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::shaderZeroInitializeWorkgroupMemory");
    }
    if (requested.dynamicRendering && !supported.dynamicRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::dynamicRendering");
    }
    if (requested.shaderIntegerDotProduct && !supported.shaderIntegerDotProduct) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::shaderIntegerDotProduct");
    }
    if (requested.maintenance4 && !supported.maintenance4) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan13Features::maintenance4");
    }
}
void merge_VkPhysicalDeviceVulkan13Features(VkPhysicalDeviceVulkan13Features & current, VkPhysicalDeviceVulkan13Features const& merge_in) {
    current.robustImageAccess = current.robustImageAccess || merge_in.robustImageAccess;
    current.inlineUniformBlock = current.inlineUniformBlock || merge_in.inlineUniformBlock;
    current.descriptorBindingInlineUniformBlockUpdateAfterBind = current.descriptorBindingInlineUniformBlockUpdateAfterBind || merge_in.descriptorBindingInlineUniformBlockUpdateAfterBind;
    current.pipelineCreationCacheControl = current.pipelineCreationCacheControl || merge_in.pipelineCreationCacheControl;
    current.privateData = current.privateData || merge_in.privateData;
    current.shaderDemoteToHelperInvocation = current.shaderDemoteToHelperInvocation || merge_in.shaderDemoteToHelperInvocation;
    current.shaderTerminateInvocation = current.shaderTerminateInvocation || merge_in.shaderTerminateInvocation;
    current.subgroupSizeControl = current.subgroupSizeControl || merge_in.subgroupSizeControl;
    current.computeFullSubgroups = current.computeFullSubgroups || merge_in.computeFullSubgroups;
    current.synchronization2 = current.synchronization2 || merge_in.synchronization2;
    current.textureCompressionASTC_HDR = current.textureCompressionASTC_HDR || merge_in.textureCompressionASTC_HDR;
    current.shaderZeroInitializeWorkgroupMemory = current.shaderZeroInitializeWorkgroupMemory || merge_in.shaderZeroInitializeWorkgroupMemory;
    current.dynamicRendering = current.dynamicRendering || merge_in.dynamicRendering;
    current.shaderIntegerDotProduct = current.shaderIntegerDotProduct || merge_in.shaderIntegerDotProduct;
    current.maintenance4 = current.maintenance4 || merge_in.maintenance4;
}
#endif //(defined(VK_VERSION_1_3))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
void compare_VkPhysicalDevicePrivateDataFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePrivateDataFeatures const& supported, VkPhysicalDevicePrivateDataFeatures const& requested) {
    if (requested.privateData && !supported.privateData) {
        error_list.push_back("Missing feature VkPhysicalDevicePrivateDataFeatures::privateData");
    }
}
void merge_VkPhysicalDevicePrivateDataFeatures(VkPhysicalDevicePrivateDataFeatures & current, VkPhysicalDevicePrivateDataFeatures const& merge_in) {
    current.privateData = current.privateData || merge_in.privateData;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
void compare_VkPhysicalDevicePrivateDataFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrivateDataFeaturesEXT const& supported, VkPhysicalDevicePrivateDataFeaturesEXT const& requested) {
    if (requested.privateData && !supported.privateData) {
        error_list.push_back("Missing feature VkPhysicalDevicePrivateDataFeaturesEXT::privateData");
    }
}
void merge_VkPhysicalDevicePrivateDataFeaturesEXT(VkPhysicalDevicePrivateDataFeaturesEXT & current, VkPhysicalDevicePrivateDataFeaturesEXT const& merge_in) {
    current.privateData = current.privateData || merge_in.privateData;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
void compare_VkPhysicalDeviceSynchronization2Features(std::vector<std::string> & error_list, VkPhysicalDeviceSynchronization2Features const& supported, VkPhysicalDeviceSynchronization2Features const& requested) {
    if (requested.synchronization2 && !supported.synchronization2) {
        error_list.push_back("Missing feature VkPhysicalDeviceSynchronization2Features::synchronization2");
    }
}
void merge_VkPhysicalDeviceSynchronization2Features(VkPhysicalDeviceSynchronization2Features & current, VkPhysicalDeviceSynchronization2Features const& merge_in) {
    current.synchronization2 = current.synchronization2 || merge_in.synchronization2;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
void compare_VkPhysicalDeviceSynchronization2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSynchronization2FeaturesKHR const& supported, VkPhysicalDeviceSynchronization2FeaturesKHR const& requested) {
    if (requested.synchronization2 && !supported.synchronization2) {
        error_list.push_back("Missing feature VkPhysicalDeviceSynchronization2FeaturesKHR::synchronization2");
    }
}
void merge_VkPhysicalDeviceSynchronization2FeaturesKHR(VkPhysicalDeviceSynchronization2FeaturesKHR & current, VkPhysicalDeviceSynchronization2FeaturesKHR const& merge_in) {
    current.synchronization2 = current.synchronization2 || merge_in.synchronization2;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
void compare_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& supported, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& requested) {
    if (requested.textureCompressionASTC_HDR && !supported.textureCompressionASTC_HDR) {
        error_list.push_back("Missing feature VkPhysicalDeviceTextureCompressionASTCHDRFeatures::textureCompressionASTC_HDR");
    }
}
void merge_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(VkPhysicalDeviceTextureCompressionASTCHDRFeatures & current, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& merge_in) {
    current.textureCompressionASTC_HDR = current.textureCompressionASTC_HDR || merge_in.textureCompressionASTC_HDR;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
void compare_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& supported, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& requested) {
    if (requested.textureCompressionASTC_HDR && !supported.textureCompressionASTC_HDR) {
        error_list.push_back("Missing feature VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT::textureCompressionASTC_HDR");
    }
}
void merge_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT & current, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& merge_in) {
    current.textureCompressionASTC_HDR = current.textureCompressionASTC_HDR || merge_in.textureCompressionASTC_HDR;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
void compare_VkPhysicalDeviceMaintenance4Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance4Features const& supported, VkPhysicalDeviceMaintenance4Features const& requested) {
    if (requested.maintenance4 && !supported.maintenance4) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance4Features::maintenance4");
    }
}
void merge_VkPhysicalDeviceMaintenance4Features(VkPhysicalDeviceMaintenance4Features & current, VkPhysicalDeviceMaintenance4Features const& merge_in) {
    current.maintenance4 = current.maintenance4 || merge_in.maintenance4;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
void compare_VkPhysicalDeviceMaintenance4FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance4FeaturesKHR const& supported, VkPhysicalDeviceMaintenance4FeaturesKHR const& requested) {
    if (requested.maintenance4 && !supported.maintenance4) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance4FeaturesKHR::maintenance4");
    }
}
void merge_VkPhysicalDeviceMaintenance4FeaturesKHR(VkPhysicalDeviceMaintenance4FeaturesKHR & current, VkPhysicalDeviceMaintenance4FeaturesKHR const& merge_in) {
    current.maintenance4 = current.maintenance4 || merge_in.maintenance4;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
void compare_VkPhysicalDeviceShaderTerminateInvocationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTerminateInvocationFeatures const& supported, VkPhysicalDeviceShaderTerminateInvocationFeatures const& requested) {
    if (requested.shaderTerminateInvocation && !supported.shaderTerminateInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderTerminateInvocationFeatures::shaderTerminateInvocation");
    }
}
void merge_VkPhysicalDeviceShaderTerminateInvocationFeatures(VkPhysicalDeviceShaderTerminateInvocationFeatures & current, VkPhysicalDeviceShaderTerminateInvocationFeatures const& merge_in) {
    current.shaderTerminateInvocation = current.shaderTerminateInvocation || merge_in.shaderTerminateInvocation;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
void compare_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& supported, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& requested) {
    if (requested.shaderTerminateInvocation && !supported.shaderTerminateInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR::shaderTerminateInvocation");
    }
}
void merge_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR & current, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& merge_in) {
    current.shaderTerminateInvocation = current.shaderTerminateInvocation || merge_in.shaderTerminateInvocation;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
void compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& supported, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& requested) {
    if (requested.shaderDemoteToHelperInvocation && !supported.shaderDemoteToHelperInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures::shaderDemoteToHelperInvocation");
    }
}
void merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures & current, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& merge_in) {
    current.shaderDemoteToHelperInvocation = current.shaderDemoteToHelperInvocation || merge_in.shaderDemoteToHelperInvocation;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
void compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& supported, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& requested) {
    if (requested.shaderDemoteToHelperInvocation && !supported.shaderDemoteToHelperInvocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT::shaderDemoteToHelperInvocation");
    }
}
void merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT & current, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& merge_in) {
    current.shaderDemoteToHelperInvocation = current.shaderDemoteToHelperInvocation || merge_in.shaderDemoteToHelperInvocation;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
void compare_VkPhysicalDevicePipelineCreationCacheControlFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCreationCacheControlFeatures const& supported, VkPhysicalDevicePipelineCreationCacheControlFeatures const& requested) {
    if (requested.pipelineCreationCacheControl && !supported.pipelineCreationCacheControl) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineCreationCacheControlFeatures::pipelineCreationCacheControl");
    }
}
void merge_VkPhysicalDevicePipelineCreationCacheControlFeatures(VkPhysicalDevicePipelineCreationCacheControlFeatures & current, VkPhysicalDevicePipelineCreationCacheControlFeatures const& merge_in) {
    current.pipelineCreationCacheControl = current.pipelineCreationCacheControl || merge_in.pipelineCreationCacheControl;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
void compare_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& supported, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& requested) {
    if (requested.pipelineCreationCacheControl && !supported.pipelineCreationCacheControl) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT::pipelineCreationCacheControl");
    }
}
void merge_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT & current, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& merge_in) {
    current.pipelineCreationCacheControl = current.pipelineCreationCacheControl || merge_in.pipelineCreationCacheControl;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
void compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& supported, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& requested) {
    if (requested.shaderZeroInitializeWorkgroupMemory && !supported.shaderZeroInitializeWorkgroupMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures::shaderZeroInitializeWorkgroupMemory");
    }
}
void merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures & current, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& merge_in) {
    current.shaderZeroInitializeWorkgroupMemory = current.shaderZeroInitializeWorkgroupMemory || merge_in.shaderZeroInitializeWorkgroupMemory;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
void compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& supported, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& requested) {
    if (requested.shaderZeroInitializeWorkgroupMemory && !supported.shaderZeroInitializeWorkgroupMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR::shaderZeroInitializeWorkgroupMemory");
    }
}
void merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR & current, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& merge_in) {
    current.shaderZeroInitializeWorkgroupMemory = current.shaderZeroInitializeWorkgroupMemory || merge_in.shaderZeroInitializeWorkgroupMemory;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
void compare_VkPhysicalDeviceImageRobustnessFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceImageRobustnessFeatures const& supported, VkPhysicalDeviceImageRobustnessFeatures const& requested) {
    if (requested.robustImageAccess && !supported.robustImageAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageRobustnessFeatures::robustImageAccess");
    }
}
void merge_VkPhysicalDeviceImageRobustnessFeatures(VkPhysicalDeviceImageRobustnessFeatures & current, VkPhysicalDeviceImageRobustnessFeatures const& merge_in) {
    current.robustImageAccess = current.robustImageAccess || merge_in.robustImageAccess;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
void compare_VkPhysicalDeviceImageRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageRobustnessFeaturesEXT const& supported, VkPhysicalDeviceImageRobustnessFeaturesEXT const& requested) {
    if (requested.robustImageAccess && !supported.robustImageAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageRobustnessFeaturesEXT::robustImageAccess");
    }
}
void merge_VkPhysicalDeviceImageRobustnessFeaturesEXT(VkPhysicalDeviceImageRobustnessFeaturesEXT & current, VkPhysicalDeviceImageRobustnessFeaturesEXT const& merge_in) {
    current.robustImageAccess = current.robustImageAccess || merge_in.robustImageAccess;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
void compare_VkPhysicalDeviceSubgroupSizeControlFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSubgroupSizeControlFeatures const& supported, VkPhysicalDeviceSubgroupSizeControlFeatures const& requested) {
    if (requested.subgroupSizeControl && !supported.subgroupSizeControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubgroupSizeControlFeatures::subgroupSizeControl");
    }
    if (requested.computeFullSubgroups && !supported.computeFullSubgroups) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubgroupSizeControlFeatures::computeFullSubgroups");
    }
}
void merge_VkPhysicalDeviceSubgroupSizeControlFeatures(VkPhysicalDeviceSubgroupSizeControlFeatures & current, VkPhysicalDeviceSubgroupSizeControlFeatures const& merge_in) {
    current.subgroupSizeControl = current.subgroupSizeControl || merge_in.subgroupSizeControl;
    current.computeFullSubgroups = current.computeFullSubgroups || merge_in.computeFullSubgroups;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
void compare_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& supported, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& requested) {
    if (requested.subgroupSizeControl && !supported.subgroupSizeControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubgroupSizeControlFeaturesEXT::subgroupSizeControl");
    }
    if (requested.computeFullSubgroups && !supported.computeFullSubgroups) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubgroupSizeControlFeaturesEXT::computeFullSubgroups");
    }
}
void merge_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT & current, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& merge_in) {
    current.subgroupSizeControl = current.subgroupSizeControl || merge_in.subgroupSizeControl;
    current.computeFullSubgroups = current.computeFullSubgroups || merge_in.computeFullSubgroups;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
void compare_VkPhysicalDeviceInlineUniformBlockFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceInlineUniformBlockFeatures const& supported, VkPhysicalDeviceInlineUniformBlockFeatures const& requested) {
    if (requested.inlineUniformBlock && !supported.inlineUniformBlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceInlineUniformBlockFeatures::inlineUniformBlock");
    }
    if (requested.descriptorBindingInlineUniformBlockUpdateAfterBind && !supported.descriptorBindingInlineUniformBlockUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceInlineUniformBlockFeatures::descriptorBindingInlineUniformBlockUpdateAfterBind");
    }
}
void merge_VkPhysicalDeviceInlineUniformBlockFeatures(VkPhysicalDeviceInlineUniformBlockFeatures & current, VkPhysicalDeviceInlineUniformBlockFeatures const& merge_in) {
    current.inlineUniformBlock = current.inlineUniformBlock || merge_in.inlineUniformBlock;
    current.descriptorBindingInlineUniformBlockUpdateAfterBind = current.descriptorBindingInlineUniformBlockUpdateAfterBind || merge_in.descriptorBindingInlineUniformBlockUpdateAfterBind;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
void compare_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& supported, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& requested) {
    if (requested.inlineUniformBlock && !supported.inlineUniformBlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceInlineUniformBlockFeaturesEXT::inlineUniformBlock");
    }
    if (requested.descriptorBindingInlineUniformBlockUpdateAfterBind && !supported.descriptorBindingInlineUniformBlockUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceInlineUniformBlockFeaturesEXT::descriptorBindingInlineUniformBlockUpdateAfterBind");
    }
}
void merge_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(VkPhysicalDeviceInlineUniformBlockFeaturesEXT & current, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& merge_in) {
    current.inlineUniformBlock = current.inlineUniformBlock || merge_in.inlineUniformBlock;
    current.descriptorBindingInlineUniformBlockUpdateAfterBind = current.descriptorBindingInlineUniformBlockUpdateAfterBind || merge_in.descriptorBindingInlineUniformBlockUpdateAfterBind;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
void compare_VkPhysicalDeviceShaderIntegerDotProductFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerDotProductFeatures const& supported, VkPhysicalDeviceShaderIntegerDotProductFeatures const& requested) {
    if (requested.shaderIntegerDotProduct && !supported.shaderIntegerDotProduct) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderIntegerDotProductFeatures::shaderIntegerDotProduct");
    }
}
void merge_VkPhysicalDeviceShaderIntegerDotProductFeatures(VkPhysicalDeviceShaderIntegerDotProductFeatures & current, VkPhysicalDeviceShaderIntegerDotProductFeatures const& merge_in) {
    current.shaderIntegerDotProduct = current.shaderIntegerDotProduct || merge_in.shaderIntegerDotProduct;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
void compare_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& supported, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& requested) {
    if (requested.shaderIntegerDotProduct && !supported.shaderIntegerDotProduct) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR::shaderIntegerDotProduct");
    }
}
void merge_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR & current, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& merge_in) {
    current.shaderIntegerDotProduct = current.shaderIntegerDotProduct || merge_in.shaderIntegerDotProduct;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
void compare_VkPhysicalDeviceDynamicRenderingFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingFeatures const& supported, VkPhysicalDeviceDynamicRenderingFeatures const& requested) {
    if (requested.dynamicRendering && !supported.dynamicRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceDynamicRenderingFeatures::dynamicRendering");
    }
}
void merge_VkPhysicalDeviceDynamicRenderingFeatures(VkPhysicalDeviceDynamicRenderingFeatures & current, VkPhysicalDeviceDynamicRenderingFeatures const& merge_in) {
    current.dynamicRendering = current.dynamicRendering || merge_in.dynamicRendering;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
void compare_VkPhysicalDeviceDynamicRenderingFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& supported, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& requested) {
    if (requested.dynamicRendering && !supported.dynamicRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceDynamicRenderingFeaturesKHR::dynamicRendering");
    }
}
void merge_VkPhysicalDeviceDynamicRenderingFeaturesKHR(VkPhysicalDeviceDynamicRenderingFeaturesKHR & current, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& merge_in) {
    current.dynamicRendering = current.dynamicRendering || merge_in.dynamicRendering;
}
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
#if (defined(VK_VERSION_1_4))
void compare_VkPhysicalDeviceVulkan14Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan14Features const& supported, VkPhysicalDeviceVulkan14Features const& requested) {
    if (requested.globalPriorityQuery && !supported.globalPriorityQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::globalPriorityQuery");
    }
    if (requested.shaderSubgroupRotate && !supported.shaderSubgroupRotate) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::shaderSubgroupRotate");
    }
    if (requested.shaderSubgroupRotateClustered && !supported.shaderSubgroupRotateClustered) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::shaderSubgroupRotateClustered");
    }
    if (requested.shaderFloatControls2 && !supported.shaderFloatControls2) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::shaderFloatControls2");
    }
    if (requested.shaderExpectAssume && !supported.shaderExpectAssume) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::shaderExpectAssume");
    }
    if (requested.rectangularLines && !supported.rectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::rectangularLines");
    }
    if (requested.bresenhamLines && !supported.bresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::bresenhamLines");
    }
    if (requested.smoothLines && !supported.smoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::smoothLines");
    }
    if (requested.stippledRectangularLines && !supported.stippledRectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::stippledRectangularLines");
    }
    if (requested.stippledBresenhamLines && !supported.stippledBresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::stippledBresenhamLines");
    }
    if (requested.stippledSmoothLines && !supported.stippledSmoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::stippledSmoothLines");
    }
    if (requested.vertexAttributeInstanceRateDivisor && !supported.vertexAttributeInstanceRateDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::vertexAttributeInstanceRateDivisor");
    }
    if (requested.vertexAttributeInstanceRateZeroDivisor && !supported.vertexAttributeInstanceRateZeroDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::vertexAttributeInstanceRateZeroDivisor");
    }
    if (requested.indexTypeUint8 && !supported.indexTypeUint8) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::indexTypeUint8");
    }
    if (requested.dynamicRenderingLocalRead && !supported.dynamicRenderingLocalRead) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::dynamicRenderingLocalRead");
    }
    if (requested.maintenance5 && !supported.maintenance5) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::maintenance5");
    }
    if (requested.maintenance6 && !supported.maintenance6) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::maintenance6");
    }
    if (requested.pipelineProtectedAccess && !supported.pipelineProtectedAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::pipelineProtectedAccess");
    }
    if (requested.pipelineRobustness && !supported.pipelineRobustness) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::pipelineRobustness");
    }
    if (requested.hostImageCopy && !supported.hostImageCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::hostImageCopy");
    }
    if (requested.pushDescriptor && !supported.pushDescriptor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVulkan14Features::pushDescriptor");
    }
}
void merge_VkPhysicalDeviceVulkan14Features(VkPhysicalDeviceVulkan14Features & current, VkPhysicalDeviceVulkan14Features const& merge_in) {
    current.globalPriorityQuery = current.globalPriorityQuery || merge_in.globalPriorityQuery;
    current.shaderSubgroupRotate = current.shaderSubgroupRotate || merge_in.shaderSubgroupRotate;
    current.shaderSubgroupRotateClustered = current.shaderSubgroupRotateClustered || merge_in.shaderSubgroupRotateClustered;
    current.shaderFloatControls2 = current.shaderFloatControls2 || merge_in.shaderFloatControls2;
    current.shaderExpectAssume = current.shaderExpectAssume || merge_in.shaderExpectAssume;
    current.rectangularLines = current.rectangularLines || merge_in.rectangularLines;
    current.bresenhamLines = current.bresenhamLines || merge_in.bresenhamLines;
    current.smoothLines = current.smoothLines || merge_in.smoothLines;
    current.stippledRectangularLines = current.stippledRectangularLines || merge_in.stippledRectangularLines;
    current.stippledBresenhamLines = current.stippledBresenhamLines || merge_in.stippledBresenhamLines;
    current.stippledSmoothLines = current.stippledSmoothLines || merge_in.stippledSmoothLines;
    current.vertexAttributeInstanceRateDivisor = current.vertexAttributeInstanceRateDivisor || merge_in.vertexAttributeInstanceRateDivisor;
    current.vertexAttributeInstanceRateZeroDivisor = current.vertexAttributeInstanceRateZeroDivisor || merge_in.vertexAttributeInstanceRateZeroDivisor;
    current.indexTypeUint8 = current.indexTypeUint8 || merge_in.indexTypeUint8;
    current.dynamicRenderingLocalRead = current.dynamicRenderingLocalRead || merge_in.dynamicRenderingLocalRead;
    current.maintenance5 = current.maintenance5 || merge_in.maintenance5;
    current.maintenance6 = current.maintenance6 || merge_in.maintenance6;
    current.pipelineProtectedAccess = current.pipelineProtectedAccess || merge_in.pipelineProtectedAccess;
    current.pipelineRobustness = current.pipelineRobustness || merge_in.pipelineRobustness;
    current.hostImageCopy = current.hostImageCopy || merge_in.hostImageCopy;
    current.pushDescriptor = current.pushDescriptor || merge_in.pushDescriptor;
}
#endif //(defined(VK_VERSION_1_4))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeatures const& supported, VkPhysicalDeviceGlobalPriorityQueryFeatures const& requested) {
    if (requested.globalPriorityQuery && !supported.globalPriorityQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceGlobalPriorityQueryFeatures::globalPriorityQuery");
    }
}
void merge_VkPhysicalDeviceGlobalPriorityQueryFeatures(VkPhysicalDeviceGlobalPriorityQueryFeatures & current, VkPhysicalDeviceGlobalPriorityQueryFeatures const& merge_in) {
    current.globalPriorityQuery = current.globalPriorityQuery || merge_in.globalPriorityQuery;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& supported, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& requested) {
    if (requested.globalPriorityQuery && !supported.globalPriorityQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR::globalPriorityQuery");
    }
}
void merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR & current, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& merge_in) {
    current.globalPriorityQuery = current.globalPriorityQuery || merge_in.globalPriorityQuery;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& supported, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& requested) {
    if (requested.globalPriorityQuery && !supported.globalPriorityQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT::globalPriorityQuery");
    }
}
void merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT & current, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& merge_in) {
    current.globalPriorityQuery = current.globalPriorityQuery || merge_in.globalPriorityQuery;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8Features(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8Features const& supported, VkPhysicalDeviceIndexTypeUint8Features const& requested) {
    if (requested.indexTypeUint8 && !supported.indexTypeUint8) {
        error_list.push_back("Missing feature VkPhysicalDeviceIndexTypeUint8Features::indexTypeUint8");
    }
}
void merge_VkPhysicalDeviceIndexTypeUint8Features(VkPhysicalDeviceIndexTypeUint8Features & current, VkPhysicalDeviceIndexTypeUint8Features const& merge_in) {
    current.indexTypeUint8 = current.indexTypeUint8 || merge_in.indexTypeUint8;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& supported, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& requested) {
    if (requested.indexTypeUint8 && !supported.indexTypeUint8) {
        error_list.push_back("Missing feature VkPhysicalDeviceIndexTypeUint8FeaturesKHR::indexTypeUint8");
    }
}
void merge_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(VkPhysicalDeviceIndexTypeUint8FeaturesKHR & current, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& merge_in) {
    current.indexTypeUint8 = current.indexTypeUint8 || merge_in.indexTypeUint8;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& supported, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& requested) {
    if (requested.indexTypeUint8 && !supported.indexTypeUint8) {
        error_list.push_back("Missing feature VkPhysicalDeviceIndexTypeUint8FeaturesEXT::indexTypeUint8");
    }
}
void merge_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(VkPhysicalDeviceIndexTypeUint8FeaturesEXT & current, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& merge_in) {
    current.indexTypeUint8 = current.indexTypeUint8 || merge_in.indexTypeUint8;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
void compare_VkPhysicalDeviceMaintenance5Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance5Features const& supported, VkPhysicalDeviceMaintenance5Features const& requested) {
    if (requested.maintenance5 && !supported.maintenance5) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance5Features::maintenance5");
    }
}
void merge_VkPhysicalDeviceMaintenance5Features(VkPhysicalDeviceMaintenance5Features & current, VkPhysicalDeviceMaintenance5Features const& merge_in) {
    current.maintenance5 = current.maintenance5 || merge_in.maintenance5;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
void compare_VkPhysicalDeviceMaintenance5FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance5FeaturesKHR const& supported, VkPhysicalDeviceMaintenance5FeaturesKHR const& requested) {
    if (requested.maintenance5 && !supported.maintenance5) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance5FeaturesKHR::maintenance5");
    }
}
void merge_VkPhysicalDeviceMaintenance5FeaturesKHR(VkPhysicalDeviceMaintenance5FeaturesKHR & current, VkPhysicalDeviceMaintenance5FeaturesKHR const& merge_in) {
    current.maintenance5 = current.maintenance5 || merge_in.maintenance5;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
void compare_VkPhysicalDeviceMaintenance6Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance6Features const& supported, VkPhysicalDeviceMaintenance6Features const& requested) {
    if (requested.maintenance6 && !supported.maintenance6) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance6Features::maintenance6");
    }
}
void merge_VkPhysicalDeviceMaintenance6Features(VkPhysicalDeviceMaintenance6Features & current, VkPhysicalDeviceMaintenance6Features const& merge_in) {
    current.maintenance6 = current.maintenance6 || merge_in.maintenance6;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
void compare_VkPhysicalDeviceMaintenance6FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance6FeaturesKHR const& supported, VkPhysicalDeviceMaintenance6FeaturesKHR const& requested) {
    if (requested.maintenance6 && !supported.maintenance6) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance6FeaturesKHR::maintenance6");
    }
}
void merge_VkPhysicalDeviceMaintenance6FeaturesKHR(VkPhysicalDeviceMaintenance6FeaturesKHR & current, VkPhysicalDeviceMaintenance6FeaturesKHR const& merge_in) {
    current.maintenance6 = current.maintenance6 || merge_in.maintenance6;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
void compare_VkPhysicalDeviceHostImageCopyFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceHostImageCopyFeatures const& supported, VkPhysicalDeviceHostImageCopyFeatures const& requested) {
    if (requested.hostImageCopy && !supported.hostImageCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceHostImageCopyFeatures::hostImageCopy");
    }
}
void merge_VkPhysicalDeviceHostImageCopyFeatures(VkPhysicalDeviceHostImageCopyFeatures & current, VkPhysicalDeviceHostImageCopyFeatures const& merge_in) {
    current.hostImageCopy = current.hostImageCopy || merge_in.hostImageCopy;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
void compare_VkPhysicalDeviceHostImageCopyFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceHostImageCopyFeaturesEXT const& supported, VkPhysicalDeviceHostImageCopyFeaturesEXT const& requested) {
    if (requested.hostImageCopy && !supported.hostImageCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceHostImageCopyFeaturesEXT::hostImageCopy");
    }
}
void merge_VkPhysicalDeviceHostImageCopyFeaturesEXT(VkPhysicalDeviceHostImageCopyFeaturesEXT & current, VkPhysicalDeviceHostImageCopyFeaturesEXT const& merge_in) {
    current.hostImageCopy = current.hostImageCopy || merge_in.hostImageCopy;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
void compare_VkPhysicalDeviceShaderSubgroupRotateFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupRotateFeatures const& supported, VkPhysicalDeviceShaderSubgroupRotateFeatures const& requested) {
    if (requested.shaderSubgroupRotate && !supported.shaderSubgroupRotate) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupRotateFeatures::shaderSubgroupRotate");
    }
    if (requested.shaderSubgroupRotateClustered && !supported.shaderSubgroupRotateClustered) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupRotateFeatures::shaderSubgroupRotateClustered");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupRotateFeatures(VkPhysicalDeviceShaderSubgroupRotateFeatures & current, VkPhysicalDeviceShaderSubgroupRotateFeatures const& merge_in) {
    current.shaderSubgroupRotate = current.shaderSubgroupRotate || merge_in.shaderSubgroupRotate;
    current.shaderSubgroupRotateClustered = current.shaderSubgroupRotateClustered || merge_in.shaderSubgroupRotateClustered;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
void compare_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& requested) {
    if (requested.shaderSubgroupRotate && !supported.shaderSubgroupRotate) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR::shaderSubgroupRotate");
    }
    if (requested.shaderSubgroupRotateClustered && !supported.shaderSubgroupRotateClustered) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR::shaderSubgroupRotateClustered");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& merge_in) {
    current.shaderSubgroupRotate = current.shaderSubgroupRotate || merge_in.shaderSubgroupRotate;
    current.shaderSubgroupRotateClustered = current.shaderSubgroupRotateClustered || merge_in.shaderSubgroupRotateClustered;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
void compare_VkPhysicalDeviceShaderFloatControls2Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloatControls2Features const& supported, VkPhysicalDeviceShaderFloatControls2Features const& requested) {
    if (requested.shaderFloatControls2 && !supported.shaderFloatControls2) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloatControls2Features::shaderFloatControls2");
    }
}
void merge_VkPhysicalDeviceShaderFloatControls2Features(VkPhysicalDeviceShaderFloatControls2Features & current, VkPhysicalDeviceShaderFloatControls2Features const& merge_in) {
    current.shaderFloatControls2 = current.shaderFloatControls2 || merge_in.shaderFloatControls2;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
void compare_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& supported, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& requested) {
    if (requested.shaderFloatControls2 && !supported.shaderFloatControls2) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloatControls2FeaturesKHR::shaderFloatControls2");
    }
}
void merge_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(VkPhysicalDeviceShaderFloatControls2FeaturesKHR & current, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& merge_in) {
    current.shaderFloatControls2 = current.shaderFloatControls2 || merge_in.shaderFloatControls2;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
void compare_VkPhysicalDeviceShaderExpectAssumeFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderExpectAssumeFeatures const& supported, VkPhysicalDeviceShaderExpectAssumeFeatures const& requested) {
    if (requested.shaderExpectAssume && !supported.shaderExpectAssume) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderExpectAssumeFeatures::shaderExpectAssume");
    }
}
void merge_VkPhysicalDeviceShaderExpectAssumeFeatures(VkPhysicalDeviceShaderExpectAssumeFeatures & current, VkPhysicalDeviceShaderExpectAssumeFeatures const& merge_in) {
    current.shaderExpectAssume = current.shaderExpectAssume || merge_in.shaderExpectAssume;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
void compare_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& supported, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& requested) {
    if (requested.shaderExpectAssume && !supported.shaderExpectAssume) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderExpectAssumeFeaturesKHR::shaderExpectAssume");
    }
}
void merge_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(VkPhysicalDeviceShaderExpectAssumeFeaturesKHR & current, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& merge_in) {
    current.shaderExpectAssume = current.shaderExpectAssume || merge_in.shaderExpectAssume;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
void compare_VkPhysicalDevicePipelineProtectedAccessFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineProtectedAccessFeatures const& supported, VkPhysicalDevicePipelineProtectedAccessFeatures const& requested) {
    if (requested.pipelineProtectedAccess && !supported.pipelineProtectedAccess) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineProtectedAccessFeatures::pipelineProtectedAccess");
    }
}
void merge_VkPhysicalDevicePipelineProtectedAccessFeatures(VkPhysicalDevicePipelineProtectedAccessFeatures & current, VkPhysicalDevicePipelineProtectedAccessFeatures const& merge_in) {
    current.pipelineProtectedAccess = current.pipelineProtectedAccess || merge_in.pipelineProtectedAccess;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
void compare_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& supported, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& requested) {
    if (requested.pipelineProtectedAccess && !supported.pipelineProtectedAccess) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineProtectedAccessFeaturesEXT::pipelineProtectedAccess");
    }
}
void merge_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(VkPhysicalDevicePipelineProtectedAccessFeaturesEXT & current, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& merge_in) {
    current.pipelineProtectedAccess = current.pipelineProtectedAccess || merge_in.pipelineProtectedAccess;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
void compare_VkPhysicalDevicePipelineRobustnessFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineRobustnessFeatures const& supported, VkPhysicalDevicePipelineRobustnessFeatures const& requested) {
    if (requested.pipelineRobustness && !supported.pipelineRobustness) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineRobustnessFeatures::pipelineRobustness");
    }
}
void merge_VkPhysicalDevicePipelineRobustnessFeatures(VkPhysicalDevicePipelineRobustnessFeatures & current, VkPhysicalDevicePipelineRobustnessFeatures const& merge_in) {
    current.pipelineRobustness = current.pipelineRobustness || merge_in.pipelineRobustness;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
void compare_VkPhysicalDevicePipelineRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& supported, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& requested) {
    if (requested.pipelineRobustness && !supported.pipelineRobustness) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineRobustnessFeaturesEXT::pipelineRobustness");
    }
}
void merge_VkPhysicalDevicePipelineRobustnessFeaturesEXT(VkPhysicalDevicePipelineRobustnessFeaturesEXT & current, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& merge_in) {
    current.pipelineRobustness = current.pipelineRobustness || merge_in.pipelineRobustness;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeatures const& supported, VkPhysicalDeviceLineRasterizationFeatures const& requested) {
    if (requested.rectangularLines && !supported.rectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::rectangularLines");
    }
    if (requested.bresenhamLines && !supported.bresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::bresenhamLines");
    }
    if (requested.smoothLines && !supported.smoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::smoothLines");
    }
    if (requested.stippledRectangularLines && !supported.stippledRectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::stippledRectangularLines");
    }
    if (requested.stippledBresenhamLines && !supported.stippledBresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::stippledBresenhamLines");
    }
    if (requested.stippledSmoothLines && !supported.stippledSmoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeatures::stippledSmoothLines");
    }
}
void merge_VkPhysicalDeviceLineRasterizationFeatures(VkPhysicalDeviceLineRasterizationFeatures & current, VkPhysicalDeviceLineRasterizationFeatures const& merge_in) {
    current.rectangularLines = current.rectangularLines || merge_in.rectangularLines;
    current.bresenhamLines = current.bresenhamLines || merge_in.bresenhamLines;
    current.smoothLines = current.smoothLines || merge_in.smoothLines;
    current.stippledRectangularLines = current.stippledRectangularLines || merge_in.stippledRectangularLines;
    current.stippledBresenhamLines = current.stippledBresenhamLines || merge_in.stippledBresenhamLines;
    current.stippledSmoothLines = current.stippledSmoothLines || merge_in.stippledSmoothLines;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeaturesKHR const& supported, VkPhysicalDeviceLineRasterizationFeaturesKHR const& requested) {
    if (requested.rectangularLines && !supported.rectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::rectangularLines");
    }
    if (requested.bresenhamLines && !supported.bresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::bresenhamLines");
    }
    if (requested.smoothLines && !supported.smoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::smoothLines");
    }
    if (requested.stippledRectangularLines && !supported.stippledRectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::stippledRectangularLines");
    }
    if (requested.stippledBresenhamLines && !supported.stippledBresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::stippledBresenhamLines");
    }
    if (requested.stippledSmoothLines && !supported.stippledSmoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesKHR::stippledSmoothLines");
    }
}
void merge_VkPhysicalDeviceLineRasterizationFeaturesKHR(VkPhysicalDeviceLineRasterizationFeaturesKHR & current, VkPhysicalDeviceLineRasterizationFeaturesKHR const& merge_in) {
    current.rectangularLines = current.rectangularLines || merge_in.rectangularLines;
    current.bresenhamLines = current.bresenhamLines || merge_in.bresenhamLines;
    current.smoothLines = current.smoothLines || merge_in.smoothLines;
    current.stippledRectangularLines = current.stippledRectangularLines || merge_in.stippledRectangularLines;
    current.stippledBresenhamLines = current.stippledBresenhamLines || merge_in.stippledBresenhamLines;
    current.stippledSmoothLines = current.stippledSmoothLines || merge_in.stippledSmoothLines;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeaturesEXT const& supported, VkPhysicalDeviceLineRasterizationFeaturesEXT const& requested) {
    if (requested.rectangularLines && !supported.rectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::rectangularLines");
    }
    if (requested.bresenhamLines && !supported.bresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::bresenhamLines");
    }
    if (requested.smoothLines && !supported.smoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::smoothLines");
    }
    if (requested.stippledRectangularLines && !supported.stippledRectangularLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::stippledRectangularLines");
    }
    if (requested.stippledBresenhamLines && !supported.stippledBresenhamLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::stippledBresenhamLines");
    }
    if (requested.stippledSmoothLines && !supported.stippledSmoothLines) {
        error_list.push_back("Missing feature VkPhysicalDeviceLineRasterizationFeaturesEXT::stippledSmoothLines");
    }
}
void merge_VkPhysicalDeviceLineRasterizationFeaturesEXT(VkPhysicalDeviceLineRasterizationFeaturesEXT & current, VkPhysicalDeviceLineRasterizationFeaturesEXT const& merge_in) {
    current.rectangularLines = current.rectangularLines || merge_in.rectangularLines;
    current.bresenhamLines = current.bresenhamLines || merge_in.bresenhamLines;
    current.smoothLines = current.smoothLines || merge_in.smoothLines;
    current.stippledRectangularLines = current.stippledRectangularLines || merge_in.stippledRectangularLines;
    current.stippledBresenhamLines = current.stippledBresenhamLines || merge_in.stippledBresenhamLines;
    current.stippledSmoothLines = current.stippledSmoothLines || merge_in.stippledSmoothLines;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeatures const& supported, VkPhysicalDeviceVertexAttributeDivisorFeatures const& requested) {
    if (requested.vertexAttributeInstanceRateDivisor && !supported.vertexAttributeInstanceRateDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeatures::vertexAttributeInstanceRateDivisor");
    }
    if (requested.vertexAttributeInstanceRateZeroDivisor && !supported.vertexAttributeInstanceRateZeroDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeatures::vertexAttributeInstanceRateZeroDivisor");
    }
}
void merge_VkPhysicalDeviceVertexAttributeDivisorFeatures(VkPhysicalDeviceVertexAttributeDivisorFeatures & current, VkPhysicalDeviceVertexAttributeDivisorFeatures const& merge_in) {
    current.vertexAttributeInstanceRateDivisor = current.vertexAttributeInstanceRateDivisor || merge_in.vertexAttributeInstanceRateDivisor;
    current.vertexAttributeInstanceRateZeroDivisor = current.vertexAttributeInstanceRateZeroDivisor || merge_in.vertexAttributeInstanceRateZeroDivisor;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& supported, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& requested) {
    if (requested.vertexAttributeInstanceRateDivisor && !supported.vertexAttributeInstanceRateDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR::vertexAttributeInstanceRateDivisor");
    }
    if (requested.vertexAttributeInstanceRateZeroDivisor && !supported.vertexAttributeInstanceRateZeroDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR::vertexAttributeInstanceRateZeroDivisor");
    }
}
void merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR & current, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& merge_in) {
    current.vertexAttributeInstanceRateDivisor = current.vertexAttributeInstanceRateDivisor || merge_in.vertexAttributeInstanceRateDivisor;
    current.vertexAttributeInstanceRateZeroDivisor = current.vertexAttributeInstanceRateZeroDivisor || merge_in.vertexAttributeInstanceRateZeroDivisor;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& supported, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& requested) {
    if (requested.vertexAttributeInstanceRateDivisor && !supported.vertexAttributeInstanceRateDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT::vertexAttributeInstanceRateDivisor");
    }
    if (requested.vertexAttributeInstanceRateZeroDivisor && !supported.vertexAttributeInstanceRateZeroDivisor) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT::vertexAttributeInstanceRateZeroDivisor");
    }
}
void merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT & current, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& merge_in) {
    current.vertexAttributeInstanceRateDivisor = current.vertexAttributeInstanceRateDivisor || merge_in.vertexAttributeInstanceRateDivisor;
    current.vertexAttributeInstanceRateZeroDivisor = current.vertexAttributeInstanceRateZeroDivisor || merge_in.vertexAttributeInstanceRateZeroDivisor;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
void compare_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& supported, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& requested) {
    if (requested.dynamicRenderingLocalRead && !supported.dynamicRenderingLocalRead) {
        error_list.push_back("Missing feature VkPhysicalDeviceDynamicRenderingLocalReadFeatures::dynamicRenderingLocalRead");
    }
}
void merge_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(VkPhysicalDeviceDynamicRenderingLocalReadFeatures & current, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& merge_in) {
    current.dynamicRenderingLocalRead = current.dynamicRenderingLocalRead || merge_in.dynamicRenderingLocalRead;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
void compare_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& supported, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& requested) {
    if (requested.dynamicRenderingLocalRead && !supported.dynamicRenderingLocalRead) {
        error_list.push_back("Missing feature VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR::dynamicRenderingLocalRead");
    }
}
void merge_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR & current, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& merge_in) {
    current.dynamicRenderingLocalRead = current.dynamicRenderingLocalRead || merge_in.dynamicRenderingLocalRead;
}
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
#if (defined(VK_KHR_performance_query))
void compare_VkPhysicalDevicePerformanceQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePerformanceQueryFeaturesKHR const& supported, VkPhysicalDevicePerformanceQueryFeaturesKHR const& requested) {
    if (requested.performanceCounterQueryPools && !supported.performanceCounterQueryPools) {
        error_list.push_back("Missing feature VkPhysicalDevicePerformanceQueryFeaturesKHR::performanceCounterQueryPools");
    }
    if (requested.performanceCounterMultipleQueryPools && !supported.performanceCounterMultipleQueryPools) {
        error_list.push_back("Missing feature VkPhysicalDevicePerformanceQueryFeaturesKHR::performanceCounterMultipleQueryPools");
    }
}
void merge_VkPhysicalDevicePerformanceQueryFeaturesKHR(VkPhysicalDevicePerformanceQueryFeaturesKHR & current, VkPhysicalDevicePerformanceQueryFeaturesKHR const& merge_in) {
    current.performanceCounterQueryPools = current.performanceCounterQueryPools || merge_in.performanceCounterQueryPools;
    current.performanceCounterMultipleQueryPools = current.performanceCounterMultipleQueryPools || merge_in.performanceCounterMultipleQueryPools;
}
#endif //(defined(VK_KHR_performance_query))
#if (defined(VK_KHR_shader_bfloat16))
void compare_VkPhysicalDeviceShaderBfloat16FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& supported, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& requested) {
    if (requested.shaderBFloat16Type && !supported.shaderBFloat16Type) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderBfloat16FeaturesKHR::shaderBFloat16Type");
    }
    if (requested.shaderBFloat16DotProduct && !supported.shaderBFloat16DotProduct) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderBfloat16FeaturesKHR::shaderBFloat16DotProduct");
    }
    if (requested.shaderBFloat16CooperativeMatrix && !supported.shaderBFloat16CooperativeMatrix) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderBfloat16FeaturesKHR::shaderBFloat16CooperativeMatrix");
    }
}
void merge_VkPhysicalDeviceShaderBfloat16FeaturesKHR(VkPhysicalDeviceShaderBfloat16FeaturesKHR & current, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& merge_in) {
    current.shaderBFloat16Type = current.shaderBFloat16Type || merge_in.shaderBFloat16Type;
    current.shaderBFloat16DotProduct = current.shaderBFloat16DotProduct || merge_in.shaderBFloat16DotProduct;
    current.shaderBFloat16CooperativeMatrix = current.shaderBFloat16CooperativeMatrix || merge_in.shaderBFloat16CooperativeMatrix;
}
#endif //(defined(VK_KHR_shader_bfloat16))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
void compare_VkPhysicalDevicePortabilitySubsetFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& supported, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& requested) {
    if (requested.constantAlphaColorBlendFactors && !supported.constantAlphaColorBlendFactors) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::constantAlphaColorBlendFactors");
    }
    if (requested.events && !supported.events) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::events");
    }
    if (requested.imageViewFormatReinterpretation && !supported.imageViewFormatReinterpretation) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::imageViewFormatReinterpretation");
    }
    if (requested.imageViewFormatSwizzle && !supported.imageViewFormatSwizzle) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::imageViewFormatSwizzle");
    }
    if (requested.imageView2DOn3DImage && !supported.imageView2DOn3DImage) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::imageView2DOn3DImage");
    }
    if (requested.multisampleArrayImage && !supported.multisampleArrayImage) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::multisampleArrayImage");
    }
    if (requested.mutableComparisonSamplers && !supported.mutableComparisonSamplers) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::mutableComparisonSamplers");
    }
    if (requested.pointPolygons && !supported.pointPolygons) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::pointPolygons");
    }
    if (requested.samplerMipLodBias && !supported.samplerMipLodBias) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::samplerMipLodBias");
    }
    if (requested.separateStencilMaskRef && !supported.separateStencilMaskRef) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::separateStencilMaskRef");
    }
    if (requested.shaderSampleRateInterpolationFunctions && !supported.shaderSampleRateInterpolationFunctions) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::shaderSampleRateInterpolationFunctions");
    }
    if (requested.tessellationIsolines && !supported.tessellationIsolines) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationIsolines");
    }
    if (requested.tessellationPointMode && !supported.tessellationPointMode) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationPointMode");
    }
    if (requested.triangleFans && !supported.triangleFans) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::triangleFans");
    }
    if (requested.vertexAttributeAccessBeyondStride && !supported.vertexAttributeAccessBeyondStride) {
        error_list.push_back("Missing feature VkPhysicalDevicePortabilitySubsetFeaturesKHR::vertexAttributeAccessBeyondStride");
    }
}
void merge_VkPhysicalDevicePortabilitySubsetFeaturesKHR(VkPhysicalDevicePortabilitySubsetFeaturesKHR & current, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& merge_in) {
    current.constantAlphaColorBlendFactors = current.constantAlphaColorBlendFactors || merge_in.constantAlphaColorBlendFactors;
    current.events = current.events || merge_in.events;
    current.imageViewFormatReinterpretation = current.imageViewFormatReinterpretation || merge_in.imageViewFormatReinterpretation;
    current.imageViewFormatSwizzle = current.imageViewFormatSwizzle || merge_in.imageViewFormatSwizzle;
    current.imageView2DOn3DImage = current.imageView2DOn3DImage || merge_in.imageView2DOn3DImage;
    current.multisampleArrayImage = current.multisampleArrayImage || merge_in.multisampleArrayImage;
    current.mutableComparisonSamplers = current.mutableComparisonSamplers || merge_in.mutableComparisonSamplers;
    current.pointPolygons = current.pointPolygons || merge_in.pointPolygons;
    current.samplerMipLodBias = current.samplerMipLodBias || merge_in.samplerMipLodBias;
    current.separateStencilMaskRef = current.separateStencilMaskRef || merge_in.separateStencilMaskRef;
    current.shaderSampleRateInterpolationFunctions = current.shaderSampleRateInterpolationFunctions || merge_in.shaderSampleRateInterpolationFunctions;
    current.tessellationIsolines = current.tessellationIsolines || merge_in.tessellationIsolines;
    current.tessellationPointMode = current.tessellationPointMode || merge_in.tessellationPointMode;
    current.triangleFans = current.triangleFans || merge_in.triangleFans;
    current.vertexAttributeAccessBeyondStride = current.vertexAttributeAccessBeyondStride || merge_in.vertexAttributeAccessBeyondStride;
}
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
#if (defined(VK_KHR_shader_clock))
void compare_VkPhysicalDeviceShaderClockFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderClockFeaturesKHR const& supported, VkPhysicalDeviceShaderClockFeaturesKHR const& requested) {
    if (requested.shaderSubgroupClock && !supported.shaderSubgroupClock) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderClockFeaturesKHR::shaderSubgroupClock");
    }
    if (requested.shaderDeviceClock && !supported.shaderDeviceClock) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderClockFeaturesKHR::shaderDeviceClock");
    }
}
void merge_VkPhysicalDeviceShaderClockFeaturesKHR(VkPhysicalDeviceShaderClockFeaturesKHR & current, VkPhysicalDeviceShaderClockFeaturesKHR const& merge_in) {
    current.shaderSubgroupClock = current.shaderSubgroupClock || merge_in.shaderSubgroupClock;
    current.shaderDeviceClock = current.shaderDeviceClock || merge_in.shaderDeviceClock;
}
#endif //(defined(VK_KHR_shader_clock))
#if (defined(VK_KHR_fragment_shading_rate))
void compare_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& supported, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& requested) {
    if (requested.pipelineFragmentShadingRate && !supported.pipelineFragmentShadingRate) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateFeaturesKHR::pipelineFragmentShadingRate");
    }
    if (requested.primitiveFragmentShadingRate && !supported.primitiveFragmentShadingRate) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateFeaturesKHR::primitiveFragmentShadingRate");
    }
    if (requested.attachmentFragmentShadingRate && !supported.attachmentFragmentShadingRate) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateFeaturesKHR::attachmentFragmentShadingRate");
    }
}
void merge_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(VkPhysicalDeviceFragmentShadingRateFeaturesKHR & current, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& merge_in) {
    current.pipelineFragmentShadingRate = current.pipelineFragmentShadingRate || merge_in.pipelineFragmentShadingRate;
    current.primitiveFragmentShadingRate = current.primitiveFragmentShadingRate || merge_in.primitiveFragmentShadingRate;
    current.attachmentFragmentShadingRate = current.attachmentFragmentShadingRate || merge_in.attachmentFragmentShadingRate;
}
#endif //(defined(VK_KHR_fragment_shading_rate))
#if (defined(VK_KHR_shader_constant_data))
void compare_VkPhysicalDeviceShaderConstantDataFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& supported, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& requested) {
    if (requested.shaderConstantData && !supported.shaderConstantData) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderConstantDataFeaturesKHR::shaderConstantData");
    }
}
void merge_VkPhysicalDeviceShaderConstantDataFeaturesKHR(VkPhysicalDeviceShaderConstantDataFeaturesKHR & current, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& merge_in) {
    current.shaderConstantData = current.shaderConstantData || merge_in.shaderConstantData;
}
#endif //(defined(VK_KHR_shader_constant_data))
#if (defined(VK_KHR_shader_abort))
void compare_VkPhysicalDeviceShaderAbortFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAbortFeaturesKHR const& supported, VkPhysicalDeviceShaderAbortFeaturesKHR const& requested) {
    if (requested.shaderAbort && !supported.shaderAbort) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAbortFeaturesKHR::shaderAbort");
    }
}
void merge_VkPhysicalDeviceShaderAbortFeaturesKHR(VkPhysicalDeviceShaderAbortFeaturesKHR & current, VkPhysicalDeviceShaderAbortFeaturesKHR const& merge_in) {
    current.shaderAbort = current.shaderAbort || merge_in.shaderAbort;
}
#endif //(defined(VK_KHR_shader_abort))
#if (defined(VK_KHR_shader_quad_control))
void compare_VkPhysicalDeviceShaderQuadControlFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& supported, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& requested) {
    if (requested.shaderQuadControl && !supported.shaderQuadControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderQuadControlFeaturesKHR::shaderQuadControl");
    }
}
void merge_VkPhysicalDeviceShaderQuadControlFeaturesKHR(VkPhysicalDeviceShaderQuadControlFeaturesKHR & current, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& merge_in) {
    current.shaderQuadControl = current.shaderQuadControl || merge_in.shaderQuadControl;
}
#endif //(defined(VK_KHR_shader_quad_control))
#if (defined(VK_KHR_present_wait))
void compare_VkPhysicalDevicePresentWaitFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentWaitFeaturesKHR const& supported, VkPhysicalDevicePresentWaitFeaturesKHR const& requested) {
    if (requested.presentWait && !supported.presentWait) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentWaitFeaturesKHR::presentWait");
    }
}
void merge_VkPhysicalDevicePresentWaitFeaturesKHR(VkPhysicalDevicePresentWaitFeaturesKHR & current, VkPhysicalDevicePresentWaitFeaturesKHR const& merge_in) {
    current.presentWait = current.presentWait || merge_in.presentWait;
}
#endif //(defined(VK_KHR_present_wait))
#if (defined(VK_KHR_pipeline_executable_properties))
void compare_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& supported, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& requested) {
    if (requested.pipelineExecutableInfo && !supported.pipelineExecutableInfo) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR::pipelineExecutableInfo");
    }
}
void merge_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR & current, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& merge_in) {
    current.pipelineExecutableInfo = current.pipelineExecutableInfo || merge_in.pipelineExecutableInfo;
}
#endif //(defined(VK_KHR_pipeline_executable_properties))
#if (defined(VK_KHR_present_id))
void compare_VkPhysicalDevicePresentIdFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentIdFeaturesKHR const& supported, VkPhysicalDevicePresentIdFeaturesKHR const& requested) {
    if (requested.presentId && !supported.presentId) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentIdFeaturesKHR::presentId");
    }
}
void merge_VkPhysicalDevicePresentIdFeaturesKHR(VkPhysicalDevicePresentIdFeaturesKHR & current, VkPhysicalDevicePresentIdFeaturesKHR const& merge_in) {
    current.presentId = current.presentId || merge_in.presentId;
}
#endif //(defined(VK_KHR_present_id))
#if (defined(VK_KHR_device_address_commands))
void compare_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& supported, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& requested) {
    if (requested.deviceAddressCommands && !supported.deviceAddressCommands) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR::deviceAddressCommands");
    }
}
void merge_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR & current, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& merge_in) {
    current.deviceAddressCommands = current.deviceAddressCommands || merge_in.deviceAddressCommands;
}
#endif //(defined(VK_KHR_device_address_commands))
#if (defined(VK_KHR_fragment_shader_barycentric))
void compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& supported, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& requested) {
    if (requested.fragmentShaderBarycentric && !supported.fragmentShaderBarycentric) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR::fragmentShaderBarycentric");
    }
}
void merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR & current, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& merge_in) {
    current.fragmentShaderBarycentric = current.fragmentShaderBarycentric || merge_in.fragmentShaderBarycentric;
}
#endif //(defined(VK_KHR_fragment_shader_barycentric))
#if (defined(VK_NV_fragment_shader_barycentric))
void compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& supported, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& requested) {
    if (requested.fragmentShaderBarycentric && !supported.fragmentShaderBarycentric) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV::fragmentShaderBarycentric");
    }
}
void merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV & current, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& merge_in) {
    current.fragmentShaderBarycentric = current.fragmentShaderBarycentric || merge_in.fragmentShaderBarycentric;
}
#endif //(defined(VK_NV_fragment_shader_barycentric))
#if (defined(VK_KHR_shader_subgroup_uniform_control_flow))
void compare_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& requested) {
    if (requested.shaderSubgroupUniformControlFlow && !supported.shaderSubgroupUniformControlFlow) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR::shaderSubgroupUniformControlFlow");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& merge_in) {
    current.shaderSubgroupUniformControlFlow = current.shaderSubgroupUniformControlFlow || merge_in.shaderSubgroupUniformControlFlow;
}
#endif //(defined(VK_KHR_shader_subgroup_uniform_control_flow))
#if (defined(VK_KHR_workgroup_memory_explicit_layout))
void compare_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& supported, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& requested) {
    if (requested.workgroupMemoryExplicitLayout && !supported.workgroupMemoryExplicitLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR::workgroupMemoryExplicitLayout");
    }
    if (requested.workgroupMemoryExplicitLayoutScalarBlockLayout && !supported.workgroupMemoryExplicitLayoutScalarBlockLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR::workgroupMemoryExplicitLayoutScalarBlockLayout");
    }
    if (requested.workgroupMemoryExplicitLayout8BitAccess && !supported.workgroupMemoryExplicitLayout8BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR::workgroupMemoryExplicitLayout8BitAccess");
    }
    if (requested.workgroupMemoryExplicitLayout16BitAccess && !supported.workgroupMemoryExplicitLayout16BitAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR::workgroupMemoryExplicitLayout16BitAccess");
    }
}
void merge_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR & current, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& merge_in) {
    current.workgroupMemoryExplicitLayout = current.workgroupMemoryExplicitLayout || merge_in.workgroupMemoryExplicitLayout;
    current.workgroupMemoryExplicitLayoutScalarBlockLayout = current.workgroupMemoryExplicitLayoutScalarBlockLayout || merge_in.workgroupMemoryExplicitLayoutScalarBlockLayout;
    current.workgroupMemoryExplicitLayout8BitAccess = current.workgroupMemoryExplicitLayout8BitAccess || merge_in.workgroupMemoryExplicitLayout8BitAccess;
    current.workgroupMemoryExplicitLayout16BitAccess = current.workgroupMemoryExplicitLayout16BitAccess || merge_in.workgroupMemoryExplicitLayout16BitAccess;
}
#endif //(defined(VK_KHR_workgroup_memory_explicit_layout))
#if (defined(VK_KHR_ray_tracing_maintenance1))
void compare_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& requested) {
    if (requested.rayTracingMaintenance1 && !supported.rayTracingMaintenance1) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR::rayTracingMaintenance1");
    }
    if (requested.rayTracingPipelineTraceRaysIndirect2 && !supported.rayTracingPipelineTraceRaysIndirect2) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR::rayTracingPipelineTraceRaysIndirect2");
    }
}
void merge_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR & current, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& merge_in) {
    current.rayTracingMaintenance1 = current.rayTracingMaintenance1 || merge_in.rayTracingMaintenance1;
    current.rayTracingPipelineTraceRaysIndirect2 = current.rayTracingPipelineTraceRaysIndirect2 || merge_in.rayTracingPipelineTraceRaysIndirect2;
}
#endif //(defined(VK_KHR_ray_tracing_maintenance1))
#if (defined(VK_KHR_shader_untyped_pointers))
void compare_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& supported, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& requested) {
    if (requested.shaderUntypedPointers && !supported.shaderUntypedPointers) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderUntypedPointersFeaturesKHR::shaderUntypedPointers");
    }
}
void merge_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(VkPhysicalDeviceShaderUntypedPointersFeaturesKHR & current, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& merge_in) {
    current.shaderUntypedPointers = current.shaderUntypedPointers || merge_in.shaderUntypedPointers;
}
#endif //(defined(VK_KHR_shader_untyped_pointers))
#if (defined(VK_KHR_shader_maximal_reconvergence))
void compare_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& supported, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& requested) {
    if (requested.shaderMaximalReconvergence && !supported.shaderMaximalReconvergence) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR::shaderMaximalReconvergence");
    }
}
void merge_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR & current, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& merge_in) {
    current.shaderMaximalReconvergence = current.shaderMaximalReconvergence || merge_in.shaderMaximalReconvergence;
}
#endif //(defined(VK_KHR_shader_maximal_reconvergence))
#if (defined(VK_KHR_present_id2))
void compare_VkPhysicalDevicePresentId2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentId2FeaturesKHR const& supported, VkPhysicalDevicePresentId2FeaturesKHR const& requested) {
    if (requested.presentId2 && !supported.presentId2) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentId2FeaturesKHR::presentId2");
    }
}
void merge_VkPhysicalDevicePresentId2FeaturesKHR(VkPhysicalDevicePresentId2FeaturesKHR & current, VkPhysicalDevicePresentId2FeaturesKHR const& merge_in) {
    current.presentId2 = current.presentId2 || merge_in.presentId2;
}
#endif //(defined(VK_KHR_present_id2))
#if (defined(VK_KHR_present_wait2))
void compare_VkPhysicalDevicePresentWait2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentWait2FeaturesKHR const& supported, VkPhysicalDevicePresentWait2FeaturesKHR const& requested) {
    if (requested.presentWait2 && !supported.presentWait2) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentWait2FeaturesKHR::presentWait2");
    }
}
void merge_VkPhysicalDevicePresentWait2FeaturesKHR(VkPhysicalDevicePresentWait2FeaturesKHR & current, VkPhysicalDevicePresentWait2FeaturesKHR const& merge_in) {
    current.presentWait2 = current.presentWait2 || merge_in.presentWait2;
}
#endif //(defined(VK_KHR_present_wait2))
#if (defined(VK_KHR_ray_tracing_position_fetch))
void compare_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& supported, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& requested) {
    if (requested.rayTracingPositionFetch && !supported.rayTracingPositionFetch) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR::rayTracingPositionFetch");
    }
}
void merge_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR & current, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& merge_in) {
    current.rayTracingPositionFetch = current.rayTracingPositionFetch || merge_in.rayTracingPositionFetch;
}
#endif //(defined(VK_KHR_ray_tracing_position_fetch))
#if (defined(VK_KHR_pipeline_binary))
void compare_VkPhysicalDevicePipelineBinaryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePipelineBinaryFeaturesKHR const& supported, VkPhysicalDevicePipelineBinaryFeaturesKHR const& requested) {
    if (requested.pipelineBinaries && !supported.pipelineBinaries) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineBinaryFeaturesKHR::pipelineBinaries");
    }
}
void merge_VkPhysicalDevicePipelineBinaryFeaturesKHR(VkPhysicalDevicePipelineBinaryFeaturesKHR & current, VkPhysicalDevicePipelineBinaryFeaturesKHR const& merge_in) {
    current.pipelineBinaries = current.pipelineBinaries || merge_in.pipelineBinaries;
}
#endif //(defined(VK_KHR_pipeline_binary))
#if (defined(VK_KHR_swapchain_maintenance1))
void compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& requested) {
    if (requested.swapchainMaintenance1 && !supported.swapchainMaintenance1) {
        error_list.push_back("Missing feature VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR::swapchainMaintenance1");
    }
}
void merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR & current, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& merge_in) {
    current.swapchainMaintenance1 = current.swapchainMaintenance1 || merge_in.swapchainMaintenance1;
}
#endif //(defined(VK_KHR_swapchain_maintenance1))
#if (defined(VK_EXT_swapchain_maintenance1))
void compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& supported, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& requested) {
    if (requested.swapchainMaintenance1 && !supported.swapchainMaintenance1) {
        error_list.push_back("Missing feature VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT::swapchainMaintenance1");
    }
}
void merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT & current, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& merge_in) {
    current.swapchainMaintenance1 = current.swapchainMaintenance1 || merge_in.swapchainMaintenance1;
}
#endif //(defined(VK_EXT_swapchain_maintenance1))
#if (defined(VK_KHR_internally_synchronized_queues))
void compare_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& supported, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& requested) {
    if (requested.internallySynchronizedQueues && !supported.internallySynchronizedQueues) {
        error_list.push_back("Missing feature VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR::internallySynchronizedQueues");
    }
}
void merge_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR & current, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& merge_in) {
    current.internallySynchronizedQueues = current.internallySynchronizedQueues || merge_in.internallySynchronizedQueues;
}
#endif //(defined(VK_KHR_internally_synchronized_queues))
#if (defined(VK_KHR_cooperative_matrix))
void compare_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& supported, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& requested) {
    if (requested.cooperativeMatrix && !supported.cooperativeMatrix) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixFeaturesKHR::cooperativeMatrix");
    }
    if (requested.cooperativeMatrixRobustBufferAccess && !supported.cooperativeMatrixRobustBufferAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixFeaturesKHR::cooperativeMatrixRobustBufferAccess");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(VkPhysicalDeviceCooperativeMatrixFeaturesKHR & current, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& merge_in) {
    current.cooperativeMatrix = current.cooperativeMatrix || merge_in.cooperativeMatrix;
    current.cooperativeMatrixRobustBufferAccess = current.cooperativeMatrixRobustBufferAccess || merge_in.cooperativeMatrixRobustBufferAccess;
}
#endif //(defined(VK_KHR_cooperative_matrix))
#if (defined(VK_KHR_compute_shader_derivatives))
void compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& supported, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& requested) {
    if (requested.computeDerivativeGroupQuads && !supported.computeDerivativeGroupQuads) {
        error_list.push_back("Missing feature VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR::computeDerivativeGroupQuads");
    }
    if (requested.computeDerivativeGroupLinear && !supported.computeDerivativeGroupLinear) {
        error_list.push_back("Missing feature VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR::computeDerivativeGroupLinear");
    }
}
void merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR & current, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& merge_in) {
    current.computeDerivativeGroupQuads = current.computeDerivativeGroupQuads || merge_in.computeDerivativeGroupQuads;
    current.computeDerivativeGroupLinear = current.computeDerivativeGroupLinear || merge_in.computeDerivativeGroupLinear;
}
#endif //(defined(VK_KHR_compute_shader_derivatives))
#if (defined(VK_NV_compute_shader_derivatives))
void compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& supported, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& requested) {
    if (requested.computeDerivativeGroupQuads && !supported.computeDerivativeGroupQuads) {
        error_list.push_back("Missing feature VkPhysicalDeviceComputeShaderDerivativesFeaturesNV::computeDerivativeGroupQuads");
    }
    if (requested.computeDerivativeGroupLinear && !supported.computeDerivativeGroupLinear) {
        error_list.push_back("Missing feature VkPhysicalDeviceComputeShaderDerivativesFeaturesNV::computeDerivativeGroupLinear");
    }
}
void merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(VkPhysicalDeviceComputeShaderDerivativesFeaturesNV & current, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& merge_in) {
    current.computeDerivativeGroupQuads = current.computeDerivativeGroupQuads || merge_in.computeDerivativeGroupQuads;
    current.computeDerivativeGroupLinear = current.computeDerivativeGroupLinear || merge_in.computeDerivativeGroupLinear;
}
#endif //(defined(VK_NV_compute_shader_derivatives))
#if (defined(VK_KHR_video_encode_av1))
void compare_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& requested) {
    if (requested.videoEncodeAV1 && !supported.videoEncodeAV1) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoEncodeAV1FeaturesKHR::videoEncodeAV1");
    }
}
void merge_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(VkPhysicalDeviceVideoEncodeAV1FeaturesKHR & current, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& merge_in) {
    current.videoEncodeAV1 = current.videoEncodeAV1 || merge_in.videoEncodeAV1;
}
#endif //(defined(VK_KHR_video_encode_av1))
#if (defined(VK_KHR_video_decode_vp9))
void compare_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& supported, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& requested) {
    if (requested.videoDecodeVP9 && !supported.videoDecodeVP9) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoDecodeVP9FeaturesKHR::videoDecodeVP9");
    }
}
void merge_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(VkPhysicalDeviceVideoDecodeVP9FeaturesKHR & current, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& merge_in) {
    current.videoDecodeVP9 = current.videoDecodeVP9 || merge_in.videoDecodeVP9;
}
#endif //(defined(VK_KHR_video_decode_vp9))
#if (defined(VK_KHR_video_maintenance1))
void compare_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& requested) {
    if (requested.videoMaintenance1 && !supported.videoMaintenance1) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoMaintenance1FeaturesKHR::videoMaintenance1");
    }
}
void merge_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(VkPhysicalDeviceVideoMaintenance1FeaturesKHR & current, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& merge_in) {
    current.videoMaintenance1 = current.videoMaintenance1 || merge_in.videoMaintenance1;
}
#endif //(defined(VK_KHR_video_maintenance1))
#if (defined(VK_KHR_unified_image_layouts))
void compare_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& supported, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& requested) {
    if (requested.unifiedImageLayouts && !supported.unifiedImageLayouts) {
        error_list.push_back("Missing feature VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR::unifiedImageLayouts");
    }
    if (requested.unifiedImageLayoutsVideo && !supported.unifiedImageLayoutsVideo) {
        error_list.push_back("Missing feature VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR::unifiedImageLayoutsVideo");
    }
}
void merge_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR & current, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& merge_in) {
    current.unifiedImageLayouts = current.unifiedImageLayouts || merge_in.unifiedImageLayouts;
    current.unifiedImageLayoutsVideo = current.unifiedImageLayoutsVideo || merge_in.unifiedImageLayoutsVideo;
}
#endif //(defined(VK_KHR_unified_image_layouts))
#if (defined(VK_KHR_copy_memory_indirect))
void compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& supported, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& requested) {
    if (requested.indirectMemoryCopy && !supported.indirectMemoryCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR::indirectMemoryCopy");
    }
    if (requested.indirectMemoryToImageCopy && !supported.indirectMemoryToImageCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR::indirectMemoryToImageCopy");
    }
}
void merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR & current, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& merge_in) {
    current.indirectMemoryCopy = current.indirectMemoryCopy || merge_in.indirectMemoryCopy;
    current.indirectMemoryToImageCopy = current.indirectMemoryToImageCopy || merge_in.indirectMemoryToImageCopy;
}
#endif //(defined(VK_KHR_copy_memory_indirect))
#if (defined(VK_KHR_video_encode_intra_refresh))
void compare_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& requested) {
    if (requested.videoEncodeIntraRefresh && !supported.videoEncodeIntraRefresh) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR::videoEncodeIntraRefresh");
    }
}
void merge_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR & current, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& merge_in) {
    current.videoEncodeIntraRefresh = current.videoEncodeIntraRefresh || merge_in.videoEncodeIntraRefresh;
}
#endif //(defined(VK_KHR_video_encode_intra_refresh))
#if (defined(VK_KHR_video_encode_quantization_map))
void compare_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& requested) {
    if (requested.videoEncodeQuantizationMap && !supported.videoEncodeQuantizationMap) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR::videoEncodeQuantizationMap");
    }
}
void merge_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR & current, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& merge_in) {
    current.videoEncodeQuantizationMap = current.videoEncodeQuantizationMap || merge_in.videoEncodeQuantizationMap;
}
#endif //(defined(VK_KHR_video_encode_quantization_map))
#if (defined(VK_KHR_shader_relaxed_extended_instruction))
void compare_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& supported, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& requested) {
    if (requested.shaderRelaxedExtendedInstruction && !supported.shaderRelaxedExtendedInstruction) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR::shaderRelaxedExtendedInstruction");
    }
}
void merge_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR & current, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& merge_in) {
    current.shaderRelaxedExtendedInstruction = current.shaderRelaxedExtendedInstruction || merge_in.shaderRelaxedExtendedInstruction;
}
#endif //(defined(VK_KHR_shader_relaxed_extended_instruction))
#if (defined(VK_KHR_maintenance7))
void compare_VkPhysicalDeviceMaintenance7FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance7FeaturesKHR const& supported, VkPhysicalDeviceMaintenance7FeaturesKHR const& requested) {
    if (requested.maintenance7 && !supported.maintenance7) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance7FeaturesKHR::maintenance7");
    }
}
void merge_VkPhysicalDeviceMaintenance7FeaturesKHR(VkPhysicalDeviceMaintenance7FeaturesKHR & current, VkPhysicalDeviceMaintenance7FeaturesKHR const& merge_in) {
    current.maintenance7 = current.maintenance7 || merge_in.maintenance7;
}
#endif //(defined(VK_KHR_maintenance7))
#if (defined(VK_KHR_device_fault))
void compare_VkPhysicalDeviceFaultFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFaultFeaturesKHR const& supported, VkPhysicalDeviceFaultFeaturesKHR const& requested) {
    if (requested.deviceFault && !supported.deviceFault) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesKHR::deviceFault");
    }
    if (requested.deviceFaultVendorBinary && !supported.deviceFaultVendorBinary) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesKHR::deviceFaultVendorBinary");
    }
    if (requested.deviceFaultReportMasked && !supported.deviceFaultReportMasked) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesKHR::deviceFaultReportMasked");
    }
    if (requested.deviceFaultDeviceLostOnMasked && !supported.deviceFaultDeviceLostOnMasked) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesKHR::deviceFaultDeviceLostOnMasked");
    }
}
void merge_VkPhysicalDeviceFaultFeaturesKHR(VkPhysicalDeviceFaultFeaturesKHR & current, VkPhysicalDeviceFaultFeaturesKHR const& merge_in) {
    current.deviceFault = current.deviceFault || merge_in.deviceFault;
    current.deviceFaultVendorBinary = current.deviceFaultVendorBinary || merge_in.deviceFaultVendorBinary;
    current.deviceFaultReportMasked = current.deviceFaultReportMasked || merge_in.deviceFaultReportMasked;
    current.deviceFaultDeviceLostOnMasked = current.deviceFaultDeviceLostOnMasked || merge_in.deviceFaultDeviceLostOnMasked;
}
#endif //(defined(VK_KHR_device_fault))
#if (defined(VK_KHR_maintenance8))
void compare_VkPhysicalDeviceMaintenance8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance8FeaturesKHR const& supported, VkPhysicalDeviceMaintenance8FeaturesKHR const& requested) {
    if (requested.maintenance8 && !supported.maintenance8) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance8FeaturesKHR::maintenance8");
    }
}
void merge_VkPhysicalDeviceMaintenance8FeaturesKHR(VkPhysicalDeviceMaintenance8FeaturesKHR & current, VkPhysicalDeviceMaintenance8FeaturesKHR const& merge_in) {
    current.maintenance8 = current.maintenance8 || merge_in.maintenance8;
}
#endif //(defined(VK_KHR_maintenance8))
#if (defined(VK_KHR_shader_fma))
void compare_VkPhysicalDeviceShaderFmaFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFmaFeaturesKHR const& supported, VkPhysicalDeviceShaderFmaFeaturesKHR const& requested) {
    if (requested.shaderFmaFloat16 && !supported.shaderFmaFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFmaFeaturesKHR::shaderFmaFloat16");
    }
    if (requested.shaderFmaFloat32 && !supported.shaderFmaFloat32) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFmaFeaturesKHR::shaderFmaFloat32");
    }
    if (requested.shaderFmaFloat64 && !supported.shaderFmaFloat64) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFmaFeaturesKHR::shaderFmaFloat64");
    }
}
void merge_VkPhysicalDeviceShaderFmaFeaturesKHR(VkPhysicalDeviceShaderFmaFeaturesKHR & current, VkPhysicalDeviceShaderFmaFeaturesKHR const& merge_in) {
    current.shaderFmaFloat16 = current.shaderFmaFloat16 || merge_in.shaderFmaFloat16;
    current.shaderFmaFloat32 = current.shaderFmaFloat32 || merge_in.shaderFmaFloat32;
    current.shaderFmaFloat64 = current.shaderFmaFloat64 || merge_in.shaderFmaFloat64;
}
#endif //(defined(VK_KHR_shader_fma))
#if (defined(VK_KHR_maintenance9))
void compare_VkPhysicalDeviceMaintenance9FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance9FeaturesKHR const& supported, VkPhysicalDeviceMaintenance9FeaturesKHR const& requested) {
    if (requested.maintenance9 && !supported.maintenance9) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance9FeaturesKHR::maintenance9");
    }
}
void merge_VkPhysicalDeviceMaintenance9FeaturesKHR(VkPhysicalDeviceMaintenance9FeaturesKHR & current, VkPhysicalDeviceMaintenance9FeaturesKHR const& merge_in) {
    current.maintenance9 = current.maintenance9 || merge_in.maintenance9;
}
#endif //(defined(VK_KHR_maintenance9))
#if (defined(VK_KHR_video_maintenance2))
void compare_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& supported, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& requested) {
    if (requested.videoMaintenance2 && !supported.videoMaintenance2) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoMaintenance2FeaturesKHR::videoMaintenance2");
    }
}
void merge_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(VkPhysicalDeviceVideoMaintenance2FeaturesKHR & current, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& merge_in) {
    current.videoMaintenance2 = current.videoMaintenance2 || merge_in.videoMaintenance2;
}
#endif //(defined(VK_KHR_video_maintenance2))
#if (defined(VK_KHR_video_encode_feedback2))
void compare_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& requested) {
    if (requested.videoEncodeFeedback2 && !supported.videoEncodeFeedback2) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR::videoEncodeFeedback2");
    }
}
void merge_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR & current, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& merge_in) {
    current.videoEncodeFeedback2 = current.videoEncodeFeedback2 || merge_in.videoEncodeFeedback2;
}
#endif //(defined(VK_KHR_video_encode_feedback2))
#if (defined(VK_KHR_depth_clamp_zero_one))
void compare_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& supported, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& requested) {
    if (requested.depthClampZeroOne && !supported.depthClampZeroOne) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthClampZeroOneFeaturesKHR::depthClampZeroOne");
    }
}
void merge_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(VkPhysicalDeviceDepthClampZeroOneFeaturesKHR & current, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& merge_in) {
    current.depthClampZeroOne = current.depthClampZeroOne || merge_in.depthClampZeroOne;
}
#endif //(defined(VK_KHR_depth_clamp_zero_one))
#if (defined(VK_EXT_depth_clamp_zero_one))
void compare_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& supported, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& requested) {
    if (requested.depthClampZeroOne && !supported.depthClampZeroOne) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthClampZeroOneFeaturesEXT::depthClampZeroOne");
    }
}
void merge_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(VkPhysicalDeviceDepthClampZeroOneFeaturesEXT & current, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& merge_in) {
    current.depthClampZeroOne = current.depthClampZeroOne || merge_in.depthClampZeroOne;
}
#endif //(defined(VK_EXT_depth_clamp_zero_one))
#if (defined(VK_KHR_robustness2))
void compare_VkPhysicalDeviceRobustness2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRobustness2FeaturesKHR const& supported, VkPhysicalDeviceRobustness2FeaturesKHR const& requested) {
    if (requested.robustBufferAccess2 && !supported.robustBufferAccess2) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesKHR::robustBufferAccess2");
    }
    if (requested.robustImageAccess2 && !supported.robustImageAccess2) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesKHR::robustImageAccess2");
    }
    if (requested.nullDescriptor && !supported.nullDescriptor) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesKHR::nullDescriptor");
    }
}
void merge_VkPhysicalDeviceRobustness2FeaturesKHR(VkPhysicalDeviceRobustness2FeaturesKHR & current, VkPhysicalDeviceRobustness2FeaturesKHR const& merge_in) {
    current.robustBufferAccess2 = current.robustBufferAccess2 || merge_in.robustBufferAccess2;
    current.robustImageAccess2 = current.robustImageAccess2 || merge_in.robustImageAccess2;
    current.nullDescriptor = current.nullDescriptor || merge_in.nullDescriptor;
}
#endif //(defined(VK_KHR_robustness2))
#if (defined(VK_EXT_robustness2))
void compare_VkPhysicalDeviceRobustness2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRobustness2FeaturesEXT const& supported, VkPhysicalDeviceRobustness2FeaturesEXT const& requested) {
    if (requested.robustBufferAccess2 && !supported.robustBufferAccess2) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesEXT::robustBufferAccess2");
    }
    if (requested.robustImageAccess2 && !supported.robustImageAccess2) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesEXT::robustImageAccess2");
    }
    if (requested.nullDescriptor && !supported.nullDescriptor) {
        error_list.push_back("Missing feature VkPhysicalDeviceRobustness2FeaturesEXT::nullDescriptor");
    }
}
void merge_VkPhysicalDeviceRobustness2FeaturesEXT(VkPhysicalDeviceRobustness2FeaturesEXT & current, VkPhysicalDeviceRobustness2FeaturesEXT const& merge_in) {
    current.robustBufferAccess2 = current.robustBufferAccess2 || merge_in.robustBufferAccess2;
    current.robustImageAccess2 = current.robustImageAccess2 || merge_in.robustImageAccess2;
    current.nullDescriptor = current.nullDescriptor || merge_in.nullDescriptor;
}
#endif //(defined(VK_EXT_robustness2))
#if (defined(VK_KHR_present_mode_fifo_latest_ready))
void compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& supported, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& requested) {
    if (requested.presentModeFifoLatestReady && !supported.presentModeFifoLatestReady) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR::presentModeFifoLatestReady");
    }
}
void merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR & current, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& merge_in) {
    current.presentModeFifoLatestReady = current.presentModeFifoLatestReady || merge_in.presentModeFifoLatestReady;
}
#endif //(defined(VK_KHR_present_mode_fifo_latest_ready))
#if (defined(VK_EXT_present_mode_fifo_latest_ready))
void compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& supported, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& requested) {
    if (requested.presentModeFifoLatestReady && !supported.presentModeFifoLatestReady) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT::presentModeFifoLatestReady");
    }
}
void merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT & current, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& merge_in) {
    current.presentModeFifoLatestReady = current.presentModeFifoLatestReady || merge_in.presentModeFifoLatestReady;
}
#endif //(defined(VK_EXT_present_mode_fifo_latest_ready))
#if (defined(VK_KHR_opacity_micromap))
void compare_VkPhysicalDeviceOpacityMicromapFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& supported, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& requested) {
    if (requested.micromap && !supported.micromap) {
        error_list.push_back("Missing feature VkPhysicalDeviceOpacityMicromapFeaturesKHR::micromap");
    }
}
void merge_VkPhysicalDeviceOpacityMicromapFeaturesKHR(VkPhysicalDeviceOpacityMicromapFeaturesKHR & current, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& merge_in) {
    current.micromap = current.micromap || merge_in.micromap;
}
#endif //(defined(VK_KHR_opacity_micromap))
#if (defined(VK_KHR_maintenance10))
void compare_VkPhysicalDeviceMaintenance10FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance10FeaturesKHR const& supported, VkPhysicalDeviceMaintenance10FeaturesKHR const& requested) {
    if (requested.maintenance10 && !supported.maintenance10) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance10FeaturesKHR::maintenance10");
    }
}
void merge_VkPhysicalDeviceMaintenance10FeaturesKHR(VkPhysicalDeviceMaintenance10FeaturesKHR & current, VkPhysicalDeviceMaintenance10FeaturesKHR const& merge_in) {
    current.maintenance10 = current.maintenance10 || merge_in.maintenance10;
}
#endif //(defined(VK_KHR_maintenance10))
#if (defined(VK_KHR_maintenance11))
void compare_VkPhysicalDeviceMaintenance11FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance11FeaturesKHR const& supported, VkPhysicalDeviceMaintenance11FeaturesKHR const& requested) {
    if (requested.maintenance11 && !supported.maintenance11) {
        error_list.push_back("Missing feature VkPhysicalDeviceMaintenance11FeaturesKHR::maintenance11");
    }
}
void merge_VkPhysicalDeviceMaintenance11FeaturesKHR(VkPhysicalDeviceMaintenance11FeaturesKHR & current, VkPhysicalDeviceMaintenance11FeaturesKHR const& merge_in) {
    current.maintenance11 = current.maintenance11 || merge_in.maintenance11;
}
#endif //(defined(VK_KHR_maintenance11))
#if (defined(VK_KHR_extended_flags))
void compare_VkPhysicalDeviceExtendedFlagsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& supported, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& requested) {
    if (requested.extendedFlags && !supported.extendedFlags) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedFlagsFeaturesKHR::extendedFlags");
    }
}
void merge_VkPhysicalDeviceExtendedFlagsFeaturesKHR(VkPhysicalDeviceExtendedFlagsFeaturesKHR & current, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& merge_in) {
    current.extendedFlags = current.extendedFlags || merge_in.extendedFlags;
}
#endif //(defined(VK_KHR_extended_flags))
#if (defined(VK_EXT_transform_feedback))
void compare_VkPhysicalDeviceTransformFeedbackFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& supported, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& requested) {
    if (requested.transformFeedback && !supported.transformFeedback) {
        error_list.push_back("Missing feature VkPhysicalDeviceTransformFeedbackFeaturesEXT::transformFeedback");
    }
    if (requested.geometryStreams && !supported.geometryStreams) {
        error_list.push_back("Missing feature VkPhysicalDeviceTransformFeedbackFeaturesEXT::geometryStreams");
    }
}
void merge_VkPhysicalDeviceTransformFeedbackFeaturesEXT(VkPhysicalDeviceTransformFeedbackFeaturesEXT & current, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& merge_in) {
    current.transformFeedback = current.transformFeedback || merge_in.transformFeedback;
    current.geometryStreams = current.geometryStreams || merge_in.geometryStreams;
}
#endif //(defined(VK_EXT_transform_feedback))
#if (defined(VK_NV_corner_sampled_image))
void compare_VkPhysicalDeviceCornerSampledImageFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCornerSampledImageFeaturesNV const& supported, VkPhysicalDeviceCornerSampledImageFeaturesNV const& requested) {
    if (requested.cornerSampledImage && !supported.cornerSampledImage) {
        error_list.push_back("Missing feature VkPhysicalDeviceCornerSampledImageFeaturesNV::cornerSampledImage");
    }
}
void merge_VkPhysicalDeviceCornerSampledImageFeaturesNV(VkPhysicalDeviceCornerSampledImageFeaturesNV & current, VkPhysicalDeviceCornerSampledImageFeaturesNV const& merge_in) {
    current.cornerSampledImage = current.cornerSampledImage || merge_in.cornerSampledImage;
}
#endif //(defined(VK_NV_corner_sampled_image))
#if (defined(VK_EXT_astc_decode_mode))
void compare_VkPhysicalDeviceASTCDecodeFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceASTCDecodeFeaturesEXT const& supported, VkPhysicalDeviceASTCDecodeFeaturesEXT const& requested) {
    if (requested.decodeModeSharedExponent && !supported.decodeModeSharedExponent) {
        error_list.push_back("Missing feature VkPhysicalDeviceASTCDecodeFeaturesEXT::decodeModeSharedExponent");
    }
}
void merge_VkPhysicalDeviceASTCDecodeFeaturesEXT(VkPhysicalDeviceASTCDecodeFeaturesEXT & current, VkPhysicalDeviceASTCDecodeFeaturesEXT const& merge_in) {
    current.decodeModeSharedExponent = current.decodeModeSharedExponent || merge_in.decodeModeSharedExponent;
}
#endif //(defined(VK_EXT_astc_decode_mode))
#if (defined(VK_EXT_conditional_rendering))
void compare_VkPhysicalDeviceConditionalRenderingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& supported, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& requested) {
    if (requested.conditionalRendering && !supported.conditionalRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceConditionalRenderingFeaturesEXT::conditionalRendering");
    }
    if (requested.inheritedConditionalRendering && !supported.inheritedConditionalRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceConditionalRenderingFeaturesEXT::inheritedConditionalRendering");
    }
}
void merge_VkPhysicalDeviceConditionalRenderingFeaturesEXT(VkPhysicalDeviceConditionalRenderingFeaturesEXT & current, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& merge_in) {
    current.conditionalRendering = current.conditionalRendering || merge_in.conditionalRendering;
    current.inheritedConditionalRendering = current.inheritedConditionalRendering || merge_in.inheritedConditionalRendering;
}
#endif //(defined(VK_EXT_conditional_rendering))
#if (defined(VK_EXT_depth_clip_enable))
void compare_VkPhysicalDeviceDepthClipEnableFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& supported, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& requested) {
    if (requested.depthClipEnable && !supported.depthClipEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthClipEnableFeaturesEXT::depthClipEnable");
    }
}
void merge_VkPhysicalDeviceDepthClipEnableFeaturesEXT(VkPhysicalDeviceDepthClipEnableFeaturesEXT & current, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& merge_in) {
    current.depthClipEnable = current.depthClipEnable || merge_in.depthClipEnable;
}
#endif //(defined(VK_EXT_depth_clip_enable))
#if (defined(VK_IMG_relaxed_line_rasterization))
void compare_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(std::vector<std::string> & error_list, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& supported, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& requested) {
    if (requested.relaxedLineRasterization && !supported.relaxedLineRasterization) {
        error_list.push_back("Missing feature VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG::relaxedLineRasterization");
    }
}
void merge_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG & current, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& merge_in) {
    current.relaxedLineRasterization = current.relaxedLineRasterization || merge_in.relaxedLineRasterization;
}
#endif //(defined(VK_IMG_relaxed_line_rasterization))
#if (defined(VK_AMD_gpa_interface))
void compare_VkPhysicalDeviceGpaFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceGpaFeaturesAMD const& supported, VkPhysicalDeviceGpaFeaturesAMD const& requested) {
    if (requested.perfCounters && !supported.perfCounters) {
        error_list.push_back("Missing feature VkPhysicalDeviceGpaFeaturesAMD::perfCounters");
    }
    if (requested.streamingPerfCounters && !supported.streamingPerfCounters) {
        error_list.push_back("Missing feature VkPhysicalDeviceGpaFeaturesAMD::streamingPerfCounters");
    }
    if (requested.sqThreadTracing && !supported.sqThreadTracing) {
        error_list.push_back("Missing feature VkPhysicalDeviceGpaFeaturesAMD::sqThreadTracing");
    }
    if (requested.clockModes && !supported.clockModes) {
        error_list.push_back("Missing feature VkPhysicalDeviceGpaFeaturesAMD::clockModes");
    }
}
void merge_VkPhysicalDeviceGpaFeaturesAMD(VkPhysicalDeviceGpaFeaturesAMD & current, VkPhysicalDeviceGpaFeaturesAMD const& merge_in) {
    current.perfCounters = current.perfCounters || merge_in.perfCounters;
    current.streamingPerfCounters = current.streamingPerfCounters || merge_in.streamingPerfCounters;
    current.sqThreadTracing = current.sqThreadTracing || merge_in.sqThreadTracing;
    current.clockModes = current.clockModes || merge_in.clockModes;
}
#endif //(defined(VK_AMD_gpa_interface))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
void compare_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(std::vector<std::string> & error_list, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& supported, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& requested) {
    if (requested.shaderEnqueue && !supported.shaderEnqueue) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderEnqueueFeaturesAMDX::shaderEnqueue");
    }
    if (requested.shaderMeshEnqueue && !supported.shaderMeshEnqueue) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderEnqueueFeaturesAMDX::shaderMeshEnqueue");
    }
}
void merge_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(VkPhysicalDeviceShaderEnqueueFeaturesAMDX & current, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& merge_in) {
    current.shaderEnqueue = current.shaderEnqueue || merge_in.shaderEnqueue;
    current.shaderMeshEnqueue = current.shaderMeshEnqueue || merge_in.shaderMeshEnqueue;
}
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
#if (defined(VK_EXT_descriptor_heap))
void compare_VkPhysicalDeviceDescriptorHeapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& supported, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& requested) {
    if (requested.descriptorHeap && !supported.descriptorHeap) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorHeapFeaturesEXT::descriptorHeap");
    }
    if (requested.descriptorHeapCaptureReplay && !supported.descriptorHeapCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorHeapFeaturesEXT::descriptorHeapCaptureReplay");
    }
}
void merge_VkPhysicalDeviceDescriptorHeapFeaturesEXT(VkPhysicalDeviceDescriptorHeapFeaturesEXT & current, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& merge_in) {
    current.descriptorHeap = current.descriptorHeap || merge_in.descriptorHeap;
    current.descriptorHeapCaptureReplay = current.descriptorHeapCaptureReplay || merge_in.descriptorHeapCaptureReplay;
}
#endif //(defined(VK_EXT_descriptor_heap))
#if (defined(VK_EXT_blend_operation_advanced))
void compare_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& supported, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& requested) {
    if (requested.advancedBlendCoherentOperations && !supported.advancedBlendCoherentOperations) {
        error_list.push_back("Missing feature VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT::advancedBlendCoherentOperations");
    }
}
void merge_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT & current, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& merge_in) {
    current.advancedBlendCoherentOperations = current.advancedBlendCoherentOperations || merge_in.advancedBlendCoherentOperations;
}
#endif //(defined(VK_EXT_blend_operation_advanced))
#if (defined(VK_NV_shader_sm_builtins))
void compare_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& supported, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& requested) {
    if (requested.shaderSMBuiltins && !supported.shaderSMBuiltins) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSMBuiltinsFeaturesNV::shaderSMBuiltins");
    }
}
void merge_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(VkPhysicalDeviceShaderSMBuiltinsFeaturesNV & current, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& merge_in) {
    current.shaderSMBuiltins = current.shaderSMBuiltins || merge_in.shaderSMBuiltins;
}
#endif //(defined(VK_NV_shader_sm_builtins))
#if (defined(VK_NV_shading_rate_image))
void compare_VkPhysicalDeviceShadingRateImageFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShadingRateImageFeaturesNV const& supported, VkPhysicalDeviceShadingRateImageFeaturesNV const& requested) {
    if (requested.shadingRateImage && !supported.shadingRateImage) {
        error_list.push_back("Missing feature VkPhysicalDeviceShadingRateImageFeaturesNV::shadingRateImage");
    }
    if (requested.shadingRateCoarseSampleOrder && !supported.shadingRateCoarseSampleOrder) {
        error_list.push_back("Missing feature VkPhysicalDeviceShadingRateImageFeaturesNV::shadingRateCoarseSampleOrder");
    }
}
void merge_VkPhysicalDeviceShadingRateImageFeaturesNV(VkPhysicalDeviceShadingRateImageFeaturesNV & current, VkPhysicalDeviceShadingRateImageFeaturesNV const& merge_in) {
    current.shadingRateImage = current.shadingRateImage || merge_in.shadingRateImage;
    current.shadingRateCoarseSampleOrder = current.shadingRateCoarseSampleOrder || merge_in.shadingRateCoarseSampleOrder;
}
#endif //(defined(VK_NV_shading_rate_image))
#if (defined(VK_NV_representative_fragment_test))
void compare_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& supported, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& requested) {
    if (requested.representativeFragmentTest && !supported.representativeFragmentTest) {
        error_list.push_back("Missing feature VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV::representativeFragmentTest");
    }
}
void merge_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV & current, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& merge_in) {
    current.representativeFragmentTest = current.representativeFragmentTest || merge_in.representativeFragmentTest;
}
#endif //(defined(VK_NV_representative_fragment_test))
#if (defined(VK_QCOM_cooperative_matrix_conversion))
void compare_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& supported, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& requested) {
    if (requested.cooperativeMatrixConversion && !supported.cooperativeMatrixConversion) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM::cooperativeMatrixConversion");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM & current, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& merge_in) {
    current.cooperativeMatrixConversion = current.cooperativeMatrixConversion || merge_in.cooperativeMatrixConversion;
}
#endif //(defined(VK_QCOM_cooperative_matrix_conversion))
#if (defined(VK_QCOM_elapsed_timer_query))
void compare_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& supported, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& requested) {
    if (requested.elapsedTimerQuery && !supported.elapsedTimerQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM::elapsedTimerQuery");
    }
}
void merge_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM & current, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& merge_in) {
    current.elapsedTimerQuery = current.elapsedTimerQuery || merge_in.elapsedTimerQuery;
}
#endif //(defined(VK_QCOM_elapsed_timer_query))
#if (defined(VK_NV_mesh_shader))
void compare_VkPhysicalDeviceMeshShaderFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceMeshShaderFeaturesNV const& supported, VkPhysicalDeviceMeshShaderFeaturesNV const& requested) {
    if (requested.taskShader && !supported.taskShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesNV::taskShader");
    }
    if (requested.meshShader && !supported.meshShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesNV::meshShader");
    }
}
void merge_VkPhysicalDeviceMeshShaderFeaturesNV(VkPhysicalDeviceMeshShaderFeaturesNV & current, VkPhysicalDeviceMeshShaderFeaturesNV const& merge_in) {
    current.taskShader = current.taskShader || merge_in.taskShader;
    current.meshShader = current.meshShader || merge_in.meshShader;
}
#endif //(defined(VK_NV_mesh_shader))
#if (defined(VK_NV_shader_image_footprint))
void compare_VkPhysicalDeviceShaderImageFootprintFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& supported, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& requested) {
    if (requested.imageFootprint && !supported.imageFootprint) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderImageFootprintFeaturesNV::imageFootprint");
    }
}
void merge_VkPhysicalDeviceShaderImageFootprintFeaturesNV(VkPhysicalDeviceShaderImageFootprintFeaturesNV & current, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& merge_in) {
    current.imageFootprint = current.imageFootprint || merge_in.imageFootprint;
}
#endif //(defined(VK_NV_shader_image_footprint))
#if (defined(VK_NV_scissor_exclusive))
void compare_VkPhysicalDeviceExclusiveScissorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExclusiveScissorFeaturesNV const& supported, VkPhysicalDeviceExclusiveScissorFeaturesNV const& requested) {
    if (requested.exclusiveScissor && !supported.exclusiveScissor) {
        error_list.push_back("Missing feature VkPhysicalDeviceExclusiveScissorFeaturesNV::exclusiveScissor");
    }
}
void merge_VkPhysicalDeviceExclusiveScissorFeaturesNV(VkPhysicalDeviceExclusiveScissorFeaturesNV & current, VkPhysicalDeviceExclusiveScissorFeaturesNV const& merge_in) {
    current.exclusiveScissor = current.exclusiveScissor || merge_in.exclusiveScissor;
}
#endif //(defined(VK_NV_scissor_exclusive))
#if (defined(VK_EXT_present_timing))
void compare_VkPhysicalDevicePresentTimingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePresentTimingFeaturesEXT const& supported, VkPhysicalDevicePresentTimingFeaturesEXT const& requested) {
    if (requested.presentTiming && !supported.presentTiming) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentTimingFeaturesEXT::presentTiming");
    }
    if (requested.presentAtAbsoluteTime && !supported.presentAtAbsoluteTime) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentTimingFeaturesEXT::presentAtAbsoluteTime");
    }
    if (requested.presentAtRelativeTime && !supported.presentAtRelativeTime) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentTimingFeaturesEXT::presentAtRelativeTime");
    }
}
void merge_VkPhysicalDevicePresentTimingFeaturesEXT(VkPhysicalDevicePresentTimingFeaturesEXT & current, VkPhysicalDevicePresentTimingFeaturesEXT const& merge_in) {
    current.presentTiming = current.presentTiming || merge_in.presentTiming;
    current.presentAtAbsoluteTime = current.presentAtAbsoluteTime || merge_in.presentAtAbsoluteTime;
    current.presentAtRelativeTime = current.presentAtRelativeTime || merge_in.presentAtRelativeTime;
}
#endif //(defined(VK_EXT_present_timing))
#if (defined(VK_INTEL_shader_integer_functions2))
void compare_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& supported, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& requested) {
    if (requested.shaderIntegerFunctions2 && !supported.shaderIntegerFunctions2) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL::shaderIntegerFunctions2");
    }
}
void merge_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL & current, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& merge_in) {
    current.shaderIntegerFunctions2 = current.shaderIntegerFunctions2 || merge_in.shaderIntegerFunctions2;
}
#endif //(defined(VK_INTEL_shader_integer_functions2))
#if (defined(VK_EXT_fragment_density_map))
void compare_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& requested) {
    if (requested.fragmentDensityMap && !supported.fragmentDensityMap) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMap");
    }
    if (requested.fragmentDensityMapDynamic && !supported.fragmentDensityMapDynamic) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMapDynamic");
    }
    if (requested.fragmentDensityMapNonSubsampledImages && !supported.fragmentDensityMapNonSubsampledImages) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMapNonSubsampledImages");
    }
}
void merge_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(VkPhysicalDeviceFragmentDensityMapFeaturesEXT & current, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& merge_in) {
    current.fragmentDensityMap = current.fragmentDensityMap || merge_in.fragmentDensityMap;
    current.fragmentDensityMapDynamic = current.fragmentDensityMapDynamic || merge_in.fragmentDensityMapDynamic;
    current.fragmentDensityMapNonSubsampledImages = current.fragmentDensityMapNonSubsampledImages || merge_in.fragmentDensityMapNonSubsampledImages;
}
#endif //(defined(VK_EXT_fragment_density_map))
#if (defined(VK_AMD_device_coherent_memory))
void compare_VkPhysicalDeviceCoherentMemoryFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& supported, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& requested) {
    if (requested.deviceCoherentMemory && !supported.deviceCoherentMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceCoherentMemoryFeaturesAMD::deviceCoherentMemory");
    }
}
void merge_VkPhysicalDeviceCoherentMemoryFeaturesAMD(VkPhysicalDeviceCoherentMemoryFeaturesAMD & current, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& merge_in) {
    current.deviceCoherentMemory = current.deviceCoherentMemory || merge_in.deviceCoherentMemory;
}
#endif //(defined(VK_AMD_device_coherent_memory))
#if (defined(VK_EXT_shader_image_atomic_int64))
void compare_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& supported, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& requested) {
    if (requested.shaderImageInt64Atomics && !supported.shaderImageInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT::shaderImageInt64Atomics");
    }
    if (requested.sparseImageInt64Atomics && !supported.sparseImageInt64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT::sparseImageInt64Atomics");
    }
}
void merge_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT & current, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& merge_in) {
    current.shaderImageInt64Atomics = current.shaderImageInt64Atomics || merge_in.shaderImageInt64Atomics;
    current.sparseImageInt64Atomics = current.sparseImageInt64Atomics || merge_in.sparseImageInt64Atomics;
}
#endif //(defined(VK_EXT_shader_image_atomic_int64))
#if (defined(VK_EXT_memory_priority))
void compare_VkPhysicalDeviceMemoryPriorityFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& supported, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& requested) {
    if (requested.memoryPriority && !supported.memoryPriority) {
        error_list.push_back("Missing feature VkPhysicalDeviceMemoryPriorityFeaturesEXT::memoryPriority");
    }
}
void merge_VkPhysicalDeviceMemoryPriorityFeaturesEXT(VkPhysicalDeviceMemoryPriorityFeaturesEXT & current, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& merge_in) {
    current.memoryPriority = current.memoryPriority || merge_in.memoryPriority;
}
#endif //(defined(VK_EXT_memory_priority))
#if (defined(VK_NV_dedicated_allocation_image_aliasing))
void compare_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& supported, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& requested) {
    if (requested.dedicatedAllocationImageAliasing && !supported.dedicatedAllocationImageAliasing) {
        error_list.push_back("Missing feature VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV::dedicatedAllocationImageAliasing");
    }
}
void merge_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV & current, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& merge_in) {
    current.dedicatedAllocationImageAliasing = current.dedicatedAllocationImageAliasing || merge_in.dedicatedAllocationImageAliasing;
}
#endif //(defined(VK_NV_dedicated_allocation_image_aliasing))
#if (defined(VK_EXT_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& supported, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& requested) {
    if (requested.bufferDeviceAddress && !supported.bufferDeviceAddress) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesEXT::bufferDeviceAddress");
    }
    if (requested.bufferDeviceAddressCaptureReplay && !supported.bufferDeviceAddressCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesEXT::bufferDeviceAddressCaptureReplay");
    }
    if (requested.bufferDeviceAddressMultiDevice && !supported.bufferDeviceAddressMultiDevice) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferDeviceAddressFeaturesEXT::bufferDeviceAddressMultiDevice");
    }
}
void merge_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(VkPhysicalDeviceBufferDeviceAddressFeaturesEXT & current, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& merge_in) {
    current.bufferDeviceAddress = current.bufferDeviceAddress || merge_in.bufferDeviceAddress;
    current.bufferDeviceAddressCaptureReplay = current.bufferDeviceAddressCaptureReplay || merge_in.bufferDeviceAddressCaptureReplay;
    current.bufferDeviceAddressMultiDevice = current.bufferDeviceAddressMultiDevice || merge_in.bufferDeviceAddressMultiDevice;
}
#endif //(defined(VK_EXT_buffer_device_address))
#if (defined(VK_EXT_buffer_device_address))
void compare_VkPhysicalDeviceBufferAddressFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBufferAddressFeaturesEXT const& supported, VkPhysicalDeviceBufferAddressFeaturesEXT const& requested) {
    if (requested.bufferDeviceAddress && !supported.bufferDeviceAddress) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferAddressFeaturesEXT::bufferDeviceAddress");
    }
    if (requested.bufferDeviceAddressCaptureReplay && !supported.bufferDeviceAddressCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferAddressFeaturesEXT::bufferDeviceAddressCaptureReplay");
    }
    if (requested.bufferDeviceAddressMultiDevice && !supported.bufferDeviceAddressMultiDevice) {
        error_list.push_back("Missing feature VkPhysicalDeviceBufferAddressFeaturesEXT::bufferDeviceAddressMultiDevice");
    }
}
void merge_VkPhysicalDeviceBufferAddressFeaturesEXT(VkPhysicalDeviceBufferAddressFeaturesEXT & current, VkPhysicalDeviceBufferAddressFeaturesEXT const& merge_in) {
    current.bufferDeviceAddress = current.bufferDeviceAddress || merge_in.bufferDeviceAddress;
    current.bufferDeviceAddressCaptureReplay = current.bufferDeviceAddressCaptureReplay || merge_in.bufferDeviceAddressCaptureReplay;
    current.bufferDeviceAddressMultiDevice = current.bufferDeviceAddressMultiDevice || merge_in.bufferDeviceAddressMultiDevice;
}
#endif //(defined(VK_EXT_buffer_device_address))
#if (defined(VK_NV_cooperative_matrix))
void compare_VkPhysicalDeviceCooperativeMatrixFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& requested) {
    if (requested.cooperativeMatrix && !supported.cooperativeMatrix) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixFeaturesNV::cooperativeMatrix");
    }
    if (requested.cooperativeMatrixRobustBufferAccess && !supported.cooperativeMatrixRobustBufferAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixFeaturesNV::cooperativeMatrixRobustBufferAccess");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrixFeaturesNV(VkPhysicalDeviceCooperativeMatrixFeaturesNV & current, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& merge_in) {
    current.cooperativeMatrix = current.cooperativeMatrix || merge_in.cooperativeMatrix;
    current.cooperativeMatrixRobustBufferAccess = current.cooperativeMatrixRobustBufferAccess || merge_in.cooperativeMatrixRobustBufferAccess;
}
#endif //(defined(VK_NV_cooperative_matrix))
#if (defined(VK_NV_coverage_reduction_mode))
void compare_VkPhysicalDeviceCoverageReductionModeFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& supported, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& requested) {
    if (requested.coverageReductionMode && !supported.coverageReductionMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceCoverageReductionModeFeaturesNV::coverageReductionMode");
    }
}
void merge_VkPhysicalDeviceCoverageReductionModeFeaturesNV(VkPhysicalDeviceCoverageReductionModeFeaturesNV & current, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& merge_in) {
    current.coverageReductionMode = current.coverageReductionMode || merge_in.coverageReductionMode;
}
#endif //(defined(VK_NV_coverage_reduction_mode))
#if (defined(VK_EXT_fragment_shader_interlock))
void compare_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& supported, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& requested) {
    if (requested.fragmentShaderSampleInterlock && !supported.fragmentShaderSampleInterlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderSampleInterlock");
    }
    if (requested.fragmentShaderPixelInterlock && !supported.fragmentShaderPixelInterlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderPixelInterlock");
    }
    if (requested.fragmentShaderShadingRateInterlock && !supported.fragmentShaderShadingRateInterlock) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderShadingRateInterlock");
    }
}
void merge_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT & current, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& merge_in) {
    current.fragmentShaderSampleInterlock = current.fragmentShaderSampleInterlock || merge_in.fragmentShaderSampleInterlock;
    current.fragmentShaderPixelInterlock = current.fragmentShaderPixelInterlock || merge_in.fragmentShaderPixelInterlock;
    current.fragmentShaderShadingRateInterlock = current.fragmentShaderShadingRateInterlock || merge_in.fragmentShaderShadingRateInterlock;
}
#endif //(defined(VK_EXT_fragment_shader_interlock))
#if (defined(VK_EXT_ycbcr_image_arrays))
void compare_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& supported, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& requested) {
    if (requested.ycbcrImageArrays && !supported.ycbcrImageArrays) {
        error_list.push_back("Missing feature VkPhysicalDeviceYcbcrImageArraysFeaturesEXT::ycbcrImageArrays");
    }
}
void merge_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT & current, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& merge_in) {
    current.ycbcrImageArrays = current.ycbcrImageArrays || merge_in.ycbcrImageArrays;
}
#endif //(defined(VK_EXT_ycbcr_image_arrays))
#if (defined(VK_EXT_provoking_vertex))
void compare_VkPhysicalDeviceProvokingVertexFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceProvokingVertexFeaturesEXT const& supported, VkPhysicalDeviceProvokingVertexFeaturesEXT const& requested) {
    if (requested.provokingVertexLast && !supported.provokingVertexLast) {
        error_list.push_back("Missing feature VkPhysicalDeviceProvokingVertexFeaturesEXT::provokingVertexLast");
    }
    if (requested.transformFeedbackPreservesProvokingVertex && !supported.transformFeedbackPreservesProvokingVertex) {
        error_list.push_back("Missing feature VkPhysicalDeviceProvokingVertexFeaturesEXT::transformFeedbackPreservesProvokingVertex");
    }
}
void merge_VkPhysicalDeviceProvokingVertexFeaturesEXT(VkPhysicalDeviceProvokingVertexFeaturesEXT & current, VkPhysicalDeviceProvokingVertexFeaturesEXT const& merge_in) {
    current.provokingVertexLast = current.provokingVertexLast || merge_in.provokingVertexLast;
    current.transformFeedbackPreservesProvokingVertex = current.transformFeedbackPreservesProvokingVertex || merge_in.transformFeedbackPreservesProvokingVertex;
}
#endif //(defined(VK_EXT_provoking_vertex))
#if (defined(VK_EXT_shader_atomic_float))
void compare_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& supported, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& requested) {
    if (requested.shaderBufferFloat32Atomics && !supported.shaderBufferFloat32Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat32Atomics");
    }
    if (requested.shaderBufferFloat32AtomicAdd && !supported.shaderBufferFloat32AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat32AtomicAdd");
    }
    if (requested.shaderBufferFloat64Atomics && !supported.shaderBufferFloat64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat64Atomics");
    }
    if (requested.shaderBufferFloat64AtomicAdd && !supported.shaderBufferFloat64AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat64AtomicAdd");
    }
    if (requested.shaderSharedFloat32Atomics && !supported.shaderSharedFloat32Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat32Atomics");
    }
    if (requested.shaderSharedFloat32AtomicAdd && !supported.shaderSharedFloat32AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat32AtomicAdd");
    }
    if (requested.shaderSharedFloat64Atomics && !supported.shaderSharedFloat64Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat64Atomics");
    }
    if (requested.shaderSharedFloat64AtomicAdd && !supported.shaderSharedFloat64AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat64AtomicAdd");
    }
    if (requested.shaderImageFloat32Atomics && !supported.shaderImageFloat32Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderImageFloat32Atomics");
    }
    if (requested.shaderImageFloat32AtomicAdd && !supported.shaderImageFloat32AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderImageFloat32AtomicAdd");
    }
    if (requested.sparseImageFloat32Atomics && !supported.sparseImageFloat32Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::sparseImageFloat32Atomics");
    }
    if (requested.sparseImageFloat32AtomicAdd && !supported.sparseImageFloat32AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::sparseImageFloat32AtomicAdd");
    }
}
void merge_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT & current, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& merge_in) {
    current.shaderBufferFloat32Atomics = current.shaderBufferFloat32Atomics || merge_in.shaderBufferFloat32Atomics;
    current.shaderBufferFloat32AtomicAdd = current.shaderBufferFloat32AtomicAdd || merge_in.shaderBufferFloat32AtomicAdd;
    current.shaderBufferFloat64Atomics = current.shaderBufferFloat64Atomics || merge_in.shaderBufferFloat64Atomics;
    current.shaderBufferFloat64AtomicAdd = current.shaderBufferFloat64AtomicAdd || merge_in.shaderBufferFloat64AtomicAdd;
    current.shaderSharedFloat32Atomics = current.shaderSharedFloat32Atomics || merge_in.shaderSharedFloat32Atomics;
    current.shaderSharedFloat32AtomicAdd = current.shaderSharedFloat32AtomicAdd || merge_in.shaderSharedFloat32AtomicAdd;
    current.shaderSharedFloat64Atomics = current.shaderSharedFloat64Atomics || merge_in.shaderSharedFloat64Atomics;
    current.shaderSharedFloat64AtomicAdd = current.shaderSharedFloat64AtomicAdd || merge_in.shaderSharedFloat64AtomicAdd;
    current.shaderImageFloat32Atomics = current.shaderImageFloat32Atomics || merge_in.shaderImageFloat32Atomics;
    current.shaderImageFloat32AtomicAdd = current.shaderImageFloat32AtomicAdd || merge_in.shaderImageFloat32AtomicAdd;
    current.sparseImageFloat32Atomics = current.sparseImageFloat32Atomics || merge_in.sparseImageFloat32Atomics;
    current.sparseImageFloat32AtomicAdd = current.sparseImageFloat32AtomicAdd || merge_in.sparseImageFloat32AtomicAdd;
}
#endif //(defined(VK_EXT_shader_atomic_float))
#if (defined(VK_EXT_extended_dynamic_state))
void compare_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& requested) {
    if (requested.extendedDynamicState && !supported.extendedDynamicState) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicStateFeaturesEXT::extendedDynamicState");
    }
}
void merge_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT & current, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& merge_in) {
    current.extendedDynamicState = current.extendedDynamicState || merge_in.extendedDynamicState;
}
#endif //(defined(VK_EXT_extended_dynamic_state))
#if (defined(VK_EXT_map_memory_placed))
void compare_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& supported, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& requested) {
    if (requested.memoryMapPlaced && !supported.memoryMapPlaced) {
        error_list.push_back("Missing feature VkPhysicalDeviceMapMemoryPlacedFeaturesEXT::memoryMapPlaced");
    }
    if (requested.memoryMapRangePlaced && !supported.memoryMapRangePlaced) {
        error_list.push_back("Missing feature VkPhysicalDeviceMapMemoryPlacedFeaturesEXT::memoryMapRangePlaced");
    }
    if (requested.memoryUnmapReserve && !supported.memoryUnmapReserve) {
        error_list.push_back("Missing feature VkPhysicalDeviceMapMemoryPlacedFeaturesEXT::memoryUnmapReserve");
    }
}
void merge_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(VkPhysicalDeviceMapMemoryPlacedFeaturesEXT & current, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& merge_in) {
    current.memoryMapPlaced = current.memoryMapPlaced || merge_in.memoryMapPlaced;
    current.memoryMapRangePlaced = current.memoryMapRangePlaced || merge_in.memoryMapRangePlaced;
    current.memoryUnmapReserve = current.memoryUnmapReserve || merge_in.memoryUnmapReserve;
}
#endif //(defined(VK_EXT_map_memory_placed))
#if (defined(VK_EXT_shader_atomic_float2))
void compare_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& supported, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& requested) {
    if (requested.shaderBufferFloat16Atomics && !supported.shaderBufferFloat16Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16Atomics");
    }
    if (requested.shaderBufferFloat16AtomicAdd && !supported.shaderBufferFloat16AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16AtomicAdd");
    }
    if (requested.shaderBufferFloat16AtomicMinMax && !supported.shaderBufferFloat16AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16AtomicMinMax");
    }
    if (requested.shaderBufferFloat32AtomicMinMax && !supported.shaderBufferFloat32AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat32AtomicMinMax");
    }
    if (requested.shaderBufferFloat64AtomicMinMax && !supported.shaderBufferFloat64AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat64AtomicMinMax");
    }
    if (requested.shaderSharedFloat16Atomics && !supported.shaderSharedFloat16Atomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16Atomics");
    }
    if (requested.shaderSharedFloat16AtomicAdd && !supported.shaderSharedFloat16AtomicAdd) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16AtomicAdd");
    }
    if (requested.shaderSharedFloat16AtomicMinMax && !supported.shaderSharedFloat16AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16AtomicMinMax");
    }
    if (requested.shaderSharedFloat32AtomicMinMax && !supported.shaderSharedFloat32AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat32AtomicMinMax");
    }
    if (requested.shaderSharedFloat64AtomicMinMax && !supported.shaderSharedFloat64AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat64AtomicMinMax");
    }
    if (requested.shaderImageFloat32AtomicMinMax && !supported.shaderImageFloat32AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderImageFloat32AtomicMinMax");
    }
    if (requested.sparseImageFloat32AtomicMinMax && !supported.sparseImageFloat32AtomicMinMax) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::sparseImageFloat32AtomicMinMax");
    }
}
void merge_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT & current, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& merge_in) {
    current.shaderBufferFloat16Atomics = current.shaderBufferFloat16Atomics || merge_in.shaderBufferFloat16Atomics;
    current.shaderBufferFloat16AtomicAdd = current.shaderBufferFloat16AtomicAdd || merge_in.shaderBufferFloat16AtomicAdd;
    current.shaderBufferFloat16AtomicMinMax = current.shaderBufferFloat16AtomicMinMax || merge_in.shaderBufferFloat16AtomicMinMax;
    current.shaderBufferFloat32AtomicMinMax = current.shaderBufferFloat32AtomicMinMax || merge_in.shaderBufferFloat32AtomicMinMax;
    current.shaderBufferFloat64AtomicMinMax = current.shaderBufferFloat64AtomicMinMax || merge_in.shaderBufferFloat64AtomicMinMax;
    current.shaderSharedFloat16Atomics = current.shaderSharedFloat16Atomics || merge_in.shaderSharedFloat16Atomics;
    current.shaderSharedFloat16AtomicAdd = current.shaderSharedFloat16AtomicAdd || merge_in.shaderSharedFloat16AtomicAdd;
    current.shaderSharedFloat16AtomicMinMax = current.shaderSharedFloat16AtomicMinMax || merge_in.shaderSharedFloat16AtomicMinMax;
    current.shaderSharedFloat32AtomicMinMax = current.shaderSharedFloat32AtomicMinMax || merge_in.shaderSharedFloat32AtomicMinMax;
    current.shaderSharedFloat64AtomicMinMax = current.shaderSharedFloat64AtomicMinMax || merge_in.shaderSharedFloat64AtomicMinMax;
    current.shaderImageFloat32AtomicMinMax = current.shaderImageFloat32AtomicMinMax || merge_in.shaderImageFloat32AtomicMinMax;
    current.sparseImageFloat32AtomicMinMax = current.sparseImageFloat32AtomicMinMax || merge_in.sparseImageFloat32AtomicMinMax;
}
#endif //(defined(VK_EXT_shader_atomic_float2))
#if (defined(VK_NV_device_generated_commands))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& supported, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& requested) {
    if (requested.deviceGeneratedCommands && !supported.deviceGeneratedCommands) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV::deviceGeneratedCommands");
    }
}
void merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV & current, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& merge_in) {
    current.deviceGeneratedCommands = current.deviceGeneratedCommands || merge_in.deviceGeneratedCommands;
}
#endif //(defined(VK_NV_device_generated_commands))
#if (defined(VK_NV_inherited_viewport_scissor))
void compare_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& supported, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& requested) {
    if (requested.inheritedViewportScissor2D && !supported.inheritedViewportScissor2D) {
        error_list.push_back("Missing feature VkPhysicalDeviceInheritedViewportScissorFeaturesNV::inheritedViewportScissor2D");
    }
}
void merge_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(VkPhysicalDeviceInheritedViewportScissorFeaturesNV & current, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& merge_in) {
    current.inheritedViewportScissor2D = current.inheritedViewportScissor2D || merge_in.inheritedViewportScissor2D;
}
#endif //(defined(VK_NV_inherited_viewport_scissor))
#if (defined(VK_EXT_texel_buffer_alignment))
void compare_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& supported, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& requested) {
    if (requested.texelBufferAlignment && !supported.texelBufferAlignment) {
        error_list.push_back("Missing feature VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT::texelBufferAlignment");
    }
}
void merge_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT & current, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& merge_in) {
    current.texelBufferAlignment = current.texelBufferAlignment || merge_in.texelBufferAlignment;
}
#endif //(defined(VK_EXT_texel_buffer_alignment))
#if (defined(VK_EXT_depth_bias_control))
void compare_VkPhysicalDeviceDepthBiasControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& supported, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& requested) {
    if (requested.depthBiasControl && !supported.depthBiasControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthBiasControlFeaturesEXT::depthBiasControl");
    }
    if (requested.leastRepresentableValueForceUnormRepresentation && !supported.leastRepresentableValueForceUnormRepresentation) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthBiasControlFeaturesEXT::leastRepresentableValueForceUnormRepresentation");
    }
    if (requested.floatRepresentation && !supported.floatRepresentation) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthBiasControlFeaturesEXT::floatRepresentation");
    }
    if (requested.depthBiasExact && !supported.depthBiasExact) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthBiasControlFeaturesEXT::depthBiasExact");
    }
}
void merge_VkPhysicalDeviceDepthBiasControlFeaturesEXT(VkPhysicalDeviceDepthBiasControlFeaturesEXT & current, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& merge_in) {
    current.depthBiasControl = current.depthBiasControl || merge_in.depthBiasControl;
    current.leastRepresentableValueForceUnormRepresentation = current.leastRepresentableValueForceUnormRepresentation || merge_in.leastRepresentableValueForceUnormRepresentation;
    current.floatRepresentation = current.floatRepresentation || merge_in.floatRepresentation;
    current.depthBiasExact = current.depthBiasExact || merge_in.depthBiasExact;
}
#endif //(defined(VK_EXT_depth_bias_control))
#if (defined(VK_EXT_device_memory_report))
void compare_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& supported, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& requested) {
    if (requested.deviceMemoryReport && !supported.deviceMemoryReport) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceMemoryReportFeaturesEXT::deviceMemoryReport");
    }
}
void merge_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(VkPhysicalDeviceDeviceMemoryReportFeaturesEXT & current, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& merge_in) {
    current.deviceMemoryReport = current.deviceMemoryReport || merge_in.deviceMemoryReport;
}
#endif //(defined(VK_EXT_device_memory_report))
#if (defined(VK_EXT_custom_border_color))
void compare_VkPhysicalDeviceCustomBorderColorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& supported, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& requested) {
    if (requested.customBorderColors && !supported.customBorderColors) {
        error_list.push_back("Missing feature VkPhysicalDeviceCustomBorderColorFeaturesEXT::customBorderColors");
    }
    if (requested.customBorderColorWithoutFormat && !supported.customBorderColorWithoutFormat) {
        error_list.push_back("Missing feature VkPhysicalDeviceCustomBorderColorFeaturesEXT::customBorderColorWithoutFormat");
    }
}
void merge_VkPhysicalDeviceCustomBorderColorFeaturesEXT(VkPhysicalDeviceCustomBorderColorFeaturesEXT & current, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& merge_in) {
    current.customBorderColors = current.customBorderColors || merge_in.customBorderColors;
    current.customBorderColorWithoutFormat = current.customBorderColorWithoutFormat || merge_in.customBorderColorWithoutFormat;
}
#endif //(defined(VK_EXT_custom_border_color))
#if (defined(VK_EXT_texture_compression_astc_3d))
void compare_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& supported, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& requested) {
    if (requested.textureCompressionASTC_3D && !supported.textureCompressionASTC_3D) {
        error_list.push_back("Missing feature VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT::textureCompressionASTC_3D");
    }
}
void merge_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT & current, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& merge_in) {
    current.textureCompressionASTC_3D = current.textureCompressionASTC_3D || merge_in.textureCompressionASTC_3D;
}
#endif //(defined(VK_EXT_texture_compression_astc_3d))
#if (defined(VK_NV_present_barrier))
void compare_VkPhysicalDevicePresentBarrierFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePresentBarrierFeaturesNV const& supported, VkPhysicalDevicePresentBarrierFeaturesNV const& requested) {
    if (requested.presentBarrier && !supported.presentBarrier) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentBarrierFeaturesNV::presentBarrier");
    }
}
void merge_VkPhysicalDevicePresentBarrierFeaturesNV(VkPhysicalDevicePresentBarrierFeaturesNV & current, VkPhysicalDevicePresentBarrierFeaturesNV const& merge_in) {
    current.presentBarrier = current.presentBarrier || merge_in.presentBarrier;
}
#endif //(defined(VK_NV_present_barrier))
#if (defined(VK_NV_device_diagnostics_config))
void compare_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& supported, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& requested) {
    if (requested.diagnosticsConfig && !supported.diagnosticsConfig) {
        error_list.push_back("Missing feature VkPhysicalDeviceDiagnosticsConfigFeaturesNV::diagnosticsConfig");
    }
}
void merge_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(VkPhysicalDeviceDiagnosticsConfigFeaturesNV & current, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& merge_in) {
    current.diagnosticsConfig = current.diagnosticsConfig || merge_in.diagnosticsConfig;
}
#endif //(defined(VK_NV_device_diagnostics_config))
#if (defined(VK_QCOM_queue_perf_hint))
void compare_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& supported, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& requested) {
    if (requested.queuePerfHint && !supported.queuePerfHint) {
        error_list.push_back("Missing feature VkPhysicalDeviceQueuePerfHintFeaturesQCOM::queuePerfHint");
    }
}
void merge_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(VkPhysicalDeviceQueuePerfHintFeaturesQCOM & current, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& merge_in) {
    current.queuePerfHint = current.queuePerfHint || merge_in.queuePerfHint;
}
#endif //(defined(VK_QCOM_queue_perf_hint))
#if (defined(VK_QCOM_image_processing3))
void compare_VkPhysicalDeviceImageProcessing3FeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& supported, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& requested) {
    if (requested.imageGatherLinear && !supported.imageGatherLinear) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessing3FeaturesQCOM::imageGatherLinear");
    }
    if (requested.imageGatherExtendedModes && !supported.imageGatherExtendedModes) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessing3FeaturesQCOM::imageGatherExtendedModes");
    }
    if (requested.blockMatchExtendedClampToEdge && !supported.blockMatchExtendedClampToEdge) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessing3FeaturesQCOM::blockMatchExtendedClampToEdge");
    }
}
void merge_VkPhysicalDeviceImageProcessing3FeaturesQCOM(VkPhysicalDeviceImageProcessing3FeaturesQCOM & current, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& merge_in) {
    current.imageGatherLinear = current.imageGatherLinear || merge_in.imageGatherLinear;
    current.imageGatherExtendedModes = current.imageGatherExtendedModes || merge_in.imageGatherExtendedModes;
    current.blockMatchExtendedClampToEdge = current.blockMatchExtendedClampToEdge || merge_in.blockMatchExtendedClampToEdge;
}
#endif //(defined(VK_QCOM_image_processing3))
#if (defined(VK_QCOM_shader_multiple_wait_queues))
void compare_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& supported, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& requested) {
    if (requested.shaderMultipleWaitQueues && !supported.shaderMultipleWaitQueues) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM::shaderMultipleWaitQueues");
    }
}
void merge_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM & current, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& merge_in) {
    current.shaderMultipleWaitQueues = current.shaderMultipleWaitQueues || merge_in.shaderMultipleWaitQueues;
}
#endif //(defined(VK_QCOM_shader_multiple_wait_queues))
#if (defined(VK_EXT_shader_split_barrier))
void compare_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& supported, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& requested) {
    if (requested.shaderSplitBarrier && !supported.shaderSplitBarrier) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSplitBarrierFeaturesEXT::shaderSplitBarrier");
    }
}
void merge_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(VkPhysicalDeviceShaderSplitBarrierFeaturesEXT & current, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& merge_in) {
    current.shaderSplitBarrier = current.shaderSplitBarrier || merge_in.shaderSplitBarrier;
}
#endif //(defined(VK_EXT_shader_split_barrier))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
void compare_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& supported, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& requested) {
    if (requested.cudaKernelLaunchFeatures && !supported.cudaKernelLaunchFeatures) {
        error_list.push_back("Missing feature VkPhysicalDeviceCudaKernelLaunchFeaturesNV::cudaKernelLaunchFeatures");
    }
}
void merge_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(VkPhysicalDeviceCudaKernelLaunchFeaturesNV & current, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& merge_in) {
    current.cudaKernelLaunchFeatures = current.cudaKernelLaunchFeatures || merge_in.cudaKernelLaunchFeatures;
}
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
#if (defined(VK_QCOM_tile_shading))
void compare_VkPhysicalDeviceTileShadingFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTileShadingFeaturesQCOM const& supported, VkPhysicalDeviceTileShadingFeaturesQCOM const& requested) {
    if (requested.tileShading && !supported.tileShading) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShading");
    }
    if (requested.tileShadingFragmentStage && !supported.tileShadingFragmentStage) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingFragmentStage");
    }
    if (requested.tileShadingColorAttachments && !supported.tileShadingColorAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingColorAttachments");
    }
    if (requested.tileShadingDepthAttachments && !supported.tileShadingDepthAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingDepthAttachments");
    }
    if (requested.tileShadingStencilAttachments && !supported.tileShadingStencilAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingStencilAttachments");
    }
    if (requested.tileShadingInputAttachments && !supported.tileShadingInputAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingInputAttachments");
    }
    if (requested.tileShadingSampledAttachments && !supported.tileShadingSampledAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingSampledAttachments");
    }
    if (requested.tileShadingPerTileDraw && !supported.tileShadingPerTileDraw) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingPerTileDraw");
    }
    if (requested.tileShadingPerTileDispatch && !supported.tileShadingPerTileDispatch) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingPerTileDispatch");
    }
    if (requested.tileShadingDispatchTile && !supported.tileShadingDispatchTile) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingDispatchTile");
    }
    if (requested.tileShadingApron && !supported.tileShadingApron) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingApron");
    }
    if (requested.tileShadingAnisotropicApron && !supported.tileShadingAnisotropicApron) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingAnisotropicApron");
    }
    if (requested.tileShadingAtomicOps && !supported.tileShadingAtomicOps) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingAtomicOps");
    }
    if (requested.tileShadingImageProcessing && !supported.tileShadingImageProcessing) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileShadingFeaturesQCOM::tileShadingImageProcessing");
    }
}
void merge_VkPhysicalDeviceTileShadingFeaturesQCOM(VkPhysicalDeviceTileShadingFeaturesQCOM & current, VkPhysicalDeviceTileShadingFeaturesQCOM const& merge_in) {
    current.tileShading = current.tileShading || merge_in.tileShading;
    current.tileShadingFragmentStage = current.tileShadingFragmentStage || merge_in.tileShadingFragmentStage;
    current.tileShadingColorAttachments = current.tileShadingColorAttachments || merge_in.tileShadingColorAttachments;
    current.tileShadingDepthAttachments = current.tileShadingDepthAttachments || merge_in.tileShadingDepthAttachments;
    current.tileShadingStencilAttachments = current.tileShadingStencilAttachments || merge_in.tileShadingStencilAttachments;
    current.tileShadingInputAttachments = current.tileShadingInputAttachments || merge_in.tileShadingInputAttachments;
    current.tileShadingSampledAttachments = current.tileShadingSampledAttachments || merge_in.tileShadingSampledAttachments;
    current.tileShadingPerTileDraw = current.tileShadingPerTileDraw || merge_in.tileShadingPerTileDraw;
    current.tileShadingPerTileDispatch = current.tileShadingPerTileDispatch || merge_in.tileShadingPerTileDispatch;
    current.tileShadingDispatchTile = current.tileShadingDispatchTile || merge_in.tileShadingDispatchTile;
    current.tileShadingApron = current.tileShadingApron || merge_in.tileShadingApron;
    current.tileShadingAnisotropicApron = current.tileShadingAnisotropicApron || merge_in.tileShadingAnisotropicApron;
    current.tileShadingAtomicOps = current.tileShadingAtomicOps || merge_in.tileShadingAtomicOps;
    current.tileShadingImageProcessing = current.tileShadingImageProcessing || merge_in.tileShadingImageProcessing;
}
#endif //(defined(VK_QCOM_tile_shading))
#if (defined(VK_EXT_descriptor_buffer))
void compare_VkPhysicalDeviceDescriptorBufferFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& supported, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& requested) {
    if (requested.descriptorBuffer && !supported.descriptorBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorBufferFeaturesEXT::descriptorBuffer");
    }
    if (requested.descriptorBufferCaptureReplay && !supported.descriptorBufferCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorBufferFeaturesEXT::descriptorBufferCaptureReplay");
    }
    if (requested.descriptorBufferImageLayoutIgnored && !supported.descriptorBufferImageLayoutIgnored) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorBufferFeaturesEXT::descriptorBufferImageLayoutIgnored");
    }
    if (requested.descriptorBufferPushDescriptors && !supported.descriptorBufferPushDescriptors) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorBufferFeaturesEXT::descriptorBufferPushDescriptors");
    }
}
void merge_VkPhysicalDeviceDescriptorBufferFeaturesEXT(VkPhysicalDeviceDescriptorBufferFeaturesEXT & current, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& merge_in) {
    current.descriptorBuffer = current.descriptorBuffer || merge_in.descriptorBuffer;
    current.descriptorBufferCaptureReplay = current.descriptorBufferCaptureReplay || merge_in.descriptorBufferCaptureReplay;
    current.descriptorBufferImageLayoutIgnored = current.descriptorBufferImageLayoutIgnored || merge_in.descriptorBufferImageLayoutIgnored;
    current.descriptorBufferPushDescriptors = current.descriptorBufferPushDescriptors || merge_in.descriptorBufferPushDescriptors;
}
#endif //(defined(VK_EXT_descriptor_buffer))
#if (defined(VK_EXT_graphics_pipeline_library))
void compare_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& supported, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& requested) {
    if (requested.graphicsPipelineLibrary && !supported.graphicsPipelineLibrary) {
        error_list.push_back("Missing feature VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT::graphicsPipelineLibrary");
    }
}
void merge_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT & current, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& merge_in) {
    current.graphicsPipelineLibrary = current.graphicsPipelineLibrary || merge_in.graphicsPipelineLibrary;
}
#endif //(defined(VK_EXT_graphics_pipeline_library))
#if (defined(VK_AMD_shader_early_and_late_fragment_tests))
void compare_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& supported, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& requested) {
    if (requested.shaderEarlyAndLateFragmentTests && !supported.shaderEarlyAndLateFragmentTests) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD::shaderEarlyAndLateFragmentTests");
    }
}
void merge_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD & current, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& merge_in) {
    current.shaderEarlyAndLateFragmentTests = current.shaderEarlyAndLateFragmentTests || merge_in.shaderEarlyAndLateFragmentTests;
}
#endif //(defined(VK_AMD_shader_early_and_late_fragment_tests))
#if (defined(VK_NV_fragment_shading_rate_enums))
void compare_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& supported, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& requested) {
    if (requested.fragmentShadingRateEnums && !supported.fragmentShadingRateEnums) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV::fragmentShadingRateEnums");
    }
    if (requested.supersampleFragmentShadingRates && !supported.supersampleFragmentShadingRates) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV::supersampleFragmentShadingRates");
    }
    if (requested.noInvocationFragmentShadingRates && !supported.noInvocationFragmentShadingRates) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV::noInvocationFragmentShadingRates");
    }
}
void merge_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV & current, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& merge_in) {
    current.fragmentShadingRateEnums = current.fragmentShadingRateEnums || merge_in.fragmentShadingRateEnums;
    current.supersampleFragmentShadingRates = current.supersampleFragmentShadingRates || merge_in.supersampleFragmentShadingRates;
    current.noInvocationFragmentShadingRates = current.noInvocationFragmentShadingRates || merge_in.noInvocationFragmentShadingRates;
}
#endif //(defined(VK_NV_fragment_shading_rate_enums))
#if (defined(VK_NV_ray_tracing_motion_blur))
void compare_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& supported, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& requested) {
    if (requested.rayTracingMotionBlur && !supported.rayTracingMotionBlur) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingMotionBlurFeaturesNV::rayTracingMotionBlur");
    }
    if (requested.rayTracingMotionBlurPipelineTraceRaysIndirect && !supported.rayTracingMotionBlurPipelineTraceRaysIndirect) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingMotionBlurFeaturesNV::rayTracingMotionBlurPipelineTraceRaysIndirect");
    }
}
void merge_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(VkPhysicalDeviceRayTracingMotionBlurFeaturesNV & current, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& merge_in) {
    current.rayTracingMotionBlur = current.rayTracingMotionBlur || merge_in.rayTracingMotionBlur;
    current.rayTracingMotionBlurPipelineTraceRaysIndirect = current.rayTracingMotionBlurPipelineTraceRaysIndirect || merge_in.rayTracingMotionBlurPipelineTraceRaysIndirect;
}
#endif //(defined(VK_NV_ray_tracing_motion_blur))
#if (defined(VK_EXT_ycbcr_2plane_444_formats))
void compare_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& supported, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& requested) {
    if (requested.ycbcr2plane444Formats && !supported.ycbcr2plane444Formats) {
        error_list.push_back("Missing feature VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT::ycbcr2plane444Formats");
    }
}
void merge_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT & current, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& merge_in) {
    current.ycbcr2plane444Formats = current.ycbcr2plane444Formats || merge_in.ycbcr2plane444Formats;
}
#endif //(defined(VK_EXT_ycbcr_2plane_444_formats))
#if (defined(VK_EXT_fragment_density_map2))
void compare_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& requested) {
    if (requested.fragmentDensityMapDeferred && !supported.fragmentDensityMapDeferred) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMap2FeaturesEXT::fragmentDensityMapDeferred");
    }
}
void merge_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(VkPhysicalDeviceFragmentDensityMap2FeaturesEXT & current, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& merge_in) {
    current.fragmentDensityMapDeferred = current.fragmentDensityMapDeferred || merge_in.fragmentDensityMapDeferred;
}
#endif //(defined(VK_EXT_fragment_density_map2))
#if (defined(VK_EXT_image_compression_control))
void compare_VkPhysicalDeviceImageCompressionControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& supported, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& requested) {
    if (requested.imageCompressionControl && !supported.imageCompressionControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageCompressionControlFeaturesEXT::imageCompressionControl");
    }
}
void merge_VkPhysicalDeviceImageCompressionControlFeaturesEXT(VkPhysicalDeviceImageCompressionControlFeaturesEXT & current, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& merge_in) {
    current.imageCompressionControl = current.imageCompressionControl || merge_in.imageCompressionControl;
}
#endif //(defined(VK_EXT_image_compression_control))
#if (defined(VK_EXT_attachment_feedback_loop_layout))
void compare_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& supported, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& requested) {
    if (requested.attachmentFeedbackLoopLayout && !supported.attachmentFeedbackLoopLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT::attachmentFeedbackLoopLayout");
    }
}
void merge_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT & current, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& merge_in) {
    current.attachmentFeedbackLoopLayout = current.attachmentFeedbackLoopLayout || merge_in.attachmentFeedbackLoopLayout;
}
#endif //(defined(VK_EXT_attachment_feedback_loop_layout))
#if (defined(VK_EXT_4444_formats))
void compare_VkPhysicalDevice4444FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevice4444FormatsFeaturesEXT const& supported, VkPhysicalDevice4444FormatsFeaturesEXT const& requested) {
    if (requested.formatA4R4G4B4 && !supported.formatA4R4G4B4) {
        error_list.push_back("Missing feature VkPhysicalDevice4444FormatsFeaturesEXT::formatA4R4G4B4");
    }
    if (requested.formatA4B4G4R4 && !supported.formatA4B4G4R4) {
        error_list.push_back("Missing feature VkPhysicalDevice4444FormatsFeaturesEXT::formatA4B4G4R4");
    }
}
void merge_VkPhysicalDevice4444FormatsFeaturesEXT(VkPhysicalDevice4444FormatsFeaturesEXT & current, VkPhysicalDevice4444FormatsFeaturesEXT const& merge_in) {
    current.formatA4R4G4B4 = current.formatA4R4G4B4 || merge_in.formatA4R4G4B4;
    current.formatA4B4G4R4 = current.formatA4B4G4R4 || merge_in.formatA4B4G4R4;
}
#endif //(defined(VK_EXT_4444_formats))
#if (defined(VK_EXT_device_fault))
void compare_VkPhysicalDeviceFaultFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFaultFeaturesEXT const& supported, VkPhysicalDeviceFaultFeaturesEXT const& requested) {
    if (requested.deviceFault && !supported.deviceFault) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesEXT::deviceFault");
    }
    if (requested.deviceFaultVendorBinary && !supported.deviceFaultVendorBinary) {
        error_list.push_back("Missing feature VkPhysicalDeviceFaultFeaturesEXT::deviceFaultVendorBinary");
    }
}
void merge_VkPhysicalDeviceFaultFeaturesEXT(VkPhysicalDeviceFaultFeaturesEXT & current, VkPhysicalDeviceFaultFeaturesEXT const& merge_in) {
    current.deviceFault = current.deviceFault || merge_in.deviceFault;
    current.deviceFaultVendorBinary = current.deviceFaultVendorBinary || merge_in.deviceFaultVendorBinary;
}
#endif //(defined(VK_EXT_device_fault))
#if (defined(VK_EXT_rasterization_order_attachment_access))
void compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& supported, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& requested) {
    if (requested.rasterizationOrderColorAttachmentAccess && !supported.rasterizationOrderColorAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderColorAttachmentAccess");
    }
    if (requested.rasterizationOrderDepthAttachmentAccess && !supported.rasterizationOrderDepthAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderDepthAttachmentAccess");
    }
    if (requested.rasterizationOrderStencilAttachmentAccess && !supported.rasterizationOrderStencilAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderStencilAttachmentAccess");
    }
}
void merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT & current, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& merge_in) {
    current.rasterizationOrderColorAttachmentAccess = current.rasterizationOrderColorAttachmentAccess || merge_in.rasterizationOrderColorAttachmentAccess;
    current.rasterizationOrderDepthAttachmentAccess = current.rasterizationOrderDepthAttachmentAccess || merge_in.rasterizationOrderDepthAttachmentAccess;
    current.rasterizationOrderStencilAttachmentAccess = current.rasterizationOrderStencilAttachmentAccess || merge_in.rasterizationOrderStencilAttachmentAccess;
}
#endif //(defined(VK_EXT_rasterization_order_attachment_access))
#if (defined(VK_ARM_rasterization_order_attachment_access))
void compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& supported, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& requested) {
    if (requested.rasterizationOrderColorAttachmentAccess && !supported.rasterizationOrderColorAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM::rasterizationOrderColorAttachmentAccess");
    }
    if (requested.rasterizationOrderDepthAttachmentAccess && !supported.rasterizationOrderDepthAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM::rasterizationOrderDepthAttachmentAccess");
    }
    if (requested.rasterizationOrderStencilAttachmentAccess && !supported.rasterizationOrderStencilAttachmentAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM::rasterizationOrderStencilAttachmentAccess");
    }
}
void merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM & current, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& merge_in) {
    current.rasterizationOrderColorAttachmentAccess = current.rasterizationOrderColorAttachmentAccess || merge_in.rasterizationOrderColorAttachmentAccess;
    current.rasterizationOrderDepthAttachmentAccess = current.rasterizationOrderDepthAttachmentAccess || merge_in.rasterizationOrderDepthAttachmentAccess;
    current.rasterizationOrderStencilAttachmentAccess = current.rasterizationOrderStencilAttachmentAccess || merge_in.rasterizationOrderStencilAttachmentAccess;
}
#endif //(defined(VK_ARM_rasterization_order_attachment_access))
#if (defined(VK_EXT_rgba10x6_formats))
void compare_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& supported, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& requested) {
    if (requested.formatRgba10x6WithoutYCbCrSampler && !supported.formatRgba10x6WithoutYCbCrSampler) {
        error_list.push_back("Missing feature VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT::formatRgba10x6WithoutYCbCrSampler");
    }
}
void merge_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT & current, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& merge_in) {
    current.formatRgba10x6WithoutYCbCrSampler = current.formatRgba10x6WithoutYCbCrSampler || merge_in.formatRgba10x6WithoutYCbCrSampler;
}
#endif //(defined(VK_EXT_rgba10x6_formats))
#if (defined(VK_EXT_mutable_descriptor_type))
void compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& supported, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& requested) {
    if (requested.mutableDescriptorType && !supported.mutableDescriptorType) {
        error_list.push_back("Missing feature VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT::mutableDescriptorType");
    }
}
void merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT & current, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& merge_in) {
    current.mutableDescriptorType = current.mutableDescriptorType || merge_in.mutableDescriptorType;
}
#endif //(defined(VK_EXT_mutable_descriptor_type))
#if (defined(VK_VALVE_mutable_descriptor_type))
void compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& supported, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& requested) {
    if (requested.mutableDescriptorType && !supported.mutableDescriptorType) {
        error_list.push_back("Missing feature VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE::mutableDescriptorType");
    }
}
void merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE & current, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& merge_in) {
    current.mutableDescriptorType = current.mutableDescriptorType || merge_in.mutableDescriptorType;
}
#endif //(defined(VK_VALVE_mutable_descriptor_type))
#if (defined(VK_EXT_vertex_input_dynamic_state))
void compare_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& requested) {
    if (requested.vertexInputDynamicState && !supported.vertexInputDynamicState) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT::vertexInputDynamicState");
    }
}
void merge_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT & current, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& merge_in) {
    current.vertexInputDynamicState = current.vertexInputDynamicState || merge_in.vertexInputDynamicState;
}
#endif //(defined(VK_EXT_vertex_input_dynamic_state))
#if (defined(VK_EXT_device_address_binding_report))
void compare_VkPhysicalDeviceAddressBindingReportFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& supported, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& requested) {
    if (requested.reportAddressBinding && !supported.reportAddressBinding) {
        error_list.push_back("Missing feature VkPhysicalDeviceAddressBindingReportFeaturesEXT::reportAddressBinding");
    }
}
void merge_VkPhysicalDeviceAddressBindingReportFeaturesEXT(VkPhysicalDeviceAddressBindingReportFeaturesEXT & current, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& merge_in) {
    current.reportAddressBinding = current.reportAddressBinding || merge_in.reportAddressBinding;
}
#endif //(defined(VK_EXT_device_address_binding_report))
#if (defined(VK_EXT_depth_clip_control))
void compare_VkPhysicalDeviceDepthClipControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClipControlFeaturesEXT const& supported, VkPhysicalDeviceDepthClipControlFeaturesEXT const& requested) {
    if (requested.depthClipControl && !supported.depthClipControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthClipControlFeaturesEXT::depthClipControl");
    }
}
void merge_VkPhysicalDeviceDepthClipControlFeaturesEXT(VkPhysicalDeviceDepthClipControlFeaturesEXT & current, VkPhysicalDeviceDepthClipControlFeaturesEXT const& merge_in) {
    current.depthClipControl = current.depthClipControl || merge_in.depthClipControl;
}
#endif //(defined(VK_EXT_depth_clip_control))
#if (defined(VK_EXT_primitive_topology_list_restart))
void compare_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& supported, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& requested) {
    if (requested.primitiveTopologyListRestart && !supported.primitiveTopologyListRestart) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT::primitiveTopologyListRestart");
    }
    if (requested.primitiveTopologyPatchListRestart && !supported.primitiveTopologyPatchListRestart) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT::primitiveTopologyPatchListRestart");
    }
}
void merge_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT & current, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& merge_in) {
    current.primitiveTopologyListRestart = current.primitiveTopologyListRestart || merge_in.primitiveTopologyListRestart;
    current.primitiveTopologyPatchListRestart = current.primitiveTopologyPatchListRestart || merge_in.primitiveTopologyPatchListRestart;
}
#endif //(defined(VK_EXT_primitive_topology_list_restart))
#if (defined(VK_HUAWEI_subpass_shading))
void compare_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& supported, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& requested) {
    if (requested.subpassShading && !supported.subpassShading) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubpassShadingFeaturesHUAWEI::subpassShading");
    }
}
void merge_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(VkPhysicalDeviceSubpassShadingFeaturesHUAWEI & current, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& merge_in) {
    current.subpassShading = current.subpassShading || merge_in.subpassShading;
}
#endif //(defined(VK_HUAWEI_subpass_shading))
#if (defined(VK_HUAWEI_invocation_mask))
void compare_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& supported, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& requested) {
    if (requested.invocationMask && !supported.invocationMask) {
        error_list.push_back("Missing feature VkPhysicalDeviceInvocationMaskFeaturesHUAWEI::invocationMask");
    }
}
void merge_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(VkPhysicalDeviceInvocationMaskFeaturesHUAWEI & current, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& merge_in) {
    current.invocationMask = current.invocationMask || merge_in.invocationMask;
}
#endif //(defined(VK_HUAWEI_invocation_mask))
#if (defined(VK_NV_external_memory_rdma))
void compare_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& supported, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& requested) {
    if (requested.externalMemoryRDMA && !supported.externalMemoryRDMA) {
        error_list.push_back("Missing feature VkPhysicalDeviceExternalMemoryRDMAFeaturesNV::externalMemoryRDMA");
    }
}
void merge_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(VkPhysicalDeviceExternalMemoryRDMAFeaturesNV & current, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& merge_in) {
    current.externalMemoryRDMA = current.externalMemoryRDMA || merge_in.externalMemoryRDMA;
}
#endif //(defined(VK_NV_external_memory_rdma))
#if (defined(VK_EXT_pipeline_properties))
void compare_VkPhysicalDevicePipelinePropertiesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& supported, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& requested) {
    if (requested.pipelinePropertiesIdentifier && !supported.pipelinePropertiesIdentifier) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelinePropertiesFeaturesEXT::pipelinePropertiesIdentifier");
    }
}
void merge_VkPhysicalDevicePipelinePropertiesFeaturesEXT(VkPhysicalDevicePipelinePropertiesFeaturesEXT & current, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& merge_in) {
    current.pipelinePropertiesIdentifier = current.pipelinePropertiesIdentifier || merge_in.pipelinePropertiesIdentifier;
}
#endif //(defined(VK_EXT_pipeline_properties))
#if (defined(VK_EXT_frame_boundary))
void compare_VkPhysicalDeviceFrameBoundaryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& supported, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& requested) {
    if (requested.frameBoundary && !supported.frameBoundary) {
        error_list.push_back("Missing feature VkPhysicalDeviceFrameBoundaryFeaturesEXT::frameBoundary");
    }
}
void merge_VkPhysicalDeviceFrameBoundaryFeaturesEXT(VkPhysicalDeviceFrameBoundaryFeaturesEXT & current, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& merge_in) {
    current.frameBoundary = current.frameBoundary || merge_in.frameBoundary;
}
#endif //(defined(VK_EXT_frame_boundary))
#if (defined(VK_EXT_multisampled_render_to_single_sampled))
void compare_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& supported, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& requested) {
    if (requested.multisampledRenderToSingleSampled && !supported.multisampledRenderToSingleSampled) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT::multisampledRenderToSingleSampled");
    }
}
void merge_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT & current, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& merge_in) {
    current.multisampledRenderToSingleSampled = current.multisampledRenderToSingleSampled || merge_in.multisampledRenderToSingleSampled;
}
#endif //(defined(VK_EXT_multisampled_render_to_single_sampled))
#if (defined(VK_EXT_extended_dynamic_state2))
void compare_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& requested) {
    if (requested.extendedDynamicState2 && !supported.extendedDynamicState2) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState2FeaturesEXT::extendedDynamicState2");
    }
    if (requested.extendedDynamicState2LogicOp && !supported.extendedDynamicState2LogicOp) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState2FeaturesEXT::extendedDynamicState2LogicOp");
    }
    if (requested.extendedDynamicState2PatchControlPoints && !supported.extendedDynamicState2PatchControlPoints) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState2FeaturesEXT::extendedDynamicState2PatchControlPoints");
    }
}
void merge_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(VkPhysicalDeviceExtendedDynamicState2FeaturesEXT & current, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& merge_in) {
    current.extendedDynamicState2 = current.extendedDynamicState2 || merge_in.extendedDynamicState2;
    current.extendedDynamicState2LogicOp = current.extendedDynamicState2LogicOp || merge_in.extendedDynamicState2LogicOp;
    current.extendedDynamicState2PatchControlPoints = current.extendedDynamicState2PatchControlPoints || merge_in.extendedDynamicState2PatchControlPoints;
}
#endif //(defined(VK_EXT_extended_dynamic_state2))
#if (defined(VK_EXT_color_write_enable))
void compare_VkPhysicalDeviceColorWriteEnableFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& supported, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& requested) {
    if (requested.colorWriteEnable && !supported.colorWriteEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceColorWriteEnableFeaturesEXT::colorWriteEnable");
    }
}
void merge_VkPhysicalDeviceColorWriteEnableFeaturesEXT(VkPhysicalDeviceColorWriteEnableFeaturesEXT & current, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& merge_in) {
    current.colorWriteEnable = current.colorWriteEnable || merge_in.colorWriteEnable;
}
#endif //(defined(VK_EXT_color_write_enable))
#if (defined(VK_EXT_primitives_generated_query))
void compare_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& supported, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& requested) {
    if (requested.primitivesGeneratedQuery && !supported.primitivesGeneratedQuery) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT::primitivesGeneratedQuery");
    }
    if (requested.primitivesGeneratedQueryWithRasterizerDiscard && !supported.primitivesGeneratedQueryWithRasterizerDiscard) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT::primitivesGeneratedQueryWithRasterizerDiscard");
    }
    if (requested.primitivesGeneratedQueryWithNonZeroStreams && !supported.primitivesGeneratedQueryWithNonZeroStreams) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT::primitivesGeneratedQueryWithNonZeroStreams");
    }
}
void merge_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT & current, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& merge_in) {
    current.primitivesGeneratedQuery = current.primitivesGeneratedQuery || merge_in.primitivesGeneratedQuery;
    current.primitivesGeneratedQueryWithRasterizerDiscard = current.primitivesGeneratedQueryWithRasterizerDiscard || merge_in.primitivesGeneratedQueryWithRasterizerDiscard;
    current.primitivesGeneratedQueryWithNonZeroStreams = current.primitivesGeneratedQueryWithNonZeroStreams || merge_in.primitivesGeneratedQueryWithNonZeroStreams;
}
#endif //(defined(VK_EXT_primitives_generated_query))
#if (defined(VK_VALVE_video_encode_rgb_conversion))
void compare_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& supported, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& requested) {
    if (requested.videoEncodeRgbConversion && !supported.videoEncodeRgbConversion) {
        error_list.push_back("Missing feature VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE::videoEncodeRgbConversion");
    }
}
void merge_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE & current, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& merge_in) {
    current.videoEncodeRgbConversion = current.videoEncodeRgbConversion || merge_in.videoEncodeRgbConversion;
}
#endif //(defined(VK_VALVE_video_encode_rgb_conversion))
#if (defined(VK_EXT_image_view_min_lod))
void compare_VkPhysicalDeviceImageViewMinLodFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& supported, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& requested) {
    if (requested.minLod && !supported.minLod) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageViewMinLodFeaturesEXT::minLod");
    }
}
void merge_VkPhysicalDeviceImageViewMinLodFeaturesEXT(VkPhysicalDeviceImageViewMinLodFeaturesEXT & current, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& merge_in) {
    current.minLod = current.minLod || merge_in.minLod;
}
#endif //(defined(VK_EXT_image_view_min_lod))
#if (defined(VK_EXT_multi_draw))
void compare_VkPhysicalDeviceMultiDrawFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultiDrawFeaturesEXT const& supported, VkPhysicalDeviceMultiDrawFeaturesEXT const& requested) {
    if (requested.multiDraw && !supported.multiDraw) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiDrawFeaturesEXT::multiDraw");
    }
}
void merge_VkPhysicalDeviceMultiDrawFeaturesEXT(VkPhysicalDeviceMultiDrawFeaturesEXT & current, VkPhysicalDeviceMultiDrawFeaturesEXT const& merge_in) {
    current.multiDraw = current.multiDraw || merge_in.multiDraw;
}
#endif //(defined(VK_EXT_multi_draw))
#if (defined(VK_EXT_image_2d_view_of_3d))
void compare_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& supported, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& requested) {
    if (requested.image2DViewOf3D && !supported.image2DViewOf3D) {
        error_list.push_back("Missing feature VkPhysicalDeviceImage2DViewOf3DFeaturesEXT::image2DViewOf3D");
    }
    if (requested.sampler2DViewOf3D && !supported.sampler2DViewOf3D) {
        error_list.push_back("Missing feature VkPhysicalDeviceImage2DViewOf3DFeaturesEXT::sampler2DViewOf3D");
    }
}
void merge_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(VkPhysicalDeviceImage2DViewOf3DFeaturesEXT & current, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& merge_in) {
    current.image2DViewOf3D = current.image2DViewOf3D || merge_in.image2DViewOf3D;
    current.sampler2DViewOf3D = current.sampler2DViewOf3D || merge_in.sampler2DViewOf3D;
}
#endif //(defined(VK_EXT_image_2d_view_of_3d))
#if (defined(VK_EXT_shader_tile_image))
void compare_VkPhysicalDeviceShaderTileImageFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTileImageFeaturesEXT const& supported, VkPhysicalDeviceShaderTileImageFeaturesEXT const& requested) {
    if (requested.shaderTileImageColorReadAccess && !supported.shaderTileImageColorReadAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderTileImageFeaturesEXT::shaderTileImageColorReadAccess");
    }
    if (requested.shaderTileImageDepthReadAccess && !supported.shaderTileImageDepthReadAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderTileImageFeaturesEXT::shaderTileImageDepthReadAccess");
    }
    if (requested.shaderTileImageStencilReadAccess && !supported.shaderTileImageStencilReadAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderTileImageFeaturesEXT::shaderTileImageStencilReadAccess");
    }
}
void merge_VkPhysicalDeviceShaderTileImageFeaturesEXT(VkPhysicalDeviceShaderTileImageFeaturesEXT & current, VkPhysicalDeviceShaderTileImageFeaturesEXT const& merge_in) {
    current.shaderTileImageColorReadAccess = current.shaderTileImageColorReadAccess || merge_in.shaderTileImageColorReadAccess;
    current.shaderTileImageDepthReadAccess = current.shaderTileImageDepthReadAccess || merge_in.shaderTileImageDepthReadAccess;
    current.shaderTileImageStencilReadAccess = current.shaderTileImageStencilReadAccess || merge_in.shaderTileImageStencilReadAccess;
}
#endif //(defined(VK_EXT_shader_tile_image))
#if (defined(VK_EXT_opacity_micromap))
void compare_VkPhysicalDeviceOpacityMicromapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& supported, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& requested) {
    if (requested.micromap && !supported.micromap) {
        error_list.push_back("Missing feature VkPhysicalDeviceOpacityMicromapFeaturesEXT::micromap");
    }
    if (requested.micromapCaptureReplay && !supported.micromapCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceOpacityMicromapFeaturesEXT::micromapCaptureReplay");
    }
    if (requested.micromapHostCommands && !supported.micromapHostCommands) {
        error_list.push_back("Missing feature VkPhysicalDeviceOpacityMicromapFeaturesEXT::micromapHostCommands");
    }
}
void merge_VkPhysicalDeviceOpacityMicromapFeaturesEXT(VkPhysicalDeviceOpacityMicromapFeaturesEXT & current, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& merge_in) {
    current.micromap = current.micromap || merge_in.micromap;
    current.micromapCaptureReplay = current.micromapCaptureReplay || merge_in.micromapCaptureReplay;
    current.micromapHostCommands = current.micromapHostCommands || merge_in.micromapHostCommands;
}
#endif //(defined(VK_EXT_opacity_micromap))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
void compare_VkPhysicalDeviceDisplacementMicromapFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& supported, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& requested) {
    if (requested.displacementMicromap && !supported.displacementMicromap) {
        error_list.push_back("Missing feature VkPhysicalDeviceDisplacementMicromapFeaturesNV::displacementMicromap");
    }
}
void merge_VkPhysicalDeviceDisplacementMicromapFeaturesNV(VkPhysicalDeviceDisplacementMicromapFeaturesNV & current, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& merge_in) {
    current.displacementMicromap = current.displacementMicromap || merge_in.displacementMicromap;
}
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
#if (defined(VK_HUAWEI_cluster_culling_shader))
void compare_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& supported, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& requested) {
    if (requested.clustercullingShader && !supported.clustercullingShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI::clustercullingShader");
    }
    if (requested.multiviewClusterCullingShader && !supported.multiviewClusterCullingShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI::multiviewClusterCullingShader");
    }
}
void merge_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI & current, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& merge_in) {
    current.clustercullingShader = current.clustercullingShader || merge_in.clustercullingShader;
    current.multiviewClusterCullingShader = current.multiviewClusterCullingShader || merge_in.multiviewClusterCullingShader;
}
#endif //(defined(VK_HUAWEI_cluster_culling_shader))
#if (defined(VK_EXT_border_color_swizzle))
void compare_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& supported, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& requested) {
    if (requested.borderColorSwizzle && !supported.borderColorSwizzle) {
        error_list.push_back("Missing feature VkPhysicalDeviceBorderColorSwizzleFeaturesEXT::borderColorSwizzle");
    }
    if (requested.borderColorSwizzleFromImage && !supported.borderColorSwizzleFromImage) {
        error_list.push_back("Missing feature VkPhysicalDeviceBorderColorSwizzleFeaturesEXT::borderColorSwizzleFromImage");
    }
}
void merge_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(VkPhysicalDeviceBorderColorSwizzleFeaturesEXT & current, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& merge_in) {
    current.borderColorSwizzle = current.borderColorSwizzle || merge_in.borderColorSwizzle;
    current.borderColorSwizzleFromImage = current.borderColorSwizzleFromImage || merge_in.borderColorSwizzleFromImage;
}
#endif //(defined(VK_EXT_border_color_swizzle))
#if (defined(VK_EXT_pageable_device_local_memory))
void compare_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& supported, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& requested) {
    if (requested.pageableDeviceLocalMemory && !supported.pageableDeviceLocalMemory) {
        error_list.push_back("Missing feature VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT::pageableDeviceLocalMemory");
    }
}
void merge_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT & current, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& merge_in) {
    current.pageableDeviceLocalMemory = current.pageableDeviceLocalMemory || merge_in.pageableDeviceLocalMemory;
}
#endif //(defined(VK_EXT_pageable_device_local_memory))
#if (defined(VK_ARM_scheduling_controls))
void compare_VkPhysicalDeviceSchedulingControlsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceSchedulingControlsFeaturesARM const& supported, VkPhysicalDeviceSchedulingControlsFeaturesARM const& requested) {
    if (requested.schedulingControls && !supported.schedulingControls) {
        error_list.push_back("Missing feature VkPhysicalDeviceSchedulingControlsFeaturesARM::schedulingControls");
    }
}
void merge_VkPhysicalDeviceSchedulingControlsFeaturesARM(VkPhysicalDeviceSchedulingControlsFeaturesARM & current, VkPhysicalDeviceSchedulingControlsFeaturesARM const& merge_in) {
    current.schedulingControls = current.schedulingControls || merge_in.schedulingControls;
}
#endif //(defined(VK_ARM_scheduling_controls))
#if (defined(VK_EXT_image_sliced_view_of_3d))
void compare_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& supported, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& requested) {
    if (requested.imageSlicedViewOf3D && !supported.imageSlicedViewOf3D) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT::imageSlicedViewOf3D");
    }
}
void merge_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT & current, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& merge_in) {
    current.imageSlicedViewOf3D = current.imageSlicedViewOf3D || merge_in.imageSlicedViewOf3D;
}
#endif //(defined(VK_EXT_image_sliced_view_of_3d))
#if (defined(VK_VALVE_descriptor_set_host_mapping))
void compare_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& supported, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& requested) {
    if (requested.descriptorSetHostMapping && !supported.descriptorSetHostMapping) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE::descriptorSetHostMapping");
    }
}
void merge_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE & current, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& merge_in) {
    current.descriptorSetHostMapping = current.descriptorSetHostMapping || merge_in.descriptorSetHostMapping;
}
#endif //(defined(VK_VALVE_descriptor_set_host_mapping))
#if (defined(VK_EXT_non_seamless_cube_map))
void compare_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& supported, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& requested) {
    if (requested.nonSeamlessCubeMap && !supported.nonSeamlessCubeMap) {
        error_list.push_back("Missing feature VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT::nonSeamlessCubeMap");
    }
}
void merge_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT & current, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& merge_in) {
    current.nonSeamlessCubeMap = current.nonSeamlessCubeMap || merge_in.nonSeamlessCubeMap;
}
#endif //(defined(VK_EXT_non_seamless_cube_map))
#if (defined(VK_ARM_render_pass_striped))
void compare_VkPhysicalDeviceRenderPassStripedFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceRenderPassStripedFeaturesARM const& supported, VkPhysicalDeviceRenderPassStripedFeaturesARM const& requested) {
    if (requested.renderPassStriped && !supported.renderPassStriped) {
        error_list.push_back("Missing feature VkPhysicalDeviceRenderPassStripedFeaturesARM::renderPassStriped");
    }
}
void merge_VkPhysicalDeviceRenderPassStripedFeaturesARM(VkPhysicalDeviceRenderPassStripedFeaturesARM & current, VkPhysicalDeviceRenderPassStripedFeaturesARM const& merge_in) {
    current.renderPassStriped = current.renderPassStriped || merge_in.renderPassStriped;
}
#endif //(defined(VK_ARM_render_pass_striped))
#if (defined(VK_EXT_fragment_density_map_offset))
void compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& requested) {
    if (requested.fragmentDensityMapOffset && !supported.fragmentDensityMapOffset) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT::fragmentDensityMapOffset");
    }
}
void merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT & current, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& merge_in) {
    current.fragmentDensityMapOffset = current.fragmentDensityMapOffset || merge_in.fragmentDensityMapOffset;
}
#endif //(defined(VK_EXT_fragment_density_map_offset))
#if (defined(VK_QCOM_fragment_density_map_offset))
void compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& supported, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& requested) {
    if (requested.fragmentDensityMapOffset && !supported.fragmentDensityMapOffset) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM::fragmentDensityMapOffset");
    }
}
void merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM & current, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& merge_in) {
    current.fragmentDensityMapOffset = current.fragmentDensityMapOffset || merge_in.fragmentDensityMapOffset;
}
#endif //(defined(VK_QCOM_fragment_density_map_offset))
#if (defined(VK_NV_copy_memory_indirect))
void compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& supported, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& requested) {
    if (requested.indirectCopy && !supported.indirectCopy) {
        error_list.push_back("Missing feature VkPhysicalDeviceCopyMemoryIndirectFeaturesNV::indirectCopy");
    }
}
void merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(VkPhysicalDeviceCopyMemoryIndirectFeaturesNV & current, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& merge_in) {
    current.indirectCopy = current.indirectCopy || merge_in.indirectCopy;
}
#endif //(defined(VK_NV_copy_memory_indirect))
#if (defined(VK_EXT_memory_decompression))
void compare_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& supported, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& requested) {
    if (requested.memoryDecompression && !supported.memoryDecompression) {
        error_list.push_back("Missing feature VkPhysicalDeviceMemoryDecompressionFeaturesEXT::memoryDecompression");
    }
}
void merge_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(VkPhysicalDeviceMemoryDecompressionFeaturesEXT & current, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& merge_in) {
    current.memoryDecompression = current.memoryDecompression || merge_in.memoryDecompression;
}
#endif //(defined(VK_EXT_memory_decompression))
#if (defined(VK_NV_memory_decompression))
void compare_VkPhysicalDeviceMemoryDecompressionFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& supported, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& requested) {
    if (requested.memoryDecompression && !supported.memoryDecompression) {
        error_list.push_back("Missing feature VkPhysicalDeviceMemoryDecompressionFeaturesNV::memoryDecompression");
    }
}
void merge_VkPhysicalDeviceMemoryDecompressionFeaturesNV(VkPhysicalDeviceMemoryDecompressionFeaturesNV & current, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& merge_in) {
    current.memoryDecompression = current.memoryDecompression || merge_in.memoryDecompression;
}
#endif //(defined(VK_NV_memory_decompression))
#if (defined(VK_NV_device_generated_commands_compute))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& supported, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& requested) {
    if (requested.deviceGeneratedCompute && !supported.deviceGeneratedCompute) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV::deviceGeneratedCompute");
    }
    if (requested.deviceGeneratedComputePipelines && !supported.deviceGeneratedComputePipelines) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV::deviceGeneratedComputePipelines");
    }
    if (requested.deviceGeneratedComputeCaptureReplay && !supported.deviceGeneratedComputeCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV::deviceGeneratedComputeCaptureReplay");
    }
}
void merge_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV & current, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& merge_in) {
    current.deviceGeneratedCompute = current.deviceGeneratedCompute || merge_in.deviceGeneratedCompute;
    current.deviceGeneratedComputePipelines = current.deviceGeneratedComputePipelines || merge_in.deviceGeneratedComputePipelines;
    current.deviceGeneratedComputeCaptureReplay = current.deviceGeneratedComputeCaptureReplay || merge_in.deviceGeneratedComputeCaptureReplay;
}
#endif //(defined(VK_NV_device_generated_commands_compute))
#if (defined(VK_NV_ray_tracing_linear_swept_spheres))
void compare_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& supported, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& requested) {
    if (requested.spheres && !supported.spheres) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV::spheres");
    }
    if (requested.linearSweptSpheres && !supported.linearSweptSpheres) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV::linearSweptSpheres");
    }
}
void merge_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV & current, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& merge_in) {
    current.spheres = current.spheres || merge_in.spheres;
    current.linearSweptSpheres = current.linearSweptSpheres || merge_in.linearSweptSpheres;
}
#endif //(defined(VK_NV_ray_tracing_linear_swept_spheres))
#if (defined(VK_NV_linear_color_attachment))
void compare_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& supported, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& requested) {
    if (requested.linearColorAttachment && !supported.linearColorAttachment) {
        error_list.push_back("Missing feature VkPhysicalDeviceLinearColorAttachmentFeaturesNV::linearColorAttachment");
    }
}
void merge_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(VkPhysicalDeviceLinearColorAttachmentFeaturesNV & current, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& merge_in) {
    current.linearColorAttachment = current.linearColorAttachment || merge_in.linearColorAttachment;
}
#endif //(defined(VK_NV_linear_color_attachment))
#if (defined(VK_EXT_image_compression_control_swapchain))
void compare_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& supported, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& requested) {
    if (requested.imageCompressionControlSwapchain && !supported.imageCompressionControlSwapchain) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT::imageCompressionControlSwapchain");
    }
}
void merge_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT & current, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& merge_in) {
    current.imageCompressionControlSwapchain = current.imageCompressionControlSwapchain || merge_in.imageCompressionControlSwapchain;
}
#endif //(defined(VK_EXT_image_compression_control_swapchain))
#if (defined(VK_QCOM_image_processing))
void compare_VkPhysicalDeviceImageProcessingFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessingFeaturesQCOM const& supported, VkPhysicalDeviceImageProcessingFeaturesQCOM const& requested) {
    if (requested.textureSampleWeighted && !supported.textureSampleWeighted) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessingFeaturesQCOM::textureSampleWeighted");
    }
    if (requested.textureBoxFilter && !supported.textureBoxFilter) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessingFeaturesQCOM::textureBoxFilter");
    }
    if (requested.textureBlockMatch && !supported.textureBlockMatch) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessingFeaturesQCOM::textureBlockMatch");
    }
}
void merge_VkPhysicalDeviceImageProcessingFeaturesQCOM(VkPhysicalDeviceImageProcessingFeaturesQCOM & current, VkPhysicalDeviceImageProcessingFeaturesQCOM const& merge_in) {
    current.textureSampleWeighted = current.textureSampleWeighted || merge_in.textureSampleWeighted;
    current.textureBoxFilter = current.textureBoxFilter || merge_in.textureBoxFilter;
    current.textureBlockMatch = current.textureBlockMatch || merge_in.textureBlockMatch;
}
#endif //(defined(VK_QCOM_image_processing))
#if (defined(VK_EXT_nested_command_buffer))
void compare_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& supported, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& requested) {
    if (requested.nestedCommandBuffer && !supported.nestedCommandBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceNestedCommandBufferFeaturesEXT::nestedCommandBuffer");
    }
    if (requested.nestedCommandBufferRendering && !supported.nestedCommandBufferRendering) {
        error_list.push_back("Missing feature VkPhysicalDeviceNestedCommandBufferFeaturesEXT::nestedCommandBufferRendering");
    }
    if (requested.nestedCommandBufferSimultaneousUse && !supported.nestedCommandBufferSimultaneousUse) {
        error_list.push_back("Missing feature VkPhysicalDeviceNestedCommandBufferFeaturesEXT::nestedCommandBufferSimultaneousUse");
    }
}
void merge_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(VkPhysicalDeviceNestedCommandBufferFeaturesEXT & current, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& merge_in) {
    current.nestedCommandBuffer = current.nestedCommandBuffer || merge_in.nestedCommandBuffer;
    current.nestedCommandBufferRendering = current.nestedCommandBufferRendering || merge_in.nestedCommandBufferRendering;
    current.nestedCommandBufferSimultaneousUse = current.nestedCommandBufferSimultaneousUse || merge_in.nestedCommandBufferSimultaneousUse;
}
#endif //(defined(VK_EXT_nested_command_buffer))
#if (defined(VK_EXT_extended_dynamic_state3))
void compare_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& requested) {
    if (requested.extendedDynamicState3TessellationDomainOrigin && !supported.extendedDynamicState3TessellationDomainOrigin) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3TessellationDomainOrigin");
    }
    if (requested.extendedDynamicState3DepthClampEnable && !supported.extendedDynamicState3DepthClampEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3DepthClampEnable");
    }
    if (requested.extendedDynamicState3PolygonMode && !supported.extendedDynamicState3PolygonMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3PolygonMode");
    }
    if (requested.extendedDynamicState3RasterizationSamples && !supported.extendedDynamicState3RasterizationSamples) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3RasterizationSamples");
    }
    if (requested.extendedDynamicState3SampleMask && !supported.extendedDynamicState3SampleMask) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3SampleMask");
    }
    if (requested.extendedDynamicState3AlphaToCoverageEnable && !supported.extendedDynamicState3AlphaToCoverageEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3AlphaToCoverageEnable");
    }
    if (requested.extendedDynamicState3AlphaToOneEnable && !supported.extendedDynamicState3AlphaToOneEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3AlphaToOneEnable");
    }
    if (requested.extendedDynamicState3LogicOpEnable && !supported.extendedDynamicState3LogicOpEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3LogicOpEnable");
    }
    if (requested.extendedDynamicState3ColorBlendEnable && !supported.extendedDynamicState3ColorBlendEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ColorBlendEnable");
    }
    if (requested.extendedDynamicState3ColorBlendEquation && !supported.extendedDynamicState3ColorBlendEquation) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ColorBlendEquation");
    }
    if (requested.extendedDynamicState3ColorWriteMask && !supported.extendedDynamicState3ColorWriteMask) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ColorWriteMask");
    }
    if (requested.extendedDynamicState3RasterizationStream && !supported.extendedDynamicState3RasterizationStream) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3RasterizationStream");
    }
    if (requested.extendedDynamicState3ConservativeRasterizationMode && !supported.extendedDynamicState3ConservativeRasterizationMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ConservativeRasterizationMode");
    }
    if (requested.extendedDynamicState3ExtraPrimitiveOverestimationSize && !supported.extendedDynamicState3ExtraPrimitiveOverestimationSize) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ExtraPrimitiveOverestimationSize");
    }
    if (requested.extendedDynamicState3DepthClipEnable && !supported.extendedDynamicState3DepthClipEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3DepthClipEnable");
    }
    if (requested.extendedDynamicState3SampleLocationsEnable && !supported.extendedDynamicState3SampleLocationsEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3SampleLocationsEnable");
    }
    if (requested.extendedDynamicState3ColorBlendAdvanced && !supported.extendedDynamicState3ColorBlendAdvanced) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ColorBlendAdvanced");
    }
    if (requested.extendedDynamicState3ProvokingVertexMode && !supported.extendedDynamicState3ProvokingVertexMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ProvokingVertexMode");
    }
    if (requested.extendedDynamicState3LineRasterizationMode && !supported.extendedDynamicState3LineRasterizationMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3LineRasterizationMode");
    }
    if (requested.extendedDynamicState3LineStippleEnable && !supported.extendedDynamicState3LineStippleEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3LineStippleEnable");
    }
    if (requested.extendedDynamicState3DepthClipNegativeOneToOne && !supported.extendedDynamicState3DepthClipNegativeOneToOne) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3DepthClipNegativeOneToOne");
    }
    if (requested.extendedDynamicState3ViewportWScalingEnable && !supported.extendedDynamicState3ViewportWScalingEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ViewportWScalingEnable");
    }
    if (requested.extendedDynamicState3ViewportSwizzle && !supported.extendedDynamicState3ViewportSwizzle) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ViewportSwizzle");
    }
    if (requested.extendedDynamicState3CoverageToColorEnable && !supported.extendedDynamicState3CoverageToColorEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageToColorEnable");
    }
    if (requested.extendedDynamicState3CoverageToColorLocation && !supported.extendedDynamicState3CoverageToColorLocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageToColorLocation");
    }
    if (requested.extendedDynamicState3CoverageModulationMode && !supported.extendedDynamicState3CoverageModulationMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageModulationMode");
    }
    if (requested.extendedDynamicState3CoverageModulationTableEnable && !supported.extendedDynamicState3CoverageModulationTableEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageModulationTableEnable");
    }
    if (requested.extendedDynamicState3CoverageModulationTable && !supported.extendedDynamicState3CoverageModulationTable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageModulationTable");
    }
    if (requested.extendedDynamicState3CoverageReductionMode && !supported.extendedDynamicState3CoverageReductionMode) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3CoverageReductionMode");
    }
    if (requested.extendedDynamicState3RepresentativeFragmentTestEnable && !supported.extendedDynamicState3RepresentativeFragmentTestEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3RepresentativeFragmentTestEnable");
    }
    if (requested.extendedDynamicState3ShadingRateImageEnable && !supported.extendedDynamicState3ShadingRateImageEnable) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3ShadingRateImageEnable");
    }
}
void merge_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(VkPhysicalDeviceExtendedDynamicState3FeaturesEXT & current, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& merge_in) {
    current.extendedDynamicState3TessellationDomainOrigin = current.extendedDynamicState3TessellationDomainOrigin || merge_in.extendedDynamicState3TessellationDomainOrigin;
    current.extendedDynamicState3DepthClampEnable = current.extendedDynamicState3DepthClampEnable || merge_in.extendedDynamicState3DepthClampEnable;
    current.extendedDynamicState3PolygonMode = current.extendedDynamicState3PolygonMode || merge_in.extendedDynamicState3PolygonMode;
    current.extendedDynamicState3RasterizationSamples = current.extendedDynamicState3RasterizationSamples || merge_in.extendedDynamicState3RasterizationSamples;
    current.extendedDynamicState3SampleMask = current.extendedDynamicState3SampleMask || merge_in.extendedDynamicState3SampleMask;
    current.extendedDynamicState3AlphaToCoverageEnable = current.extendedDynamicState3AlphaToCoverageEnable || merge_in.extendedDynamicState3AlphaToCoverageEnable;
    current.extendedDynamicState3AlphaToOneEnable = current.extendedDynamicState3AlphaToOneEnable || merge_in.extendedDynamicState3AlphaToOneEnable;
    current.extendedDynamicState3LogicOpEnable = current.extendedDynamicState3LogicOpEnable || merge_in.extendedDynamicState3LogicOpEnable;
    current.extendedDynamicState3ColorBlendEnable = current.extendedDynamicState3ColorBlendEnable || merge_in.extendedDynamicState3ColorBlendEnable;
    current.extendedDynamicState3ColorBlendEquation = current.extendedDynamicState3ColorBlendEquation || merge_in.extendedDynamicState3ColorBlendEquation;
    current.extendedDynamicState3ColorWriteMask = current.extendedDynamicState3ColorWriteMask || merge_in.extendedDynamicState3ColorWriteMask;
    current.extendedDynamicState3RasterizationStream = current.extendedDynamicState3RasterizationStream || merge_in.extendedDynamicState3RasterizationStream;
    current.extendedDynamicState3ConservativeRasterizationMode = current.extendedDynamicState3ConservativeRasterizationMode || merge_in.extendedDynamicState3ConservativeRasterizationMode;
    current.extendedDynamicState3ExtraPrimitiveOverestimationSize = current.extendedDynamicState3ExtraPrimitiveOverestimationSize || merge_in.extendedDynamicState3ExtraPrimitiveOverestimationSize;
    current.extendedDynamicState3DepthClipEnable = current.extendedDynamicState3DepthClipEnable || merge_in.extendedDynamicState3DepthClipEnable;
    current.extendedDynamicState3SampleLocationsEnable = current.extendedDynamicState3SampleLocationsEnable || merge_in.extendedDynamicState3SampleLocationsEnable;
    current.extendedDynamicState3ColorBlendAdvanced = current.extendedDynamicState3ColorBlendAdvanced || merge_in.extendedDynamicState3ColorBlendAdvanced;
    current.extendedDynamicState3ProvokingVertexMode = current.extendedDynamicState3ProvokingVertexMode || merge_in.extendedDynamicState3ProvokingVertexMode;
    current.extendedDynamicState3LineRasterizationMode = current.extendedDynamicState3LineRasterizationMode || merge_in.extendedDynamicState3LineRasterizationMode;
    current.extendedDynamicState3LineStippleEnable = current.extendedDynamicState3LineStippleEnable || merge_in.extendedDynamicState3LineStippleEnable;
    current.extendedDynamicState3DepthClipNegativeOneToOne = current.extendedDynamicState3DepthClipNegativeOneToOne || merge_in.extendedDynamicState3DepthClipNegativeOneToOne;
    current.extendedDynamicState3ViewportWScalingEnable = current.extendedDynamicState3ViewportWScalingEnable || merge_in.extendedDynamicState3ViewportWScalingEnable;
    current.extendedDynamicState3ViewportSwizzle = current.extendedDynamicState3ViewportSwizzle || merge_in.extendedDynamicState3ViewportSwizzle;
    current.extendedDynamicState3CoverageToColorEnable = current.extendedDynamicState3CoverageToColorEnable || merge_in.extendedDynamicState3CoverageToColorEnable;
    current.extendedDynamicState3CoverageToColorLocation = current.extendedDynamicState3CoverageToColorLocation || merge_in.extendedDynamicState3CoverageToColorLocation;
    current.extendedDynamicState3CoverageModulationMode = current.extendedDynamicState3CoverageModulationMode || merge_in.extendedDynamicState3CoverageModulationMode;
    current.extendedDynamicState3CoverageModulationTableEnable = current.extendedDynamicState3CoverageModulationTableEnable || merge_in.extendedDynamicState3CoverageModulationTableEnable;
    current.extendedDynamicState3CoverageModulationTable = current.extendedDynamicState3CoverageModulationTable || merge_in.extendedDynamicState3CoverageModulationTable;
    current.extendedDynamicState3CoverageReductionMode = current.extendedDynamicState3CoverageReductionMode || merge_in.extendedDynamicState3CoverageReductionMode;
    current.extendedDynamicState3RepresentativeFragmentTestEnable = current.extendedDynamicState3RepresentativeFragmentTestEnable || merge_in.extendedDynamicState3RepresentativeFragmentTestEnable;
    current.extendedDynamicState3ShadingRateImageEnable = current.extendedDynamicState3ShadingRateImageEnable || merge_in.extendedDynamicState3ShadingRateImageEnable;
}
#endif //(defined(VK_EXT_extended_dynamic_state3))
#if (defined(VK_EXT_subpass_merge_feedback))
void compare_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& supported, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& requested) {
    if (requested.subpassMergeFeedback && !supported.subpassMergeFeedback) {
        error_list.push_back("Missing feature VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT::subpassMergeFeedback");
    }
}
void merge_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT & current, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& merge_in) {
    current.subpassMergeFeedback = current.subpassMergeFeedback || merge_in.subpassMergeFeedback;
}
#endif //(defined(VK_EXT_subpass_merge_feedback))
#if (defined(VK_ARM_tensors))
void compare_VkPhysicalDeviceTensorFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceTensorFeaturesARM const& supported, VkPhysicalDeviceTensorFeaturesARM const& requested) {
    if (requested.tensorNonPacked && !supported.tensorNonPacked) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::tensorNonPacked");
    }
    if (requested.shaderTensorAccess && !supported.shaderTensorAccess) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::shaderTensorAccess");
    }
    if (requested.shaderStorageTensorArrayDynamicIndexing && !supported.shaderStorageTensorArrayDynamicIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::shaderStorageTensorArrayDynamicIndexing");
    }
    if (requested.shaderStorageTensorArrayNonUniformIndexing && !supported.shaderStorageTensorArrayNonUniformIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::shaderStorageTensorArrayNonUniformIndexing");
    }
    if (requested.descriptorBindingStorageTensorUpdateAfterBind && !supported.descriptorBindingStorageTensorUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::descriptorBindingStorageTensorUpdateAfterBind");
    }
    if (requested.tensors && !supported.tensors) {
        error_list.push_back("Missing feature VkPhysicalDeviceTensorFeaturesARM::tensors");
    }
}
void merge_VkPhysicalDeviceTensorFeaturesARM(VkPhysicalDeviceTensorFeaturesARM & current, VkPhysicalDeviceTensorFeaturesARM const& merge_in) {
    current.tensorNonPacked = current.tensorNonPacked || merge_in.tensorNonPacked;
    current.shaderTensorAccess = current.shaderTensorAccess || merge_in.shaderTensorAccess;
    current.shaderStorageTensorArrayDynamicIndexing = current.shaderStorageTensorArrayDynamicIndexing || merge_in.shaderStorageTensorArrayDynamicIndexing;
    current.shaderStorageTensorArrayNonUniformIndexing = current.shaderStorageTensorArrayNonUniformIndexing || merge_in.shaderStorageTensorArrayNonUniformIndexing;
    current.descriptorBindingStorageTensorUpdateAfterBind = current.descriptorBindingStorageTensorUpdateAfterBind || merge_in.descriptorBindingStorageTensorUpdateAfterBind;
    current.tensors = current.tensors || merge_in.tensors;
}
#endif //(defined(VK_ARM_tensors))
#if (defined(VK_ARM_tensors))
void compare_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& supported, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& requested) {
    if (requested.descriptorBufferTensorDescriptors && !supported.descriptorBufferTensorDescriptors) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorBufferTensorFeaturesARM::descriptorBufferTensorDescriptors");
    }
}
void merge_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(VkPhysicalDeviceDescriptorBufferTensorFeaturesARM & current, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& merge_in) {
    current.descriptorBufferTensorDescriptors = current.descriptorBufferTensorDescriptors || merge_in.descriptorBufferTensorDescriptors;
}
#endif //(defined(VK_ARM_tensors))
#if (defined(VK_EXT_shader_module_identifier))
void compare_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& supported, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& requested) {
    if (requested.shaderModuleIdentifier && !supported.shaderModuleIdentifier) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT::shaderModuleIdentifier");
    }
}
void merge_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT & current, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& merge_in) {
    current.shaderModuleIdentifier = current.shaderModuleIdentifier || merge_in.shaderModuleIdentifier;
}
#endif //(defined(VK_EXT_shader_module_identifier))
#if (defined(VK_NV_optical_flow))
void compare_VkPhysicalDeviceOpticalFlowFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceOpticalFlowFeaturesNV const& supported, VkPhysicalDeviceOpticalFlowFeaturesNV const& requested) {
    if (requested.opticalFlow && !supported.opticalFlow) {
        error_list.push_back("Missing feature VkPhysicalDeviceOpticalFlowFeaturesNV::opticalFlow");
    }
}
void merge_VkPhysicalDeviceOpticalFlowFeaturesNV(VkPhysicalDeviceOpticalFlowFeaturesNV & current, VkPhysicalDeviceOpticalFlowFeaturesNV const& merge_in) {
    current.opticalFlow = current.opticalFlow || merge_in.opticalFlow;
}
#endif //(defined(VK_NV_optical_flow))
#if (defined(VK_EXT_legacy_dithering))
void compare_VkPhysicalDeviceLegacyDitheringFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& supported, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& requested) {
    if (requested.legacyDithering && !supported.legacyDithering) {
        error_list.push_back("Missing feature VkPhysicalDeviceLegacyDitheringFeaturesEXT::legacyDithering");
    }
}
void merge_VkPhysicalDeviceLegacyDitheringFeaturesEXT(VkPhysicalDeviceLegacyDitheringFeaturesEXT & current, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& merge_in) {
    current.legacyDithering = current.legacyDithering || merge_in.legacyDithering;
}
#endif //(defined(VK_EXT_legacy_dithering))
#if defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
void compare_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(std::vector<std::string> & error_list, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& supported, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& requested) {
    if (requested.externalFormatResolve && !supported.externalFormatResolve) {
        error_list.push_back("Missing feature VkPhysicalDeviceExternalFormatResolveFeaturesANDROID::externalFormatResolve");
    }
}
void merge_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(VkPhysicalDeviceExternalFormatResolveFeaturesANDROID & current, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& merge_in) {
    current.externalFormatResolve = current.externalFormatResolve || merge_in.externalFormatResolve;
}
#endif //defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
#if (defined(VK_AMD_anti_lag))
void compare_VkPhysicalDeviceAntiLagFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceAntiLagFeaturesAMD const& supported, VkPhysicalDeviceAntiLagFeaturesAMD const& requested) {
    if (requested.antiLag && !supported.antiLag) {
        error_list.push_back("Missing feature VkPhysicalDeviceAntiLagFeaturesAMD::antiLag");
    }
}
void merge_VkPhysicalDeviceAntiLagFeaturesAMD(VkPhysicalDeviceAntiLagFeaturesAMD & current, VkPhysicalDeviceAntiLagFeaturesAMD const& merge_in) {
    current.antiLag = current.antiLag || merge_in.antiLag;
}
#endif //(defined(VK_AMD_anti_lag))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
void compare_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(std::vector<std::string> & error_list, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& supported, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& requested) {
    if (requested.denseGeometryFormat && !supported.denseGeometryFormat) {
        error_list.push_back("Missing feature VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX::denseGeometryFormat");
    }
}
void merge_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX & current, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& merge_in) {
    current.denseGeometryFormat = current.denseGeometryFormat || merge_in.denseGeometryFormat;
}
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
#if (defined(VK_EXT_shader_object))
void compare_VkPhysicalDeviceShaderObjectFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderObjectFeaturesEXT const& supported, VkPhysicalDeviceShaderObjectFeaturesEXT const& requested) {
    if (requested.shaderObject && !supported.shaderObject) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderObjectFeaturesEXT::shaderObject");
    }
}
void merge_VkPhysicalDeviceShaderObjectFeaturesEXT(VkPhysicalDeviceShaderObjectFeaturesEXT & current, VkPhysicalDeviceShaderObjectFeaturesEXT const& merge_in) {
    current.shaderObject = current.shaderObject || merge_in.shaderObject;
}
#endif //(defined(VK_EXT_shader_object))
#if (defined(VK_QCOM_tile_properties))
void compare_VkPhysicalDeviceTilePropertiesFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& supported, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& requested) {
    if (requested.tileProperties && !supported.tileProperties) {
        error_list.push_back("Missing feature VkPhysicalDeviceTilePropertiesFeaturesQCOM::tileProperties");
    }
}
void merge_VkPhysicalDeviceTilePropertiesFeaturesQCOM(VkPhysicalDeviceTilePropertiesFeaturesQCOM & current, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& merge_in) {
    current.tileProperties = current.tileProperties || merge_in.tileProperties;
}
#endif //(defined(VK_QCOM_tile_properties))
#if (defined(VK_SEC_amigo_profiling))
void compare_VkPhysicalDeviceAmigoProfilingFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& supported, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& requested) {
    if (requested.amigoProfiling && !supported.amigoProfiling) {
        error_list.push_back("Missing feature VkPhysicalDeviceAmigoProfilingFeaturesSEC::amigoProfiling");
    }
}
void merge_VkPhysicalDeviceAmigoProfilingFeaturesSEC(VkPhysicalDeviceAmigoProfilingFeaturesSEC & current, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& merge_in) {
    current.amigoProfiling = current.amigoProfiling || merge_in.amigoProfiling;
}
#endif //(defined(VK_SEC_amigo_profiling))
#if (defined(VK_QCOM_multiview_per_view_viewports))
void compare_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& supported, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& requested) {
    if (requested.multiviewPerViewViewports && !supported.multiviewPerViewViewports) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM::multiviewPerViewViewports");
    }
}
void merge_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM & current, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& merge_in) {
    current.multiviewPerViewViewports = current.multiviewPerViewViewports || merge_in.multiviewPerViewViewports;
}
#endif //(defined(VK_QCOM_multiview_per_view_viewports))
#if (defined(VK_NV_ray_tracing_invocation_reorder))
void compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& supported, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& requested) {
    if (requested.rayTracingInvocationReorder && !supported.rayTracingInvocationReorder) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV::rayTracingInvocationReorder");
    }
}
void merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV & current, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& merge_in) {
    current.rayTracingInvocationReorder = current.rayTracingInvocationReorder || merge_in.rayTracingInvocationReorder;
}
#endif //(defined(VK_NV_ray_tracing_invocation_reorder))
#if (defined(VK_NV_cooperative_vector))
void compare_VkPhysicalDeviceCooperativeVectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeVectorFeaturesNV const& supported, VkPhysicalDeviceCooperativeVectorFeaturesNV const& requested) {
    if (requested.cooperativeVector && !supported.cooperativeVector) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeVectorFeaturesNV::cooperativeVector");
    }
    if (requested.cooperativeVectorTraining && !supported.cooperativeVectorTraining) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeVectorFeaturesNV::cooperativeVectorTraining");
    }
}
void merge_VkPhysicalDeviceCooperativeVectorFeaturesNV(VkPhysicalDeviceCooperativeVectorFeaturesNV & current, VkPhysicalDeviceCooperativeVectorFeaturesNV const& merge_in) {
    current.cooperativeVector = current.cooperativeVector || merge_in.cooperativeVector;
    current.cooperativeVectorTraining = current.cooperativeVectorTraining || merge_in.cooperativeVectorTraining;
}
#endif //(defined(VK_NV_cooperative_vector))
#if (defined(VK_NV_extended_sparse_address_space))
void compare_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& supported, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& requested) {
    if (requested.extendedSparseAddressSpace && !supported.extendedSparseAddressSpace) {
        error_list.push_back("Missing feature VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV::extendedSparseAddressSpace");
    }
}
void merge_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV & current, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& merge_in) {
    current.extendedSparseAddressSpace = current.extendedSparseAddressSpace || merge_in.extendedSparseAddressSpace;
}
#endif //(defined(VK_NV_extended_sparse_address_space))
#if (defined(VK_EXT_legacy_vertex_attributes))
void compare_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& supported, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& requested) {
    if (requested.legacyVertexAttributes && !supported.legacyVertexAttributes) {
        error_list.push_back("Missing feature VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT::legacyVertexAttributes");
    }
}
void merge_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT & current, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& merge_in) {
    current.legacyVertexAttributes = current.legacyVertexAttributes || merge_in.legacyVertexAttributes;
}
#endif //(defined(VK_EXT_legacy_vertex_attributes))
#if (defined(VK_ARM_shader_core_builtins))
void compare_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& supported, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& requested) {
    if (requested.shaderCoreBuiltins && !supported.shaderCoreBuiltins) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM::shaderCoreBuiltins");
    }
}
void merge_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM & current, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& merge_in) {
    current.shaderCoreBuiltins = current.shaderCoreBuiltins || merge_in.shaderCoreBuiltins;
}
#endif //(defined(VK_ARM_shader_core_builtins))
#if (defined(VK_EXT_pipeline_library_group_handles))
void compare_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& supported, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& requested) {
    if (requested.pipelineLibraryGroupHandles && !supported.pipelineLibraryGroupHandles) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT::pipelineLibraryGroupHandles");
    }
}
void merge_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT & current, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& merge_in) {
    current.pipelineLibraryGroupHandles = current.pipelineLibraryGroupHandles || merge_in.pipelineLibraryGroupHandles;
}
#endif //(defined(VK_EXT_pipeline_library_group_handles))
#if (defined(VK_EXT_dynamic_rendering_unused_attachments))
void compare_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& supported, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& requested) {
    if (requested.dynamicRenderingUnusedAttachments && !supported.dynamicRenderingUnusedAttachments) {
        error_list.push_back("Missing feature VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT::dynamicRenderingUnusedAttachments");
    }
}
void merge_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT & current, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& merge_in) {
    current.dynamicRenderingUnusedAttachments = current.dynamicRenderingUnusedAttachments || merge_in.dynamicRenderingUnusedAttachments;
}
#endif //(defined(VK_EXT_dynamic_rendering_unused_attachments))
#if (defined(VK_ARM_data_graph))
void compare_VkPhysicalDeviceDataGraphFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphFeaturesARM const& supported, VkPhysicalDeviceDataGraphFeaturesARM const& requested) {
    if (requested.dataGraph && !supported.dataGraph) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphFeaturesARM::dataGraph");
    }
    if (requested.dataGraphUpdateAfterBind && !supported.dataGraphUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphFeaturesARM::dataGraphUpdateAfterBind");
    }
    if (requested.dataGraphSpecializationConstants && !supported.dataGraphSpecializationConstants) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphFeaturesARM::dataGraphSpecializationConstants");
    }
    if (requested.dataGraphDescriptorBuffer && !supported.dataGraphDescriptorBuffer) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphFeaturesARM::dataGraphDescriptorBuffer");
    }
    if (requested.dataGraphShaderModule && !supported.dataGraphShaderModule) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphFeaturesARM::dataGraphShaderModule");
    }
}
void merge_VkPhysicalDeviceDataGraphFeaturesARM(VkPhysicalDeviceDataGraphFeaturesARM & current, VkPhysicalDeviceDataGraphFeaturesARM const& merge_in) {
    current.dataGraph = current.dataGraph || merge_in.dataGraph;
    current.dataGraphUpdateAfterBind = current.dataGraphUpdateAfterBind || merge_in.dataGraphUpdateAfterBind;
    current.dataGraphSpecializationConstants = current.dataGraphSpecializationConstants || merge_in.dataGraphSpecializationConstants;
    current.dataGraphDescriptorBuffer = current.dataGraphDescriptorBuffer || merge_in.dataGraphDescriptorBuffer;
    current.dataGraphShaderModule = current.dataGraphShaderModule || merge_in.dataGraphShaderModule;
}
#endif //(defined(VK_ARM_data_graph))
#if (defined(VK_QCOM_multiview_per_view_render_areas))
void compare_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& supported, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& requested) {
    if (requested.multiviewPerViewRenderAreas && !supported.multiviewPerViewRenderAreas) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM::multiviewPerViewRenderAreas");
    }
}
void merge_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM & current, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& merge_in) {
    current.multiviewPerViewRenderAreas = current.multiviewPerViewRenderAreas || merge_in.multiviewPerViewRenderAreas;
}
#endif //(defined(VK_QCOM_multiview_per_view_render_areas))
#if (defined(VK_NV_per_stage_descriptor_set))
void compare_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& supported, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& requested) {
    if (requested.perStageDescriptorSet && !supported.perStageDescriptorSet) {
        error_list.push_back("Missing feature VkPhysicalDevicePerStageDescriptorSetFeaturesNV::perStageDescriptorSet");
    }
    if (requested.dynamicPipelineLayout && !supported.dynamicPipelineLayout) {
        error_list.push_back("Missing feature VkPhysicalDevicePerStageDescriptorSetFeaturesNV::dynamicPipelineLayout");
    }
}
void merge_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(VkPhysicalDevicePerStageDescriptorSetFeaturesNV & current, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& merge_in) {
    current.perStageDescriptorSet = current.perStageDescriptorSet || merge_in.perStageDescriptorSet;
    current.dynamicPipelineLayout = current.dynamicPipelineLayout || merge_in.dynamicPipelineLayout;
}
#endif //(defined(VK_NV_per_stage_descriptor_set))
#if (defined(VK_QCOM_image_processing2))
void compare_VkPhysicalDeviceImageProcessing2FeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& supported, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& requested) {
    if (requested.textureBlockMatch2 && !supported.textureBlockMatch2) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageProcessing2FeaturesQCOM::textureBlockMatch2");
    }
}
void merge_VkPhysicalDeviceImageProcessing2FeaturesQCOM(VkPhysicalDeviceImageProcessing2FeaturesQCOM & current, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& merge_in) {
    current.textureBlockMatch2 = current.textureBlockMatch2 || merge_in.textureBlockMatch2;
}
#endif //(defined(VK_QCOM_image_processing2))
#if (defined(VK_QCOM_filter_cubic_weights))
void compare_VkPhysicalDeviceCubicWeightsFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& supported, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& requested) {
    if (requested.selectableCubicWeights && !supported.selectableCubicWeights) {
        error_list.push_back("Missing feature VkPhysicalDeviceCubicWeightsFeaturesQCOM::selectableCubicWeights");
    }
}
void merge_VkPhysicalDeviceCubicWeightsFeaturesQCOM(VkPhysicalDeviceCubicWeightsFeaturesQCOM & current, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& merge_in) {
    current.selectableCubicWeights = current.selectableCubicWeights || merge_in.selectableCubicWeights;
}
#endif //(defined(VK_QCOM_filter_cubic_weights))
#if (defined(VK_QCOM_ycbcr_degamma))
void compare_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& supported, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& requested) {
    if (requested.ycbcrDegamma && !supported.ycbcrDegamma) {
        error_list.push_back("Missing feature VkPhysicalDeviceYcbcrDegammaFeaturesQCOM::ycbcrDegamma");
    }
}
void merge_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(VkPhysicalDeviceYcbcrDegammaFeaturesQCOM & current, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& merge_in) {
    current.ycbcrDegamma = current.ycbcrDegamma || merge_in.ycbcrDegamma;
}
#endif //(defined(VK_QCOM_ycbcr_degamma))
#if (defined(VK_QCOM_filter_cubic_clamp))
void compare_VkPhysicalDeviceCubicClampFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCubicClampFeaturesQCOM const& supported, VkPhysicalDeviceCubicClampFeaturesQCOM const& requested) {
    if (requested.cubicRangeClamp && !supported.cubicRangeClamp) {
        error_list.push_back("Missing feature VkPhysicalDeviceCubicClampFeaturesQCOM::cubicRangeClamp");
    }
}
void merge_VkPhysicalDeviceCubicClampFeaturesQCOM(VkPhysicalDeviceCubicClampFeaturesQCOM & current, VkPhysicalDeviceCubicClampFeaturesQCOM const& merge_in) {
    current.cubicRangeClamp = current.cubicRangeClamp || merge_in.cubicRangeClamp;
}
#endif //(defined(VK_QCOM_filter_cubic_clamp))
#if (defined(VK_EXT_attachment_feedback_loop_dynamic_state))
void compare_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& requested) {
    if (requested.attachmentFeedbackLoopDynamicState && !supported.attachmentFeedbackLoopDynamicState) {
        error_list.push_back("Missing feature VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT::attachmentFeedbackLoopDynamicState");
    }
}
void merge_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT & current, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& merge_in) {
    current.attachmentFeedbackLoopDynamicState = current.attachmentFeedbackLoopDynamicState || merge_in.attachmentFeedbackLoopDynamicState;
}
#endif //(defined(VK_EXT_attachment_feedback_loop_dynamic_state))
#if defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
void compare_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(std::vector<std::string> & error_list, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& supported, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& requested) {
    if (requested.screenBufferImport && !supported.screenBufferImport) {
        error_list.push_back("Missing feature VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX::screenBufferImport");
    }
}
void merge_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX & current, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& merge_in) {
    current.screenBufferImport = current.screenBufferImport || merge_in.screenBufferImport;
}
#endif //defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
#if (defined(VK_NV_descriptor_pool_overallocation))
void compare_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& supported, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& requested) {
    if (requested.descriptorPoolOverallocation && !supported.descriptorPoolOverallocation) {
        error_list.push_back("Missing feature VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV::descriptorPoolOverallocation");
    }
}
void merge_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV & current, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& merge_in) {
    current.descriptorPoolOverallocation = current.descriptorPoolOverallocation || merge_in.descriptorPoolOverallocation;
}
#endif //(defined(VK_NV_descriptor_pool_overallocation))
#if (defined(VK_QCOM_tile_memory_heap))
void compare_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& supported, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& requested) {
    if (requested.tileMemoryHeap && !supported.tileMemoryHeap) {
        error_list.push_back("Missing feature VkPhysicalDeviceTileMemoryHeapFeaturesQCOM::tileMemoryHeap");
    }
}
void merge_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(VkPhysicalDeviceTileMemoryHeapFeaturesQCOM & current, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& merge_in) {
    current.tileMemoryHeap = current.tileMemoryHeap || merge_in.tileMemoryHeap;
}
#endif //(defined(VK_QCOM_tile_memory_heap))
#if (defined(VK_NV_raw_access_chains))
void compare_VkPhysicalDeviceRawAccessChainsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRawAccessChainsFeaturesNV const& supported, VkPhysicalDeviceRawAccessChainsFeaturesNV const& requested) {
    if (requested.shaderRawAccessChains && !supported.shaderRawAccessChains) {
        error_list.push_back("Missing feature VkPhysicalDeviceRawAccessChainsFeaturesNV::shaderRawAccessChains");
    }
}
void merge_VkPhysicalDeviceRawAccessChainsFeaturesNV(VkPhysicalDeviceRawAccessChainsFeaturesNV & current, VkPhysicalDeviceRawAccessChainsFeaturesNV const& merge_in) {
    current.shaderRawAccessChains = current.shaderRawAccessChains || merge_in.shaderRawAccessChains;
}
#endif //(defined(VK_NV_raw_access_chains))
#if (defined(VK_NV_command_buffer_inheritance))
void compare_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& supported, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& requested) {
    if (requested.commandBufferInheritance && !supported.commandBufferInheritance) {
        error_list.push_back("Missing feature VkPhysicalDeviceCommandBufferInheritanceFeaturesNV::commandBufferInheritance");
    }
}
void merge_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(VkPhysicalDeviceCommandBufferInheritanceFeaturesNV & current, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& merge_in) {
    current.commandBufferInheritance = current.commandBufferInheritance || merge_in.commandBufferInheritance;
}
#endif //(defined(VK_NV_command_buffer_inheritance))
#if (defined(VK_NV_shader_atomic_float16_vector))
void compare_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& supported, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& requested) {
    if (requested.shaderFloat16VectorAtomics && !supported.shaderFloat16VectorAtomics) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV::shaderFloat16VectorAtomics");
    }
}
void merge_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV & current, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& merge_in) {
    current.shaderFloat16VectorAtomics = current.shaderFloat16VectorAtomics || merge_in.shaderFloat16VectorAtomics;
}
#endif //(defined(VK_NV_shader_atomic_float16_vector))
#if (defined(VK_EXT_shader_replicated_composites))
void compare_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& supported, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& requested) {
    if (requested.shaderReplicatedComposites && !supported.shaderReplicatedComposites) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT::shaderReplicatedComposites");
    }
}
void merge_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT & current, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& merge_in) {
    current.shaderReplicatedComposites = current.shaderReplicatedComposites || merge_in.shaderReplicatedComposites;
}
#endif //(defined(VK_EXT_shader_replicated_composites))
#if (defined(VK_EXT_shader_float8))
void compare_VkPhysicalDeviceShaderFloat8FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat8FeaturesEXT const& supported, VkPhysicalDeviceShaderFloat8FeaturesEXT const& requested) {
    if (requested.shaderFloat8 && !supported.shaderFloat8) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat8FeaturesEXT::shaderFloat8");
    }
    if (requested.shaderFloat8CooperativeMatrix && !supported.shaderFloat8CooperativeMatrix) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderFloat8FeaturesEXT::shaderFloat8CooperativeMatrix");
    }
}
void merge_VkPhysicalDeviceShaderFloat8FeaturesEXT(VkPhysicalDeviceShaderFloat8FeaturesEXT & current, VkPhysicalDeviceShaderFloat8FeaturesEXT const& merge_in) {
    current.shaderFloat8 = current.shaderFloat8 || merge_in.shaderFloat8;
    current.shaderFloat8CooperativeMatrix = current.shaderFloat8CooperativeMatrix || merge_in.shaderFloat8CooperativeMatrix;
}
#endif //(defined(VK_EXT_shader_float8))
#if (defined(VK_NV_ray_tracing_validation))
void compare_VkPhysicalDeviceRayTracingValidationFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingValidationFeaturesNV const& supported, VkPhysicalDeviceRayTracingValidationFeaturesNV const& requested) {
    if (requested.rayTracingValidation && !supported.rayTracingValidation) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingValidationFeaturesNV::rayTracingValidation");
    }
}
void merge_VkPhysicalDeviceRayTracingValidationFeaturesNV(VkPhysicalDeviceRayTracingValidationFeaturesNV & current, VkPhysicalDeviceRayTracingValidationFeaturesNV const& merge_in) {
    current.rayTracingValidation = current.rayTracingValidation || merge_in.rayTracingValidation;
}
#endif //(defined(VK_NV_ray_tracing_validation))
#if (defined(VK_NV_cluster_acceleration_structure))
void compare_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& supported, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& requested) {
    if (requested.clusterAccelerationStructure && !supported.clusterAccelerationStructure) {
        error_list.push_back("Missing feature VkPhysicalDeviceClusterAccelerationStructureFeaturesNV::clusterAccelerationStructure");
    }
}
void merge_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(VkPhysicalDeviceClusterAccelerationStructureFeaturesNV & current, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& merge_in) {
    current.clusterAccelerationStructure = current.clusterAccelerationStructure || merge_in.clusterAccelerationStructure;
}
#endif //(defined(VK_NV_cluster_acceleration_structure))
#if (defined(VK_NV_partitioned_acceleration_structure))
void compare_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& supported, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& requested) {
    if (requested.partitionedAccelerationStructure && !supported.partitionedAccelerationStructure) {
        error_list.push_back("Missing feature VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV::partitionedAccelerationStructure");
    }
}
void merge_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV & current, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& merge_in) {
    current.partitionedAccelerationStructure = current.partitionedAccelerationStructure || merge_in.partitionedAccelerationStructure;
}
#endif //(defined(VK_NV_partitioned_acceleration_structure))
#if (defined(VK_EXT_device_generated_commands))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& supported, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& requested) {
    if (requested.deviceGeneratedCommands && !supported.deviceGeneratedCommands) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT::deviceGeneratedCommands");
    }
    if (requested.dynamicGeneratedPipelineLayout && !supported.dynamicGeneratedPipelineLayout) {
        error_list.push_back("Missing feature VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT::dynamicGeneratedPipelineLayout");
    }
}
void merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT & current, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& merge_in) {
    current.deviceGeneratedCommands = current.deviceGeneratedCommands || merge_in.deviceGeneratedCommands;
    current.dynamicGeneratedPipelineLayout = current.dynamicGeneratedPipelineLayout || merge_in.dynamicGeneratedPipelineLayout;
}
#endif //(defined(VK_EXT_device_generated_commands))
#if (defined(VK_MESA_image_alignment_control))
void compare_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(std::vector<std::string> & error_list, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& supported, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& requested) {
    if (requested.imageAlignmentControl && !supported.imageAlignmentControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageAlignmentControlFeaturesMESA::imageAlignmentControl");
    }
}
void merge_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(VkPhysicalDeviceImageAlignmentControlFeaturesMESA & current, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& merge_in) {
    current.imageAlignmentControl = current.imageAlignmentControl || merge_in.imageAlignmentControl;
}
#endif //(defined(VK_MESA_image_alignment_control))
#if (defined(VK_NV_push_constant_bank))
void compare_VkPhysicalDevicePushConstantBankFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePushConstantBankFeaturesNV const& supported, VkPhysicalDevicePushConstantBankFeaturesNV const& requested) {
    if (requested.pushConstantBank && !supported.pushConstantBank) {
        error_list.push_back("Missing feature VkPhysicalDevicePushConstantBankFeaturesNV::pushConstantBank");
    }
}
void merge_VkPhysicalDevicePushConstantBankFeaturesNV(VkPhysicalDevicePushConstantBankFeaturesNV & current, VkPhysicalDevicePushConstantBankFeaturesNV const& merge_in) {
    current.pushConstantBank = current.pushConstantBank || merge_in.pushConstantBank;
}
#endif //(defined(VK_NV_push_constant_bank))
#if (defined(VK_EXT_ray_tracing_invocation_reorder))
void compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& supported, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& requested) {
    if (requested.rayTracingInvocationReorder && !supported.rayTracingInvocationReorder) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT::rayTracingInvocationReorder");
    }
}
void merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT & current, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& merge_in) {
    current.rayTracingInvocationReorder = current.rayTracingInvocationReorder || merge_in.rayTracingInvocationReorder;
}
#endif //(defined(VK_EXT_ray_tracing_invocation_reorder))
#if (defined(VK_EXT_depth_clamp_control))
void compare_VkPhysicalDeviceDepthClampControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampControlFeaturesEXT const& supported, VkPhysicalDeviceDepthClampControlFeaturesEXT const& requested) {
    if (requested.depthClampControl && !supported.depthClampControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceDepthClampControlFeaturesEXT::depthClampControl");
    }
}
void merge_VkPhysicalDeviceDepthClampControlFeaturesEXT(VkPhysicalDeviceDepthClampControlFeaturesEXT & current, VkPhysicalDeviceDepthClampControlFeaturesEXT const& merge_in) {
    current.depthClampControl = current.depthClampControl || merge_in.depthClampControl;
}
#endif //(defined(VK_EXT_depth_clamp_control))
#if (defined(VK_HUAWEI_hdr_vivid))
void compare_VkPhysicalDeviceHdrVividFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& supported, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& requested) {
    if (requested.hdrVivid && !supported.hdrVivid) {
        error_list.push_back("Missing feature VkPhysicalDeviceHdrVividFeaturesHUAWEI::hdrVivid");
    }
}
void merge_VkPhysicalDeviceHdrVividFeaturesHUAWEI(VkPhysicalDeviceHdrVividFeaturesHUAWEI & current, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& merge_in) {
    current.hdrVivid = current.hdrVivid || merge_in.hdrVivid;
}
#endif //(defined(VK_HUAWEI_hdr_vivid))
#if (defined(VK_NV_cooperative_matrix2))
void compare_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& requested) {
    if (requested.cooperativeMatrixWorkgroupScope && !supported.cooperativeMatrixWorkgroupScope) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixWorkgroupScope");
    }
    if (requested.cooperativeMatrixFlexibleDimensions && !supported.cooperativeMatrixFlexibleDimensions) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixFlexibleDimensions");
    }
    if (requested.cooperativeMatrixReductions && !supported.cooperativeMatrixReductions) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixReductions");
    }
    if (requested.cooperativeMatrixConversions && !supported.cooperativeMatrixConversions) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixConversions");
    }
    if (requested.cooperativeMatrixPerElementOperations && !supported.cooperativeMatrixPerElementOperations) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixPerElementOperations");
    }
    if (requested.cooperativeMatrixTensorAddressing && !supported.cooperativeMatrixTensorAddressing) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixTensorAddressing");
    }
    if (requested.cooperativeMatrixBlockLoads && !supported.cooperativeMatrixBlockLoads) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrix2FeaturesNV::cooperativeMatrixBlockLoads");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(VkPhysicalDeviceCooperativeMatrix2FeaturesNV & current, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& merge_in) {
    current.cooperativeMatrixWorkgroupScope = current.cooperativeMatrixWorkgroupScope || merge_in.cooperativeMatrixWorkgroupScope;
    current.cooperativeMatrixFlexibleDimensions = current.cooperativeMatrixFlexibleDimensions || merge_in.cooperativeMatrixFlexibleDimensions;
    current.cooperativeMatrixReductions = current.cooperativeMatrixReductions || merge_in.cooperativeMatrixReductions;
    current.cooperativeMatrixConversions = current.cooperativeMatrixConversions || merge_in.cooperativeMatrixConversions;
    current.cooperativeMatrixPerElementOperations = current.cooperativeMatrixPerElementOperations || merge_in.cooperativeMatrixPerElementOperations;
    current.cooperativeMatrixTensorAddressing = current.cooperativeMatrixTensorAddressing || merge_in.cooperativeMatrixTensorAddressing;
    current.cooperativeMatrixBlockLoads = current.cooperativeMatrixBlockLoads || merge_in.cooperativeMatrixBlockLoads;
}
#endif //(defined(VK_NV_cooperative_matrix2))
#if (defined(VK_ARM_pipeline_opacity_micromap))
void compare_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& supported, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& requested) {
    if (requested.pipelineOpacityMicromap && !supported.pipelineOpacityMicromap) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineOpacityMicromapFeaturesARM::pipelineOpacityMicromap");
    }
}
void merge_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(VkPhysicalDevicePipelineOpacityMicromapFeaturesARM & current, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& merge_in) {
    current.pipelineOpacityMicromap = current.pipelineOpacityMicromap || merge_in.pipelineOpacityMicromap;
}
#endif //(defined(VK_ARM_pipeline_opacity_micromap))
#if (defined(VK_ARM_performance_counters_by_region))
void compare_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& supported, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& requested) {
    if (requested.performanceCountersByRegion && !supported.performanceCountersByRegion) {
        error_list.push_back("Missing feature VkPhysicalDevicePerformanceCountersByRegionFeaturesARM::performanceCountersByRegion");
    }
}
void merge_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(VkPhysicalDevicePerformanceCountersByRegionFeaturesARM & current, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& merge_in) {
    current.performanceCountersByRegion = current.performanceCountersByRegion || merge_in.performanceCountersByRegion;
}
#endif //(defined(VK_ARM_performance_counters_by_region))
#if (defined(VK_ARM_shader_instrumentation))
void compare_VkPhysicalDeviceShaderInstrumentationFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& supported, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& requested) {
    if (requested.shaderInstrumentation && !supported.shaderInstrumentation) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderInstrumentationFeaturesARM::shaderInstrumentation");
    }
}
void merge_VkPhysicalDeviceShaderInstrumentationFeaturesARM(VkPhysicalDeviceShaderInstrumentationFeaturesARM & current, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& merge_in) {
    current.shaderInstrumentation = current.shaderInstrumentation || merge_in.shaderInstrumentation;
}
#endif //(defined(VK_ARM_shader_instrumentation))
#if (defined(VK_EXT_vertex_attribute_robustness))
void compare_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& supported, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& requested) {
    if (requested.vertexAttributeRobustness && !supported.vertexAttributeRobustness) {
        error_list.push_back("Missing feature VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT::vertexAttributeRobustness");
    }
}
void merge_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT & current, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& merge_in) {
    current.vertexAttributeRobustness = current.vertexAttributeRobustness || merge_in.vertexAttributeRobustness;
}
#endif //(defined(VK_EXT_vertex_attribute_robustness))
#if (defined(VK_ARM_format_pack))
void compare_VkPhysicalDeviceFormatPackFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceFormatPackFeaturesARM const& supported, VkPhysicalDeviceFormatPackFeaturesARM const& requested) {
    if (requested.formatPack && !supported.formatPack) {
        error_list.push_back("Missing feature VkPhysicalDeviceFormatPackFeaturesARM::formatPack");
    }
}
void merge_VkPhysicalDeviceFormatPackFeaturesARM(VkPhysicalDeviceFormatPackFeaturesARM & current, VkPhysicalDeviceFormatPackFeaturesARM const& merge_in) {
    current.formatPack = current.formatPack || merge_in.formatPack;
}
#endif //(defined(VK_ARM_format_pack))
#if (defined(VK_VALVE_fragment_density_map_layered))
void compare_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& supported, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& requested) {
    if (requested.fragmentDensityMapLayered && !supported.fragmentDensityMapLayered) {
        error_list.push_back("Missing feature VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE::fragmentDensityMapLayered");
    }
}
void merge_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE & current, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& merge_in) {
    current.fragmentDensityMapLayered = current.fragmentDensityMapLayered || merge_in.fragmentDensityMapLayered;
}
#endif //(defined(VK_VALVE_fragment_density_map_layered))
#if (VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
void compare_VkPhysicalDevicePresentMeteringFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePresentMeteringFeaturesNV const& supported, VkPhysicalDevicePresentMeteringFeaturesNV const& requested) {
    if (requested.presentMetering && !supported.presentMetering) {
        error_list.push_back("Missing feature VkPhysicalDevicePresentMeteringFeaturesNV::presentMetering");
    }
}
void merge_VkPhysicalDevicePresentMeteringFeaturesNV(VkPhysicalDevicePresentMeteringFeaturesNV & current, VkPhysicalDevicePresentMeteringFeaturesNV const& merge_in) {
    current.presentMetering = current.presentMetering || merge_in.presentMetering;
}
#endif //(VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
#if (defined(VK_EXT_multisampled_render_to_swapchain))
void compare_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& supported, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& requested) {
    if (requested.multisampledRenderToSwapchain && !supported.multisampledRenderToSwapchain) {
        error_list.push_back("Missing feature VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT::multisampledRenderToSwapchain");
    }
}
void merge_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT & current, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& merge_in) {
    current.multisampledRenderToSwapchain = current.multisampledRenderToSwapchain || merge_in.multisampledRenderToSwapchain;
}
#endif //(defined(VK_EXT_multisampled_render_to_swapchain))
#if (defined(VK_EXT_zero_initialize_device_memory))
void compare_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& supported, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& requested) {
    if (requested.zeroInitializeDeviceMemory && !supported.zeroInitializeDeviceMemory) {
        error_list.push_back("Missing feature VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT::zeroInitializeDeviceMemory");
    }
}
void merge_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT & current, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& merge_in) {
    current.zeroInitializeDeviceMemory = current.zeroInitializeDeviceMemory || merge_in.zeroInitializeDeviceMemory;
}
#endif //(defined(VK_EXT_zero_initialize_device_memory))
#if (defined(VK_EXT_shader_64bit_indexing))
void compare_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& supported, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& requested) {
    if (requested.shader64BitIndexing && !supported.shader64BitIndexing) {
        error_list.push_back("Missing feature VkPhysicalDeviceShader64BitIndexingFeaturesEXT::shader64BitIndexing");
    }
}
void merge_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(VkPhysicalDeviceShader64BitIndexingFeaturesEXT & current, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& merge_in) {
    current.shader64BitIndexing = current.shader64BitIndexing || merge_in.shader64BitIndexing;
}
#endif //(defined(VK_EXT_shader_64bit_indexing))
#if (defined(VK_EXT_custom_resolve))
void compare_VkPhysicalDeviceCustomResolveFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCustomResolveFeaturesEXT const& supported, VkPhysicalDeviceCustomResolveFeaturesEXT const& requested) {
    if (requested.customResolve && !supported.customResolve) {
        error_list.push_back("Missing feature VkPhysicalDeviceCustomResolveFeaturesEXT::customResolve");
    }
}
void merge_VkPhysicalDeviceCustomResolveFeaturesEXT(VkPhysicalDeviceCustomResolveFeaturesEXT & current, VkPhysicalDeviceCustomResolveFeaturesEXT const& merge_in) {
    current.customResolve = current.customResolve || merge_in.customResolve;
}
#endif //(defined(VK_EXT_custom_resolve))
#if (defined(VK_QCOM_data_graph_model))
void compare_VkPhysicalDeviceDataGraphModelFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& supported, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& requested) {
    if (requested.dataGraphModel && !supported.dataGraphModel) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphModelFeaturesQCOM::dataGraphModel");
    }
}
void merge_VkPhysicalDeviceDataGraphModelFeaturesQCOM(VkPhysicalDeviceDataGraphModelFeaturesQCOM & current, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& merge_in) {
    current.dataGraphModel = current.dataGraphModel || merge_in.dataGraphModel;
}
#endif //(defined(VK_QCOM_data_graph_model))
#if (defined(VK_ARM_data_graph_optical_flow))
void compare_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& supported, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& requested) {
    if (requested.dataGraphOpticalFlow && !supported.dataGraphOpticalFlow) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM::dataGraphOpticalFlow");
    }
}
void merge_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM & current, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& merge_in) {
    current.dataGraphOpticalFlow = current.dataGraphOpticalFlow || merge_in.dataGraphOpticalFlow;
}
#endif //(defined(VK_ARM_data_graph_optical_flow))
#if (defined(VK_EXT_shader_long_vector))
void compare_VkPhysicalDeviceShaderLongVectorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& supported, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& requested) {
    if (requested.longVector && !supported.longVector) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderLongVectorFeaturesEXT::longVector");
    }
}
void merge_VkPhysicalDeviceShaderLongVectorFeaturesEXT(VkPhysicalDeviceShaderLongVectorFeaturesEXT & current, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& merge_in) {
    current.longVector = current.longVector || merge_in.longVector;
}
#endif //(defined(VK_EXT_shader_long_vector))
#if (defined(VK_SEC_pipeline_cache_incremental_mode))
void compare_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& supported, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& requested) {
    if (requested.pipelineCacheIncrementalMode && !supported.pipelineCacheIncrementalMode) {
        error_list.push_back("Missing feature VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC::pipelineCacheIncrementalMode");
    }
}
void merge_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC & current, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& merge_in) {
    current.pipelineCacheIncrementalMode = current.pipelineCacheIncrementalMode || merge_in.pipelineCacheIncrementalMode;
}
#endif //(defined(VK_SEC_pipeline_cache_incremental_mode))
#if (defined(VK_EXT_shader_uniform_buffer_unsized_array))
void compare_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& supported, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& requested) {
    if (requested.shaderUniformBufferUnsizedArray && !supported.shaderUniformBufferUnsizedArray) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT::shaderUniformBufferUnsizedArray");
    }
}
void merge_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT & current, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& merge_in) {
    current.shaderUniformBufferUnsizedArray = current.shaderUniformBufferUnsizedArray || merge_in.shaderUniformBufferUnsizedArray;
}
#endif //(defined(VK_EXT_shader_uniform_buffer_unsized_array))
#if (defined(VK_NV_compute_occupancy_priority))
void compare_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& supported, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& requested) {
    if (requested.computeOccupancyPriority && !supported.computeOccupancyPriority) {
        error_list.push_back("Missing feature VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV::computeOccupancyPriority");
    }
}
void merge_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV & current, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& merge_in) {
    current.computeOccupancyPriority = current.computeOccupancyPriority || merge_in.computeOccupancyPriority;
}
#endif //(defined(VK_NV_compute_occupancy_priority))
#if (defined(VK_EXT_cooperative_matrix_maintenance1))
void compare_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& supported, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& requested) {
    if (requested.cooperativeMatrixProperties2 && !supported.cooperativeMatrixProperties2) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT::cooperativeMatrixProperties2");
    }
    if (requested.cooperativeMatrixReductions && !supported.cooperativeMatrixReductions) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT::cooperativeMatrixReductions");
    }
    if (requested.cooperativeMatrixConversions && !supported.cooperativeMatrixConversions) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT::cooperativeMatrixConversions");
    }
    if (requested.cooperativeMatrixPerElementOperations && !supported.cooperativeMatrixPerElementOperations) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT::cooperativeMatrixPerElementOperations");
    }
    if (requested.cooperativeMatrixGetCoordinate && !supported.cooperativeMatrixGetCoordinate) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT::cooperativeMatrixGetCoordinate");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT & current, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& merge_in) {
    current.cooperativeMatrixProperties2 = current.cooperativeMatrixProperties2 || merge_in.cooperativeMatrixProperties2;
    current.cooperativeMatrixReductions = current.cooperativeMatrixReductions || merge_in.cooperativeMatrixReductions;
    current.cooperativeMatrixConversions = current.cooperativeMatrixConversions || merge_in.cooperativeMatrixConversions;
    current.cooperativeMatrixPerElementOperations = current.cooperativeMatrixPerElementOperations || merge_in.cooperativeMatrixPerElementOperations;
    current.cooperativeMatrixGetCoordinate = current.cooperativeMatrixGetCoordinate || merge_in.cooperativeMatrixGetCoordinate;
}
#endif //(defined(VK_EXT_cooperative_matrix_maintenance1))
#if (defined(VK_EXT_shader_subgroup_partitioned))
void compare_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& supported, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& requested) {
    if (requested.shaderSubgroupPartitioned && !supported.shaderSubgroupPartitioned) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT::shaderSubgroupPartitioned");
    }
}
void merge_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT & current, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& merge_in) {
    current.shaderSubgroupPartitioned = current.shaderSubgroupPartitioned || merge_in.shaderSubgroupPartitioned;
}
#endif //(defined(VK_EXT_shader_subgroup_partitioned))
#if (defined(VK_EXT_shader_ocp_microscaling_types))
void compare_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& supported, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& requested) {
    if (requested.shaderFloat4 && !supported.shaderFloat4) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT::shaderFloat4");
    }
    if (requested.shaderFloat6 && !supported.shaderFloat6) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT::shaderFloat6");
    }
    if (requested.shaderFloat8UnsignedE8M0 && !supported.shaderFloat8UnsignedE8M0) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT::shaderFloat8UnsignedE8M0");
    }
    if (requested.shaderMXInt8 && !supported.shaderMXInt8) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT::shaderMXInt8");
    }
}
void merge_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT & current, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& merge_in) {
    current.shaderFloat4 = current.shaderFloat4 || merge_in.shaderFloat4;
    current.shaderFloat6 = current.shaderFloat6 || merge_in.shaderFloat6;
    current.shaderFloat8UnsignedE8M0 = current.shaderFloat8UnsignedE8M0 || merge_in.shaderFloat8UnsignedE8M0;
    current.shaderMXInt8 = current.shaderMXInt8 || merge_in.shaderMXInt8;
}
#endif //(defined(VK_EXT_shader_ocp_microscaling_types))
#if (defined(VK_VALVE_shader_mixed_float_dot_product))
void compare_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& supported, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& requested) {
    if (requested.shaderMixedFloatDotProductFloat16AccFloat32 && !supported.shaderMixedFloatDotProductFloat16AccFloat32) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE::shaderMixedFloatDotProductFloat16AccFloat32");
    }
    if (requested.shaderMixedFloatDotProductFloat16AccFloat16 && !supported.shaderMixedFloatDotProductFloat16AccFloat16) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE::shaderMixedFloatDotProductFloat16AccFloat16");
    }
    if (requested.shaderMixedFloatDotProductBFloat16Acc && !supported.shaderMixedFloatDotProductBFloat16Acc) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE::shaderMixedFloatDotProductBFloat16Acc");
    }
    if (requested.shaderMixedFloatDotProductFloat8AccFloat32 && !supported.shaderMixedFloatDotProductFloat8AccFloat32) {
        error_list.push_back("Missing feature VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE::shaderMixedFloatDotProductFloat8AccFloat32");
    }
}
void merge_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE & current, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& merge_in) {
    current.shaderMixedFloatDotProductFloat16AccFloat32 = current.shaderMixedFloatDotProductFloat16AccFloat32 || merge_in.shaderMixedFloatDotProductFloat16AccFloat32;
    current.shaderMixedFloatDotProductFloat16AccFloat16 = current.shaderMixedFloatDotProductFloat16AccFloat16 || merge_in.shaderMixedFloatDotProductFloat16AccFloat16;
    current.shaderMixedFloatDotProductBFloat16Acc = current.shaderMixedFloatDotProductBFloat16Acc || merge_in.shaderMixedFloatDotProductBFloat16Acc;
    current.shaderMixedFloatDotProductFloat8AccFloat32 = current.shaderMixedFloatDotProductFloat8AccFloat32 || merge_in.shaderMixedFloatDotProductFloat8AccFloat32;
}
#endif //(defined(VK_VALVE_shader_mixed_float_dot_product))
#if (defined(VK_SEC_throttle_hint))
void compare_VkPhysicalDeviceThrottleHintFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDeviceThrottleHintFeaturesSEC const& supported, VkPhysicalDeviceThrottleHintFeaturesSEC const& requested) {
    if (requested.throttleHint && !supported.throttleHint) {
        error_list.push_back("Missing feature VkPhysicalDeviceThrottleHintFeaturesSEC::throttleHint");
    }
}
void merge_VkPhysicalDeviceThrottleHintFeaturesSEC(VkPhysicalDeviceThrottleHintFeaturesSEC & current, VkPhysicalDeviceThrottleHintFeaturesSEC const& merge_in) {
    current.throttleHint = current.throttleHint || merge_in.throttleHint;
}
#endif //(defined(VK_SEC_throttle_hint))
#if (defined(VK_ARM_data_graph_neural_accelerator_statistics))
void compare_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& supported, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& requested) {
    if (requested.dataGraphNeuralAcceleratorStatistics && !supported.dataGraphNeuralAcceleratorStatistics) {
        error_list.push_back("Missing feature VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM::dataGraphNeuralAcceleratorStatistics");
    }
}
void merge_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM & current, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& merge_in) {
    current.dataGraphNeuralAcceleratorStatistics = current.dataGraphNeuralAcceleratorStatistics || merge_in.dataGraphNeuralAcceleratorStatistics;
}
#endif //(defined(VK_ARM_data_graph_neural_accelerator_statistics))
#if (defined(VK_EXT_primitive_restart_index))
void compare_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& supported, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& requested) {
    if (requested.primitiveRestartIndex && !supported.primitiveRestartIndex) {
        error_list.push_back("Missing feature VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT::primitiveRestartIndex");
    }
}
void merge_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT & current, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& merge_in) {
    current.primitiveRestartIndex = current.primitiveRestartIndex || merge_in.primitiveRestartIndex;
}
#endif //(defined(VK_EXT_primitive_restart_index))
#if (defined(VK_EXT_image_tiling_control))
void compare_VkPhysicalDeviceImageTilingControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageTilingControlFeaturesEXT const& supported, VkPhysicalDeviceImageTilingControlFeaturesEXT const& requested) {
    if (requested.imageTilingControl && !supported.imageTilingControl) {
        error_list.push_back("Missing feature VkPhysicalDeviceImageTilingControlFeaturesEXT::imageTilingControl");
    }
}
void merge_VkPhysicalDeviceImageTilingControlFeaturesEXT(VkPhysicalDeviceImageTilingControlFeaturesEXT & current, VkPhysicalDeviceImageTilingControlFeaturesEXT const& merge_in) {
    current.imageTilingControl = current.imageTilingControl || merge_in.imageTilingControl;
}
#endif //(defined(VK_EXT_image_tiling_control))
#if (defined(VK_NV_cooperative_matrix_decode_vector))
void compare_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& requested) {
    if (requested.cooperativeMatrixDecodeVector && !supported.cooperativeMatrixDecodeVector) {
        error_list.push_back("Missing feature VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV::cooperativeMatrixDecodeVector");
    }
}
void merge_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV & current, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& merge_in) {
    current.cooperativeMatrixDecodeVector = current.cooperativeMatrixDecodeVector || merge_in.cooperativeMatrixDecodeVector;
}
#endif //(defined(VK_NV_cooperative_matrix_decode_vector))
#if (defined(VK_KHR_acceleration_structure))
void compare_VkPhysicalDeviceAccelerationStructureFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& supported, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& requested) {
    if (requested.accelerationStructure && !supported.accelerationStructure) {
        error_list.push_back("Missing feature VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure");
    }
    if (requested.accelerationStructureCaptureReplay && !supported.accelerationStructureCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructureCaptureReplay");
    }
    if (requested.accelerationStructureIndirectBuild && !supported.accelerationStructureIndirectBuild) {
        error_list.push_back("Missing feature VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructureIndirectBuild");
    }
    if (requested.accelerationStructureHostCommands && !supported.accelerationStructureHostCommands) {
        error_list.push_back("Missing feature VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructureHostCommands");
    }
    if (requested.descriptorBindingAccelerationStructureUpdateAfterBind && !supported.descriptorBindingAccelerationStructureUpdateAfterBind) {
        error_list.push_back("Missing feature VkPhysicalDeviceAccelerationStructureFeaturesKHR::descriptorBindingAccelerationStructureUpdateAfterBind");
    }
}
void merge_VkPhysicalDeviceAccelerationStructureFeaturesKHR(VkPhysicalDeviceAccelerationStructureFeaturesKHR & current, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& merge_in) {
    current.accelerationStructure = current.accelerationStructure || merge_in.accelerationStructure;
    current.accelerationStructureCaptureReplay = current.accelerationStructureCaptureReplay || merge_in.accelerationStructureCaptureReplay;
    current.accelerationStructureIndirectBuild = current.accelerationStructureIndirectBuild || merge_in.accelerationStructureIndirectBuild;
    current.accelerationStructureHostCommands = current.accelerationStructureHostCommands || merge_in.accelerationStructureHostCommands;
    current.descriptorBindingAccelerationStructureUpdateAfterBind = current.descriptorBindingAccelerationStructureUpdateAfterBind || merge_in.descriptorBindingAccelerationStructureUpdateAfterBind;
}
#endif //(defined(VK_KHR_acceleration_structure))
#if (defined(VK_KHR_ray_tracing_pipeline))
void compare_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& supported, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& requested) {
    if (requested.rayTracingPipeline && !supported.rayTracingPipeline) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline");
    }
    if (requested.rayTracingPipelineShaderGroupHandleCaptureReplay && !supported.rayTracingPipelineShaderGroupHandleCaptureReplay) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipelineShaderGroupHandleCaptureReplay");
    }
    if (requested.rayTracingPipelineShaderGroupHandleCaptureReplayMixed && !supported.rayTracingPipelineShaderGroupHandleCaptureReplayMixed) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipelineShaderGroupHandleCaptureReplayMixed");
    }
    if (requested.rayTracingPipelineTraceRaysIndirect && !supported.rayTracingPipelineTraceRaysIndirect) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipelineTraceRaysIndirect");
    }
    if (requested.rayTraversalPrimitiveCulling && !supported.rayTraversalPrimitiveCulling) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTraversalPrimitiveCulling");
    }
}
void merge_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(VkPhysicalDeviceRayTracingPipelineFeaturesKHR & current, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& merge_in) {
    current.rayTracingPipeline = current.rayTracingPipeline || merge_in.rayTracingPipeline;
    current.rayTracingPipelineShaderGroupHandleCaptureReplay = current.rayTracingPipelineShaderGroupHandleCaptureReplay || merge_in.rayTracingPipelineShaderGroupHandleCaptureReplay;
    current.rayTracingPipelineShaderGroupHandleCaptureReplayMixed = current.rayTracingPipelineShaderGroupHandleCaptureReplayMixed || merge_in.rayTracingPipelineShaderGroupHandleCaptureReplayMixed;
    current.rayTracingPipelineTraceRaysIndirect = current.rayTracingPipelineTraceRaysIndirect || merge_in.rayTracingPipelineTraceRaysIndirect;
    current.rayTraversalPrimitiveCulling = current.rayTraversalPrimitiveCulling || merge_in.rayTraversalPrimitiveCulling;
}
#endif //(defined(VK_KHR_ray_tracing_pipeline))
#if (defined(VK_KHR_ray_query))
void compare_VkPhysicalDeviceRayQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayQueryFeaturesKHR const& supported, VkPhysicalDeviceRayQueryFeaturesKHR const& requested) {
    if (requested.rayQuery && !supported.rayQuery) {
        error_list.push_back("Missing feature VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery");
    }
}
void merge_VkPhysicalDeviceRayQueryFeaturesKHR(VkPhysicalDeviceRayQueryFeaturesKHR & current, VkPhysicalDeviceRayQueryFeaturesKHR const& merge_in) {
    current.rayQuery = current.rayQuery || merge_in.rayQuery;
}
#endif //(defined(VK_KHR_ray_query))
#if (defined(VK_EXT_mesh_shader))
void compare_VkPhysicalDeviceMeshShaderFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMeshShaderFeaturesEXT const& supported, VkPhysicalDeviceMeshShaderFeaturesEXT const& requested) {
    if (requested.taskShader && !supported.taskShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesEXT::taskShader");
    }
    if (requested.meshShader && !supported.meshShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesEXT::meshShader");
    }
    if (requested.multiviewMeshShader && !supported.multiviewMeshShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesEXT::multiviewMeshShader");
    }
    if (requested.primitiveFragmentShadingRateMeshShader && !supported.primitiveFragmentShadingRateMeshShader) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesEXT::primitiveFragmentShadingRateMeshShader");
    }
    if (requested.meshShaderQueries && !supported.meshShaderQueries) {
        error_list.push_back("Missing feature VkPhysicalDeviceMeshShaderFeaturesEXT::meshShaderQueries");
    }
}
void merge_VkPhysicalDeviceMeshShaderFeaturesEXT(VkPhysicalDeviceMeshShaderFeaturesEXT & current, VkPhysicalDeviceMeshShaderFeaturesEXT const& merge_in) {
    current.taskShader = current.taskShader || merge_in.taskShader;
    current.meshShader = current.meshShader || merge_in.meshShader;
    current.multiviewMeshShader = current.multiviewMeshShader || merge_in.multiviewMeshShader;
    current.primitiveFragmentShadingRateMeshShader = current.primitiveFragmentShadingRateMeshShader || merge_in.primitiveFragmentShadingRateMeshShader;
    current.meshShaderQueries = current.meshShaderQueries || merge_in.meshShaderQueries;
}
#endif //(defined(VK_EXT_mesh_shader))
void compare_feature_struct(VkStructureType sType, std::vector<std::string> & error_list, const void* supported, const void* requested) {
    switch (sType) {
#if (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES):
            compare_VkPhysicalDeviceProtectedMemoryFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceProtectedMemoryFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceProtectedMemoryFeatures*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES):
            compare_VkPhysicalDevice16BitStorageFeatures(error_list, *reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR):
            compare_VkPhysicalDevice16BitStorageFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevice16BitStorageFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevice16BitStorageFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES):
            compare_VkPhysicalDeviceVariablePointersFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES):
            compare_VkPhysicalDeviceVariablePointerFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES):
            compare_VkPhysicalDeviceVariablePointerFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES):
            compare_VkPhysicalDeviceVariablePointersFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES):
            compare_VkPhysicalDeviceSamplerYcbcrConversionFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR):
            compare_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES):
            compare_VkPhysicalDeviceMultiviewFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR):
            compare_VkPhysicalDeviceMultiviewFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMultiviewFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultiviewFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES):
            compare_VkPhysicalDeviceShaderDrawParametersFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES):
            compare_VkPhysicalDeviceShaderDrawParameterFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderDrawParameterFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderDrawParameterFeatures*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES):
            compare_VkPhysicalDeviceVulkan11Features(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES):
            compare_VkPhysicalDeviceVulkan12Features(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkan12Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkan12Features*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES):
            compare_VkPhysicalDeviceVulkanMemoryModelFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR):
            compare_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES):
            compare_VkPhysicalDeviceHostQueryResetFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT):
            compare_VkPhysicalDeviceHostQueryResetFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES):
            compare_VkPhysicalDeviceTimelineSemaphoreFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR):
            compare_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES):
            compare_VkPhysicalDeviceBufferDeviceAddressFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR):
            compare_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES):
            compare_VkPhysicalDevice8BitStorageFeatures(error_list, *reinterpret_cast<const VkPhysicalDevice8BitStorageFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevice8BitStorageFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR):
            compare_VkPhysicalDevice8BitStorageFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevice8BitStorageFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevice8BitStorageFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES):
            compare_VkPhysicalDeviceShaderAtomicInt64Features(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES):
            compare_VkPhysicalDeviceShaderFloat16Int8Features(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR):
            compare_VkPhysicalDeviceFloat16Int8FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceFloat16Int8FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceFloat16Int8FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES):
            compare_VkPhysicalDeviceDescriptorIndexingFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT):
            compare_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES):
            compare_VkPhysicalDeviceScalarBlockLayoutFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT):
            compare_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES):
            compare_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR):
            compare_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES):
            compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES):
            compare_VkPhysicalDeviceImagelessFramebufferFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR):
            compare_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES):
            compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR):
            compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES):
            compare_VkPhysicalDeviceVulkan13Features(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkan13Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkan13Features*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES):
            compare_VkPhysicalDevicePrivateDataFeatures(error_list, *reinterpret_cast<const VkPhysicalDevicePrivateDataFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevicePrivateDataFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT):
            compare_VkPhysicalDevicePrivateDataFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePrivateDataFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePrivateDataFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES):
            compare_VkPhysicalDeviceSynchronization2Features(error_list, *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR):
            compare_VkPhysicalDeviceSynchronization2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceSynchronization2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceSynchronization2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES):
            compare_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT):
            compare_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES):
            compare_VkPhysicalDeviceMaintenance4Features(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance4Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance4Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance4FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance4FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance4FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES):
            compare_VkPhysicalDeviceShaderTerminateInvocationFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES):
            compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES):
            compare_VkPhysicalDevicePipelineCreationCacheControlFeatures(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES):
            compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR):
            compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES):
            compare_VkPhysicalDeviceImageRobustnessFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT):
            compare_VkPhysicalDeviceImageRobustnessFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES):
            compare_VkPhysicalDeviceSubgroupSizeControlFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES):
            compare_VkPhysicalDeviceInlineUniformBlockFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT):
            compare_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES):
            compare_VkPhysicalDeviceShaderIntegerDotProductFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES):
            compare_VkPhysicalDeviceDynamicRenderingFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR):
            compare_VkPhysicalDeviceDynamicRenderingFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES):
            compare_VkPhysicalDeviceVulkan14Features(error_list, *reinterpret_cast<const VkPhysicalDeviceVulkan14Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceVulkan14Features*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES):
            compare_VkPhysicalDeviceGlobalPriorityQueryFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_KHR):
            compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_KHR):
            compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES):
            compare_VkPhysicalDeviceIndexTypeUint8Features(error_list, *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT):
            compare_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT):
            compare_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES):
            compare_VkPhysicalDeviceMaintenance5Features(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance5Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance5Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance5FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance5FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance5FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES):
            compare_VkPhysicalDeviceMaintenance6Features(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance6Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance6Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance6FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance6FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance6FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES):
            compare_VkPhysicalDeviceHostImageCopyFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES_EXT):
            compare_VkPhysicalDeviceHostImageCopyFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES):
            compare_VkPhysicalDeviceShaderSubgroupRotateFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES):
            compare_VkPhysicalDeviceShaderFloatControls2Features(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2Features*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2Features*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES):
            compare_VkPhysicalDeviceShaderExpectAssumeFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES):
            compare_VkPhysicalDevicePipelineProtectedAccessFeatures(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES_EXT):
            compare_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES):
            compare_VkPhysicalDevicePipelineRobustnessFeatures(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeatures*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES_EXT):
            compare_VkPhysicalDevicePipelineRobustnessFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES):
            compare_VkPhysicalDeviceLineRasterizationFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT):
            compare_VkPhysicalDeviceLineRasterizationFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT):
            compare_VkPhysicalDeviceLineRasterizationFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES):
            compare_VkPhysicalDeviceVertexAttributeDivisorFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT):
            compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT):
            compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES):
            compare_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(error_list, *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(supported), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(requested));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES_KHR):
            compare_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_performance_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR):
            compare_VkPhysicalDevicePerformanceQueryFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePerformanceQueryFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePerformanceQueryFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_bfloat16))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderBfloat16FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderBfloat16FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderBfloat16FeaturesKHR*>(requested));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR):
            compare_VkPhysicalDevicePortabilitySubsetFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_clock))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderClockFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderClockFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderClockFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_fragment_shading_rate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR):
            compare_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_constant_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderConstantDataFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderConstantDataFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderConstantDataFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_abort))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderAbortFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAbortFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAbortFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_quad_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderQuadControlFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderQuadControlFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderQuadControlFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_present_wait))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR):
            compare_VkPhysicalDevicePresentWaitFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePresentWaitFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentWaitFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_pipeline_executable_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR):
            compare_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_present_id))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR):
            compare_VkPhysicalDevicePresentIdFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePresentIdFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentIdFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_device_address_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR):
            compare_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_fragment_shader_barycentric))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR):
            compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR*>(requested));
            break;
#elif (defined(VK_NV_fragment_shader_barycentric))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV):
            compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_subgroup_uniform_control_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_workgroup_memory_explicit_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR):
            compare_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR):
            compare_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_untyped_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_maximal_reconvergence))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_present_id2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR):
            compare_VkPhysicalDevicePresentId2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePresentId2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentId2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_present_wait2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR):
            compare_VkPhysicalDevicePresentWait2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePresentWait2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentWait2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_position_fetch))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR):
            compare_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_pipeline_binary))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR):
            compare_VkPhysicalDevicePipelineBinaryFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineBinaryFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineBinaryFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_swapchain_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR):
            compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR*>(requested));
            break;
#elif (defined(VK_EXT_swapchain_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT):
            compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_KHR_internally_synchronized_queues))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR):
            compare_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_cooperative_matrix))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR):
            compare_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_compute_shader_derivatives))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR):
            compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR*>(requested));
            break;
#elif (defined(VK_NV_compute_shader_derivatives))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV):
            compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_encode_av1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_decode_vp9))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance1FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance1FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_unified_image_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR):
            compare_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_copy_memory_indirect))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR):
            compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_encode_intra_refresh))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_encode_quantization_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_relaxed_extended_instruction))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_maintenance7))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance7FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance7FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance7FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_device_fault))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR):
            compare_VkPhysicalDeviceFaultFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_maintenance8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance8FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance8FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance8FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_shader_fma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR):
            compare_VkPhysicalDeviceShaderFmaFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFmaFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFmaFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_maintenance9))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance9FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance9FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance9FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_maintenance2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_video_encode_feedback2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR):
            compare_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_depth_clamp_zero_one))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR):
            compare_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR*>(requested));
            break;
#elif (defined(VK_EXT_depth_clamp_zero_one))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_EXT):
            compare_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_KHR_robustness2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR):
            compare_VkPhysicalDeviceRobustness2FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesKHR*>(requested));
            break;
#elif (defined(VK_EXT_robustness2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT):
            compare_VkPhysicalDeviceRobustness2FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_KHR_present_mode_fifo_latest_ready))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR):
            compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR*>(requested));
            break;
#elif (defined(VK_EXT_present_mode_fifo_latest_ready))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_EXT):
            compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_KHR_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR):
            compare_VkPhysicalDeviceOpacityMicromapFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_maintenance10))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance10FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance10FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance10FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_maintenance11))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR):
            compare_VkPhysicalDeviceMaintenance11FeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceMaintenance11FeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceMaintenance11FeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_extended_flags))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR):
            compare_VkPhysicalDeviceExtendedFlagsFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceExtendedFlagsFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceExtendedFlagsFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_EXT_transform_feedback))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT):
            compare_VkPhysicalDeviceTransformFeedbackFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceTransformFeedbackFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceTransformFeedbackFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_corner_sampled_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV):
            compare_VkPhysicalDeviceCornerSampledImageFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCornerSampledImageFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCornerSampledImageFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_astc_decode_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT):
            compare_VkPhysicalDeviceASTCDecodeFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceASTCDecodeFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceASTCDecodeFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_conditional_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT):
            compare_VkPhysicalDeviceConditionalRenderingFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceConditionalRenderingFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceConditionalRenderingFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_depth_clip_enable))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT):
            compare_VkPhysicalDeviceDepthClipEnableFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_IMG_relaxed_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG):
            compare_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(error_list, *reinterpret_cast<const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG*>(supported), *reinterpret_cast<const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG*>(requested));
            break;
#endif
#if (defined(VK_AMD_gpa_interface))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD):
            compare_VkPhysicalDeviceGpaFeaturesAMD(error_list, *reinterpret_cast<const VkPhysicalDeviceGpaFeaturesAMD*>(supported), *reinterpret_cast<const VkPhysicalDeviceGpaFeaturesAMD*>(requested));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ENQUEUE_FEATURES_AMDX):
            compare_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderEnqueueFeaturesAMDX*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderEnqueueFeaturesAMDX*>(requested));
            break;
#endif
#if (defined(VK_EXT_descriptor_heap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT):
            compare_VkPhysicalDeviceDescriptorHeapFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorHeapFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorHeapFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_blend_operation_advanced))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT):
            compare_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_shader_sm_builtins))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV):
            compare_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_shading_rate_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV):
            compare_VkPhysicalDeviceShadingRateImageFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceShadingRateImageFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceShadingRateImageFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_representative_fragment_test))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV):
            compare_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_QCOM_cooperative_matrix_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM):
            compare_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_elapsed_timer_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM):
            compare_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_NV_mesh_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV):
            compare_VkPhysicalDeviceMeshShaderFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_shader_image_footprint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV):
            compare_VkPhysicalDeviceShaderImageFootprintFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderImageFootprintFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderImageFootprintFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_scissor_exclusive))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV):
            compare_VkPhysicalDeviceExclusiveScissorFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceExclusiveScissorFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceExclusiveScissorFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_present_timing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT):
            compare_VkPhysicalDevicePresentTimingFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePresentTimingFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentTimingFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_INTEL_shader_integer_functions2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL):
            compare_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL*>(requested));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT):
            compare_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_AMD_device_coherent_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD):
            compare_VkPhysicalDeviceCoherentMemoryFeaturesAMD(error_list, *reinterpret_cast<const VkPhysicalDeviceCoherentMemoryFeaturesAMD*>(supported), *reinterpret_cast<const VkPhysicalDeviceCoherentMemoryFeaturesAMD*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_image_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_memory_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT):
            compare_VkPhysicalDeviceMemoryPriorityFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMemoryPriorityFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMemoryPriorityFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_dedicated_allocation_image_aliasing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV):
            compare_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT):
            compare_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT*>(requested));
            break;
#elif (defined(VK_EXT_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT):
            compare_VkPhysicalDeviceBufferAddressFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceBufferAddressFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceBufferAddressFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV):
            compare_VkPhysicalDeviceCooperativeMatrixFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_coverage_reduction_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV):
            compare_VkPhysicalDeviceCoverageReductionModeFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCoverageReductionModeFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCoverageReductionModeFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_fragment_shader_interlock))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT):
            compare_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_ycbcr_image_arrays))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT):
            compare_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_provoking_vertex))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT):
            compare_VkPhysicalDeviceProvokingVertexFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceProvokingVertexFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceProvokingVertexFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_atomic_float))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT):
            compare_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_map_memory_placed))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT):
            compare_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_atomic_float2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_device_generated_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV):
            compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_inherited_viewport_scissor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV):
            compare_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceInheritedViewportScissorFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceInheritedViewportScissorFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_texel_buffer_alignment))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT):
            compare_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_depth_bias_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceDepthBiasControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthBiasControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthBiasControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_device_memory_report))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT):
            compare_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_custom_border_color))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT):
            compare_VkPhysicalDeviceCustomBorderColorFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_texture_compression_astc_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT):
            compare_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_present_barrier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV):
            compare_VkPhysicalDevicePresentBarrierFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDevicePresentBarrierFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentBarrierFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_device_diagnostics_config))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV):
            compare_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDiagnosticsConfigFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDiagnosticsConfigFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_QCOM_queue_perf_hint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM):
            compare_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceQueuePerfHintFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceQueuePerfHintFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_image_processing3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM):
            compare_VkPhysicalDeviceImageProcessing3FeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceImageProcessing3FeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageProcessing3FeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_shader_multiple_wait_queues))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM):
            compare_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_split_barrier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT*>(requested));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUDA_KERNEL_LAUNCH_FEATURES_NV):
            compare_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCudaKernelLaunchFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCudaKernelLaunchFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_QCOM_tile_shading))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM):
            compare_VkPhysicalDeviceTileShadingFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceTileShadingFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceTileShadingFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_EXT_descriptor_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT):
            compare_VkPhysicalDeviceDescriptorBufferFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_graphics_pipeline_library))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT):
            compare_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_AMD_shader_early_and_late_fragment_tests))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD):
            compare_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD*>(requested));
            break;
#endif
#if (defined(VK_NV_fragment_shading_rate_enums))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV):
            compare_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_ray_tracing_motion_blur))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV):
            compare_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_ycbcr_2plane_444_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT):
            compare_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT):
            compare_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_compression_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceImageCompressionControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_attachment_feedback_loop_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT):
            compare_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_4444_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT):
            compare_VkPhysicalDevice4444FormatsFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevice4444FormatsFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevice4444FormatsFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_device_fault))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT):
            compare_VkPhysicalDeviceFaultFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_rasterization_order_attachment_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT):
            compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT*>(requested));
            break;
#elif (defined(VK_ARM_rasterization_order_attachment_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM):
            compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_rgba10x6_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT):
            compare_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_mutable_descriptor_type))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT):
            compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT*>(requested));
            break;
#elif (defined(VK_VALVE_mutable_descriptor_type))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE):
            compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(error_list, *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE*>(supported), *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE*>(requested));
            break;
#endif
#if (defined(VK_EXT_vertex_input_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT):
            compare_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_device_address_binding_report))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT):
            compare_VkPhysicalDeviceAddressBindingReportFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceAddressBindingReportFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceAddressBindingReportFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_depth_clip_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceDepthClipControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthClipControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthClipControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_primitive_topology_list_restart))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT):
            compare_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_HUAWEI_subpass_shading))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI):
            compare_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(error_list, *reinterpret_cast<const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI*>(supported), *reinterpret_cast<const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI*>(requested));
            break;
#endif
#if (defined(VK_HUAWEI_invocation_mask))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI):
            compare_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(error_list, *reinterpret_cast<const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI*>(supported), *reinterpret_cast<const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI*>(requested));
            break;
#endif
#if (defined(VK_NV_external_memory_rdma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV):
            compare_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_pipeline_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT):
            compare_VkPhysicalDevicePipelinePropertiesFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePipelinePropertiesFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelinePropertiesFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_frame_boundary))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT):
            compare_VkPhysicalDeviceFrameBoundaryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFrameBoundaryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFrameBoundaryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_multisampled_render_to_single_sampled))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT):
            compare_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT):
            compare_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_color_write_enable))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT):
            compare_VkPhysicalDeviceColorWriteEnableFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_primitives_generated_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT):
            compare_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VALVE_video_encode_rgb_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE):
            compare_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(error_list, *reinterpret_cast<const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE*>(supported), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_view_min_lod))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT):
            compare_VkPhysicalDeviceImageViewMinLodFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageViewMinLodFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageViewMinLodFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_multi_draw))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT):
            compare_VkPhysicalDeviceMultiDrawFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMultiDrawFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultiDrawFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_2d_view_of_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT):
            compare_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_tile_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderTileImageFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderTileImageFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderTileImageFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT):
            compare_VkPhysicalDeviceOpacityMicromapFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesEXT*>(requested));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISPLACEMENT_MICROMAP_FEATURES_NV):
            compare_VkPhysicalDeviceDisplacementMicromapFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDisplacementMicromapFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDisplacementMicromapFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_HUAWEI_cluster_culling_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI):
            compare_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(error_list, *reinterpret_cast<const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI*>(supported), *reinterpret_cast<const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI*>(requested));
            break;
#endif
#if (defined(VK_EXT_border_color_swizzle))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT):
            compare_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_pageable_device_local_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT):
            compare_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_scheduling_controls))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM):
            compare_VkPhysicalDeviceSchedulingControlsFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceSchedulingControlsFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceSchedulingControlsFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_sliced_view_of_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT):
            compare_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VALVE_descriptor_set_host_mapping))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE):
            compare_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE*>(requested));
            break;
#endif
#if (defined(VK_EXT_non_seamless_cube_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT):
            compare_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_render_pass_striped))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM):
            compare_VkPhysicalDeviceRenderPassStripedFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceRenderPassStripedFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceRenderPassStripedFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map_offset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT):
            compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT*>(requested));
            break;
#elif (defined(VK_QCOM_fragment_density_map_offset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM):
            compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_NV_copy_memory_indirect))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV):
            compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_memory_decompression))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT):
            compare_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesEXT*>(requested));
            break;
#elif (defined(VK_NV_memory_decompression))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_NV):
            compare_VkPhysicalDeviceMemoryDecompressionFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_device_generated_commands_compute))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV):
            compare_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_ray_tracing_linear_swept_spheres))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV):
            compare_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_linear_color_attachment))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV):
            compare_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceLinearColorAttachmentFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceLinearColorAttachmentFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_compression_control_swapchain))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT):
            compare_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_QCOM_image_processing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM):
            compare_VkPhysicalDeviceImageProcessingFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceImageProcessingFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageProcessingFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_EXT_nested_command_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT):
            compare_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceNestedCommandBufferFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceNestedCommandBufferFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT):
            compare_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_subpass_merge_feedback))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT):
            compare_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_tensors))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM):
            compare_VkPhysicalDeviceTensorFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceTensorFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceTensorFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_ARM_tensors))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM):
            compare_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_module_identifier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_optical_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV):
            compare_VkPhysicalDeviceOpticalFlowFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceOpticalFlowFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceOpticalFlowFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_legacy_dithering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT):
            compare_VkPhysicalDeviceLegacyDitheringFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceLegacyDitheringFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceLegacyDitheringFeaturesEXT*>(requested));
            break;
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID):
            compare_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(error_list, *reinterpret_cast<const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID*>(supported), *reinterpret_cast<const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID*>(requested));
            break;
#endif
#if (defined(VK_AMD_anti_lag))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD):
            compare_VkPhysicalDeviceAntiLagFeaturesAMD(error_list, *reinterpret_cast<const VkPhysicalDeviceAntiLagFeaturesAMD*>(supported), *reinterpret_cast<const VkPhysicalDeviceAntiLagFeaturesAMD*>(requested));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DENSE_GEOMETRY_FORMAT_FEATURES_AMDX):
            compare_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(error_list, *reinterpret_cast<const VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX*>(supported), *reinterpret_cast<const VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_object))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderObjectFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderObjectFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderObjectFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_QCOM_tile_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM):
            compare_VkPhysicalDeviceTilePropertiesFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceTilePropertiesFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceTilePropertiesFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_SEC_amigo_profiling))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC):
            compare_VkPhysicalDeviceAmigoProfilingFeaturesSEC(error_list, *reinterpret_cast<const VkPhysicalDeviceAmigoProfilingFeaturesSEC*>(supported), *reinterpret_cast<const VkPhysicalDeviceAmigoProfilingFeaturesSEC*>(requested));
            break;
#endif
#if (defined(VK_QCOM_multiview_per_view_viewports))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM):
            compare_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_NV_ray_tracing_invocation_reorder))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV):
            compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_cooperative_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV):
            compare_VkPhysicalDeviceCooperativeVectorFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeVectorFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeVectorFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_extended_sparse_address_space))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV):
            compare_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_legacy_vertex_attributes))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT):
            compare_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_shader_core_builtins))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM):
            compare_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_pipeline_library_group_handles))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT):
            compare_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_dynamic_rendering_unused_attachments))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT):
            compare_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_data_graph))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM):
            compare_VkPhysicalDeviceDataGraphFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceDataGraphFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceDataGraphFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_multiview_per_view_render_areas))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM):
            compare_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_NV_per_stage_descriptor_set))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV):
            compare_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDevicePerStageDescriptorSetFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDevicePerStageDescriptorSetFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_QCOM_image_processing2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM):
            compare_VkPhysicalDeviceImageProcessing2FeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceImageProcessing2FeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageProcessing2FeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_filter_cubic_weights))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM):
            compare_VkPhysicalDeviceCubicWeightsFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceCubicWeightsFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceCubicWeightsFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_ycbcr_degamma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM):
            compare_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_QCOM_filter_cubic_clamp))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM):
            compare_VkPhysicalDeviceCubicClampFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceCubicClampFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceCubicClampFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_EXT_attachment_feedback_loop_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT):
            compare_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT*>(requested));
            break;
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX):
            compare_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(error_list, *reinterpret_cast<const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(supported), *reinterpret_cast<const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(requested));
            break;
#endif
#if (defined(VK_NV_descriptor_pool_overallocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV):
            compare_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_QCOM_tile_memory_heap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM):
            compare_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_NV_raw_access_chains))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV):
            compare_VkPhysicalDeviceRawAccessChainsFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRawAccessChainsFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRawAccessChainsFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_command_buffer_inheritance))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV):
            compare_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_shader_atomic_float16_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV):
            compare_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_replicated_composites))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_float8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderFloat8FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderFloat8FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderFloat8FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_ray_tracing_validation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV):
            compare_VkPhysicalDeviceRayTracingValidationFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingValidationFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingValidationFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_cluster_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV):
            compare_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_NV_partitioned_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV):
            compare_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_device_generated_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT):
            compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_MESA_image_alignment_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA):
            compare_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(error_list, *reinterpret_cast<const VkPhysicalDeviceImageAlignmentControlFeaturesMESA*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageAlignmentControlFeaturesMESA*>(requested));
            break;
#endif
#if (defined(VK_NV_push_constant_bank))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV):
            compare_VkPhysicalDevicePushConstantBankFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDevicePushConstantBankFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDevicePushConstantBankFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_ray_tracing_invocation_reorder))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT):
            compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_depth_clamp_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceDepthClampControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceDepthClampControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceDepthClampControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_HUAWEI_hdr_vivid))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI):
            compare_VkPhysicalDeviceHdrVividFeaturesHUAWEI(error_list, *reinterpret_cast<const VkPhysicalDeviceHdrVividFeaturesHUAWEI*>(supported), *reinterpret_cast<const VkPhysicalDeviceHdrVividFeaturesHUAWEI*>(requested));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV):
            compare_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrix2FeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrix2FeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_ARM_pipeline_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM):
            compare_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_ARM_performance_counters_by_region))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM):
            compare_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_ARM_shader_instrumentation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM):
            compare_VkPhysicalDeviceShaderInstrumentationFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderInstrumentationFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderInstrumentationFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_vertex_attribute_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT):
            compare_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_ARM_format_pack))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM):
            compare_VkPhysicalDeviceFormatPackFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceFormatPackFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceFormatPackFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_VALVE_fragment_density_map_layered))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE):
            compare_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(error_list, *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE*>(supported), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE*>(requested));
            break;
#endif
#if (VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV):
            compare_VkPhysicalDevicePresentMeteringFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDevicePresentMeteringFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDevicePresentMeteringFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_multisampled_render_to_swapchain))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT):
            compare_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_zero_initialize_device_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT):
            compare_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_64bit_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT):
            compare_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShader64BitIndexingFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShader64BitIndexingFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_custom_resolve))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT):
            compare_VkPhysicalDeviceCustomResolveFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceCustomResolveFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceCustomResolveFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_QCOM_data_graph_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM):
            compare_VkPhysicalDeviceDataGraphModelFeaturesQCOM(error_list, *reinterpret_cast<const VkPhysicalDeviceDataGraphModelFeaturesQCOM*>(supported), *reinterpret_cast<const VkPhysicalDeviceDataGraphModelFeaturesQCOM*>(requested));
            break;
#endif
#if (defined(VK_ARM_data_graph_optical_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM):
            compare_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_long_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderLongVectorFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderLongVectorFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderLongVectorFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_SEC_pipeline_cache_incremental_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC):
            compare_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(error_list, *reinterpret_cast<const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC*>(supported), *reinterpret_cast<const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_uniform_buffer_unsized_array))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_compute_occupancy_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV):
            compare_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_EXT_cooperative_matrix_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_MAINTENANCE_1_FEATURES_EXT):
            compare_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_subgroup_partitioned))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_shader_ocp_microscaling_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OCP_MICROSCALING_TYPES_FEATURES_EXT):
            compare_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_VALVE_shader_mixed_float_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE):
            compare_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(error_list, *reinterpret_cast<const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE*>(supported), *reinterpret_cast<const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE*>(requested));
            break;
#endif
#if (defined(VK_SEC_throttle_hint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC):
            compare_VkPhysicalDeviceThrottleHintFeaturesSEC(error_list, *reinterpret_cast<const VkPhysicalDeviceThrottleHintFeaturesSEC*>(supported), *reinterpret_cast<const VkPhysicalDeviceThrottleHintFeaturesSEC*>(requested));
            break;
#endif
#if (defined(VK_ARM_data_graph_neural_accelerator_statistics))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM):
            compare_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(error_list, *reinterpret_cast<const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM*>(supported), *reinterpret_cast<const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM*>(requested));
            break;
#endif
#if (defined(VK_EXT_primitive_restart_index))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT):
            compare_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_EXT_image_tiling_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_TILING_CONTROL_FEATURES_EXT):
            compare_VkPhysicalDeviceImageTilingControlFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceImageTilingControlFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceImageTilingControlFeaturesEXT*>(requested));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix_decode_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV):
            compare_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(error_list, *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV*>(supported), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV*>(requested));
            break;
#endif
#if (defined(VK_KHR_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR):
            compare_VkPhysicalDeviceAccelerationStructureFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceAccelerationStructureFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceAccelerationStructureFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_pipeline))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR):
            compare_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_KHR_ray_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR):
            compare_VkPhysicalDeviceRayQueryFeaturesKHR(error_list, *reinterpret_cast<const VkPhysicalDeviceRayQueryFeaturesKHR*>(supported), *reinterpret_cast<const VkPhysicalDeviceRayQueryFeaturesKHR*>(requested));
            break;
#endif
#if (defined(VK_EXT_mesh_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT):
            compare_VkPhysicalDeviceMeshShaderFeaturesEXT(error_list, *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesEXT*>(supported), *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesEXT*>(requested));
            break;
#endif
        default:
            break;
    }
}
void merge_feature_struct(VkStructureType sType, void* current, const void* merge_in) {
    switch (sType) {
#if (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES):
            merge_VkPhysicalDeviceProtectedMemoryFeatures(*reinterpret_cast<VkPhysicalDeviceProtectedMemoryFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceProtectedMemoryFeatures*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES):
            merge_VkPhysicalDevice16BitStorageFeatures(*reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(current), *reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR):
            merge_VkPhysicalDevice16BitStorageFeaturesKHR(*reinterpret_cast<VkPhysicalDevice16BitStorageFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevice16BitStorageFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES):
            merge_VkPhysicalDeviceVariablePointersFeatures(*reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES):
            merge_VkPhysicalDeviceVariablePointerFeatures(*reinterpret_cast<VkPhysicalDeviceVariablePointerFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES):
            merge_VkPhysicalDeviceVariablePointerFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVariablePointerFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVariablePointerFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES):
            merge_VkPhysicalDeviceVariablePointersFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVariablePointersFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES):
            merge_VkPhysicalDeviceSamplerYcbcrConversionFeatures(*reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR):
            merge_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES):
            merge_VkPhysicalDeviceMultiviewFeatures(*reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR):
            merge_VkPhysicalDeviceMultiviewFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMultiviewFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMultiviewFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES):
            merge_VkPhysicalDeviceShaderDrawParametersFeatures(*reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES):
            merge_VkPhysicalDeviceShaderDrawParameterFeatures(*reinterpret_cast<VkPhysicalDeviceShaderDrawParameterFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderDrawParameterFeatures*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES):
            merge_VkPhysicalDeviceVulkan11Features(*reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES):
            merge_VkPhysicalDeviceVulkan12Features(*reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkan12Features*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES):
            merge_VkPhysicalDeviceVulkanMemoryModelFeatures(*reinterpret_cast<VkPhysicalDeviceVulkanMemoryModelFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR):
            merge_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVulkanMemoryModelFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES):
            merge_VkPhysicalDeviceHostQueryResetFeatures(*reinterpret_cast<VkPhysicalDeviceHostQueryResetFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT):
            merge_VkPhysicalDeviceHostQueryResetFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceHostQueryResetFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES):
            merge_VkPhysicalDeviceTimelineSemaphoreFeatures(*reinterpret_cast<VkPhysicalDeviceTimelineSemaphoreFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR):
            merge_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceTimelineSemaphoreFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES):
            merge_VkPhysicalDeviceBufferDeviceAddressFeatures(*reinterpret_cast<VkPhysicalDeviceBufferDeviceAddressFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR):
            merge_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceBufferDeviceAddressFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES):
            merge_VkPhysicalDevice8BitStorageFeatures(*reinterpret_cast<VkPhysicalDevice8BitStorageFeatures*>(current), *reinterpret_cast<const VkPhysicalDevice8BitStorageFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR):
            merge_VkPhysicalDevice8BitStorageFeaturesKHR(*reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevice8BitStorageFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES):
            merge_VkPhysicalDeviceShaderAtomicInt64Features(*reinterpret_cast<VkPhysicalDeviceShaderAtomicInt64Features*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderAtomicInt64FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES):
            merge_VkPhysicalDeviceShaderFloat16Int8Features(*reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8Features*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR):
            merge_VkPhysicalDeviceFloat16Int8FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceFloat16Int8FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceFloat16Int8FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES):
            merge_VkPhysicalDeviceDescriptorIndexingFeatures(*reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT):
            merge_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES):
            merge_VkPhysicalDeviceScalarBlockLayoutFeatures(*reinterpret_cast<VkPhysicalDeviceScalarBlockLayoutFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT):
            merge_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceScalarBlockLayoutFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES):
            merge_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(*reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR):
            merge_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES):
            merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES):
            merge_VkPhysicalDeviceImagelessFramebufferFeatures(*reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR):
            merge_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES):
            merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(*reinterpret_cast<VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR):
            merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES):
            merge_VkPhysicalDeviceVulkan13Features(*reinterpret_cast<VkPhysicalDeviceVulkan13Features*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkan13Features*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES):
            merge_VkPhysicalDevicePrivateDataFeatures(*reinterpret_cast<VkPhysicalDevicePrivateDataFeatures*>(current), *reinterpret_cast<const VkPhysicalDevicePrivateDataFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT):
            merge_VkPhysicalDevicePrivateDataFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePrivateDataFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePrivateDataFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES):
            merge_VkPhysicalDeviceSynchronization2Features(*reinterpret_cast<VkPhysicalDeviceSynchronization2Features*>(current), *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR):
            merge_VkPhysicalDeviceSynchronization2FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceSynchronization2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceSynchronization2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES):
            merge_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(*reinterpret_cast<VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT):
            merge_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES):
            merge_VkPhysicalDeviceMaintenance4Features(*reinterpret_cast<VkPhysicalDeviceMaintenance4Features*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance4Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance4FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance4FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance4FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES):
            merge_VkPhysicalDeviceShaderTerminateInvocationFeatures(*reinterpret_cast<VkPhysicalDeviceShaderTerminateInvocationFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES):
            merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(*reinterpret_cast<VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES):
            merge_VkPhysicalDevicePipelineCreationCacheControlFeatures(*reinterpret_cast<VkPhysicalDevicePipelineCreationCacheControlFeatures*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES):
            merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(*reinterpret_cast<VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR):
            merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES):
            merge_VkPhysicalDeviceImageRobustnessFeatures(*reinterpret_cast<VkPhysicalDeviceImageRobustnessFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT):
            merge_VkPhysicalDeviceImageRobustnessFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageRobustnessFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES):
            merge_VkPhysicalDeviceSubgroupSizeControlFeatures(*reinterpret_cast<VkPhysicalDeviceSubgroupSizeControlFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceSubgroupSizeControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES):
            merge_VkPhysicalDeviceInlineUniformBlockFeatures(*reinterpret_cast<VkPhysicalDeviceInlineUniformBlockFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT):
            merge_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceInlineUniformBlockFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES):
            merge_VkPhysicalDeviceShaderIntegerDotProductFeatures(*reinterpret_cast<VkPhysicalDeviceShaderIntegerDotProductFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES):
            merge_VkPhysicalDeviceDynamicRenderingFeatures(*reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR):
            merge_VkPhysicalDeviceDynamicRenderingFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES):
            merge_VkPhysicalDeviceVulkan14Features(*reinterpret_cast<VkPhysicalDeviceVulkan14Features*>(current), *reinterpret_cast<const VkPhysicalDeviceVulkan14Features*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES):
            merge_VkPhysicalDeviceGlobalPriorityQueryFeatures(*reinterpret_cast<VkPhysicalDeviceGlobalPriorityQueryFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_KHR):
            merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_KHR):
            merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES):
            merge_VkPhysicalDeviceIndexTypeUint8Features(*reinterpret_cast<VkPhysicalDeviceIndexTypeUint8Features*>(current), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT):
            merge_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceIndexTypeUint8FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT):
            merge_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceIndexTypeUint8FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES):
            merge_VkPhysicalDeviceMaintenance5Features(*reinterpret_cast<VkPhysicalDeviceMaintenance5Features*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance5Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance5FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance5FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance5FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES):
            merge_VkPhysicalDeviceMaintenance6Features(*reinterpret_cast<VkPhysicalDeviceMaintenance6Features*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance6Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance6FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance6FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance6FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES):
            merge_VkPhysicalDeviceHostImageCopyFeatures(*reinterpret_cast<VkPhysicalDeviceHostImageCopyFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES_EXT):
            merge_VkPhysicalDeviceHostImageCopyFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceHostImageCopyFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES):
            merge_VkPhysicalDeviceShaderSubgroupRotateFeatures(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupRotateFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES):
            merge_VkPhysicalDeviceShaderFloatControls2Features(*reinterpret_cast<VkPhysicalDeviceShaderFloatControls2Features*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2Features*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderFloatControls2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES):
            merge_VkPhysicalDeviceShaderExpectAssumeFeatures(*reinterpret_cast<VkPhysicalDeviceShaderExpectAssumeFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderExpectAssumeFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES):
            merge_VkPhysicalDevicePipelineProtectedAccessFeatures(*reinterpret_cast<VkPhysicalDevicePipelineProtectedAccessFeatures*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES_EXT):
            merge_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePipelineProtectedAccessFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES):
            merge_VkPhysicalDevicePipelineRobustnessFeatures(*reinterpret_cast<VkPhysicalDevicePipelineRobustnessFeatures*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES_EXT):
            merge_VkPhysicalDevicePipelineRobustnessFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePipelineRobustnessFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES):
            merge_VkPhysicalDeviceLineRasterizationFeatures(*reinterpret_cast<VkPhysicalDeviceLineRasterizationFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT):
            merge_VkPhysicalDeviceLineRasterizationFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceLineRasterizationFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT):
            merge_VkPhysicalDeviceLineRasterizationFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceLineRasterizationFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES):
            merge_VkPhysicalDeviceVertexAttributeDivisorFeatures(*reinterpret_cast<VkPhysicalDeviceVertexAttributeDivisorFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT):
            merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT):
            merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES):
            merge_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(*reinterpret_cast<VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(current), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(merge_in));
            break;
#elif (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES_KHR):
            merge_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_performance_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR):
            merge_VkPhysicalDevicePerformanceQueryFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePerformanceQueryFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePerformanceQueryFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_bfloat16))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderBfloat16FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderBfloat16FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderBfloat16FeaturesKHR*>(merge_in));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR):
            merge_VkPhysicalDevicePortabilitySubsetFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_clock))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderClockFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderClockFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderClockFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_fragment_shading_rate))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR):
            merge_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_constant_data))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderConstantDataFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderConstantDataFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderConstantDataFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_abort))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderAbortFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderAbortFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAbortFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_quad_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderQuadControlFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderQuadControlFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderQuadControlFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_present_wait))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR):
            merge_VkPhysicalDevicePresentWaitFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePresentWaitFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePresentWaitFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_pipeline_executable_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR):
            merge_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_present_id))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR):
            merge_VkPhysicalDevicePresentIdFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePresentIdFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePresentIdFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_device_address_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR):
            merge_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_fragment_shader_barycentric))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR):
            merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_NV_fragment_shader_barycentric))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV):
            merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(*reinterpret_cast<VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_subgroup_uniform_control_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_workgroup_memory_explicit_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR):
            merge_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR):
            merge_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_untyped_pointers))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderUntypedPointersFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_maximal_reconvergence))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_present_id2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR):
            merge_VkPhysicalDevicePresentId2FeaturesKHR(*reinterpret_cast<VkPhysicalDevicePresentId2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePresentId2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_present_wait2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR):
            merge_VkPhysicalDevicePresentWait2FeaturesKHR(*reinterpret_cast<VkPhysicalDevicePresentWait2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePresentWait2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_position_fetch))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR):
            merge_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_pipeline_binary))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR):
            merge_VkPhysicalDevicePipelineBinaryFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePipelineBinaryFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineBinaryFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_swapchain_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR):
            merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_EXT_swapchain_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT):
            merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_internally_synchronized_queues))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR):
            merge_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_cooperative_matrix))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR):
            merge_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrixFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_compute_shader_derivatives))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR):
            merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_NV_compute_shader_derivatives))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV):
            merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(*reinterpret_cast<VkPhysicalDeviceComputeShaderDerivativesFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceComputeShaderDerivativesFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_encode_av1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoEncodeAV1FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_decode_vp9))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoDecodeVP9FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoMaintenance1FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance1FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_unified_image_layouts))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR):
            merge_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_copy_memory_indirect))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR):
            merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_encode_intra_refresh))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_encode_quantization_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_relaxed_extended_instruction))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_maintenance7))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance7FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance7FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance7FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_device_fault))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR):
            merge_VkPhysicalDeviceFaultFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceFaultFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_maintenance8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance8FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance8FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance8FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_shader_fma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR):
            merge_VkPhysicalDeviceShaderFmaFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceShaderFmaFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFmaFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_maintenance9))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance9FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance9FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance9FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_maintenance2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoMaintenance2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoMaintenance2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_video_encode_feedback2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR):
            merge_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_depth_clamp_zero_one))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR):
            merge_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceDepthClampZeroOneFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_EXT_depth_clamp_zero_one))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_EXT):
            merge_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDepthClampZeroOneFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthClampZeroOneFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_robustness2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR):
            merge_VkPhysicalDeviceRobustness2FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceRobustness2FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_EXT_robustness2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT):
            merge_VkPhysicalDeviceRobustness2FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceRobustness2FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_present_mode_fifo_latest_ready))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR):
            merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(*reinterpret_cast<VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR*>(merge_in));
            break;
#elif (defined(VK_EXT_present_mode_fifo_latest_ready))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_EXT):
            merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR):
            merge_VkPhysicalDeviceOpacityMicromapFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceOpacityMicromapFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_maintenance10))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance10FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance10FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance10FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_maintenance11))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR):
            merge_VkPhysicalDeviceMaintenance11FeaturesKHR(*reinterpret_cast<VkPhysicalDeviceMaintenance11FeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceMaintenance11FeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_extended_flags))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR):
            merge_VkPhysicalDeviceExtendedFlagsFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceExtendedFlagsFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceExtendedFlagsFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_transform_feedback))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT):
            merge_VkPhysicalDeviceTransformFeedbackFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceTransformFeedbackFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceTransformFeedbackFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_corner_sampled_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV):
            merge_VkPhysicalDeviceCornerSampledImageFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCornerSampledImageFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCornerSampledImageFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_astc_decode_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT):
            merge_VkPhysicalDeviceASTCDecodeFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceASTCDecodeFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceASTCDecodeFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_conditional_rendering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT):
            merge_VkPhysicalDeviceConditionalRenderingFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceConditionalRenderingFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceConditionalRenderingFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_depth_clip_enable))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT):
            merge_VkPhysicalDeviceDepthClipEnableFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_IMG_relaxed_line_rasterization))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG):
            merge_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(*reinterpret_cast<VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG*>(current), *reinterpret_cast<const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG*>(merge_in));
            break;
#endif
#if (defined(VK_AMD_gpa_interface))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD):
            merge_VkPhysicalDeviceGpaFeaturesAMD(*reinterpret_cast<VkPhysicalDeviceGpaFeaturesAMD*>(current), *reinterpret_cast<const VkPhysicalDeviceGpaFeaturesAMD*>(merge_in));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ENQUEUE_FEATURES_AMDX):
            merge_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(*reinterpret_cast<VkPhysicalDeviceShaderEnqueueFeaturesAMDX*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderEnqueueFeaturesAMDX*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_descriptor_heap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT):
            merge_VkPhysicalDeviceDescriptorHeapFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDescriptorHeapFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorHeapFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_blend_operation_advanced))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT):
            merge_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_shader_sm_builtins))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV):
            merge_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(*reinterpret_cast<VkPhysicalDeviceShaderSMBuiltinsFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_shading_rate_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV):
            merge_VkPhysicalDeviceShadingRateImageFeaturesNV(*reinterpret_cast<VkPhysicalDeviceShadingRateImageFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceShadingRateImageFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_representative_fragment_test))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV):
            merge_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_cooperative_matrix_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM):
            merge_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_elapsed_timer_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM):
            merge_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_NV_mesh_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV):
            merge_VkPhysicalDeviceMeshShaderFeaturesNV(*reinterpret_cast<VkPhysicalDeviceMeshShaderFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_shader_image_footprint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV):
            merge_VkPhysicalDeviceShaderImageFootprintFeaturesNV(*reinterpret_cast<VkPhysicalDeviceShaderImageFootprintFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderImageFootprintFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_scissor_exclusive))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV):
            merge_VkPhysicalDeviceExclusiveScissorFeaturesNV(*reinterpret_cast<VkPhysicalDeviceExclusiveScissorFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceExclusiveScissorFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_present_timing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT):
            merge_VkPhysicalDevicePresentTimingFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePresentTimingFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePresentTimingFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_INTEL_shader_integer_functions2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL):
            merge_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(*reinterpret_cast<VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT):
            merge_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFragmentDensityMapFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_AMD_device_coherent_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD):
            merge_VkPhysicalDeviceCoherentMemoryFeaturesAMD(*reinterpret_cast<VkPhysicalDeviceCoherentMemoryFeaturesAMD*>(current), *reinterpret_cast<const VkPhysicalDeviceCoherentMemoryFeaturesAMD*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_image_atomic_int64))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_memory_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT):
            merge_VkPhysicalDeviceMemoryPriorityFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMemoryPriorityFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMemoryPriorityFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_dedicated_allocation_image_aliasing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV):
            merge_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT):
            merge_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceBufferDeviceAddressFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT*>(merge_in));
            break;
#elif (defined(VK_EXT_buffer_device_address))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT):
            merge_VkPhysicalDeviceBufferAddressFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceBufferAddressFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceBufferAddressFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV):
            merge_VkPhysicalDeviceCooperativeMatrixFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrixFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_coverage_reduction_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV):
            merge_VkPhysicalDeviceCoverageReductionModeFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCoverageReductionModeFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCoverageReductionModeFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_fragment_shader_interlock))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT):
            merge_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_ycbcr_image_arrays))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT):
            merge_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_provoking_vertex))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT):
            merge_VkPhysicalDeviceProvokingVertexFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceProvokingVertexFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceProvokingVertexFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_atomic_float))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT):
            merge_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_map_memory_placed))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT):
            merge_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMapMemoryPlacedFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_atomic_float2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_device_generated_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV):
            merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_inherited_viewport_scissor))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV):
            merge_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(*reinterpret_cast<VkPhysicalDeviceInheritedViewportScissorFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceInheritedViewportScissorFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_texel_buffer_alignment))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT):
            merge_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_depth_bias_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceDepthBiasControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDepthBiasControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthBiasControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_device_memory_report))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT):
            merge_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDeviceMemoryReportFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_custom_border_color))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT):
            merge_VkPhysicalDeviceCustomBorderColorFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_texture_compression_astc_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT):
            merge_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_present_barrier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV):
            merge_VkPhysicalDevicePresentBarrierFeaturesNV(*reinterpret_cast<VkPhysicalDevicePresentBarrierFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDevicePresentBarrierFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_device_diagnostics_config))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV):
            merge_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDiagnosticsConfigFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDiagnosticsConfigFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_queue_perf_hint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM):
            merge_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceQueuePerfHintFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceQueuePerfHintFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_image_processing3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM):
            merge_VkPhysicalDeviceImageProcessing3FeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceImageProcessing3FeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceImageProcessing3FeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_shader_multiple_wait_queues))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM):
            merge_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_split_barrier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderSplitBarrierFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT*>(merge_in));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUDA_KERNEL_LAUNCH_FEATURES_NV):
            merge_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCudaKernelLaunchFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCudaKernelLaunchFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_tile_shading))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM):
            merge_VkPhysicalDeviceTileShadingFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceTileShadingFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceTileShadingFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_descriptor_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT):
            merge_VkPhysicalDeviceDescriptorBufferFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDescriptorBufferFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_graphics_pipeline_library))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT):
            merge_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_AMD_shader_early_and_late_fragment_tests))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD):
            merge_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(*reinterpret_cast<VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD*>(merge_in));
            break;
#endif
#if (defined(VK_NV_fragment_shading_rate_enums))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV):
            merge_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(*reinterpret_cast<VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_ray_tracing_motion_blur))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV):
            merge_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRayTracingMotionBlurFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_ycbcr_2plane_444_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT):
            merge_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT):
            merge_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFragmentDensityMap2FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_compression_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceImageCompressionControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageCompressionControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_attachment_feedback_loop_layout))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT):
            merge_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_4444_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT):
            merge_VkPhysicalDevice4444FormatsFeaturesEXT(*reinterpret_cast<VkPhysicalDevice4444FormatsFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevice4444FormatsFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_device_fault))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT):
            merge_VkPhysicalDeviceFaultFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFaultFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFaultFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_rasterization_order_attachment_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT):
            merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT*>(merge_in));
            break;
#elif (defined(VK_ARM_rasterization_order_attachment_access))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM):
            merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(*reinterpret_cast<VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_rgba10x6_formats))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT):
            merge_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_mutable_descriptor_type))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT):
            merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT*>(merge_in));
            break;
#elif (defined(VK_VALVE_mutable_descriptor_type))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE):
            merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(*reinterpret_cast<VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE*>(current), *reinterpret_cast<const VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_vertex_input_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT):
            merge_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_device_address_binding_report))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT):
            merge_VkPhysicalDeviceAddressBindingReportFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceAddressBindingReportFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceAddressBindingReportFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_depth_clip_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceDepthClipControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDepthClipControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthClipControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_primitive_topology_list_restart))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT):
            merge_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_HUAWEI_subpass_shading))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI):
            merge_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(*reinterpret_cast<VkPhysicalDeviceSubpassShadingFeaturesHUAWEI*>(current), *reinterpret_cast<const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI*>(merge_in));
            break;
#endif
#if (defined(VK_HUAWEI_invocation_mask))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI):
            merge_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(*reinterpret_cast<VkPhysicalDeviceInvocationMaskFeaturesHUAWEI*>(current), *reinterpret_cast<const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI*>(merge_in));
            break;
#endif
#if (defined(VK_NV_external_memory_rdma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV):
            merge_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(*reinterpret_cast<VkPhysicalDeviceExternalMemoryRDMAFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_pipeline_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT):
            merge_VkPhysicalDevicePipelinePropertiesFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePipelinePropertiesFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePipelinePropertiesFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_frame_boundary))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT):
            merge_VkPhysicalDeviceFrameBoundaryFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFrameBoundaryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFrameBoundaryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_multisampled_render_to_single_sampled))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT):
            merge_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT):
            merge_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_color_write_enable))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT):
            merge_VkPhysicalDeviceColorWriteEnableFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_primitives_generated_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT):
            merge_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VALVE_video_encode_rgb_conversion))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE):
            merge_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(*reinterpret_cast<VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE*>(current), *reinterpret_cast<const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_view_min_lod))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT):
            merge_VkPhysicalDeviceImageViewMinLodFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageViewMinLodFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageViewMinLodFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_multi_draw))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT):
            merge_VkPhysicalDeviceMultiDrawFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMultiDrawFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMultiDrawFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_2d_view_of_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT):
            merge_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImage2DViewOf3DFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_tile_image))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderTileImageFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderTileImageFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderTileImageFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT):
            merge_VkPhysicalDeviceOpacityMicromapFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceOpacityMicromapFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceOpacityMicromapFeaturesEXT*>(merge_in));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISPLACEMENT_MICROMAP_FEATURES_NV):
            merge_VkPhysicalDeviceDisplacementMicromapFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDisplacementMicromapFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDisplacementMicromapFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_HUAWEI_cluster_culling_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI):
            merge_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(*reinterpret_cast<VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI*>(current), *reinterpret_cast<const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_border_color_swizzle))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT):
            merge_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceBorderColorSwizzleFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_pageable_device_local_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT):
            merge_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_scheduling_controls))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM):
            merge_VkPhysicalDeviceSchedulingControlsFeaturesARM(*reinterpret_cast<VkPhysicalDeviceSchedulingControlsFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceSchedulingControlsFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_sliced_view_of_3d))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT):
            merge_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VALVE_descriptor_set_host_mapping))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE):
            merge_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(*reinterpret_cast<VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_non_seamless_cube_map))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT):
            merge_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_render_pass_striped))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM):
            merge_VkPhysicalDeviceRenderPassStripedFeaturesARM(*reinterpret_cast<VkPhysicalDeviceRenderPassStripedFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceRenderPassStripedFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_fragment_density_map_offset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT):
            merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT*>(merge_in));
            break;
#elif (defined(VK_QCOM_fragment_density_map_offset))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM):
            merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_NV_copy_memory_indirect))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV):
            merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCopyMemoryIndirectFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_memory_decompression))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT):
            merge_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMemoryDecompressionFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesEXT*>(merge_in));
            break;
#elif (defined(VK_NV_memory_decompression))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_NV):
            merge_VkPhysicalDeviceMemoryDecompressionFeaturesNV(*reinterpret_cast<VkPhysicalDeviceMemoryDecompressionFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceMemoryDecompressionFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_device_generated_commands_compute))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV):
            merge_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_ray_tracing_linear_swept_spheres))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV):
            merge_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_linear_color_attachment))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV):
            merge_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(*reinterpret_cast<VkPhysicalDeviceLinearColorAttachmentFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceLinearColorAttachmentFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_compression_control_swapchain))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT):
            merge_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_image_processing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM):
            merge_VkPhysicalDeviceImageProcessingFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceImageProcessingFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceImageProcessingFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_nested_command_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT):
            merge_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceNestedCommandBufferFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceNestedCommandBufferFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_extended_dynamic_state3))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT):
            merge_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceExtendedDynamicState3FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_subpass_merge_feedback))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT):
            merge_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_tensors))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM):
            merge_VkPhysicalDeviceTensorFeaturesARM(*reinterpret_cast<VkPhysicalDeviceTensorFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceTensorFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_tensors))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM):
            merge_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(*reinterpret_cast<VkPhysicalDeviceDescriptorBufferTensorFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_module_identifier))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_optical_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV):
            merge_VkPhysicalDeviceOpticalFlowFeaturesNV(*reinterpret_cast<VkPhysicalDeviceOpticalFlowFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceOpticalFlowFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_legacy_dithering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT):
            merge_VkPhysicalDeviceLegacyDitheringFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceLegacyDitheringFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceLegacyDitheringFeaturesEXT*>(merge_in));
            break;
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID):
            merge_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(*reinterpret_cast<VkPhysicalDeviceExternalFormatResolveFeaturesANDROID*>(current), *reinterpret_cast<const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID*>(merge_in));
            break;
#endif
#if (defined(VK_AMD_anti_lag))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD):
            merge_VkPhysicalDeviceAntiLagFeaturesAMD(*reinterpret_cast<VkPhysicalDeviceAntiLagFeaturesAMD*>(current), *reinterpret_cast<const VkPhysicalDeviceAntiLagFeaturesAMD*>(merge_in));
            break;
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DENSE_GEOMETRY_FORMAT_FEATURES_AMDX):
            merge_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(*reinterpret_cast<VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX*>(current), *reinterpret_cast<const VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_object))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderObjectFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderObjectFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderObjectFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_tile_properties))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM):
            merge_VkPhysicalDeviceTilePropertiesFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceTilePropertiesFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceTilePropertiesFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_SEC_amigo_profiling))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC):
            merge_VkPhysicalDeviceAmigoProfilingFeaturesSEC(*reinterpret_cast<VkPhysicalDeviceAmigoProfilingFeaturesSEC*>(current), *reinterpret_cast<const VkPhysicalDeviceAmigoProfilingFeaturesSEC*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_multiview_per_view_viewports))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM):
            merge_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_NV_ray_tracing_invocation_reorder))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV):
            merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_cooperative_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV):
            merge_VkPhysicalDeviceCooperativeVectorFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCooperativeVectorFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeVectorFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_extended_sparse_address_space))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV):
            merge_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(*reinterpret_cast<VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_legacy_vertex_attributes))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT):
            merge_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_shader_core_builtins))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM):
            merge_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(*reinterpret_cast<VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_pipeline_library_group_handles))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT):
            merge_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_dynamic_rendering_unused_attachments))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT):
            merge_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_data_graph))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM):
            merge_VkPhysicalDeviceDataGraphFeaturesARM(*reinterpret_cast<VkPhysicalDeviceDataGraphFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceDataGraphFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_multiview_per_view_render_areas))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM):
            merge_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_NV_per_stage_descriptor_set))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV):
            merge_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(*reinterpret_cast<VkPhysicalDevicePerStageDescriptorSetFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDevicePerStageDescriptorSetFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_image_processing2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM):
            merge_VkPhysicalDeviceImageProcessing2FeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceImageProcessing2FeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceImageProcessing2FeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_filter_cubic_weights))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM):
            merge_VkPhysicalDeviceCubicWeightsFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceCubicWeightsFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceCubicWeightsFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_ycbcr_degamma))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM):
            merge_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceYcbcrDegammaFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_filter_cubic_clamp))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM):
            merge_VkPhysicalDeviceCubicClampFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceCubicClampFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceCubicClampFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_attachment_feedback_loop_dynamic_state))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT):
            merge_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT*>(merge_in));
            break;
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX):
            merge_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(*reinterpret_cast<VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(current), *reinterpret_cast<const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(merge_in));
            break;
#endif
#if (defined(VK_NV_descriptor_pool_overallocation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV):
            merge_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(*reinterpret_cast<VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_tile_memory_heap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM):
            merge_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceTileMemoryHeapFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_NV_raw_access_chains))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV):
            merge_VkPhysicalDeviceRawAccessChainsFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRawAccessChainsFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRawAccessChainsFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_command_buffer_inheritance))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV):
            merge_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCommandBufferInheritanceFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_shader_atomic_float16_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV):
            merge_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(*reinterpret_cast<VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_replicated_composites))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_float8))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderFloat8FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderFloat8FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderFloat8FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_ray_tracing_validation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV):
            merge_VkPhysicalDeviceRayTracingValidationFeaturesNV(*reinterpret_cast<VkPhysicalDeviceRayTracingValidationFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingValidationFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_cluster_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV):
            merge_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(*reinterpret_cast<VkPhysicalDeviceClusterAccelerationStructureFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_NV_partitioned_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV):
            merge_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(*reinterpret_cast<VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_device_generated_commands))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT):
            merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_MESA_image_alignment_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA):
            merge_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(*reinterpret_cast<VkPhysicalDeviceImageAlignmentControlFeaturesMESA*>(current), *reinterpret_cast<const VkPhysicalDeviceImageAlignmentControlFeaturesMESA*>(merge_in));
            break;
#endif
#if (defined(VK_NV_push_constant_bank))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV):
            merge_VkPhysicalDevicePushConstantBankFeaturesNV(*reinterpret_cast<VkPhysicalDevicePushConstantBankFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDevicePushConstantBankFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_ray_tracing_invocation_reorder))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT):
            merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_depth_clamp_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceDepthClampControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceDepthClampControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceDepthClampControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_HUAWEI_hdr_vivid))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI):
            merge_VkPhysicalDeviceHdrVividFeaturesHUAWEI(*reinterpret_cast<VkPhysicalDeviceHdrVividFeaturesHUAWEI*>(current), *reinterpret_cast<const VkPhysicalDeviceHdrVividFeaturesHUAWEI*>(merge_in));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix2))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV):
            merge_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrix2FeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrix2FeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_pipeline_opacity_micromap))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM):
            merge_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(*reinterpret_cast<VkPhysicalDevicePipelineOpacityMicromapFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_performance_counters_by_region))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM):
            merge_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(*reinterpret_cast<VkPhysicalDevicePerformanceCountersByRegionFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_shader_instrumentation))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM):
            merge_VkPhysicalDeviceShaderInstrumentationFeaturesARM(*reinterpret_cast<VkPhysicalDeviceShaderInstrumentationFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderInstrumentationFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_vertex_attribute_robustness))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT):
            merge_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_format_pack))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM):
            merge_VkPhysicalDeviceFormatPackFeaturesARM(*reinterpret_cast<VkPhysicalDeviceFormatPackFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceFormatPackFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_VALVE_fragment_density_map_layered))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE):
            merge_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(*reinterpret_cast<VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE*>(current), *reinterpret_cast<const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE*>(merge_in));
            break;
#endif
#if (VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV):
            merge_VkPhysicalDevicePresentMeteringFeaturesNV(*reinterpret_cast<VkPhysicalDevicePresentMeteringFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDevicePresentMeteringFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_multisampled_render_to_swapchain))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT):
            merge_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_zero_initialize_device_memory))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT):
            merge_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_64bit_indexing))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT):
            merge_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShader64BitIndexingFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShader64BitIndexingFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_custom_resolve))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT):
            merge_VkPhysicalDeviceCustomResolveFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceCustomResolveFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceCustomResolveFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_QCOM_data_graph_model))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM):
            merge_VkPhysicalDeviceDataGraphModelFeaturesQCOM(*reinterpret_cast<VkPhysicalDeviceDataGraphModelFeaturesQCOM*>(current), *reinterpret_cast<const VkPhysicalDeviceDataGraphModelFeaturesQCOM*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_data_graph_optical_flow))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM):
            merge_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(*reinterpret_cast<VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_long_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderLongVectorFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderLongVectorFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderLongVectorFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_SEC_pipeline_cache_incremental_mode))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC):
            merge_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(*reinterpret_cast<VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC*>(current), *reinterpret_cast<const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_uniform_buffer_unsized_array))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_compute_occupancy_priority))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV):
            merge_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(*reinterpret_cast<VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_cooperative_matrix_maintenance1))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_MAINTENANCE_1_FEATURES_EXT):
            merge_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_subgroup_partitioned))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_shader_ocp_microscaling_types))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OCP_MICROSCALING_TYPES_FEATURES_EXT):
            merge_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_VALVE_shader_mixed_float_dot_product))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE):
            merge_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(*reinterpret_cast<VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE*>(current), *reinterpret_cast<const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE*>(merge_in));
            break;
#endif
#if (defined(VK_SEC_throttle_hint))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC):
            merge_VkPhysicalDeviceThrottleHintFeaturesSEC(*reinterpret_cast<VkPhysicalDeviceThrottleHintFeaturesSEC*>(current), *reinterpret_cast<const VkPhysicalDeviceThrottleHintFeaturesSEC*>(merge_in));
            break;
#endif
#if (defined(VK_ARM_data_graph_neural_accelerator_statistics))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM):
            merge_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(*reinterpret_cast<VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM*>(current), *reinterpret_cast<const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_primitive_restart_index))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT):
            merge_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(*reinterpret_cast<VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_image_tiling_control))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_TILING_CONTROL_FEATURES_EXT):
            merge_VkPhysicalDeviceImageTilingControlFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceImageTilingControlFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceImageTilingControlFeaturesEXT*>(merge_in));
            break;
#endif
#if (defined(VK_NV_cooperative_matrix_decode_vector))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV):
            merge_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(*reinterpret_cast<VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV*>(current), *reinterpret_cast<const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_acceleration_structure))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR):
            merge_VkPhysicalDeviceAccelerationStructureFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceAccelerationStructureFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceAccelerationStructureFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_ray_tracing_pipeline))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR):
            merge_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceRayTracingPipelineFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_KHR_ray_query))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR):
            merge_VkPhysicalDeviceRayQueryFeaturesKHR(*reinterpret_cast<VkPhysicalDeviceRayQueryFeaturesKHR*>(current), *reinterpret_cast<const VkPhysicalDeviceRayQueryFeaturesKHR*>(merge_in));
            break;
#endif
#if (defined(VK_EXT_mesh_shader))
        case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT):
            merge_VkPhysicalDeviceMeshShaderFeaturesEXT(*reinterpret_cast<VkPhysicalDeviceMeshShaderFeaturesEXT*>(current), *reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesEXT*>(merge_in));
            break;
#endif
        default:
            break;
    }
}
} // namespace vkb