/*
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
 * Copyright © 2020 Charles Giessen (charles@lunarg.com)
 */

#include "VkBootstrap.h"

#include <cstring>

#if defined(_WIN32)
#include <fcntl.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <dlfcn.h>
#endif

#include <mutex>
#include <algorithm>

#include "VkBootstrapFeatureChain.inl"

namespace vkb {

namespace detail {

bool FeaturesChain::empty() const { return structure_infos.empty(); }

bool FeaturesChain::is_feature_struct_in_chain(VkStructureType sType) const {
    return structure_infos.end() != find_sType(sType);
}

void FeaturesChain::add_structure(VkStructureType sType, size_t struct_size, const void* structure) {
#if !defined(NDEBUG)
    // Validation
    assert(sType != static_cast<VkStructureType>(0) && "Features struct sType must be filled with the struct's "
                                                       "corresponding VkStructureType enum");
    assert(sType != VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 &&
           "Do not pass VkPhysicalDeviceFeatures2 as a required extension feature structure. An "
           "instance of this is managed internally for selection criteria and device creation.");
#endif

    auto found = find_sType(sType);
    if (found != structure_infos.end()) {
        // Merge structure into the current structure
#if !defined(NDEBUG)
        assert(found->starting_location + found->struct_size <= structures.size() &&
               "Internal Consistency Error: FeatureChain::add_structure tyring to merge structures into memory that is "
               "past the end of the structures array");
#endif
        merge_feature_struct(sType, &(structures.at(found->starting_location)), structure);
    } else {
        // Add a structure into the chain
        structure_infos.push_back(StructInfo{ sType, structures.size(), struct_size });
        auto& new_structure_info = structure_infos.back();
        structures.insert(structures.end(), struct_size, std::byte(0));
        memcpy(&(structures.at(new_structure_info.starting_location)), structure, struct_size);
    }
}

void FeaturesChain::remove_structure(VkStructureType sType) {
    auto found = find_sType(sType);
    if (found != structure_infos.end()) {
        if (found->starting_location + found->struct_size < structures.size()) {
            structures.erase(structures.begin() + static_cast<int>(found->starting_location),
                structures.begin() + static_cast<int>(found->starting_location + found->struct_size));
            structure_infos.erase(found);
        }
    }
}

bool FeaturesChain::match(VkStructureType sType, const void* structure) const {
    auto found = find_sType(sType);
    if (found != structure_infos.end()) {
        std::vector<std::string> error_list;
        compare_feature_struct(sType, error_list, &(structures.at(found->starting_location)), structure);
        return error_list.empty();
    } else {
        return false;
    }
}

void FeaturesChain::match_all(std::vector<std::string>& error_list, FeaturesChain const& requested_features_chain) const {
    if (structure_infos.size() != requested_features_chain.structure_infos.size()) {
        return;
    }
    for (size_t i = 0; i < structure_infos.size(); ++i) {
        compare_feature_struct(structure_infos.at(i).sType,
            error_list,
            &(structures.at(structure_infos.at(i).starting_location)),
            &(requested_features_chain.structures.at(requested_features_chain.structure_infos.at(i).starting_location)));
    }
}

void FeaturesChain::create_chained_features(VkPhysicalDeviceFeatures2& features2) {
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &structures.at(0);
    // Write the address of structure N+1 to the pNext member of structure N
    for (size_t i = 0; i < structure_infos.size() - 1; i++) {
        VkBaseOutStructure structure{};
        memcpy(&structure, &(structures.at(structure_infos.at(i).starting_location)), sizeof(VkBaseOutStructure));
        structure.pNext = reinterpret_cast<VkBaseOutStructure*>(&(structures.at(structure_infos.at(i + 1).starting_location)));
        memcpy(&(structures.at(structure_infos.at(i).starting_location)), &structure, sizeof(VkBaseOutStructure));
    }
    // Write nullptr to the last structures pNext member
    VkBaseOutStructure structure{};
    memcpy(&structure, &(structures.at(structure_infos.back().starting_location)), sizeof(VkBaseOutStructure));
    structure.pNext = nullptr;
    memcpy(&(structures.at(structure_infos.back().starting_location)), &structure, sizeof(VkBaseOutStructure));
}

std::vector<void*> FeaturesChain::get_pNext_chain_members() {
    std::vector<void*> members;
    for (const auto& structure_info : structure_infos) {
        members.push_back(&(structures.at(structure_info.starting_location)));
    }
    return members;
}

std::vector<vkb::detail::FeaturesChain::StructInfo>::const_iterator FeaturesChain::find_sType(VkStructureType sType) const {
    return std::find_if(structure_infos.begin(), structure_infos.end(), [sType](StructInfo const& struct_info) {
        return struct_info.sType == sType;
    });
}

void add_all_pnext_structures(std::vector<void*>& pNext_chain, void* structure_to_add) {
    auto pNext_iter = structure_to_add;
    while (pNext_iter != nullptr) {
        pNext_chain.push_back(pNext_iter);
        VkBaseOutStructure out_structure{};
        memcpy(&out_structure, pNext_iter, sizeof(VkBaseOutStructure));
        pNext_iter = out_structure.pNext;
    }
}


class VulkanFunctions {
    private:
    std::mutex init_mutex;
    bool initialized = false;

    std::mutex instance_functions_mutex;
    bool instance_functions_initialized = false;

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    void* library = nullptr;
#elif defined(_WIN32)
    HMODULE library = nullptr;
#endif

    bool load_vulkan_library() {
        // Can immediately return if it has already been loaded
        if (library) {
            return true;
        }
#if defined(__linux__) || defined(__FreeBSD__)
        library = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#elif defined(__APPLE__)
        library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
#elif defined(_WIN32)
        library = LoadLibrary(TEXT("vulkan-1.dll"));
#else
        assert(false && "Unsupported platform");
#endif
        if (!library) return false;
        load_func(ptr_vkGetInstanceProcAddr, "vkGetInstanceProcAddr");
        return ptr_vkGetInstanceProcAddr != nullptr;
    }

    template <typename T> void load_func(T& func_dest, const char* func_name) {
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        func_dest = reinterpret_cast<T>(dlsym(library, func_name));
#elif defined(_WIN32)
        // GetProcAddress returns FARPROC, so need to cast it into a void* which can safely be cast to T
        func_dest = reinterpret_cast<T>(reinterpret_cast<void*>(GetProcAddress(library, func_name)));
#endif
    }
    void unload_vulkan_library() {
        if (!library) {
            return;
        }
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        dlclose(library);
#elif defined(_WIN32)
        FreeLibrary(library);
#endif
        library = nullptr;
    }

    public:
    bool init_vulkan_funcs(PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr = nullptr) {
        std::lock_guard<std::mutex> lg(init_mutex);
        if (initialized) {
            return true;
        }
        if (fp_vkGetInstanceProcAddr != nullptr) {
            ptr_vkGetInstanceProcAddr = fp_vkGetInstanceProcAddr;
        } else {
            bool ret = load_vulkan_library();
            if (!ret) return false;
        }

        fp_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            ptr_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties"));
        fp_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(
            ptr_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties"));
        fp_vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
            ptr_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"));
        fp_vkCreateInstance =
            reinterpret_cast<PFN_vkCreateInstance>(ptr_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance"));
        initialized = true;
        return true;
    }

    template <typename T> void get_inst_proc_addr(T& out_ptr, const char* func_name) {
        out_ptr = reinterpret_cast<T>(ptr_vkGetInstanceProcAddr(instance, func_name));
    }

    template <typename T> void get_device_proc_addr(VkDevice device, T& out_ptr, const char* func_name) {
        out_ptr = reinterpret_cast<T>(fp_vkGetDeviceProcAddr(device, func_name));
    }

    PFN_vkGetInstanceProcAddr ptr_vkGetInstanceProcAddr = nullptr;
    VkInstance instance = nullptr;

    PFN_vkEnumerateInstanceExtensionProperties fp_vkEnumerateInstanceExtensionProperties = nullptr;
    PFN_vkEnumerateInstanceLayerProperties fp_vkEnumerateInstanceLayerProperties = nullptr;
    PFN_vkEnumerateInstanceVersion fp_vkEnumerateInstanceVersion = nullptr;
    PFN_vkCreateInstance fp_vkCreateInstance = nullptr;

    PFN_vkDestroyInstance fp_vkDestroyInstance = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT fp_vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT fp_vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkEnumeratePhysicalDevices fp_vkEnumeratePhysicalDevices = nullptr;
    PFN_vkGetPhysicalDeviceFeatures fp_vkGetPhysicalDeviceFeatures = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 fp_vkGetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2KHR fp_vkGetPhysicalDeviceFeatures2KHR = nullptr;
    PFN_vkGetPhysicalDeviceProperties fp_vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties fp_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties fp_vkGetPhysicalDeviceMemoryProperties = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties fp_vkEnumerateDeviceExtensionProperties = nullptr;

    PFN_vkCreateDevice fp_vkCreateDevice = nullptr;
    PFN_vkGetDeviceProcAddr fp_vkGetDeviceProcAddr = nullptr;

    PFN_vkDestroySurfaceKHR fp_vkDestroySurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fp_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fp_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fp_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;

    void init_instance_funcs(VkInstance inst) {
        std::lock_guard<std::mutex> lg(instance_functions_mutex);
        if (instance_functions_initialized) return;
        instance = inst;
        get_inst_proc_addr(fp_vkDestroyInstance, "vkDestroyInstance");
        get_inst_proc_addr(fp_vkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT");
        get_inst_proc_addr(fp_vkDestroyDebugUtilsMessengerEXT, "vkDestroyDebugUtilsMessengerEXT");
        get_inst_proc_addr(fp_vkEnumeratePhysicalDevices, "vkEnumeratePhysicalDevices");

        get_inst_proc_addr(fp_vkGetPhysicalDeviceFeatures, "vkGetPhysicalDeviceFeatures");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceFeatures2KHR, "vkGetPhysicalDeviceFeatures2KHR");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceProperties, "vkGetPhysicalDeviceProperties");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceQueueFamilyProperties, "vkGetPhysicalDeviceQueueFamilyProperties");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceMemoryProperties, "vkGetPhysicalDeviceMemoryProperties");
        get_inst_proc_addr(fp_vkEnumerateDeviceExtensionProperties, "vkEnumerateDeviceExtensionProperties");

        get_inst_proc_addr(fp_vkCreateDevice, "vkCreateDevice");
        get_inst_proc_addr(fp_vkGetDeviceProcAddr, "vkGetDeviceProcAddr");

        get_inst_proc_addr(fp_vkDestroySurfaceKHR, "vkDestroySurfaceKHR");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceSurfaceSupportKHR, "vkGetPhysicalDeviceSurfaceSupportKHR");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceSurfaceFormatsKHR, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceSurfacePresentModesKHR, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        get_inst_proc_addr(fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        instance_functions_initialized = true;
    }

    void deinit_all() {
        {
            std::lock_guard<std::mutex> lg(instance_functions_mutex);
            if (instance_functions_initialized) {
                instance_functions_initialized = false;
            }
        }
        {
            std::lock_guard<std::mutex> lg(init_mutex);
            if (initialized) {
                unload_vulkan_library();
                initialized = false;
            }
        }
    }
};

static VulkanFunctions& vulkan_functions() {
    static VulkanFunctions v;
    return v;
}

// Helper for robustly executing the two-call pattern
template <typename T, typename F, typename... Ts> auto get_vector(std::vector<T>& out, F&& f, Ts&&... ts) -> VkResult {
    uint32_t count = 0;
    VkResult err;
    do {
        err = f(ts..., &count, nullptr);
        if (err != VK_SUCCESS) {
            return err;
        };
        out.resize(count);
        err = f(ts..., &count, out.data());
        out.resize(count);
    } while (err == VK_INCOMPLETE);
    return err;
}

template <typename T, typename F, typename... Ts> auto get_vector_noerror(F&& f, Ts&&... ts) -> std::vector<T> {
    uint32_t count = 0;
    std::vector<T> results;
    f(ts..., &count, nullptr);
    results.resize(count);
    f(ts..., &count, results.data());
    results.resize(count);
    return results;
}
} // namespace detail

const char* to_string_device_type(const VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "integrated";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "discrete";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "virtual_gpu";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "cpu";
        default:
            return "unknown";
    }
}


const char* to_string_message_severity(VkDebugUtilsMessageSeverityFlagBitsEXT s) {
    switch (s) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return "VERBOSE";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return "ERROR";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return "WARNING";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return "INFO";
        default:
            return "UNKNOWN";
    }
}
const char* to_string_message_type(VkDebugUtilsMessageTypeFlagsEXT s) {
    if (s == 7) return "General | Validation | Performance";
    if (s == 6) return "Validation | Performance";
    if (s == 5) return "General | Performance";
    if (s == 4 /*VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT*/) return "Performance";
    if (s == 3) return "General | Validation";
    if (s == 2 /*VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT*/) return "Validation";
    if (s == 1 /*VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT*/) return "General";
    return "Unknown";
}

VkResult create_debug_utils_messenger(VkInstance instance,
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback,
    VkDebugUtilsMessageSeverityFlagsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    void* user_data_pointer,
    VkDebugUtilsMessengerEXT* pDebugMessenger,
    VkAllocationCallbacks* allocation_callbacks) {

    if (debug_callback == nullptr) debug_callback = default_debug_callback;
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
    messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerCreateInfo.pNext = nullptr;
    messengerCreateInfo.messageSeverity = severity;
    messengerCreateInfo.messageType = type;
    messengerCreateInfo.pfnUserCallback = debug_callback;
    messengerCreateInfo.pUserData = user_data_pointer;

    if (detail::vulkan_functions().fp_vkCreateDebugUtilsMessengerEXT != nullptr) {
        return detail::vulkan_functions().fp_vkCreateDebugUtilsMessengerEXT(
            instance, &messengerCreateInfo, allocation_callbacks, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks* allocation_callbacks) {

    if (detail::vulkan_functions().fp_vkDestroyDebugUtilsMessengerEXT != nullptr) {
        detail::vulkan_functions().fp_vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocation_callbacks);
    }
}

namespace detail {
bool check_layer_supported(std::vector<VkLayerProperties> const& available_layers, const char* layer_name) {
    if (!layer_name) return false;
    for (const auto& layer_properties : available_layers) {
        if (strcmp(layer_name, layer_properties.layerName) == 0) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> check_layers_supported(
    std::vector<VkLayerProperties> const& available_layers, std::vector<const char*> const& layer_names) {
    std::vector<std::string> not_found;
    for (const auto& layer_name : layer_names) {
        bool found = check_layer_supported(available_layers, layer_name);
        if (!found) not_found.push_back(layer_name);
    }
    return not_found;
}

bool check_extension_supported(std::vector<VkExtensionProperties> const& available_extensions, const char* extension_name) {
    if (!extension_name) return false;
    for (const auto& extension_properties : available_extensions) {
        if (strcmp(extension_name, extension_properties.extensionName) == 0) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> check_extensions_supported(
    std::vector<VkExtensionProperties> const& available_extensions, std::vector<const char*> const& extension_names) {
    std::vector<std::string> not_found;
    for (const auto& extension_name : extension_names) {
        bool found = check_extension_supported(available_extensions, extension_name);
        if (!found) not_found.push_back(extension_name);
    }
    return not_found;
}

template <typename T> void setup_pNext_chain(T& structure, std::vector<void*> const& structs) {
    structure.pNext = nullptr;
    if (structs.empty()) return;
    for (size_t i = 0; i < structs.size() - 1; i++) {
        // Make sure we aren't adding the same struct more than once. We do not care about duplicate sTypes here.
        bool is_duplicate = false;
        for (size_t j = 0; j != i; j++) {
            if (structs.at(i) == structs.at(j)) {
                is_duplicate = true;
                break;
            }
        }
        if (is_duplicate) {
            continue;
        }
        VkBaseOutStructure out_structure{};
        memcpy(&out_structure, structs.at(i), sizeof(VkBaseOutStructure));
#if !defined(NDEBUG)
        assert(out_structure.sType != VK_STRUCTURE_TYPE_APPLICATION_INFO);
#endif
        out_structure.pNext = static_cast<VkBaseOutStructure*>(structs.at(i + 1));
        memcpy(structs.at(i), &out_structure, sizeof(VkBaseOutStructure));
    }
    VkBaseOutStructure out_structure{};
    memcpy(&out_structure, structs.back(), sizeof(VkBaseOutStructure));
    out_structure.pNext = nullptr;
#if !defined(NDEBUG)
    assert(out_structure.sType != VK_STRUCTURE_TYPE_APPLICATION_INFO);
#endif
    memcpy(structs.back(), &out_structure, sizeof(VkBaseOutStructure));
    structure.pNext = structs.at(0);
}
const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";

struct InstanceErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vkb_instance"; }
    std::string message(int err) const override { return to_string(static_cast<InstanceError>(err)); }
};
const InstanceErrorCategory instance_error_category;

struct PhysicalDeviceErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vkb_physical_device"; }
    std::string message(int err) const override { return to_string(static_cast<PhysicalDeviceError>(err)); }
};
const PhysicalDeviceErrorCategory physical_device_error_category;

struct QueueErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vkb_queue"; }
    std::string message(int err) const override { return to_string(static_cast<QueueError>(err)); }
};
const QueueErrorCategory queue_error_category;

struct DeviceErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vkb_device"; }
    std::string message(int err) const override { return to_string(static_cast<DeviceError>(err)); }
};
const DeviceErrorCategory device_error_category;

struct SwapchainErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vbk_swapchain"; }
    std::string message(int err) const override { return to_string(static_cast<SwapchainError>(err)); }
};
const SwapchainErrorCategory swapchain_error_category;

} // namespace detail

std::error_code make_error_code(InstanceError instance_error) {
    return { static_cast<int>(instance_error), detail::instance_error_category };
}
std::error_code make_error_code(PhysicalDeviceError physical_device_error) {
    return { static_cast<int>(physical_device_error), detail::physical_device_error_category };
}
std::error_code make_error_code(QueueError queue_error) {
    return { static_cast<int>(queue_error), detail::queue_error_category };
}
std::error_code make_error_code(DeviceError device_error) {
    return { static_cast<int>(device_error), detail::device_error_category };
}
std::error_code make_error_code(SwapchainError swapchain_error) {
    return { static_cast<int>(swapchain_error), detail::swapchain_error_category };
}
#define CASE_TO_STRING(CATEGORY, TYPE)                                                                                 \
    case CATEGORY::TYPE:                                                                                               \
        return #TYPE;

const char* to_string(InstanceError err) {
    switch (err) {
        CASE_TO_STRING(InstanceError, vulkan_unavailable)
        CASE_TO_STRING(InstanceError, vulkan_version_unavailable)
        CASE_TO_STRING(InstanceError, vulkan_version_1_1_unavailable)
        CASE_TO_STRING(InstanceError, vulkan_version_1_2_unavailable)
        CASE_TO_STRING(InstanceError, vulkan_version_1_3_unavailable)
        CASE_TO_STRING(InstanceError, vulkan_version_1_4_unavailable)
        CASE_TO_STRING(InstanceError, failed_create_debug_messenger)
        CASE_TO_STRING(InstanceError, failed_create_instance)
        CASE_TO_STRING(InstanceError, requested_layers_not_present)
        CASE_TO_STRING(InstanceError, requested_extensions_not_present)
        CASE_TO_STRING(InstanceError, windowing_extensions_not_present)
        default:
            return "";
    }
}
const char* to_string(PhysicalDeviceError err) {
    switch (err) {
        CASE_TO_STRING(PhysicalDeviceError, no_surface_provided)
        CASE_TO_STRING(PhysicalDeviceError, failed_enumerate_physical_devices)
        CASE_TO_STRING(PhysicalDeviceError, no_physical_devices_found)
        CASE_TO_STRING(PhysicalDeviceError, no_suitable_device)
        default:
            return "";
    }
}
const char* to_string(QueueError err) {
    switch (err) {
        CASE_TO_STRING(QueueError, present_unavailable)
        CASE_TO_STRING(QueueError, graphics_unavailable)
        CASE_TO_STRING(QueueError, compute_unavailable)
        CASE_TO_STRING(QueueError, transfer_unavailable)
        CASE_TO_STRING(QueueError, queue_index_out_of_range)
        CASE_TO_STRING(QueueError, invalid_queue_family_index)
        default:
            return "";
    }
}
const char* to_string(DeviceError err) {
    switch (err) {
        CASE_TO_STRING(DeviceError, failed_create_device)
        default:
            return "";
    }
}
const char* to_string(SwapchainError err) {
    switch (err) {
        CASE_TO_STRING(SwapchainError, surface_handle_not_provided)
        CASE_TO_STRING(SwapchainError, failed_query_surface_support_details)
        CASE_TO_STRING(SwapchainError, failed_create_swapchain)
        CASE_TO_STRING(SwapchainError, failed_get_swapchain_images)
        CASE_TO_STRING(SwapchainError, failed_create_swapchain_image_views)
        CASE_TO_STRING(SwapchainError, required_min_image_count_too_low)
        CASE_TO_STRING(SwapchainError, required_usage_not_supported)
        default:
            return "";
    }
}

Result<SystemInfo> SystemInfo::get_system_info() {
    if (!detail::vulkan_functions().init_vulkan_funcs(nullptr)) {
        return make_error_code(InstanceError::vulkan_unavailable);
    }
    return SystemInfo();
}

Result<SystemInfo> SystemInfo::get_system_info(PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr) {
    // Using externally provided function pointers, assume the loader is available
    if (!detail::vulkan_functions().init_vulkan_funcs(fp_vkGetInstanceProcAddr)) {
        return make_error_code(InstanceError::vulkan_unavailable);
    }
    return SystemInfo();
}

SystemInfo::SystemInfo() {
    auto available_layers_ret = detail::get_vector<VkLayerProperties>(
        this->available_layers, detail::vulkan_functions().fp_vkEnumerateInstanceLayerProperties);
    if (available_layers_ret != VK_SUCCESS) {
        this->available_layers.clear();
    }

    for (auto& layer : this->available_layers)
        if (strcmp(layer.layerName, detail::validation_layer_name) == 0) validation_layers_available = true;

    auto available_extensions_ret = detail::get_vector<VkExtensionProperties>(
        this->available_extensions, detail::vulkan_functions().fp_vkEnumerateInstanceExtensionProperties, nullptr);
    if (available_extensions_ret != VK_SUCCESS) {
        this->available_extensions.clear();
    }

    for (auto& ext : this->available_extensions) {
        if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            debug_utils_available = true;
        }
    }

    for (auto& layer : this->available_layers) {
        std::vector<VkExtensionProperties> layer_extensions;
        auto layer_extensions_ret = detail::get_vector<VkExtensionProperties>(
            layer_extensions, detail::vulkan_functions().fp_vkEnumerateInstanceExtensionProperties, layer.layerName);
        if (layer_extensions_ret == VK_SUCCESS) {
            this->available_extensions.insert(
                this->available_extensions.end(), layer_extensions.begin(), layer_extensions.end());
            for (auto& ext : layer_extensions) {
                if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
                    debug_utils_available = true;
                }
            }
        }
    }

    PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion = detail::vulkan_functions().fp_vkEnumerateInstanceVersion;

    if (pfn_vkEnumerateInstanceVersion != nullptr) {
        VkResult res = pfn_vkEnumerateInstanceVersion(&instance_api_version);
        if (res != VK_SUCCESS) {
            instance_api_version = VKB_VK_API_VERSION_1_0;
        }
    }
}
bool SystemInfo::is_extension_available(const char* extension_name) const {
    if (!extension_name) return false;
    return detail::check_extension_supported(available_extensions, extension_name);
}
bool SystemInfo::is_layer_available(const char* layer_name) const {
    if (!layer_name) return false;
    return detail::check_layer_supported(available_layers, layer_name);
}
bool SystemInfo::is_instance_version_available(uint32_t major_api_version, uint32_t minor_api_version) {
    return instance_api_version >= VKB_MAKE_VK_VERSION(0, major_api_version, minor_api_version, 0);
}
bool SystemInfo::is_instance_version_available(uint32_t api_version) { return instance_api_version >= api_version; }

void destroy_surface(Instance const& instance, VkSurfaceKHR surface) {
    if (instance.instance != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
        detail::vulkan_functions().fp_vkDestroySurfaceKHR(instance.instance, surface, instance.allocation_callbacks);
    }
}
void destroy_surface(VkInstance instance, VkSurfaceKHR surface, VkAllocationCallbacks* callbacks) {
    if (instance != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
        detail::vulkan_functions().fp_vkDestroySurfaceKHR(instance, surface, callbacks);
    }
}
void destroy_instance(Instance const& instance) {
    if (instance.instance != VK_NULL_HANDLE) {
        if (instance.debug_messenger != VK_NULL_HANDLE)
            destroy_debug_utils_messenger(instance.instance, instance.debug_messenger, instance.allocation_callbacks);
        detail::vulkan_functions().fp_vkDestroyInstance(instance.instance, instance.allocation_callbacks);
    }
}

Instance::operator VkInstance() const { return this->instance; }

InstanceDispatchTable Instance::make_table() const { return { instance, fp_vkGetInstanceProcAddr }; }

InstanceBuilder::InstanceBuilder(PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr) {
    info.fp_vkGetInstanceProcAddr = fp_vkGetInstanceProcAddr;
}
InstanceBuilder::InstanceBuilder() {}

Result<Instance> InstanceBuilder::build() const {

    auto sys_info_ret = SystemInfo::get_system_info(info.fp_vkGetInstanceProcAddr);
    if (!sys_info_ret) return sys_info_ret.error();
    auto system = sys_info_ret.value();

    uint32_t instance_version = VKB_VK_API_VERSION_1_0;

    if (info.minimum_instance_version > VKB_VK_API_VERSION_1_0 || info.required_api_version > VKB_VK_API_VERSION_1_0) {
        PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion = detail::vulkan_functions().fp_vkEnumerateInstanceVersion;

        if (pfn_vkEnumerateInstanceVersion != nullptr) {
            VkResult res = pfn_vkEnumerateInstanceVersion(&instance_version);
            // Should always return VK_SUCCESS
            if (res != VK_SUCCESS && (info.required_api_version > 0 || info.minimum_instance_version > 0)) {
                return make_error_code(InstanceError::vulkan_version_unavailable);
            }
        }
        if (pfn_vkEnumerateInstanceVersion == nullptr ||
            (info.minimum_instance_version > 0 && instance_version < info.minimum_instance_version) ||
            (info.minimum_instance_version == 0 && instance_version < info.required_api_version)) {

            uint32_t version_error = info.minimum_instance_version == 0 ? info.required_api_version : info.minimum_instance_version;
            if (VK_VERSION_MINOR(version_error) == 4)
                return make_error_code(InstanceError::vulkan_version_1_4_unavailable);
            else if (VK_VERSION_MINOR(version_error) == 3)
                return make_error_code(InstanceError::vulkan_version_1_3_unavailable);
            else if (VK_VERSION_MINOR(version_error) == 2)
                return make_error_code(InstanceError::vulkan_version_1_2_unavailable);
            else if (VK_VERSION_MINOR(version_error) == 1)
                return make_error_code(InstanceError::vulkan_version_1_1_unavailable);
            else
                return make_error_code(InstanceError::vulkan_version_unavailable);
        }
    }

    // The API version to use is set by required_api_version, unless it isn't set, then it comes from minimum_instance_version
    uint32_t api_version = VKB_VK_API_VERSION_1_0;
    if (info.required_api_version > VKB_VK_API_VERSION_1_0) {
        api_version = info.required_api_version;
    } else if (info.minimum_instance_version > 0) {
        api_version = info.minimum_instance_version;
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = info.app_name != nullptr ? info.app_name : "";
    app_info.applicationVersion = info.application_version;
    app_info.pEngineName = info.engine_name != nullptr ? info.engine_name : "";
    app_info.engineVersion = info.engine_version;
    app_info.apiVersion = api_version;

    std::vector<const char*> extensions;
    std::vector<const char*> layers;

    for (auto& ext : info.extensions)
        extensions.push_back(ext);
    if (info.debug_callback != nullptr && info.use_debug_messenger && system.debug_utils_available) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    bool properties2_ext_enabled =
        api_version < VKB_VK_API_VERSION_1_1 && detail::check_extension_supported(system.available_extensions,
                                                    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    if (properties2_ext_enabled) {
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (info.layer_settings.size() > 0) {
        extensions.push_back(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);
    }

#if defined(VK_KHR_portability_enumeration)
    bool portability_enumeration_support =
        detail::check_extension_supported(system.available_extensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    if (portability_enumeration_support) {
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
#else
    bool portability_enumeration_support = false;
#endif
    if (!info.headless_context) {
        auto check_add_window_ext = [&](const char* name) -> bool {
            if (!detail::check_extension_supported(system.available_extensions, name)) return false;
            extensions.push_back(name);
            return true;
        };
        bool khr_surface_added = check_add_window_ext("VK_KHR_surface");
#if defined(_WIN32)
        bool added_window_exts = check_add_window_ext("VK_KHR_win32_surface");
#elif defined(__ANDROID__)
        bool added_window_exts = check_add_window_ext("VK_KHR_android_surface");
#elif defined(_DIRECT2DISPLAY)
        bool added_window_exts = check_add_window_ext("VK_KHR_display");
#elif defined(__linux__) || defined(__FreeBSD__)
        // make sure all three calls to check_add_window_ext, don't allow short circuiting
        bool added_window_exts = check_add_window_ext("VK_KHR_xcb_surface");
        added_window_exts = check_add_window_ext("VK_KHR_xlib_surface") || added_window_exts;
        added_window_exts = check_add_window_ext("VK_KHR_wayland_surface") || added_window_exts;
#elif defined(__APPLE__)
        bool added_window_exts = check_add_window_ext("VK_EXT_metal_surface");
#endif
        if (!khr_surface_added || !added_window_exts)
            return make_error_code(InstanceError::windowing_extensions_not_present);
    }
    std::vector<std::string> unsupported_extensions = detail::check_extensions_supported(system.available_extensions, extensions);
    if (!unsupported_extensions.empty()) {
        return { make_error_code(InstanceError::requested_extensions_not_present), unsupported_extensions };
    }

    for (auto& layer : info.layers)
        layers.push_back(layer);

    if (info.enable_validation_layers || (info.request_validation_layers && system.validation_layers_available)) {
        layers.push_back(detail::validation_layer_name);
    }
    std::vector<std::string> unsupported_layers = detail::check_layers_supported(system.available_layers, layers);
    if (!unsupported_layers.empty()) {
        return { make_error_code(InstanceError::requested_layers_not_present), unsupported_layers };
    }

    std::vector<void*> pNext_chain;

    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
    if (info.use_debug_messenger) {
        messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerCreateInfo.pNext = nullptr;
        messengerCreateInfo.messageSeverity = info.debug_message_severity;
        messengerCreateInfo.messageType = info.debug_message_type;
        messengerCreateInfo.pfnUserCallback = info.debug_callback;
        messengerCreateInfo.pUserData = info.debug_user_data_pointer;
        pNext_chain.push_back(&messengerCreateInfo);
    }

    VkValidationFeaturesEXT features{};
    if (info.enabled_validation_features.size() != 0 || info.disabled_validation_features.size()) {
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.pNext = nullptr;
        features.enabledValidationFeatureCount = static_cast<uint32_t>(info.enabled_validation_features.size());
        features.pEnabledValidationFeatures = info.enabled_validation_features.data();
        features.disabledValidationFeatureCount = static_cast<uint32_t>(info.disabled_validation_features.size());
        features.pDisabledValidationFeatures = info.disabled_validation_features.data();
        pNext_chain.push_back(&features);
    }

    VkValidationFlagsEXT checks{};
    if (info.disabled_validation_checks.size() != 0) {
        checks.sType = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT;
        checks.pNext = nullptr;
        checks.disabledValidationCheckCount = static_cast<uint32_t>(info.disabled_validation_checks.size());
        checks.pDisabledValidationChecks = info.disabled_validation_checks.data();
        pNext_chain.push_back(&checks);
    }

    VkLayerSettingsCreateInfoEXT layer_settings_ci{};
    if (info.layer_settings.size() > 0) {
        layer_settings_ci.sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT;
        layer_settings_ci.pNext = nullptr;
        layer_settings_ci.settingCount = static_cast<uint32_t>(info.layer_settings.size());
        layer_settings_ci.pSettings = info.layer_settings.data();
        pNext_chain.push_back(&layer_settings_ci);
    }

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    detail::setup_pNext_chain(instance_create_info, pNext_chain);

    instance_create_info.flags = info.flags;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_create_info.ppEnabledExtensionNames = extensions.data();
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instance_create_info.ppEnabledLayerNames = layers.data();
#if defined(VK_KHR_portability_enumeration)
    if (portability_enumeration_support) {
        instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
#endif

    Instance instance;
    VkResult res =
        detail::vulkan_functions().fp_vkCreateInstance(&instance_create_info, info.allocation_callbacks, &instance.instance);
    if (res != VK_SUCCESS) return Result<Instance>(InstanceError::failed_create_instance, res);

    detail::vulkan_functions().init_instance_funcs(instance.instance);

    if (info.use_debug_messenger) {
        res = create_debug_utils_messenger(instance.instance,
            info.debug_callback,
            info.debug_message_severity,
            info.debug_message_type,
            info.debug_user_data_pointer,
            &instance.debug_messenger,
            info.allocation_callbacks);
        if (res != VK_SUCCESS) {
            return Result<Instance>(InstanceError::failed_create_debug_messenger, res);
        }
    }

    instance.headless = info.headless_context;
    instance.properties2_ext_enabled = properties2_ext_enabled;
    instance.allocation_callbacks = info.allocation_callbacks;
    instance.instance_version = instance_version;
    instance.api_version = api_version;
    instance.fp_vkGetInstanceProcAddr = detail::vulkan_functions().ptr_vkGetInstanceProcAddr;
    instance.fp_vkGetDeviceProcAddr = detail::vulkan_functions().fp_vkGetDeviceProcAddr;
    return instance;
}

InstanceBuilder& InstanceBuilder::set_app_name(const char* app_name) {
    if (!app_name) return *this;
    info.app_name = app_name;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_engine_name(const char* engine_name) {
    if (!engine_name) return *this;
    info.engine_name = engine_name;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_app_version(uint32_t app_version) {
    info.application_version = app_version;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_app_version(uint32_t major, uint32_t minor, uint32_t patch) {
    info.application_version = VKB_MAKE_VK_VERSION(0, major, minor, patch);
    return *this;
}
InstanceBuilder& InstanceBuilder::set_engine_version(uint32_t engine_version) {
    info.engine_version = engine_version;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_engine_version(uint32_t major, uint32_t minor, uint32_t patch) {
    info.engine_version = VKB_MAKE_VK_VERSION(0, major, minor, patch);
    return *this;
}
InstanceBuilder& InstanceBuilder::require_api_version(uint32_t required_api_version) {
    info.required_api_version = required_api_version;
    return *this;
}
InstanceBuilder& InstanceBuilder::require_api_version(uint32_t major, uint32_t minor, uint32_t patch) {
    info.required_api_version = VKB_MAKE_VK_VERSION(0, major, minor, patch);
    return *this;
}
InstanceBuilder& InstanceBuilder::set_minimum_instance_version(uint32_t minimum_instance_version) {
    info.minimum_instance_version = minimum_instance_version;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_minimum_instance_version(uint32_t major, uint32_t minor, uint32_t patch) {
    info.minimum_instance_version = VKB_MAKE_VK_VERSION(0, major, minor, patch);
    return *this;
}
InstanceBuilder& InstanceBuilder::enable_layer(const char* layer_name) {
    if (!layer_name) return *this;
    info.layers.push_back(layer_name);
    return *this;
}
InstanceBuilder& InstanceBuilder::enable_extension(const char* extension_name) {
    if (!extension_name) return *this;
    info.extensions.push_back(extension_name);
    return *this;
}
InstanceBuilder& InstanceBuilder::enable_extensions(size_t count, const char* const* extensions) {
    if (!extensions || count == 0) return *this;
    for (size_t i = 0; i < count; i++) {
        info.extensions.push_back(extensions[i]);
    }
    return *this;
}
InstanceBuilder& InstanceBuilder::enable_validation_layers(bool enable_validation) {
    info.enable_validation_layers = enable_validation;
    return *this;
}
InstanceBuilder& InstanceBuilder::request_validation_layers(bool enable_validation) {
    info.request_validation_layers = enable_validation;
    return *this;
}

InstanceBuilder& InstanceBuilder::use_default_debug_messenger() {
    info.use_debug_messenger = true;
    info.debug_callback = default_debug_callback;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_callback(PFN_vkDebugUtilsMessengerCallbackEXT callback) {
    info.use_debug_messenger = true;
    info.debug_callback = callback;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_callback_user_data_pointer(void* user_data_pointer) {
    info.debug_user_data_pointer = user_data_pointer;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_headless(bool headless) {
    info.headless_context = headless;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_messenger_severity(VkDebugUtilsMessageSeverityFlagsEXT severity) {
    info.debug_message_severity = severity;
    return *this;
}
InstanceBuilder& InstanceBuilder::add_debug_messenger_severity(VkDebugUtilsMessageSeverityFlagsEXT severity) {
    info.debug_message_severity = info.debug_message_severity | severity;
    return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_messenger_type(VkDebugUtilsMessageTypeFlagsEXT type) {
    info.debug_message_type = type;
    return *this;
}
InstanceBuilder& InstanceBuilder::add_debug_messenger_type(VkDebugUtilsMessageTypeFlagsEXT type) {
    info.debug_message_type = info.debug_message_type | type;
    return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_disable(VkValidationCheckEXT check) {
    info.disabled_validation_checks.push_back(check);
    return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_feature_enable(VkValidationFeatureEnableEXT enable) {
    info.enabled_validation_features.push_back(enable);
    return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_feature_disable(VkValidationFeatureDisableEXT disable) {
    info.disabled_validation_features.push_back(disable);
    return *this;
}
InstanceBuilder& InstanceBuilder::set_allocation_callbacks(VkAllocationCallbacks* callbacks) {
    info.allocation_callbacks = callbacks;
    return *this;
}
InstanceBuilder& InstanceBuilder::add_layer_setting(VkLayerSettingEXT setting) {
    info.layer_settings.push_back(setting);
    return *this;
}

void destroy_debug_messenger(VkInstance const instance, VkDebugUtilsMessengerEXT const messenger);


// ---- Physical Device ---- //

namespace detail {

std::vector<std::string> find_unsupported_extensions_in_list(
    std::vector<std::string> const& available_extensions, std::vector<std::string> const& required_extensions) {
    std::vector<std::string> unavailable_extensions;

    for (auto& req_ext : required_extensions) {
        if (!std::binary_search(available_extensions.begin(), available_extensions.end(), req_ext)) {
            unavailable_extensions.push_back(req_ext);
        }
    }
    return unavailable_extensions;
}

// Finds the first queue which supports the desired operations. Returns QUEUE_INDEX_MAX_VALUE if none is found
uint32_t get_first_queue_index(std::vector<VkQueueFamilyProperties> const& families, VkQueueFlags desired_flags) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); i++) {
        if ((families[i].queueFlags & desired_flags) == desired_flags) return i;
    }
    return QUEUE_INDEX_MAX_VALUE;
}
// Finds the queue which is separate from the graphics queue and has the desired flag and not the
// undesired flag, but will select it if no better options are available compute support. Returns
// QUEUE_INDEX_MAX_VALUE if none is found.
uint32_t get_separate_queue_index(
    std::vector<VkQueueFamilyProperties> const& families, VkQueueFlags desired_flags, VkQueueFlags undesired_flags) {
    uint32_t index = QUEUE_INDEX_MAX_VALUE;
    for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); i++) {
        if ((families[i].queueFlags & desired_flags) == desired_flags && ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            if ((families[i].queueFlags & undesired_flags) == 0) {
                return i;
            } else {
                index = i;
            }
        }
    }
    return index;
}

// finds the first queue which supports only the desired flag (not graphics or transfer). Returns QUEUE_INDEX_MAX_VALUE if none is found.
uint32_t get_dedicated_queue_index(
    std::vector<VkQueueFamilyProperties> const& families, VkQueueFlags desired_flags, VkQueueFlags undesired_flags) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); i++) {
        if ((families[i].queueFlags & desired_flags) == desired_flags &&
            (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && (families[i].queueFlags & undesired_flags) == 0)
            return i;
    }
    return QUEUE_INDEX_MAX_VALUE;
}

// finds the first queue which supports presenting. returns QUEUE_INDEX_MAX_VALUE if none is found
uint32_t get_present_queue_index(
    VkPhysicalDevice const phys_device, VkSurfaceKHR const surface, std::vector<VkQueueFamilyProperties> const& families) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); i++) {
        VkBool32 presentSupport = VK_FALSE;
        if (surface != VK_NULL_HANDLE) {
            VkResult res = detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfaceSupportKHR(phys_device, i, surface, &presentSupport);
            if (res != VK_SUCCESS) return QUEUE_INDEX_MAX_VALUE; // TODO: determine if this should fail another way
        }
        if (presentSupport == VK_TRUE) return i;
    }
    return QUEUE_INDEX_MAX_VALUE;
}
} // namespace detail

PhysicalDevice PhysicalDeviceSelector::populate_device_details(
    VkPhysicalDevice vk_phys_device, detail::FeaturesChain const& src_extended_features_chain) const {
    PhysicalDevice physical_device{};
    physical_device.physical_device = vk_phys_device;
    physical_device.surface = instance_info.surface;
    physical_device.defer_surface_initialization = criteria.defer_surface_initialization;
    physical_device.instance_version = instance_info.version;
    auto queue_families = detail::get_vector_noerror<VkQueueFamilyProperties>(
        detail::vulkan_functions().fp_vkGetPhysicalDeviceQueueFamilyProperties, vk_phys_device);
    physical_device.queue_families = queue_families;

    detail::vulkan_functions().fp_vkGetPhysicalDeviceProperties(vk_phys_device, &physical_device.properties);
    detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures(vk_phys_device, &physical_device.features);
    detail::vulkan_functions().fp_vkGetPhysicalDeviceMemoryProperties(vk_phys_device, &physical_device.memory_properties);

    physical_device.name = physical_device.properties.deviceName;

    std::vector<VkExtensionProperties> available_extensions;
    auto available_extensions_ret = detail::get_vector<VkExtensionProperties>(
        available_extensions, detail::vulkan_functions().fp_vkEnumerateDeviceExtensionProperties, vk_phys_device, nullptr);
    if (available_extensions_ret != VK_SUCCESS) return physical_device;
    for (const auto& ext : available_extensions) {
        physical_device.available_extensions.push_back(&ext.extensionName[0]);
    }
    // Lets us quickly find extensions as this list can be 300+ elements long
    std::sort(physical_device.available_extensions.begin(), physical_device.available_extensions.end());

    physical_device.properties2_ext_enabled = instance_info.properties2_ext_enabled;

    auto fill_chain = src_extended_features_chain;

    bool instance_is_1_1 = instance_info.version >= VKB_VK_API_VERSION_1_1;
    if (!fill_chain.empty() && (instance_is_1_1 || instance_info.properties2_ext_enabled)) {
        VkPhysicalDeviceFeatures2 local_features{};
        fill_chain.create_chained_features(local_features);
        // Use KHR function if not able to use the core function
        if (instance_is_1_1) {
            detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures2(vk_phys_device, &local_features);
        } else {
            detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures2KHR(vk_phys_device, &local_features);
        }
        physical_device.extended_features_chain = std::move(fill_chain);
    }

    return physical_device;
}

PhysicalDevice::Suitable PhysicalDeviceSelector::is_device_suitable(
    PhysicalDevice const& pd, std::vector<std::string>& unsuitability_reasons) const {
    PhysicalDevice::Suitable suitable = PhysicalDevice::Suitable::yes;

    if (criteria.name.size() > 0 && criteria.name != pd.properties.deviceName) {
        unsuitability_reasons.push_back(
            "VkPhysicalDeviceProperties::deviceName doesn't match requested name \"" + criteria.name + "\"");
        return PhysicalDevice::Suitable::no;
    }

    if (criteria.required_version > pd.properties.apiVersion) {
        unsuitability_reasons.push_back(
            "VkPhysicalDeviceProperties::apiVersion " + std::to_string(VK_API_VERSION_MAJOR(pd.properties.apiVersion)) +
            "." + std::to_string(VK_API_VERSION_MINOR(pd.properties.apiVersion)) + " lower than required version " +
            std::to_string(VK_API_VERSION_MAJOR(criteria.required_version)) + "." +
            std::to_string(VK_API_VERSION_MINOR(criteria.required_version)));
        return PhysicalDevice::Suitable::no;
    }

    bool dedicated_compute = detail::get_dedicated_queue_index(pd.queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) !=
                             detail::QUEUE_INDEX_MAX_VALUE;
    bool dedicated_transfer = detail::get_dedicated_queue_index(pd.queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) !=
                              detail::QUEUE_INDEX_MAX_VALUE;
    bool separate_compute = detail::get_separate_queue_index(pd.queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) !=
                            detail::QUEUE_INDEX_MAX_VALUE;
    bool separate_transfer = detail::get_separate_queue_index(pd.queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) !=
                             detail::QUEUE_INDEX_MAX_VALUE;

    bool present_queue = detail::get_present_queue_index(pd.physical_device, instance_info.surface, pd.queue_families) !=
                         detail::QUEUE_INDEX_MAX_VALUE;

    if (criteria.require_dedicated_compute_queue && !dedicated_compute) {
        unsuitability_reasons.push_back("No dedicated compute queue");
        return PhysicalDevice::Suitable::no;
    }
    if (criteria.require_dedicated_transfer_queue && !dedicated_transfer) {
        unsuitability_reasons.push_back("No dedicated transfer queue");
        return PhysicalDevice::Suitable::no;
    }
    if (criteria.require_separate_compute_queue && !separate_compute) {
        unsuitability_reasons.push_back("No separate compute queue");
        return PhysicalDevice::Suitable::no;
    }
    if (criteria.require_separate_transfer_queue && !separate_transfer) {
        unsuitability_reasons.push_back("No separate transfer queue");
        return PhysicalDevice::Suitable::no;
    }
    if (criteria.require_present && !present_queue && !criteria.defer_surface_initialization) {
        unsuitability_reasons.push_back("No queue capable of present operations");
        return PhysicalDevice::Suitable::no;
    }
    const auto unsupported_extensions =
        detail::find_unsupported_extensions_in_list(pd.available_extensions, criteria.required_extensions);
    if (unsupported_extensions.size() > 0) {
        for (auto const& unsupported_ext : unsupported_extensions) {
            unsuitability_reasons.push_back("Device extension " + unsupported_ext + " not supported");
        }
        return PhysicalDevice::Suitable::no;
    }
    if (!criteria.defer_surface_initialization && criteria.require_present) {
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;

        auto formats_ret = detail::get_vector<VkSurfaceFormatKHR>(formats,
            detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfaceFormatsKHR,
            pd.physical_device,
            instance_info.surface);
        auto present_modes_ret = detail::get_vector<VkPresentModeKHR>(present_modes,
            detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfacePresentModesKHR,
            pd.physical_device,
            instance_info.surface);

        if (formats_ret != VK_SUCCESS || present_modes_ret != VK_SUCCESS || formats.empty() || present_modes.empty()) {
            if (formats_ret != VK_SUCCESS) {
                unsuitability_reasons.push_back(
                    "vkGetPhysicalDeviceSurfaceFormatsKHR returned error code " + std::to_string(formats_ret));
            }
            if (present_modes_ret != VK_SUCCESS) {
                unsuitability_reasons.push_back(
                    "vkGetPhysicalDeviceSurfacePresentModesKHR returned error code " + std::to_string(present_modes_ret));
            }
            if (formats.empty()) {
                unsuitability_reasons.push_back("vkGetPhysicalDeviceSurfaceFormatsKHR returned zero surface formats");
            }
            if (present_modes.empty()) {
                unsuitability_reasons.push_back(
                    "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes");
            }
            return PhysicalDevice::Suitable::no;
        }
    }

    if (pd.properties.deviceType != static_cast<VkPhysicalDeviceType>(criteria.preferred_type)) {
        if (criteria.allow_any_type) {
            suitable = PhysicalDevice::Suitable::partial;
        } else {
            suitable = PhysicalDevice::Suitable::no;
            unsuitability_reasons.push_back(
                std::string("VkPhysicalDeviceType \"") + to_string_device_type(pd.properties.deviceType) + "\" does not match preferred type \"" +
                to_string_device_type(static_cast<VkPhysicalDeviceType>(criteria.preferred_type)) + "\"");
        }
    }

    detail::compare_VkPhysicalDeviceFeatures(unsuitability_reasons, pd.features, criteria.required_features);
    pd.extended_features_chain.match_all(unsuitability_reasons, criteria.extended_features_chain);
    if (!unsuitability_reasons.empty()) {
        return PhysicalDevice::Suitable::no;
    }

    for (uint32_t i = 0; i < pd.memory_properties.memoryHeapCount; i++) {
        if (pd.memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            if (pd.memory_properties.memoryHeaps[i].size < criteria.required_mem_size) {
                unsuitability_reasons.push_back("Did not contain a Device Local memory heap with enough size");
                return PhysicalDevice::Suitable::no;
            }
        }
    }

    return suitable;
}
// delegate construction to the one with an explicit surface parameter
PhysicalDeviceSelector::PhysicalDeviceSelector(Instance const& instance)
: PhysicalDeviceSelector(instance, VK_NULL_HANDLE) {}

PhysicalDeviceSelector::PhysicalDeviceSelector(Instance const& instance, VkSurfaceKHR surface) {
    instance_info.instance = instance.instance;
    instance_info.version = instance.instance_version;
    instance_info.properties2_ext_enabled = instance.properties2_ext_enabled;
    instance_info.surface = surface;
    criteria.require_present = !instance.headless;
    criteria.required_version = instance.api_version;
}

// Return all devices which are considered suitable - intended for applications which want to let the user pick the physical device
Result<std::vector<PhysicalDevice>> PhysicalDeviceSelector::select_devices() const {
    if (criteria.require_present && !criteria.defer_surface_initialization) {
        if (instance_info.surface == VK_NULL_HANDLE)
            return Result<std::vector<PhysicalDevice>>{ PhysicalDeviceError::no_surface_provided };
    }

    // Get the VkPhysicalDevice handles on the system
    std::vector<VkPhysicalDevice> vk_physical_devices;

    auto vk_physical_devices_ret = detail::get_vector<VkPhysicalDevice>(
        vk_physical_devices, detail::vulkan_functions().fp_vkEnumeratePhysicalDevices, instance_info.instance);
    if (vk_physical_devices_ret != VK_SUCCESS) {
        return Result<std::vector<PhysicalDevice>>{ PhysicalDeviceError::failed_enumerate_physical_devices, vk_physical_devices_ret };
    }
    if (vk_physical_devices.empty()) {
        return Result<std::vector<PhysicalDevice>>{ PhysicalDeviceError::no_physical_devices_found };
    }

    auto fill_out_phys_dev_with_criteria = [&](PhysicalDevice& phys_dev) {
        phys_dev.features = criteria.required_features;
        phys_dev.extended_features_chain = criteria.extended_features_chain;

        bool portability_ext_available =
            criteria.enable_portability_subset &&
            std::binary_search(phys_dev.available_extensions.begin(), phys_dev.available_extensions.end(), "VK_KHR_portability_subset");

        phys_dev.extensions_to_enable.clear();
        phys_dev.extensions_to_enable.insert(
            phys_dev.extensions_to_enable.end(), criteria.required_extensions.begin(), criteria.required_extensions.end());
        if (portability_ext_available) {
            phys_dev.extensions_to_enable.push_back("VK_KHR_portability_subset");
        }
        // Lets us quickly find extensions as this list can be 300+ elements long
        std::sort(phys_dev.extensions_to_enable.begin(), phys_dev.extensions_to_enable.end());
    };

    // if this option is set, always return only the first physical device found
    if (criteria.use_first_gpu_unconditionally && vk_physical_devices.size() > 0) {
        PhysicalDevice physical_device = populate_device_details(vk_physical_devices[0], criteria.extended_features_chain);
        fill_out_phys_dev_with_criteria(physical_device);
        return std::vector<PhysicalDevice>{ physical_device };
    }

    // Populate their details and check their suitability
    std::vector<std::string> unsuitability_reasons;
    std::vector<PhysicalDevice> physical_devices;
    for (auto& vk_physical_device : vk_physical_devices) {
        PhysicalDevice phys_dev = populate_device_details(vk_physical_device, criteria.extended_features_chain);
        std::vector<std::string> gpu_unsuitability_reasons;
        phys_dev.suitable = is_device_suitable(phys_dev, gpu_unsuitability_reasons);
        if (phys_dev.suitable != PhysicalDevice::Suitable::no) {
            physical_devices.push_back(phys_dev);
        } else {
            for (auto const& reason : gpu_unsuitability_reasons) {
                unsuitability_reasons.push_back(
                    std::string("Physical Device ") + phys_dev.properties.deviceName + " not selected due to: " + reason);
            }
        }
    }

    // No suitable devices found, return an error which contains the list of reason why it wasn't suitable
    if (physical_devices.empty()) {
        return Result<std::vector<PhysicalDevice>>{ PhysicalDeviceError::no_suitable_device, unsuitability_reasons };
    }

    // sort the list into fully and partially suitable devices. use stable_partition to maintain relative order
    std::stable_partition(physical_devices.begin(), physical_devices.end(), [](auto const& pd) {
        return pd.suitable == PhysicalDevice::Suitable::yes;
    });

    // Make the physical device ready to be used to create a Device from it
    for (auto& physical_device : physical_devices) {
        fill_out_phys_dev_with_criteria(physical_device);
    }

    return physical_devices;
}

Result<PhysicalDevice> PhysicalDeviceSelector::select() const {
    auto const selected_devices = select_devices();

    if (!selected_devices) return Result<PhysicalDevice>{ selected_devices.full_error() };
    return selected_devices.value().at(0);
}

Result<std::vector<std::string>> PhysicalDeviceSelector::select_device_names() const {
    auto const selected_devices = select_devices();
    if (!selected_devices) return Result<std::vector<std::string>>{ selected_devices.full_error() };

    std::vector<std::string> names;
    for (const auto& pd : selected_devices.value()) {
        names.push_back(pd.name);
    }
    return names;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_surface(VkSurfaceKHR surface) {
    instance_info.surface = surface;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_name(std::string const& name) {
    criteria.name = name;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::prefer_gpu_device_type(PreferredDeviceType type) {
    criteria.preferred_type = type;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::allow_any_gpu_device_type(bool allow_any_type) {
    criteria.allow_any_type = allow_any_type;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_present(bool require) {
    criteria.require_present = require;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_dedicated_transfer_queue() {
    criteria.require_dedicated_transfer_queue = true;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_dedicated_compute_queue() {
    criteria.require_dedicated_compute_queue = true;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_separate_transfer_queue() {
    criteria.require_separate_transfer_queue = true;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_separate_compute_queue() {
    criteria.require_separate_compute_queue = true;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::required_device_memory_size(VkDeviceSize size) {
    criteria.required_mem_size = size;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_required_extension(const char* extension) {
    criteria.required_extensions.push_back(extension);
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_required_extensions(size_t count, const char* const* extensions) {
    if (!extensions || count == 0) return *this;
    for (size_t i = 0; i < count; i++) {
        criteria.required_extensions.push_back(extensions[i]);
    }
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_minimum_version(uint32_t major, uint32_t minor) {
    criteria.required_version = VKB_MAKE_VK_VERSION(0, major, minor, 0);
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_minimum_version(uint32_t minimum_api_version) {
    criteria.required_version = minimum_api_version;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::disable_portability_subset() {
    criteria.enable_portability_subset = false;
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features(VkPhysicalDeviceFeatures const& features) {
    detail::merge_VkPhysicalDeviceFeatures(criteria.required_features, features);
    return *this;
}
#if defined(VKB_VK_API_VERSION_1_2)
// The implementation of the set_required_features_1X functions sets the sType manually. This was a poor choice since
// users of Vulkan should expect to fill out their structs properly. To make the functions take the struct parameter by
// const reference, a local copy must be made in order to set the sType.
PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features_11(VkPhysicalDeviceVulkan11Features const& features_11) {
    VkPhysicalDeviceVulkan11Features features_11_copy = features_11;
    features_11_copy.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    add_required_extension_features(features_11_copy);
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features_12(VkPhysicalDeviceVulkan12Features const& features_12) {
    VkPhysicalDeviceVulkan12Features features_12_copy = features_12;
    features_12_copy.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    add_required_extension_features(features_12_copy);
    return *this;
}
#endif
#if defined(VKB_VK_API_VERSION_1_3)
PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features_13(VkPhysicalDeviceVulkan13Features const& features_13) {
    VkPhysicalDeviceVulkan13Features features_13_copy = features_13;
    features_13_copy.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    add_required_extension_features(features_13_copy);
    return *this;
}
#endif
#if defined(VKB_VK_API_VERSION_1_4)
PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features_14(VkPhysicalDeviceVulkan14Features const& features_14) {
    VkPhysicalDeviceVulkan14Features features_14_copy = features_14;
    features_14_copy.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    add_required_extension_features(features_14_copy);
    return *this;
}
#endif
PhysicalDeviceSelector& PhysicalDeviceSelector::defer_surface_initialization() {
    criteria.defer_surface_initialization = true;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::select_first_device_unconditionally(bool unconditionally) {
    criteria.use_first_gpu_unconditionally = unconditionally;
    return *this;
}

// PhysicalDevice
bool PhysicalDevice::has_dedicated_compute_queue() const {
    return detail::get_dedicated_queue_index(queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) != detail::QUEUE_INDEX_MAX_VALUE;
}
bool PhysicalDevice::has_separate_compute_queue() const {
    return detail::get_separate_queue_index(queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) != detail::QUEUE_INDEX_MAX_VALUE;
}
bool PhysicalDevice::has_dedicated_transfer_queue() const {
    return detail::get_dedicated_queue_index(queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) != detail::QUEUE_INDEX_MAX_VALUE;
}
bool PhysicalDevice::has_separate_transfer_queue() const {
    return detail::get_separate_queue_index(queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) != detail::QUEUE_INDEX_MAX_VALUE;
}
std::vector<VkQueueFamilyProperties> PhysicalDevice::get_queue_families() const { return queue_families; }
std::vector<std::string> PhysicalDevice::get_extensions() const { return extensions_to_enable; }
std::vector<std::string> PhysicalDevice::get_available_extensions() const { return available_extensions; }
bool PhysicalDevice::is_extension_present(const char* ext) const {
    return std::binary_search(std::begin(available_extensions), std::end(available_extensions), ext);
}
bool PhysicalDevice::enable_extension_if_present(const char* extension) {
    if (std::binary_search(std::begin(available_extensions), std::end(available_extensions), extension)) {
        extensions_to_enable.insert(
            std::upper_bound(std::begin(extensions_to_enable), std::end(extensions_to_enable), extension), extension);
        return true;
    }
    return false;
}
bool PhysicalDevice::enable_extensions_if_present(size_t count, const char* const* extensions) {
    for (size_t i = 0; i < count; ++i) {
        const auto extension = extensions[i];
        if (!std::binary_search(std::begin(available_extensions), std::end(available_extensions), extension)) {
            return false;
        }
    }

    for (size_t i = 0; i < count; ++i) {
        extensions_to_enable.insert(
            std::upper_bound(std::begin(extensions_to_enable), std::end(extensions_to_enable), extensions[i]), extensions[i]);
    }
    return true;
}

bool PhysicalDevice::enable_features_if_present(const VkPhysicalDeviceFeatures& features_to_enable) {
    VkPhysicalDeviceFeatures actual_pdf{};
    detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures(physical_device, &actual_pdf);

    std::vector<std::string> unsupported_features;
    detail::compare_VkPhysicalDeviceFeatures(unsupported_features, actual_pdf, features_to_enable);
    if (unsupported_features.empty()) {
        detail::merge_VkPhysicalDeviceFeatures(features, features_to_enable);
        return true;
    }
    return false;
}

bool PhysicalDevice::enable_features_struct_if_present(
    VkStructureType sType, size_t struct_size, const void* features_struct, void* query_struct) {
    VkPhysicalDeviceFeatures2 actual_pdf2{};
    actual_pdf2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    actual_pdf2.pNext = query_struct;

    bool instance_is_1_1 = instance_version >= VKB_VK_API_VERSION_1_1;
    if (instance_is_1_1 || properties2_ext_enabled) {
        if (instance_is_1_1) {
            detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures2(physical_device, &actual_pdf2);
        } else {
            detail::vulkan_functions().fp_vkGetPhysicalDeviceFeatures2KHR(physical_device, &actual_pdf2);
        }

        std::vector<std::string> error_list;
        detail::compare_feature_struct(sType, error_list, query_struct, features_struct);

        if (error_list.empty()) {
            extended_features_chain.add_structure(sType, struct_size, features_struct);
            return true;
        }
    }
    return false;
}


PhysicalDevice::operator VkPhysicalDevice() const { return this->physical_device; }

// ---- Queues ---- //

Result<uint32_t> Device::get_queue_index(QueueType type) const {
    uint32_t index = detail::QUEUE_INDEX_MAX_VALUE;
    switch (type) {
        case QueueType::present:
            index = detail::get_present_queue_index(physical_device.physical_device, surface, queue_families);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::present_unavailable };
            break;
        case QueueType::graphics:
            index = detail::get_first_queue_index(queue_families, VK_QUEUE_GRAPHICS_BIT);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::graphics_unavailable };
            break;
        case QueueType::compute:
            index = detail::get_separate_queue_index(queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) {
                index = detail::get_first_queue_index(queue_families, VK_QUEUE_COMPUTE_BIT);
                if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::compute_unavailable };
            }
            break;
        case QueueType::transfer:
            index = detail::get_separate_queue_index(queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) {
                index = detail::get_first_queue_index(queue_families, VK_QUEUE_TRANSFER_BIT);
                if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::transfer_unavailable };
            }
            break;
        default:
            return Result<uint32_t>{ QueueError::invalid_queue_family_index };
    }
    return index;
}
Result<uint32_t> Device::get_dedicated_queue_index(QueueType type) const {
    uint32_t index = detail::QUEUE_INDEX_MAX_VALUE;
    switch (type) {
        case QueueType::compute:
            index = detail::get_dedicated_queue_index(queue_families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::compute_unavailable };
            break;
        case QueueType::transfer:
            index = detail::get_dedicated_queue_index(queue_families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT);
            if (index == detail::QUEUE_INDEX_MAX_VALUE) return Result<uint32_t>{ QueueError::transfer_unavailable };
            break;
        default:
            return Result<uint32_t>{ QueueError::invalid_queue_family_index };
    }
    return index;
}

Result<VkQueue> Device::get_queue(QueueType type) const {
    auto index = get_queue_index(type);
    if (!index.has_value()) return { index.error() };
    VkQueue out_queue;
    internal_table.fp_vkGetDeviceQueue(device, index.value(), 0, &out_queue);
    return out_queue;
}
Result<VkQueue> Device::get_dedicated_queue(QueueType type) const {
    auto index = get_dedicated_queue_index(type);
    if (!index.has_value()) return { index.error() };
    VkQueue out_queue;
    internal_table.fp_vkGetDeviceQueue(device, index.value(), 0, &out_queue);
    return out_queue;
}

Result<std::pair<VkQueue, uint32_t>> Device::get_queue_and_index(QueueType type) const {
    auto index = get_queue_index(type);
    if (!index.has_value()) return { index.error() };
    VkQueue out_queue;
    internal_table.fp_vkGetDeviceQueue(device, index.value(), 0, &out_queue);
    return std::pair<VkQueue, uint32_t>(out_queue, index.value());
}

Result<std::pair<VkQueue, uint32_t>> Device::get_dedicated_queue_and_index(QueueType type) const {
    auto index = get_dedicated_queue_index(type);
    if (!index.has_value()) return { index.error() };
    VkQueue out_queue;
    internal_table.fp_vkGetDeviceQueue(device, index.value(), 0, &out_queue);
    return std::pair<VkQueue, uint32_t>(out_queue, index.value());
}

// ---- Dispatch ---- //

DispatchTable Device::make_table() const { return { device, fp_vkGetDeviceProcAddr }; }

// ---- Device ---- //

Device::operator VkDevice() const { return this->device; }

void destroy_device(Device const& device) {
    if (device.device != VK_NULL_HANDLE) {
        device.internal_table.fp_vkDestroyDevice(device.device, device.allocation_callbacks);
    }
}

DeviceBuilder::DeviceBuilder(PhysicalDevice phys_device) { physical_device = std::move(phys_device); }

Result<Device> DeviceBuilder::build() const {

    std::vector<CustomQueueDescription> queue_descriptions;
    queue_descriptions.insert(queue_descriptions.end(), info.queue_descriptions.begin(), info.queue_descriptions.end());

    if (queue_descriptions.empty()) {
        for (uint32_t i = 0; i < physical_device.queue_families.size(); i++) {
            queue_descriptions.emplace_back(i, std::vector<float>{ 1.0f });
        }
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto& desc : queue_descriptions) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = desc.index;
        queue_create_info.queueCount = static_cast<std::uint32_t>(desc.priorities.size());
        queue_create_info.pQueuePriorities = desc.priorities.data();
        queueCreateInfos.push_back(queue_create_info);
    }

    std::vector<const char*> extensions_to_enable;
    for (const auto& ext : physical_device.extensions_to_enable) {
        extensions_to_enable.push_back(ext.c_str());
    }
    if (physical_device.surface != VK_NULL_HANDLE || physical_device.defer_surface_initialization)
        extensions_to_enable.push_back({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });

    std::vector<void*> final_pnext_chain;
    VkDeviceCreateInfo device_create_info = {};

    bool user_defined_phys_dev_features_2 = false;
    for (auto& pnext : info.pNext_chain) {
        VkBaseOutStructure out_structure{};
        memcpy(&out_structure, pnext, sizeof(VkBaseOutStructure));
        if (out_structure.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) {
            user_defined_phys_dev_features_2 = true;
            break;
        }
    }

    if (user_defined_phys_dev_features_2 && !physical_device.extended_features_chain.empty()) {
        return { DeviceError::VkPhysicalDeviceFeatures2_in_pNext_chain_while_using_add_required_extension_features };
    }

    // These objects must be alive during the call to vkCreateDevice
    auto physical_device_extension_features_copy = physical_device.extended_features_chain;
    VkPhysicalDeviceFeatures2 local_features2{};
    local_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    local_features2.features = physical_device.features;

    if (!user_defined_phys_dev_features_2) {
        if (physical_device.instance_version >= VKB_VK_API_VERSION_1_1 || physical_device.properties2_ext_enabled) {
            final_pnext_chain.push_back(&local_features2);
            auto features_chain_members = physical_device_extension_features_copy.get_pNext_chain_members();
            for (auto& features_struct : features_chain_members) {
                final_pnext_chain.push_back(features_struct);
            }
        } else {
            // Only set device_create_info.pEnabledFeatures when the pNext chain does not contain a VkPhysicalDeviceFeatures2 structure
            device_create_info.pEnabledFeatures = &physical_device.features;
        }
    }

    for (auto& pNext : info.pNext_chain) {
        final_pnext_chain.push_back(pNext);
    }

    detail::setup_pNext_chain(device_create_info, final_pnext_chain);

    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.flags = info.flags;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    device_create_info.pQueueCreateInfos = queueCreateInfos.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions_to_enable.size());
    device_create_info.ppEnabledExtensionNames = extensions_to_enable.data();

    Device device;

    VkResult res = detail::vulkan_functions().fp_vkCreateDevice(
        physical_device.physical_device, &device_create_info, info.allocation_callbacks, &device.device);
    if (res != VK_SUCCESS) {
        return { DeviceError::failed_create_device, res };
    }

    device.physical_device = physical_device;
    device.surface = physical_device.surface;
    device.queue_families = physical_device.queue_families;
    device.allocation_callbacks = info.allocation_callbacks;
    device.fp_vkGetDeviceProcAddr = detail::vulkan_functions().fp_vkGetDeviceProcAddr;
    detail::vulkan_functions().get_device_proc_addr(device.device, device.internal_table.fp_vkGetDeviceQueue, "vkGetDeviceQueue");
    detail::vulkan_functions().get_device_proc_addr(device.device, device.internal_table.fp_vkDestroyDevice, "vkDestroyDevice");
    device.instance_version = physical_device.instance_version;
    return device;
}
DeviceBuilder& DeviceBuilder::set_allocation_callbacks(VkAllocationCallbacks* callbacks) {
    info.allocation_callbacks = callbacks;
    return *this;
}
DeviceBuilder& DeviceBuilder::custom_queue_setup(size_t count, CustomQueueDescription const* queue_descriptions) {
    info.queue_descriptions.assign(queue_descriptions, queue_descriptions + count);
    return *this;
}
DeviceBuilder& DeviceBuilder::custom_queue_setup(std::vector<CustomQueueDescription> const& queue_descriptions) {
    info.queue_descriptions = queue_descriptions;
    return *this;
}
DeviceBuilder& DeviceBuilder::custom_queue_setup(std::vector<CustomQueueDescription>&& queue_descriptions) {
    info.queue_descriptions = std::move(queue_descriptions);
    return *this;
}
#if VKB_SPAN_OVERLOADS
DeviceBuilder& DeviceBuilder::custom_queue_setup(std::span<const CustomQueueDescription> queue_descriptions) {
    info.queue_descriptions.assign(queue_descriptions.begin(), queue_descriptions.end());
    return *this;
}
#endif
DeviceBuilder& DeviceBuilder::add_pNext(void* structure_to_add) {
    detail::add_all_pnext_structures(info.pNext_chain, structure_to_add);
    return *this;
}

// ---- Swapchain ---- //

namespace detail {
struct SurfaceSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

enum class SurfaceSupportError {
    surface_handle_null,
    failed_get_surface_capabilities,
    failed_enumerate_surface_formats,
    failed_enumerate_present_modes,
    no_suitable_desired_format
};

struct SurfaceSupportErrorCategory : std::error_category {
    const char* name() const noexcept override { return "vbk_surface_support"; }
    std::string message(int err) const override {
        switch (static_cast<SurfaceSupportError>(err)) {
            CASE_TO_STRING(SurfaceSupportError, surface_handle_null)
            CASE_TO_STRING(SurfaceSupportError, failed_get_surface_capabilities)
            CASE_TO_STRING(SurfaceSupportError, failed_enumerate_surface_formats)
            CASE_TO_STRING(SurfaceSupportError, failed_enumerate_present_modes)
            CASE_TO_STRING(SurfaceSupportError, no_suitable_desired_format)
            default:
                return "";
        }
    }
};
const SurfaceSupportErrorCategory surface_support_error_category;

std::error_code make_error_code(SurfaceSupportError surface_support_error) {
    return { static_cast<int>(surface_support_error), detail::surface_support_error_category };
}

Result<SurfaceSupportDetails> query_surface_support_details(VkPhysicalDevice phys_device, VkSurfaceKHR surface) {
    if (surface == VK_NULL_HANDLE) return make_error_code(SurfaceSupportError::surface_handle_null);

    VkSurfaceCapabilitiesKHR capabilities;
    VkResult res = detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys_device, surface, &capabilities);
    if (res != VK_SUCCESS) {
        return { make_error_code(SurfaceSupportError::failed_get_surface_capabilities), res };
    }

    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    auto formats_ret = detail::get_vector<VkSurfaceFormatKHR>(
        formats, detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfaceFormatsKHR, phys_device, surface);
    if (formats_ret != VK_SUCCESS)
        return { make_error_code(SurfaceSupportError::failed_enumerate_surface_formats), formats_ret };
    auto present_modes_ret = detail::get_vector<VkPresentModeKHR>(
        present_modes, detail::vulkan_functions().fp_vkGetPhysicalDeviceSurfacePresentModesKHR, phys_device, surface);
    if (present_modes_ret != VK_SUCCESS)
        return { make_error_code(SurfaceSupportError::failed_enumerate_present_modes), present_modes_ret };

    return SurfaceSupportDetails{ capabilities, formats, present_modes };
}

Result<VkSurfaceFormatKHR> find_desired_surface_format(
    std::vector<VkSurfaceFormatKHR> const& available_formats, std::vector<VkSurfaceFormatKHR> const& desired_formats) {
    for (auto const& desired_format : desired_formats) {
        for (auto const& available_format : available_formats) {
            // finds the first format that is desired and available
            if (desired_format.format == available_format.format && desired_format.colorSpace == available_format.colorSpace) {
                return desired_format;
            }
        }
    }

    // if no desired format is available, we report that no format is suitable to the user request
    return { make_error_code(SurfaceSupportError::no_suitable_desired_format) };
}

VkSurfaceFormatKHR find_best_surface_format(
    std::vector<VkSurfaceFormatKHR> const& available_formats, std::vector<VkSurfaceFormatKHR> const& desired_formats) {
    auto surface_format_ret = detail::find_desired_surface_format(available_formats, desired_formats);
    if (surface_format_ret.has_value()) return surface_format_ret.value();

    // use the first available format as a fallback if any desired formats aren't found
    return available_formats[0];
}

VkPresentModeKHR find_present_mode(std::vector<VkPresentModeKHR> const& available_resent_modes,
    std::vector<VkPresentModeKHR> const& desired_present_modes) {
    for (auto const& desired_pm : desired_present_modes) {
        for (auto const& available_pm : available_resent_modes) {
            // finds the first present mode that is desired and available
            if (desired_pm == available_pm) return desired_pm;
        }
    }
    // only present mode required, use as a fallback
    return VK_PRESENT_MODE_FIFO_KHR;
}

template <typename T> T minimum(T a, T b) { return a < b ? a : b; }
template <typename T> T maximum(T a, T b) { return a > b ? a : b; }

VkExtent2D find_extent(VkSurfaceCapabilitiesKHR const& capabilities, uint32_t desired_width, uint32_t desired_height) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { desired_width, desired_height };

        actualExtent.width =
            maximum(capabilities.minImageExtent.width, minimum(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height =
            maximum(capabilities.minImageExtent.height, minimum(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
} // namespace detail

void destroy_swapchain(Swapchain const& swapchain) {
    if (swapchain.device != VK_NULL_HANDLE && swapchain.swapchain != VK_NULL_HANDLE) {
        swapchain.internal_table.fp_vkDestroySwapchainKHR(swapchain.device, swapchain.swapchain, swapchain.allocation_callbacks);
    }
}

SwapchainBuilder::SwapchainBuilder(Device const& device) {
    info.physical_device = device.physical_device.physical_device;
    info.device = device.device;
    info.surface = device.surface;
    info.instance_version = device.instance_version;
    auto present = device.get_queue_index(QueueType::present);
    auto graphics = device.get_queue_index(QueueType::graphics);
    assert(graphics.has_value() && present.has_value() && "Graphics and Present queue indexes must be valid");
    info.graphics_queue_index = graphics.value();
    info.present_queue_index = present.value();
}
SwapchainBuilder::SwapchainBuilder(Device const& device, VkSurfaceKHR const surface) {
    info.physical_device = device.physical_device.physical_device;
    info.device = device.device;
    info.surface = surface;
    info.instance_version = device.instance_version;
    Device temp_device = device;
    temp_device.surface = surface;
    auto present = temp_device.get_queue_index(QueueType::present);
    auto graphics = temp_device.get_queue_index(QueueType::graphics);
    assert(graphics.has_value() && present.has_value() && "Graphics and Present queue indexes must be valid");
    info.graphics_queue_index = graphics.value();
    info.present_queue_index = present.value();
}
SwapchainBuilder::SwapchainBuilder(VkPhysicalDevice const physical_device,
    VkDevice const device,
    VkSurfaceKHR const surface,
    uint32_t graphics_queue_index,
    uint32_t present_queue_index) {
    info.physical_device = physical_device;
    info.device = device;
    info.surface = surface;
    info.graphics_queue_index = graphics_queue_index;
    info.present_queue_index = present_queue_index;
    if (graphics_queue_index == detail::QUEUE_INDEX_MAX_VALUE || present_queue_index == detail::QUEUE_INDEX_MAX_VALUE) {
        auto queue_families = detail::get_vector_noerror<VkQueueFamilyProperties>(
            detail::vulkan_functions().fp_vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
        if (graphics_queue_index == detail::QUEUE_INDEX_MAX_VALUE)
            info.graphics_queue_index = detail::get_first_queue_index(queue_families, VK_QUEUE_GRAPHICS_BIT);
        if (present_queue_index == detail::QUEUE_INDEX_MAX_VALUE)
            info.present_queue_index = detail::get_present_queue_index(physical_device, surface, queue_families);
    }
}
Result<Swapchain> SwapchainBuilder::build() const {
    if (info.surface == VK_NULL_HANDLE) {
        return Result<Swapchain>{ SwapchainError::surface_handle_not_provided };
    }

    auto desired_formats = info.desired_formats;
    if (desired_formats.empty()) add_desired_formats(desired_formats);
    auto desired_present_modes = info.desired_present_modes;
    if (desired_present_modes.empty()) add_desired_present_modes(desired_present_modes);

    auto surface_support_ret = detail::query_surface_support_details(info.physical_device, info.surface);
    if (!surface_support_ret.has_value())
        return Result<Swapchain>{ SwapchainError::failed_query_surface_support_details, surface_support_ret.vk_result() };
    auto surface_support = surface_support_ret.value();

    uint32_t image_count = info.min_image_count;
    if (info.required_min_image_count >= 1) {
        if (info.required_min_image_count < surface_support.capabilities.minImageCount)
            return make_error_code(SwapchainError::required_min_image_count_too_low);

        image_count = info.required_min_image_count;
    } else if (info.min_image_count == 0) {
        // We intentionally use minImageCount + 1 to maintain existing behavior, even if it typically results in triple buffering on most systems.
        image_count = surface_support.capabilities.minImageCount + 1;
    } else {
        image_count = info.min_image_count;
        if (image_count < surface_support.capabilities.minImageCount)
            image_count = surface_support.capabilities.minImageCount;
    }
    if (surface_support.capabilities.maxImageCount > 0 && image_count > surface_support.capabilities.maxImageCount) {
        image_count = surface_support.capabilities.maxImageCount;
    }

    VkSurfaceFormatKHR surface_format = detail::find_best_surface_format(surface_support.formats, desired_formats);

    VkExtent2D extent = detail::find_extent(surface_support.capabilities, info.desired_width, info.desired_height);

    uint32_t image_array_layers = info.array_layer_count;
    if (surface_support.capabilities.maxImageArrayLayers < info.array_layer_count)
        image_array_layers = surface_support.capabilities.maxImageArrayLayers;
    if (info.array_layer_count == 0) image_array_layers = 1;

    uint32_t queue_family_indices[] = { info.graphics_queue_index, info.present_queue_index };


    VkPresentModeKHR present_mode = detail::find_present_mode(surface_support.present_modes, desired_present_modes);

    // VkSurfaceCapabilitiesKHR::supportedUsageFlags is only only valid for some present modes. For shared present modes, we should also check VkSharedPresentSurfaceCapabilitiesKHR::sharedPresentSupportedUsageFlags.
    auto is_unextended_present_mode = [](VkPresentModeKHR present_mode) {
        return (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) || (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) ||
               (present_mode == VK_PRESENT_MODE_FIFO_KHR) || (present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR);
    };

    if (is_unextended_present_mode(present_mode) &&
        (info.image_usage_flags & surface_support.capabilities.supportedUsageFlags) != info.image_usage_flags) {
        return Result<Swapchain>{ SwapchainError::required_usage_not_supported };
    }

    VkSurfaceTransformFlagBitsKHR pre_transform = info.pre_transform;
    if (info.pre_transform == static_cast<VkSurfaceTransformFlagBitsKHR>(0))
        pre_transform = surface_support.capabilities.currentTransform;

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    detail::setup_pNext_chain(swapchain_create_info, info.pNext_chain);

    swapchain_create_info.flags = info.create_flags;
    swapchain_create_info.surface = info.surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = image_array_layers;
    swapchain_create_info.imageUsage = info.image_usage_flags;

    if (info.graphics_queue_index != info.present_queue_index) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchain_create_info.preTransform = pre_transform;
    swapchain_create_info.compositeAlpha = info.composite_alpha;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = info.clipped;
    swapchain_create_info.oldSwapchain = info.old_swapchain;
    Swapchain swapchain{};
    PFN_vkCreateSwapchainKHR swapchain_create_proc;
    detail::vulkan_functions().get_device_proc_addr(info.device, swapchain_create_proc, "vkCreateSwapchainKHR");
    auto res = swapchain_create_proc(info.device, &swapchain_create_info, info.allocation_callbacks, &swapchain.swapchain);

    if (res != VK_SUCCESS) {
        return Result<Swapchain>{ SwapchainError::failed_create_swapchain, res };
    }
    swapchain.device = info.device;
    swapchain.image_format = surface_format.format;
    swapchain.color_space = surface_format.colorSpace;
    swapchain.image_usage_flags = info.image_usage_flags;
    swapchain.extent = extent;
    detail::vulkan_functions().get_device_proc_addr(
        info.device, swapchain.internal_table.fp_vkGetSwapchainImagesKHR, "vkGetSwapchainImagesKHR");
    detail::vulkan_functions().get_device_proc_addr(info.device, swapchain.internal_table.fp_vkCreateImageView, "vkCreateImageView");
    detail::vulkan_functions().get_device_proc_addr(info.device, swapchain.internal_table.fp_vkDestroyImageView, "vkDestroyImageView");
    detail::vulkan_functions().get_device_proc_addr(
        info.device, swapchain.internal_table.fp_vkDestroySwapchainKHR, "vkDestroySwapchainKHR");
    auto images = swapchain.get_images();
    if (!images) {
        return Result<Swapchain>{ SwapchainError::failed_get_swapchain_images };
    }
    swapchain.requested_min_image_count = image_count;
    swapchain.present_mode = present_mode;
    swapchain.image_count = static_cast<uint32_t>(images.value().size());
    swapchain.instance_version = info.instance_version;
    swapchain.allocation_callbacks = info.allocation_callbacks;
    return swapchain;
}
Result<std::vector<VkImage>> Swapchain::get_images() {
    std::vector<VkImage> swapchain_images;

    auto swapchain_images_ret =
        detail::get_vector<VkImage>(swapchain_images, internal_table.fp_vkGetSwapchainImagesKHR, device, swapchain);
    if (swapchain_images_ret != VK_SUCCESS) {
        return Result<std::vector<VkImage>>{ SwapchainError::failed_get_swapchain_images, swapchain_images_ret };
    }
    return swapchain_images;
}
Result<std::vector<VkImageView>> Swapchain::get_image_views() { return get_image_views(nullptr); }
Result<std::vector<VkImageView>> Swapchain::get_image_views(const void* pNext) {
    auto images_and_image_views_ret = get_images_and_image_views(pNext);
    if (!images_and_image_views_ret) return { images_and_image_views_ret.error() };
    return std::move(images_and_image_views_ret.value().second);
}
Result<std::pair<std::vector<VkImage>, std::vector<VkImageView>>> Swapchain::get_images_and_image_views() {
    return get_images_and_image_views(nullptr);
}
Result<std::pair<std::vector<VkImage>, std::vector<VkImageView>>> Swapchain::get_images_and_image_views(const void* pNext) {
    auto swapchain_images_ret = get_images();
    if (!swapchain_images_ret) return swapchain_images_ret.error();
    auto& swapchain_images = swapchain_images_ret.value();

    bool already_contains_image_view_usage = false;
    const void* pNext_iter = pNext;
    while (pNext_iter) {
        VkBaseOutStructure structure{};
        memcpy(&structure, pNext, sizeof(VkBaseOutStructure));
        if (structure.sType == VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO) {
            already_contains_image_view_usage = true;
            break;
        }
        pNext_iter = structure.pNext;
    }
    VkImageViewUsageCreateInfo desired_flags{};
    desired_flags.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO;
    desired_flags.pNext = pNext;
    desired_flags.usage = image_usage_flags;

    std::vector<VkImageView> views(swapchain_images.size());
    for (size_t i = 0; i < swapchain_images.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        if (instance_version >= VKB_VK_API_VERSION_1_1 && !already_contains_image_view_usage) {
            createInfo.pNext = &desired_flags;
        } else {
            createInfo.pNext = pNext;
        }

        createInfo.image = swapchain_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = image_format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        VkResult res = internal_table.fp_vkCreateImageView(device, &createInfo, allocation_callbacks, &views[i]);
        if (res != VK_SUCCESS) {
            // Cleanup already created image views
            destroy_image_views(i, views.data());
            return Result<std::pair<std::vector<VkImage>, std::vector<VkImageView>>>{ SwapchainError::failed_create_swapchain_image_views,
                res };
        }
    }
    return std::pair{ std::move(swapchain_images), std::move(views) };
}
void Swapchain::destroy_image_views(size_t count, VkImageView const* image_views) {
    for (size_t i = 0; i < count; ++i) {
        internal_table.fp_vkDestroyImageView(device, image_views[i], allocation_callbacks);
    }
}
void Swapchain::destroy_image_views(std::vector<VkImageView> const& image_views) {
    destroy_image_views(image_views.size(), image_views.data());
}
#if VKB_SPAN_OVERLOADS
void Swapchain::destroy_image_views(std::span<const VkImageView> image_views) {
    destroy_image_views(image_views.size(), image_views.data());
}
#endif
Swapchain::operator VkSwapchainKHR() const { return this->swapchain; }
SwapchainBuilder& SwapchainBuilder::set_old_swapchain(VkSwapchainKHR old_swapchain) {
    info.old_swapchain = old_swapchain;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_old_swapchain(Swapchain const& swapchain) {
    info.old_swapchain = swapchain.swapchain;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_desired_extent(uint32_t width, uint32_t height) {
    info.desired_width = width;
    info.desired_height = height;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_desired_format(VkSurfaceFormatKHR format) {
    info.desired_formats.insert(info.desired_formats.begin(), format);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::add_fallback_format(VkSurfaceFormatKHR format) {
    info.desired_formats.push_back(format);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_format_selection() {
    info.desired_formats.clear();
    add_desired_formats(info.desired_formats);
    return *this;
}

SwapchainBuilder& SwapchainBuilder::set_desired_present_mode(VkPresentModeKHR present_mode) {
    info.desired_present_modes.insert(info.desired_present_modes.begin(), present_mode);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::add_fallback_present_mode(VkPresentModeKHR present_mode) {
    info.desired_present_modes.push_back(present_mode);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_present_mode_selection() {
    info.desired_present_modes.clear();
    add_desired_present_modes(info.desired_present_modes);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_allocation_callbacks(VkAllocationCallbacks* callbacks) {
    info.allocation_callbacks = callbacks;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::add_pNext(void* structure_to_add) {
    detail::add_all_pnext_structures(info.pNext_chain, structure_to_add);
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_image_usage_flags(VkImageUsageFlags usage_flags) {
    info.image_usage_flags = usage_flags;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::add_image_usage_flags(VkImageUsageFlags usage_flags) {
    info.image_usage_flags = info.image_usage_flags | usage_flags;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_image_usage_flags() {
    info.image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_image_array_layer_count(uint32_t array_layer_count) {
    info.array_layer_count = array_layer_count;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_desired_min_image_count(uint32_t min_image_count) {
    info.min_image_count = min_image_count;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_required_min_image_count(uint32_t required_min_image_count) {
    info.required_min_image_count = required_min_image_count;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_clipped(bool clipped) {
    info.clipped = clipped;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_create_flags(VkSwapchainCreateFlagBitsKHR create_flags) {
    info.create_flags = create_flags;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_pre_transform_flags(VkSurfaceTransformFlagBitsKHR pre_transform_flags) {
    info.pre_transform = pre_transform_flags;
    return *this;
}
SwapchainBuilder& SwapchainBuilder::set_composite_alpha_flags(VkCompositeAlphaFlagBitsKHR composite_alpha_flags) {
    info.composite_alpha = composite_alpha_flags;
    return *this;
}

void SwapchainBuilder::add_desired_formats(std::vector<VkSurfaceFormatKHR>& formats) const {
    formats.push_back({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
    formats.push_back({ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
}
void SwapchainBuilder::add_desired_present_modes(std::vector<VkPresentModeKHR>& modes) const {
    modes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
    modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
}
} // namespace vkb
