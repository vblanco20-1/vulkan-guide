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

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#include <fcntl.h>
#define NOMINMAX
#include <windows.h>
#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

#include <mutex>

namespace vkb {

namespace detail {

class VulkanFunctions {
	private:
	std::mutex init_mutex;
	struct VulkanLibrary {
#if defined(__linux__) || defined(__APPLE__)
		void* library;
#elif defined(_WIN32)
		HMODULE library;
#endif
		PFN_vkGetInstanceProcAddr ptr_vkGetInstanceProcAddr = VK_NULL_HANDLE;

		VulkanLibrary () {
#if defined(__linux__)
			library = dlopen ("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
			if (!library) library = dlopen ("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#elif defined(__APPLE__)
			library = dlopen ("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
			if (!library) library = dlopen ("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
#elif defined(_WIN32)
			library = LoadLibrary (TEXT ("vulkan-1.dll"));
#else
			assert (false && "Unsupported platform");
#endif
			if (!library) return;
			load_func (ptr_vkGetInstanceProcAddr, "vkGetInstanceProcAddr");
		}

		template <typename T> void load_func (T& func_dest, const char* func_name) {
#if defined(__linux__) || defined(__APPLE__)
			func_dest = reinterpret_cast<T> (dlsym (library, func_name));
#elif defined(_WIN32)
			func_dest = reinterpret_cast<T> (GetProcAddress (library, func_name));
#endif
		}
		void close () {
#if defined(__linux__) || defined(__APPLE__)
			dlclose (library);
#elif defined(_WIN32)
			FreeLibrary (library);
#endif
			library = 0;
		}
	};
	VulkanLibrary& get_vulkan_library () {
		static VulkanLibrary lib;
		return lib;
	}

	bool load_vulkan (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr = nullptr) {
		if (fp_vkGetInstanceProcAddr != nullptr) {
			ptr_vkGetInstanceProcAddr = fp_vkGetInstanceProcAddr;
			return true;
		} else {
			auto& lib = get_vulkan_library ();
			ptr_vkGetInstanceProcAddr = lib.ptr_vkGetInstanceProcAddr;
			return lib.library != nullptr && lib.ptr_vkGetInstanceProcAddr != VK_NULL_HANDLE;
		}
	}

	template <typename T> void get_proc_addr (T& out_ptr, const char* func_name) {
		out_ptr = reinterpret_cast<T> (ptr_vkGetInstanceProcAddr (instance, func_name));
	}

	void init_pre_instance_funcs () {
		get_proc_addr (fp_vkEnumerateInstanceExtensionProperties, "vkEnumerateInstanceExtensionProperties");
		get_proc_addr (fp_vkEnumerateInstanceLayerProperties, "vkEnumerateInstanceLayerProperties");
		get_proc_addr (fp_vkEnumerateInstanceVersion, "vkEnumerateInstanceVersion");
		get_proc_addr (fp_vkCreateInstance, "vkCreateInstance");
	}

	public:
	PFN_vkGetInstanceProcAddr ptr_vkGetInstanceProcAddr = nullptr;
	VkInstance instance = nullptr;

	PFN_vkEnumerateInstanceExtensionProperties fp_vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties fp_vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateInstanceVersion fp_vkEnumerateInstanceVersion = nullptr;
	PFN_vkCreateInstance fp_vkCreateInstance = nullptr;
	PFN_vkDestroyInstance fp_vkDestroyInstance = nullptr;

	PFN_vkEnumeratePhysicalDevices fp_vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceFeatures fp_vkGetPhysicalDeviceFeatures = nullptr;
	PFN_vkGetPhysicalDeviceFeatures2 fp_vkGetPhysicalDeviceFeatures2 = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties fp_vkGetPhysicalDeviceFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties fp_vkGetPhysicalDeviceImageFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties fp_vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties2 fp_vkGetPhysicalDeviceProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties fp_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 fp_vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties fp_vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties2 fp_vkGetPhysicalDeviceFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties2 fp_vkGetPhysicalDeviceMemoryProperties2 = nullptr;

	PFN_vkCreateDevice fp_vkCreateDevice = nullptr;
	PFN_vkDestroyDevice fp_vkDestroyDevice = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties fp_vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkGetDeviceQueue fp_vkGetDeviceQueue = nullptr;

	PFN_vkCreateImageView fp_vkCreateImageView = nullptr;
	PFN_vkDestroyImageView fp_vkDestroyImageView = nullptr;

	PFN_vkDestroySurfaceKHR fp_vkDestroySurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fp_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fp_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fp_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkCreateSwapchainKHR fp_vkCreateSwapchainKHR = nullptr;
	PFN_vkDestroySwapchainKHR fp_vkDestroySwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR fp_vkGetSwapchainImagesKHR = nullptr;

	bool init_vulkan_funcs (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr) {
		std::lock_guard<std::mutex> lg (init_mutex);
		if (!load_vulkan (fp_vkGetInstanceProcAddr)) return false;
		init_pre_instance_funcs ();
		return true;
	}

	template <typename T> void get_inst_proc_addr (T& out_ptr, const char* func_name) {
		std::lock_guard<std::mutex> lg (init_mutex);
		get_proc_addr (out_ptr, func_name);
	}

	void init_instance_funcs (VkInstance inst) {
		std::lock_guard<std::mutex> lg (init_mutex);

		instance = inst;
		get_proc_addr (fp_vkDestroyInstance, "vkDestroyInstance");
		get_proc_addr (fp_vkEnumeratePhysicalDevices, "vkEnumeratePhysicalDevices");
		get_proc_addr (fp_vkGetPhysicalDeviceFeatures, "vkGetPhysicalDeviceFeatures");
		get_proc_addr (fp_vkGetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2");
		get_proc_addr (fp_vkGetPhysicalDeviceFormatProperties, "vkGetPhysicalDeviceFormatProperties");
		get_proc_addr (fp_vkGetPhysicalDeviceImageFormatProperties, "vkGetPhysicalDeviceImageFormatProperties");
		get_proc_addr (fp_vkGetPhysicalDeviceProperties, "vkGetPhysicalDeviceProperties");
		get_proc_addr (fp_vkGetPhysicalDeviceProperties2, "vkGetPhysicalDeviceProperties2");
		get_proc_addr (fp_vkGetPhysicalDeviceQueueFamilyProperties, "vkGetPhysicalDeviceQueueFamilyProperties");
		get_proc_addr (fp_vkGetPhysicalDeviceQueueFamilyProperties2, "vkGetPhysicalDeviceQueueFamilyProperties2");
		get_proc_addr (fp_vkGetPhysicalDeviceMemoryProperties, "vkGetPhysicalDeviceMemoryProperties");
		get_proc_addr (fp_vkGetPhysicalDeviceFormatProperties2, "vkGetPhysicalDeviceFormatProperties2");
		get_proc_addr (fp_vkGetPhysicalDeviceMemoryProperties2, "vkGetPhysicalDeviceMemoryProperties2");

		get_proc_addr (fp_vkCreateDevice, "vkCreateDevice");
		get_proc_addr (fp_vkDestroyDevice, "vkDestroyDevice");
		get_proc_addr (fp_vkEnumerateDeviceExtensionProperties, "vkEnumerateDeviceExtensionProperties");
		get_proc_addr (fp_vkGetDeviceQueue, "vkGetDeviceQueue");

		get_proc_addr (fp_vkCreateImageView, "vkCreateImageView");
		get_proc_addr (fp_vkDestroyImageView, "vkDestroyImageView");

		get_proc_addr (fp_vkDestroySurfaceKHR, "vkDestroySurfaceKHR");
		get_proc_addr (fp_vkGetPhysicalDeviceSurfaceSupportKHR, "vkGetPhysicalDeviceSurfaceSupportKHR");
		get_proc_addr (fp_vkGetPhysicalDeviceSurfaceFormatsKHR, "vkGetPhysicalDeviceSurfaceFormatsKHR");
		get_proc_addr (fp_vkGetPhysicalDeviceSurfacePresentModesKHR, "vkGetPhysicalDeviceSurfacePresentModesKHR");
		get_proc_addr (fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
		get_proc_addr (fp_vkCreateSwapchainKHR, "vkCreateSwapchainKHR");
		get_proc_addr (fp_vkDestroySwapchainKHR, "vkDestroySwapchainKHR");
		get_proc_addr (fp_vkGetSwapchainImagesKHR, "vkGetSwapchainImagesKHR");
	}
};

VulkanFunctions& vulkan_functions () {
	static VulkanFunctions v;
	return v;
}

// Helper for robustly executing the two-call pattern
template <typename T, typename F, typename... Ts>
auto get_vector (std::vector<T>& out, F&& f, Ts&&... ts) -> VkResult {
	uint32_t count = 0;
	VkResult err;
	do {
		err = f (ts..., &count, nullptr);
		if (err) {
			return err;
		};
		out.resize (count);
		err = f (ts..., &count, out.data ());
		out.resize (count);
	} while (err == VK_INCOMPLETE);
	return err;
}

template <typename T, typename F, typename... Ts>
auto get_vector_noerror (F&& f, Ts&&... ts) -> std::vector<T> {
	uint32_t count = 0;
	std::vector<T> results;
	f (ts..., &count, nullptr);
	results.resize (count);
	f (ts..., &count, results.data ());
	results.resize (count);
	return results;
}
} // namespace detail

const char* to_string_message_severity (VkDebugUtilsMessageSeverityFlagBitsEXT s) {
	switch (s) {
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			return "VERBOSE";
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			return "ERROR";
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			return "WARNING";
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			return "INFO";
		default:
			return "UNKNOWN";
	}
}
const char* to_string_message_type (VkDebugUtilsMessageTypeFlagsEXT s) {
	if (s == 7) return "General | Validation | Performance";
	if (s == 6) return "Validation | Performance";
	if (s == 5) return "General | Performance";
	if (s == 4 /*VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT*/) return "Performance";
	if (s == 3) return "General | Validation";
	if (s == 2 /*VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT*/) return "Validation";
	if (s == 1 /*VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT*/) return "General";
	return "Unknown";
}

VkResult create_debug_utils_messenger (VkInstance instance,
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback,
    VkDebugUtilsMessageSeverityFlagsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    VkDebugUtilsMessengerEXT* pDebugMessenger,
    VkAllocationCallbacks* allocation_callbacks) {

	if (debug_callback == nullptr) debug_callback = default_debug_callback;
	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
	messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerCreateInfo.pNext = nullptr;
	messengerCreateInfo.messageSeverity = severity;
	messengerCreateInfo.messageType = type;
	messengerCreateInfo.pfnUserCallback = debug_callback;

	PFN_vkCreateDebugUtilsMessengerEXT createMessengerFunc;
	detail::vulkan_functions ().get_inst_proc_addr (createMessengerFunc, "vkCreateDebugUtilsMessengerEXT");

	if (createMessengerFunc != nullptr) {
		return createMessengerFunc (instance, &messengerCreateInfo, allocation_callbacks, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroy_debug_utils_messenger (
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks* allocation_callbacks) {

	PFN_vkDestroyDebugUtilsMessengerEXT deleteMessengerFunc;
	detail::vulkan_functions ().get_inst_proc_addr (deleteMessengerFunc, "vkDestroyDebugUtilsMessengerEXT");

	if (deleteMessengerFunc != nullptr) {
		deleteMessengerFunc (instance, debugMessenger, allocation_callbacks);
	}
}

VkBool32 default_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*) {
	auto ms = to_string_message_severity (messageSeverity);
	auto mt = to_string_message_type (messageType);
	printf ("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

	return VK_FALSE;
}

namespace detail {
bool check_layer_supported (std::vector<VkLayerProperties> const& available_layers, const char* layer_name) {
	if (!layer_name) return false;
	for (const auto& layer_properties : available_layers) {
		if (strcmp (layer_name, layer_properties.layerName) == 0) {
			return true;
		}
	}
	return false;
}

bool check_layers_supported (std::vector<VkLayerProperties> const& available_layers,
    std::vector<const char*> const& layer_names) {
	bool all_found = true;
	for (const auto& layer_name : layer_names) {
		bool found = check_layer_supported (available_layers, layer_name);
		if (!found) all_found = false;
	}
	return all_found;
}

bool check_extension_supported (
    std::vector<VkExtensionProperties> const& available_extensions, const char* extension_name) {
	if (!extension_name) return false;
	for (const auto& extension_properties : available_extensions) {
		if (strcmp (extension_name, extension_properties.extensionName) == 0) {
			return true;
		}
	}
	return false;
}

bool check_extensions_supported (std::vector<VkExtensionProperties> const& available_extensions,
    std::vector<const char*> const& extension_names) {
	bool all_found = true;
	for (const auto& extension_name : extension_names) {
		bool found = check_extension_supported (available_extensions, extension_name);
		if (!found) all_found = false;
	}
	return all_found;
}

template <typename T>
void setup_pNext_chain (T& structure, std::vector<VkBaseOutStructure*> const& structs) {
	structure.pNext = nullptr;
	if (structs.size () <= 0) return;
	for (size_t i = 0; i < structs.size () - 1; i++) {
		structs.at (i)->pNext = structs.at (i + 1);
	}
	structure.pNext = structs.at (0);
}
const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";

struct InstanceErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vkb_instance"; }
	std::string message (int err) const override {
		return to_string (static_cast<InstanceError> (err));
	}
};
const InstanceErrorCategory instance_error_category;

struct PhysicalDeviceErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vkb_physical_device"; }
	std::string message (int err) const override {
		return to_string (static_cast<PhysicalDeviceError> (err));
	}
};
const PhysicalDeviceErrorCategory physical_device_error_category;

struct QueueErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vkb_queue"; }
	std::string message (int err) const override {
		return to_string (static_cast<QueueError> (err));
	}
};
const QueueErrorCategory queue_error_category;

struct DeviceErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vkb_device"; }
	std::string message (int err) const override {
		return to_string (static_cast<DeviceError> (err));
	}
};
const DeviceErrorCategory device_error_category;

struct SwapchainErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vbk_swapchain"; }
	std::string message (int err) const override {
		return to_string (static_cast<SwapchainError> (err));
	}
};
const SwapchainErrorCategory swapchain_error_category;

} // namespace detail

std::error_code make_error_code (InstanceError instance_error) {
	return { static_cast<int> (instance_error), detail::instance_error_category };
}
std::error_code make_error_code (PhysicalDeviceError physical_device_error) {
	return { static_cast<int> (physical_device_error), detail::physical_device_error_category };
}
std::error_code make_error_code (QueueError queue_error) {
	return { static_cast<int> (queue_error), detail::queue_error_category };
}
std::error_code make_error_code (DeviceError device_error) {
	return { static_cast<int> (device_error), detail::device_error_category };
}
std::error_code make_error_code (SwapchainError swapchain_error) {
	return { static_cast<int> (swapchain_error), detail::swapchain_error_category };
}

const char* to_string (InstanceError err) {
	switch (err) {
		case InstanceError::vulkan_unavailable:
			return "vulkan_unavailable";
		case InstanceError::vulkan_version_unavailable:
			return "vulkan_version_unavailable";
		case InstanceError::vulkan_version_1_1_unavailable:
			return "vulkan_version_1_1_unavailable";
		case InstanceError::vulkan_version_1_2_unavailable:
			return "vulkan_version_1_2_unavailable";
		case InstanceError::failed_create_debug_messenger:
			return "failed_create_debug_messenger";
		case InstanceError::failed_create_instance:
			return "failed_create_instance";
		case InstanceError::requested_layers_not_present:
			return "requested_layers_not_present";
		case InstanceError::requested_extensions_not_present:
			return "requested_extensions_not_present";
		case InstanceError::windowing_extensions_not_present:
			return "windowing_extensions_not_present";
		default:
			return "";
	}
}
const char* to_string (PhysicalDeviceError err) {
	switch (err) {
		case PhysicalDeviceError::no_surface_provided:
			return "no_surface_provided";
		case PhysicalDeviceError::failed_enumerate_physical_devices:
			return "failed_enumerate_physical_devices";
		case PhysicalDeviceError::no_physical_devices_found:
			return "no_physical_devices_found";
		case PhysicalDeviceError::no_suitable_device:
			return "no_suitable_device";
		default:
			return "";
	}
}
const char* to_string (QueueError err) {
	switch (err) {
		case QueueError::present_unavailable:
			return "present_unavailable";
		case QueueError::graphics_unavailable:
			return "graphics_unavailable";
		case QueueError::compute_unavailable:
			return "compute_unavailable";
		case QueueError::transfer_unavailable:
			return "transfer_unavailable";
		case QueueError::queue_index_out_of_range:
			return "queue_index_out_of_range";
		case QueueError::invalid_queue_family_index:
			return "invalid_queue_family_index";
		default:
			return "";
	}
}
const char* to_string (DeviceError err) {
	switch (err) {
		case DeviceError::failed_create_device:
			return "failed_create_device";
		default:
			return "";
	}
}
const char* to_string (SwapchainError err) {
	switch (err) {
		case SwapchainError::surface_handle_not_provided:
			return "surface_handle_not_provided";
		case SwapchainError::failed_query_surface_support_details:
			return "failed_query_surface_support_details";
		case SwapchainError::failed_create_swapchain:
			return "failed_create_swapchain";
		case SwapchainError::failed_get_swapchain_images:
			return "failed_get_swapchain_images";
		case SwapchainError::failed_create_swapchain_image_views:
			return "failed_create_swapchain_image_views";
		default:
			return "";
	}
}

detail::Result<SystemInfo> SystemInfo::get_system_info () {
	if (!detail::vulkan_functions ().init_vulkan_funcs (nullptr)) {
		return make_error_code (InstanceError::vulkan_unavailable);
	}
	return SystemInfo ();
}

detail::Result<SystemInfo> SystemInfo::get_system_info (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr) {
	// Using externally provided function pointers, assume the loader is available
	detail::vulkan_functions ().init_vulkan_funcs (fp_vkGetInstanceProcAddr);
	return SystemInfo ();
}

SystemInfo::SystemInfo () {
	auto available_layers_ret = detail::get_vector<VkLayerProperties> (
	    this->available_layers, detail::vulkan_functions ().fp_vkEnumerateInstanceLayerProperties);
	if (available_layers_ret != VK_SUCCESS) {
		this->available_layers.clear ();
	}

	for (auto& layer : this->available_layers)
		if (strcmp (layer.layerName, detail::validation_layer_name) == 0)
			validation_layers_available = true;

	auto available_extensions_ret = detail::get_vector<VkExtensionProperties> (this->available_extensions,
	    detail::vulkan_functions ().fp_vkEnumerateInstanceExtensionProperties,
	    nullptr);
	if (available_extensions_ret != VK_SUCCESS) {
		this->available_extensions.clear ();
	}

	for (auto& ext : this->available_extensions)
		if (strcmp (ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			debug_utils_available = true;

	for (auto& layer : this->available_layers) {
		std::vector<VkExtensionProperties> layer_extensions;
		auto layer_extensions_ret = detail::get_vector<VkExtensionProperties> (layer_extensions,
		    detail::vulkan_functions ().fp_vkEnumerateInstanceExtensionProperties,
		    layer.layerName);
		if (layer_extensions_ret != VK_SUCCESS) {
			for (auto& ext : layer_extensions)
				if (strcmp (ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
					debug_utils_available = true;
		}
	}
}
bool SystemInfo::is_extension_available (const char* extension_name) const {
	if (!extension_name) return false;
	return detail::check_extension_supported (available_extensions, extension_name);
}
bool SystemInfo::is_layer_available (const char* layer_name) const {
	if (!layer_name) return false;
	return detail::check_layer_supported (available_layers, layer_name);
}

void destroy_instance (Instance instance) {
	if (instance.instance != VK_NULL_HANDLE) {
		if (instance.debug_messenger != nullptr)
			destroy_debug_utils_messenger (instance.instance, instance.debug_messenger, instance.allocation_callbacks);
		detail::vulkan_functions ().fp_vkDestroyInstance (instance.instance, instance.allocation_callbacks);
	}
}

InstanceBuilder::InstanceBuilder (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr) {
	info.fp_vkGetInstanceProcAddr = fp_vkGetInstanceProcAddr;
}
InstanceBuilder::InstanceBuilder () {}

detail::Result<Instance> InstanceBuilder::build () const {

	auto sys_info_ret = SystemInfo::get_system_info ();
	if (!sys_info_ret) return sys_info_ret.error ();
	auto system = sys_info_ret.value ();

	uint32_t api_version = VK_MAKE_VERSION (1, 0, 0);

	if (info.required_api_version > VK_MAKE_VERSION (1, 0, 0) ||
	    info.desired_api_version > VK_MAKE_VERSION (1, 0, 0)) {
		PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion =
		    detail::vulkan_functions ().fp_vkEnumerateInstanceVersion;

		uint32_t queried_api_version = VK_MAKE_VERSION (1, 0, 0);
		if (pfn_vkEnumerateInstanceVersion != nullptr) {
			VkResult res = pfn_vkEnumerateInstanceVersion (&queried_api_version);
			// Should always return VK_SUCCESS
			if (res != VK_SUCCESS && info.required_api_version > 0)
				return make_error_code (InstanceError::vulkan_version_unavailable);
		}
		if (pfn_vkEnumerateInstanceVersion == nullptr || queried_api_version < info.required_api_version) {
			if (VK_VERSION_MINOR (info.required_api_version) == 2)
				return make_error_code (InstanceError::vulkan_version_1_2_unavailable);
			else if (VK_VERSION_MINOR (info.required_api_version))
				return make_error_code (InstanceError::vulkan_version_1_1_unavailable);
			else
				return make_error_code (InstanceError::vulkan_version_unavailable);
		}
		if (info.required_api_version > VK_MAKE_VERSION (1, 0, 0)) {
			api_version = info.required_api_version;
		} else if (info.desired_api_version > VK_MAKE_VERSION (1, 0, 0)) {
			if (queried_api_version >= info.desired_api_version)
				api_version = info.desired_api_version;
			else
				api_version = queried_api_version;
		}
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
	for (auto& ext : info.extensions)
		extensions.push_back (ext);
	if (info.debug_callback != nullptr && system.debug_utils_available) {
		extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!info.headless_context) {
		auto check_add_window_ext = [&] (const char* name) -> bool {
			if (!detail::check_extension_supported (system.available_extensions, name))
				return false;
			extensions.push_back (name);
			return true;
		};
		bool khr_surface_added = check_add_window_ext ("VK_KHR_surface");
#if defined(_WIN32)
		bool added_window_exts = check_add_window_ext ("VK_KHR_win32_surface");
#elif defined(__ANDROID__)
		bool added_window_exts = check_add_window_ext ("VK_KHR_android_surface");
#elif defined(_DIRECT2DISPLAY)
		bool added_window_exts = check_add_window_ext ("VK_KHR_display");
#elif defined(__linux__)
		bool added_window_exts = check_add_window_ext ("VK_KHR_xcb_surface");
        added_window_exts = check_add_window_ext ("VK_KHR_xlib_surface") || added_window_exts;
        added_window_exts = check_add_window_ext ("VK_KHR_wayland_surface") || added_window_exts;
#elif defined(__APPLE__)
		bool added_window_exts = check_add_window_ext ("VK_KHR_metal_surface");
#endif
		if (!khr_surface_added || !added_window_exts)
			return make_error_code (InstanceError::windowing_extensions_not_present);
	}
	bool all_extensions_supported = detail::check_extensions_supported (system.available_extensions, extensions);
	if (!all_extensions_supported) {
		return make_error_code (InstanceError::requested_extensions_not_present);
	}

	std::vector<const char*> layers;
	for (auto& layer : info.layers)
		layers.push_back (layer);

	if (info.enable_validation_layers || (info.request_validation_layers && system.validation_layers_available)) {
		layers.push_back (detail::validation_layer_name);
	}
	bool all_layers_supported = detail::check_layers_supported (system.available_layers, layers);
	if (!all_layers_supported) {
		return make_error_code (InstanceError::requested_layers_not_present);
	}

	std::vector<VkBaseOutStructure*> pNext_chain;

	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
	if (info.use_debug_messenger) {
		messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerCreateInfo.pNext = nullptr;
		messengerCreateInfo.messageSeverity = info.debug_message_severity;
		messengerCreateInfo.messageType = info.debug_message_type;
		messengerCreateInfo.pfnUserCallback = info.debug_callback;
		pNext_chain.push_back (reinterpret_cast<VkBaseOutStructure*> (&messengerCreateInfo));
	}

	VkValidationFeaturesEXT features{};
	if (info.enabled_validation_features.size () != 0 || info.disabled_validation_features.size ()) {
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.pNext = nullptr;
		features.enabledValidationFeatureCount =
		    static_cast<uint32_t> (info.enabled_validation_features.size ());
		features.pEnabledValidationFeatures = info.enabled_validation_features.data ();
		features.disabledValidationFeatureCount =
		    static_cast<uint32_t> (info.disabled_validation_features.size ());
		features.pDisabledValidationFeatures = info.disabled_validation_features.data ();
		pNext_chain.push_back (reinterpret_cast<VkBaseOutStructure*> (&features));
	}

	VkValidationFlagsEXT checks{};
	if (info.disabled_validation_checks.size () != 0) {
		checks.sType = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT;
		checks.pNext = nullptr;
		checks.disabledValidationCheckCount =
		    static_cast<uint32_t> (info.disabled_validation_checks.size ());
		checks.pDisabledValidationChecks = info.disabled_validation_checks.data ();
		pNext_chain.push_back (reinterpret_cast<VkBaseOutStructure*> (&checks));
	}

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	detail::setup_pNext_chain (instance_create_info, pNext_chain);
	instance_create_info.flags = info.flags;
	instance_create_info.pApplicationInfo = &app_info;
	instance_create_info.enabledExtensionCount = static_cast<uint32_t> (extensions.size ());
	instance_create_info.ppEnabledExtensionNames = extensions.data ();
	instance_create_info.enabledLayerCount = static_cast<uint32_t> (layers.size ());
	instance_create_info.ppEnabledLayerNames = layers.data ();

	Instance instance;
	VkResult res = detail::vulkan_functions ().fp_vkCreateInstance (
	    &instance_create_info, info.allocation_callbacks, &instance.instance);
	if (res != VK_SUCCESS)
		return detail::Result<Instance> (InstanceError::failed_create_instance, res);

	detail::vulkan_functions ().init_instance_funcs (instance.instance);

	if (info.use_debug_messenger) {
		res = create_debug_utils_messenger (instance.instance,
		    info.debug_callback,
		    info.debug_message_severity,
		    info.debug_message_type,
		    &instance.debug_messenger,
		    info.allocation_callbacks);
		if (res != VK_SUCCESS) {
			return detail::Result<Instance> (InstanceError::failed_create_debug_messenger, res);
		}
	}

	if (info.headless_context) {
		instance.headless = true;
	}
	instance.allocation_callbacks = info.allocation_callbacks;
	instance.instance_version = api_version;
	instance.fp_vkGetInstanceProcAddr = detail::vulkan_functions ().ptr_vkGetInstanceProcAddr;
	return instance;
}

InstanceBuilder& InstanceBuilder::set_app_name (const char* app_name) {
	if (!app_name) return *this;
	info.app_name = app_name;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_engine_name (const char* engine_name) {
	if (!engine_name) return *this;
	info.engine_name = engine_name;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_app_version (uint32_t major, uint32_t minor, uint32_t patch) {
	info.application_version = VK_MAKE_VERSION (major, minor, patch);
	return *this;
}
InstanceBuilder& InstanceBuilder::set_engine_version (uint32_t major, uint32_t minor, uint32_t patch) {
	info.engine_version = VK_MAKE_VERSION (major, minor, patch);
	return *this;
}
InstanceBuilder& InstanceBuilder::require_api_version (uint32_t major, uint32_t minor, uint32_t patch) {
	info.required_api_version = VK_MAKE_VERSION (major, minor, patch);
	return *this;
}
InstanceBuilder& InstanceBuilder::desire_api_version (uint32_t major, uint32_t minor, uint32_t patch) {
	info.desired_api_version = VK_MAKE_VERSION (major, minor, patch);
	return *this;
}
InstanceBuilder& InstanceBuilder::enable_layer (const char* layer_name) {
	if (!layer_name) return *this;
	info.layers.push_back (layer_name);
	return *this;
}
InstanceBuilder& InstanceBuilder::enable_extension (const char* extension_name) {
	if (!extension_name) return *this;
	info.extensions.push_back (extension_name);
	return *this;
}
InstanceBuilder& InstanceBuilder::enable_validation_layers (bool enable_validation) {
	info.enable_validation_layers = enable_validation;
	return *this;
}
InstanceBuilder& InstanceBuilder::request_validation_layers (bool enable_validation) {
	info.request_validation_layers = enable_validation;
	return *this;
}
InstanceBuilder& InstanceBuilder::use_default_debug_messenger () {
	info.use_debug_messenger = true;
	info.debug_callback = default_debug_callback;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_callback (PFN_vkDebugUtilsMessengerCallbackEXT callback) {
	info.use_debug_messenger = true;
	info.debug_callback = callback;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_headless (bool headless) {
	info.headless_context = headless;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_messenger_severity (VkDebugUtilsMessageSeverityFlagsEXT severity) {
	info.debug_message_severity = severity;
	return *this;
}
InstanceBuilder& InstanceBuilder::add_debug_messenger_severity (VkDebugUtilsMessageSeverityFlagsEXT severity) {
	info.debug_message_severity = info.debug_message_severity | severity;
	return *this;
}
InstanceBuilder& InstanceBuilder::set_debug_messenger_type (VkDebugUtilsMessageTypeFlagsEXT type) {
	info.debug_message_type = type;
	return *this;
}
InstanceBuilder& InstanceBuilder::add_debug_messenger_type (VkDebugUtilsMessageTypeFlagsEXT type) {
	info.debug_message_type = info.debug_message_type | type;
	return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_disable (VkValidationCheckEXT check) {
	info.disabled_validation_checks.push_back (check);
	return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_feature_enable (VkValidationFeatureEnableEXT enable) {
	info.enabled_validation_features.push_back (enable);
	return *this;
}
InstanceBuilder& InstanceBuilder::add_validation_feature_disable (VkValidationFeatureDisableEXT disable) {
	info.disabled_validation_features.push_back (disable);
	return *this;
}
InstanceBuilder& InstanceBuilder::set_allocation_callbacks (VkAllocationCallbacks* callbacks) {
	info.allocation_callbacks = callbacks;
	return *this;
}

// ---- Physical Device ---- //

namespace detail {

std::vector<const char*> check_device_extension_support (
    VkPhysicalDevice device, std::vector<const char*> desired_extensions) {
	std::vector<VkExtensionProperties> available_extensions;
	auto available_extensions_ret = detail::get_vector<VkExtensionProperties> (
	    available_extensions, detail::vulkan_functions ().fp_vkEnumerateDeviceExtensionProperties, device, nullptr);
	if (available_extensions_ret != VK_SUCCESS) return {};

	std::vector<const char*> extensions_to_enable;
	for (const auto& extension : available_extensions) {
		for (auto& req_ext : desired_extensions) {
			if (strcmp (req_ext, extension.extensionName) == 0) {
				extensions_to_enable.push_back (req_ext);
				break;
			}
		}
	}
	return extensions_to_enable;
}

bool supports_features (VkPhysicalDeviceFeatures supported, VkPhysicalDeviceFeatures requested) {
	// clang-format off
    if (requested.robustBufferAccess && !supported.robustBufferAccess) return false;
    if (requested.fullDrawIndexUint32 && !supported.fullDrawIndexUint32) return false;
    if (requested.imageCubeArray && !supported.imageCubeArray) return false;
    if (requested.independentBlend && !supported.independentBlend) return false;
    if (requested.geometryShader && !supported.geometryShader) return false;
    if (requested.tessellationShader && !supported.tessellationShader) return false;
    if (requested.sampleRateShading && !supported.sampleRateShading) return false;
    if (requested.dualSrcBlend && !supported.dualSrcBlend) return false;
    if (requested.logicOp && !supported.logicOp) return false;
    if (requested.multiDrawIndirect && !supported.multiDrawIndirect) return false;
    if (requested.drawIndirectFirstInstance && !supported.drawIndirectFirstInstance) return false;
    if (requested.depthClamp && !supported.depthClamp) return false;
    if (requested.depthBiasClamp && !supported.depthBiasClamp) return false;
    if (requested.fillModeNonSolid && !supported.fillModeNonSolid) return false;
    if (requested.depthBounds && !supported.depthBounds) return false;
    if (requested.wideLines && !supported.wideLines) return false;
    if (requested.largePoints && !supported.largePoints) return false;
    if (requested.alphaToOne && !supported.alphaToOne) return false;
    if (requested.multiViewport && !supported.multiViewport) return false;
    if (requested.samplerAnisotropy && !supported.samplerAnisotropy) return false;
    if (requested.textureCompressionETC2 && !supported.textureCompressionETC2) return false;
    if (requested.textureCompressionASTC_LDR && !supported.textureCompressionASTC_LDR) return false;
    if (requested.textureCompressionBC && !supported.textureCompressionBC) return false;
    if (requested.occlusionQueryPrecise && !supported.occlusionQueryPrecise) return false;
    if (requested.pipelineStatisticsQuery && !supported.pipelineStatisticsQuery) return false;
    if (requested.vertexPipelineStoresAndAtomics && !supported.vertexPipelineStoresAndAtomics) return false;
    if (requested.fragmentStoresAndAtomics && !supported.fragmentStoresAndAtomics) return false;
    if (requested.shaderTessellationAndGeometryPointSize && !supported.shaderTessellationAndGeometryPointSize) return false;
    if (requested.shaderImageGatherExtended && !supported.shaderImageGatherExtended) return false;
    if (requested.shaderStorageImageExtendedFormats && !supported.shaderStorageImageExtendedFormats) return false;
    if (requested.shaderStorageImageMultisample && !supported.shaderStorageImageMultisample) return false;
    if (requested.shaderStorageImageReadWithoutFormat && !supported.shaderStorageImageReadWithoutFormat) return false;
    if (requested.shaderStorageImageWriteWithoutFormat && !supported.shaderStorageImageWriteWithoutFormat) return false;
    if (requested.shaderUniformBufferArrayDynamicIndexing && !supported.shaderUniformBufferArrayDynamicIndexing) return false;
    if (requested.shaderSampledImageArrayDynamicIndexing && !supported.shaderSampledImageArrayDynamicIndexing) return false;
    if (requested.shaderStorageBufferArrayDynamicIndexing && !supported.shaderStorageBufferArrayDynamicIndexing) return false;
    if (requested.shaderStorageImageArrayDynamicIndexing && !supported.shaderStorageImageArrayDynamicIndexing) return false;
    if (requested.shaderClipDistance && !supported.shaderClipDistance) return false;
    if (requested.shaderCullDistance && !supported.shaderCullDistance) return false;
    if (requested.shaderFloat64 && !supported.shaderFloat64) return false;
    if (requested.shaderInt64 && !supported.shaderInt64) return false;
    if (requested.shaderInt16 && !supported.shaderInt16) return false;
    if (requested.shaderResourceResidency && !supported.shaderResourceResidency) return false;
    if (requested.shaderResourceMinLod && !supported.shaderResourceMinLod) return false;
    if (requested.sparseBinding && !supported.sparseBinding) return false;
    if (requested.sparseResidencyBuffer && !supported.sparseResidencyBuffer) return false;
    if (requested.sparseResidencyImage2D && !supported.sparseResidencyImage2D) return false;
    if (requested.sparseResidencyImage3D && !supported.sparseResidencyImage3D) return false;
    if (requested.sparseResidency2Samples && !supported.sparseResidency2Samples) return false;
    if (requested.sparseResidency4Samples && !supported.sparseResidency4Samples) return false;
    if (requested.sparseResidency8Samples && !supported.sparseResidency8Samples) return false;
    if (requested.sparseResidency16Samples && !supported.sparseResidency16Samples) return false;
    if (requested.sparseResidencyAliased && !supported.sparseResidencyAliased) return false;
    if (requested.variableMultisampleRate && !supported.variableMultisampleRate) return false;
    if (requested.inheritedQueries && !supported.inheritedQueries) return false;
	// clang-format on
	return true;
}

// finds the first queue which supports graphics operations. returns -1 if none is found
int get_graphics_queue_index (std::vector<VkQueueFamilyProperties> const& families) {
	for (size_t i = 0; i < families.size (); i++) {
		if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) return static_cast<int> (i);
	}
	return -1;
}
// finds a compute queue which is separate from the graphics queue and tries to find one without
// transfer support returns -1 if none is found
int get_separate_compute_queue_index (std::vector<VkQueueFamilyProperties> const& families) {
	int compute = -1;
	for (size_t i = 0; i < families.size (); i++) {
		if ((families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
		    ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
			if ((families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0) {
				return static_cast<int> (i);
			} else {
				compute = static_cast<int> (i);
			}
		}
	}
	return compute;
}
// finds a transfer queue which is separate from the graphics queue and tries to find one without
// compute support returns -1 if none is found
int get_separate_transfer_queue_index (std::vector<VkQueueFamilyProperties> const& families) {
	int transfer = -1;
	for (size_t i = 0; i < families.size (); i++) {
		if ((families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
		    ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
			if ((families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0) {
				return static_cast<int> (i);
			} else {
				transfer = static_cast<int> (i);
			}
		}
	}
	return transfer;
}
// finds the first queue which supports only compute (not graphics or transfer). returns -1 if none is found
int get_dedicated_compute_queue_index (std::vector<VkQueueFamilyProperties> const& families) {
	for (size_t i = 0; i < families.size (); i++) {
		if ((families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
		    (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
		    (families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
			return static_cast<int> (i);
	}
	return -1;
}
// finds the first queue which supports only transfer (not graphics or compute). returns -1 if none is found
int get_dedicated_transfer_queue_index (std::vector<VkQueueFamilyProperties> const& families) {
	for (size_t i = 0; i < families.size (); i++) {
		if ((families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
		    (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
		    (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
			return static_cast<int> (i);
	}
	return -1;
}
// finds the first queue which supports presenting. returns -1 if none is found
int get_present_queue_index (VkPhysicalDevice const phys_device,
    VkSurfaceKHR const surface,
    std::vector<VkQueueFamilyProperties> const& families) {
	for (size_t i = 0; i < families.size (); i++) {
		VkBool32 presentSupport = false;
		if (surface != VK_NULL_HANDLE) {
			VkResult res = detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfaceSupportKHR (
			    phys_device, static_cast<uint32_t> (i), surface, &presentSupport);
			if (res != VK_SUCCESS) return -1; // TODO: determine if this should fail another way
		}
		if (presentSupport == VK_TRUE) return static_cast<int> (i);
	}
	return -1;
}
} // namespace detail


PhysicalDeviceSelector::PhysicalDeviceDesc PhysicalDeviceSelector::populate_device_details (
    VkPhysicalDevice phys_device) const {
	PhysicalDeviceSelector::PhysicalDeviceDesc desc{};
	desc.phys_device = phys_device;
	auto queue_families = detail::get_vector_noerror<VkQueueFamilyProperties> (
	    detail::vulkan_functions ().fp_vkGetPhysicalDeviceQueueFamilyProperties, phys_device);
	desc.queue_families = queue_families;

	detail::vulkan_functions ().fp_vkGetPhysicalDeviceProperties (phys_device, &desc.device_properties);
	detail::vulkan_functions ().fp_vkGetPhysicalDeviceFeatures (phys_device, &desc.device_features);
	detail::vulkan_functions ().fp_vkGetPhysicalDeviceMemoryProperties (phys_device, &desc.mem_properties);
	return desc;
}

PhysicalDeviceSelector::Suitable PhysicalDeviceSelector::is_device_suitable (PhysicalDeviceDesc pd) const {
	Suitable suitable = Suitable::yes;

	if (criteria.required_version > pd.device_properties.apiVersion) return Suitable::no;
	if (criteria.desired_version > pd.device_properties.apiVersion) suitable = Suitable::partial;

	bool dedicated_compute = detail::get_dedicated_compute_queue_index (pd.queue_families) >= 0;
	bool dedicated_transfer = detail::get_dedicated_transfer_queue_index (pd.queue_families) >= 0;
	bool separate_compute = detail::get_separate_compute_queue_index (pd.queue_families) >= 0;
	bool separate_transfer = detail::get_separate_transfer_queue_index (pd.queue_families) >= 0;

	bool present_queue =
	    detail::get_present_queue_index (pd.phys_device, system_info.surface, pd.queue_families) >= 0;

	if (criteria.require_dedicated_compute_queue && !dedicated_compute) return Suitable::no;
	if (criteria.require_dedicated_transfer_queue && !dedicated_transfer) return Suitable::no;
	if (criteria.require_separate_compute_queue && !separate_compute) return Suitable::no;
	if (criteria.require_separate_transfer_queue && !separate_transfer) return Suitable::no;
	if (criteria.require_present && !present_queue && !criteria.defer_surface_initialization)
		return Suitable::no;

	auto required_extensions_supported =
	    detail::check_device_extension_support (pd.phys_device, criteria.required_extensions);
	if (required_extensions_supported.size () != criteria.required_extensions.size ())
		return Suitable::no;

	auto desired_extensions_supported =
	    detail::check_device_extension_support (pd.phys_device, criteria.desired_extensions);
	if (desired_extensions_supported.size () != criteria.desired_extensions.size ())
		suitable = Suitable::partial;


	bool swapChainAdequate = false;
	if (criteria.defer_surface_initialization) {
		swapChainAdequate = true;
	} else if (!system_info.headless) {
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;

		auto formats_ret = detail::get_vector<VkSurfaceFormatKHR> (formats,
		    detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfaceFormatsKHR,
		    pd.phys_device,
		    system_info.surface);
		auto present_modes_ret = detail::get_vector<VkPresentModeKHR> (present_modes,
		    detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfacePresentModesKHR,
		    pd.phys_device,
		    system_info.surface);

		if (formats_ret == VK_SUCCESS && present_modes_ret == VK_SUCCESS) {
			swapChainAdequate = !formats.empty () && !present_modes.empty ();
		}
	}
	if (criteria.require_present && !swapChainAdequate) return Suitable::no;

	if (pd.device_properties.deviceType != static_cast<VkPhysicalDeviceType> (criteria.preferred_type)) {
		if (criteria.allow_any_type)
			suitable = Suitable::partial;
		else
			return Suitable::no;
	}

	bool required_features_supported =
	    detail::supports_features (pd.device_features, criteria.required_features);
	if (!required_features_supported) return Suitable::no;

	bool has_required_memory = false;
	bool has_preferred_memory = false;
	for (uint32_t i = 0; i < pd.mem_properties.memoryHeapCount; i++) {
		if (pd.mem_properties.memoryHeaps[i].flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			if (pd.mem_properties.memoryHeaps[i].size > criteria.required_mem_size) {
				has_required_memory = true;
			}
			if (pd.mem_properties.memoryHeaps[i].size > criteria.desired_mem_size) {
				has_preferred_memory = true;
			}
		}
	}
	if (!has_required_memory) return Suitable::no;
	if (!has_preferred_memory) suitable = Suitable::partial;

	return suitable;
}

PhysicalDeviceSelector::PhysicalDeviceSelector (Instance const& instance) {
	system_info.instance = instance.instance;
	system_info.headless = instance.headless;
	criteria.require_present = !instance.headless;
	criteria.required_version = instance.instance_version;
	criteria.desired_version = instance.instance_version;
}

detail::Result<PhysicalDevice> PhysicalDeviceSelector::select () const {
	if (!system_info.headless && !criteria.defer_surface_initialization) {
		if (system_info.surface == nullptr)
			return detail::Result<PhysicalDevice>{ PhysicalDeviceError::no_surface_provided };
	}


	std::vector<VkPhysicalDevice> physical_devices;

	auto physical_devices_ret = detail::get_vector<VkPhysicalDevice> (
	    physical_devices, detail::vulkan_functions ().fp_vkEnumeratePhysicalDevices, system_info.instance);
	if (physical_devices_ret != VK_SUCCESS) {
		return detail::Result<PhysicalDevice>{ PhysicalDeviceError::failed_enumerate_physical_devices,
			physical_devices_ret };
	}
	if (physical_devices.size () == 0) {
		return detail::Result<PhysicalDevice>{ PhysicalDeviceError::no_physical_devices_found };
	}

	std::vector<PhysicalDeviceDesc> phys_device_descriptions;
	for (auto& phys_device : physical_devices) {
		phys_device_descriptions.push_back (populate_device_details (phys_device));
	}

	PhysicalDeviceDesc selected_device{};

	if (criteria.use_first_gpu_unconditionally) {
		selected_device = phys_device_descriptions.at (0);
	} else {
		for (const auto& device : phys_device_descriptions) {
			auto suitable = is_device_suitable (device);
			if (suitable == Suitable::yes) {
				selected_device = device;
				break;
			} else if (suitable == Suitable::partial) {
				selected_device = device;
			}
		}
	}

	if (selected_device.phys_device == VK_NULL_HANDLE) {
		return detail::Result<PhysicalDevice>{ PhysicalDeviceError::no_suitable_device };
	}
	PhysicalDevice out_device{};
	out_device.physical_device = selected_device.phys_device;
	out_device.surface = system_info.surface;
	out_device.features = criteria.required_features;
	out_device.properties = selected_device.device_properties;
	out_device.memory_properties = selected_device.mem_properties;
	out_device.queue_families = selected_device.queue_families;
	out_device.defer_surface_initialization = criteria.defer_surface_initialization;

	out_device.extensions_to_enable.insert (out_device.extensions_to_enable.end (),
	    criteria.required_extensions.begin (),
	    criteria.required_extensions.end ());
	auto desired_extensions_supported =
	    detail::check_device_extension_support (out_device.physical_device, criteria.desired_extensions);
	out_device.extensions_to_enable.insert (out_device.extensions_to_enable.end (),
	    desired_extensions_supported.begin (),
	    desired_extensions_supported.end ());
	return out_device;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::set_surface (VkSurfaceKHR surface) {
	system_info.surface = surface;
	system_info.headless = false;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::prefer_gpu_device_type (PreferredDeviceType type) {
	criteria.preferred_type = type;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::allow_any_gpu_device_type (bool allow_any_type) {
	criteria.allow_any_type = allow_any_type;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_present (bool require) {
	criteria.require_present = require;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_dedicated_transfer_queue () {
	criteria.require_dedicated_transfer_queue = true;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_dedicated_compute_queue () {
	criteria.require_dedicated_compute_queue = true;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_separate_transfer_queue () {
	criteria.require_separate_transfer_queue = true;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::require_separate_compute_queue () {
	criteria.require_separate_compute_queue = true;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::required_device_memory_size (VkDeviceSize size) {
	criteria.required_mem_size = size;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::desired_device_memory_size (VkDeviceSize size) {
	criteria.desired_mem_size = size;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_required_extension (const char* extension) {
	criteria.required_extensions.push_back (extension);
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_required_extensions (std::vector<const char*> extensions) {
	criteria.required_extensions.insert (
	    criteria.required_extensions.end (), extensions.begin (), extensions.end ());
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_desired_extension (const char* extension) {
	criteria.desired_extensions.push_back (extension);
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::add_desired_extensions (std::vector<const char*> extensions) {
	criteria.desired_extensions.insert (
	    criteria.desired_extensions.end (), extensions.begin (), extensions.end ());
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_minimum_version (uint32_t major, uint32_t minor) {
	criteria.required_version = VK_MAKE_VERSION (major, minor, 0);
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_desired_version (uint32_t major, uint32_t minor) {
	criteria.desired_version = VK_MAKE_VERSION (major, minor, 0);
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::set_required_features (VkPhysicalDeviceFeatures features) {
	criteria.required_features = features;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::defer_surface_initialization () {
	criteria.defer_surface_initialization = true;
	return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::select_first_device_unconditionally (bool unconditionally) {
	criteria.use_first_gpu_unconditionally = unconditionally;
	return *this;
}

bool PhysicalDevice::has_dedicated_compute_queue () const {
	return detail::get_dedicated_compute_queue_index (queue_families) >= 0;
}
bool PhysicalDevice::has_separate_compute_queue () const {
	return detail::get_separate_compute_queue_index (queue_families) >= 0;
}
bool PhysicalDevice::has_dedicated_transfer_queue () const {
	return detail::get_dedicated_transfer_queue_index (queue_families) >= 0;
}
bool PhysicalDevice::has_separate_transfer_queue () const {
	return detail::get_separate_transfer_queue_index (queue_families) >= 0;
}
std::vector<VkQueueFamilyProperties> PhysicalDevice::get_queue_families () const {
	return queue_families;
}

// ---- Queues ---- //

detail::Result<uint32_t> Device::get_queue_index (QueueType type) const {
	int index = -1;
	switch (type) {
		case QueueType::present:
			index = detail::get_present_queue_index (physical_device.physical_device, surface, queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::present_unavailable };
			break;
		case QueueType::graphics:
			index = detail::get_graphics_queue_index (queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::graphics_unavailable };
			break;
		case QueueType::compute:
			index = detail::get_separate_compute_queue_index (queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::compute_unavailable };
			break;
		case QueueType::transfer:
			index = detail::get_separate_transfer_queue_index (queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::transfer_unavailable };
			break;
		default:
			return detail::Result<uint32_t>{ QueueError::invalid_queue_family_index };
	}
	return static_cast<uint32_t> (index);
}
detail::Result<uint32_t> Device::get_dedicated_queue_index (QueueType type) const {
	int index = -1;
	switch (type) {
		case QueueType::compute:
			index = detail::get_dedicated_compute_queue_index (queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::compute_unavailable };
			break;
		case QueueType::transfer:
			index = detail::get_dedicated_transfer_queue_index (queue_families);
			if (index < 0) return detail::Result<uint32_t>{ QueueError::transfer_unavailable };
			break;
		default:
			return detail::Result<uint32_t>{ QueueError::invalid_queue_family_index };
	}
	return static_cast<uint32_t> (index);
}
namespace detail {
VkQueue get_queue (VkDevice device, uint32_t family) {
	VkQueue out_queue;
	detail::vulkan_functions ().fp_vkGetDeviceQueue (device, family, 0, &out_queue);
	return out_queue;
}
} // namespace detail
detail::Result<VkQueue> Device::get_queue (QueueType type) const {
	auto index = get_queue_index (type);
	if (!index.has_value ()) return { index.error () };
	return detail::get_queue (device, index.value ());
}
detail::Result<VkQueue> Device::get_dedicated_queue (QueueType type) const {
	auto index = get_dedicated_queue_index (type);
	if (!index.has_value ()) return { index.error () };
	return detail::get_queue (device, index.value ());
}

// ---- Device ---- //

CustomQueueDescription::CustomQueueDescription (uint32_t index, uint32_t count, std::vector<float> priorities)
: index (index), count (count), priorities (priorities) {
	assert (count == priorities.size ());
}

void destroy_device (Device device) {
	detail::vulkan_functions ().fp_vkDestroyDevice (device.device, device.allocation_callbacks);
}

DeviceBuilder::DeviceBuilder (PhysicalDevice phys_device) {
	info.physical_device = phys_device;
	info.surface = phys_device.surface;
	info.queue_families = phys_device.queue_families;
	info.features = phys_device.features;
	info.extensions_to_enable = phys_device.extensions_to_enable;
	info.defer_surface_initialization = phys_device.defer_surface_initialization;
}

detail::Result<Device> DeviceBuilder::build () const {

	std::vector<CustomQueueDescription> queue_descriptions;
	queue_descriptions.insert (
	    queue_descriptions.end (), info.queue_descriptions.begin (), info.queue_descriptions.end ());

	if (queue_descriptions.size () == 0) {
		for (uint32_t i = 0; i < info.queue_families.size (); i++) {
			queue_descriptions.push_back (CustomQueueDescription{ i, 1, std::vector<float>{ 1.0f } });
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto& desc : queue_descriptions) {
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = desc.index;
		queue_create_info.queueCount = desc.count;
		queue_create_info.pQueuePriorities = desc.priorities.data ();
		queueCreateInfos.push_back (queue_create_info);
	}

	std::vector<const char*> extensions = info.extensions_to_enable;
	if (info.surface != VK_NULL_HANDLE || info.defer_surface_initialization)
		extensions.push_back ({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });

	// VUID-VkDeviceCreateInfo-pNext-00373 - don't add pEnabledFeatures if the phys_dev_features_2 is present
	bool has_phys_dev_features_2 = false;
	for (auto& pNext_struct : info.pNext_chain) {
		if (pNext_struct->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) {
			has_phys_dev_features_2 = true;
		}
	}

	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	detail::setup_pNext_chain (device_create_info, info.pNext_chain);
	device_create_info.flags = info.flags;
	device_create_info.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size ());
	device_create_info.pQueueCreateInfos = queueCreateInfos.data ();
	device_create_info.enabledExtensionCount = static_cast<uint32_t> (extensions.size ());
	device_create_info.ppEnabledExtensionNames = extensions.data ();
	if (!has_phys_dev_features_2) {
		device_create_info.pEnabledFeatures = &info.features;
	}

	Device device;
	VkResult res = detail::vulkan_functions ().fp_vkCreateDevice (info.physical_device.physical_device,
	    &device_create_info,
	    info.allocation_callbacks,
	    &device.device);
	if (res != VK_SUCCESS) {
		return { DeviceError::failed_create_device, res };
	}
	device.physical_device = info.physical_device;
	device.surface = info.surface;
	device.queue_families = info.queue_families;
	device.allocation_callbacks = info.allocation_callbacks;
	return device;
}
DeviceBuilder& DeviceBuilder::custom_queue_setup (std::vector<CustomQueueDescription> queue_descriptions) {
	info.queue_descriptions = queue_descriptions;
	return *this;
}
DeviceBuilder& DeviceBuilder::set_allocation_callbacks (VkAllocationCallbacks* callbacks) {
	info.allocation_callbacks = callbacks;
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
	failed_enumerate_present_modes
};

struct SurfaceSupportErrorCategory : std::error_category {
	const char* name () const noexcept override { return "vbk_surface_support"; }
	std::string message (int err) const override {
		switch (static_cast<SurfaceSupportError> (err)) {
			case SurfaceSupportError::surface_handle_null:
				return "surface_handle_null";
			case SurfaceSupportError::failed_get_surface_capabilities:
				return "failed_get_surface_capabilities";
			case SurfaceSupportError::failed_enumerate_surface_formats:
				return "failed_enumerate_surface_formats";
			case SurfaceSupportError::failed_enumerate_present_modes:
				return "failed_enumerate_present_modes";
			default:
				return "";
		}
	}
};
const SurfaceSupportErrorCategory surface_support_error_category;

std::error_code make_error_code (SurfaceSupportError surface_support_error) {
	return { static_cast<int> (surface_support_error), detail::surface_support_error_category };
}

Result<SurfaceSupportDetails> query_surface_support_details (VkPhysicalDevice phys_device, VkSurfaceKHR surface) {
	if (surface == VK_NULL_HANDLE)
		return make_error_code (SurfaceSupportError::surface_handle_null);

	VkSurfaceCapabilitiesKHR capabilities;
	VkResult res = detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfaceCapabilitiesKHR (
	    phys_device, surface, &capabilities);
	if (res != VK_SUCCESS) {
		return { make_error_code (SurfaceSupportError::failed_get_surface_capabilities), res };
	}

	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;

	auto formats_ret = detail::get_vector<VkSurfaceFormatKHR> (
	    formats, detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfaceFormatsKHR, phys_device, surface);
	if (formats_ret != VK_SUCCESS)
		return { make_error_code (SurfaceSupportError::failed_enumerate_surface_formats), formats_ret };
	auto present_modes_ret = detail::get_vector<VkPresentModeKHR> (
	    present_modes, detail::vulkan_functions ().fp_vkGetPhysicalDeviceSurfacePresentModesKHR, phys_device, surface);
	if (present_modes_ret != VK_SUCCESS)
		return { make_error_code (SurfaceSupportError::failed_enumerate_present_modes), present_modes_ret };

	return SurfaceSupportDetails{ capabilities, formats, present_modes };
}

VkSurfaceFormatKHR find_surface_format (std::vector<VkSurfaceFormatKHR> const& available_formats,
    std::vector<VkSurfaceFormatKHR> const& desired_formats) {
	for (auto const& desired_format : desired_formats) {
		for (auto const& available_format : available_formats) {
			// finds the first format that is desired and available
			if (desired_format.format == available_format.format &&
			    desired_format.colorSpace == available_format.colorSpace) {
				return desired_format;
			}
		}
	}

	// use the first available one if any desired formats aren't found
	return available_formats[0];
}

VkPresentModeKHR find_present_mode (std::vector<VkPresentModeKHR> const& available_resent_modes,
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

template <typename T> T minimum (T a, T b) { return a < b ? a : b; }
template <typename T> T maximum (T a, T b) { return a > b ? a : b; }

VkExtent2D find_extent (
    VkSurfaceCapabilitiesKHR const& capabilities, uint32_t desired_width, uint32_t desired_height) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = { desired_width, desired_height };

		actualExtent.width = maximum (capabilities.minImageExtent.width,
		    minimum (capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = maximum (capabilities.minImageExtent.height,
		    minimum (capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
} // namespace detail

void destroy_swapchain (Swapchain const& swapchain) {
	if (swapchain.device != VK_NULL_HANDLE && swapchain.swapchain != VK_NULL_HANDLE) {
		detail::vulkan_functions ().fp_vkDestroySwapchainKHR (
		    swapchain.device, swapchain.swapchain, swapchain.allocation_callbacks);
	}
}

SwapchainBuilder::SwapchainBuilder (Device const& device) {
	info.device = device.device;
	info.physical_device = device.physical_device.physical_device;
	info.surface = device.surface;
	auto present = device.get_queue_index (QueueType::present);
	auto graphics = device.get_queue_index (QueueType::graphics);
	// TODO: handle error of queue's not available
	info.graphics_queue_index = present.value ();
	info.present_queue_index = graphics.value ();
}
SwapchainBuilder::SwapchainBuilder (Device const& device, VkSurfaceKHR const surface) {
	info.device = device.device;
	info.physical_device = device.physical_device.physical_device;
	info.surface = surface;
	Device temp_device = device;
	temp_device.surface = surface;
	auto present = temp_device.get_queue_index (QueueType::present);
	auto graphics = temp_device.get_queue_index (QueueType::graphics);
	// TODO: handle error of queue's not available
	info.graphics_queue_index = present.value ();
	info.present_queue_index = graphics.value ();
}
SwapchainBuilder::SwapchainBuilder (VkPhysicalDevice const physical_device, VkDevice const device, VkSurfaceKHR const surface, int32_t graphics_queue_index, int32_t present_queue_index){
	info.physical_device = physical_device;
    info.device = device;
	info.surface = surface;
    info.graphics_queue_index = static_cast<uint32_t>(graphics_queue_index);
	info.present_queue_index = static_cast<uint32_t>(present_queue_index);
	if (graphics_queue_index < 0 || present_queue_index < 0) {
		auto queue_families = detail::get_vector_noerror<VkQueueFamilyProperties> (
			detail::vulkan_functions().fp_vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
		if (graphics_queue_index < 0)
			info.graphics_queue_index = static_cast<uint32_t>(detail::get_graphics_queue_index (queue_families));
		if (present_queue_index < 0)
			info.present_queue_index = static_cast<uint32_t>(detail::get_present_queue_index (physical_device, surface, queue_families));
	}
}
detail::Result<Swapchain> SwapchainBuilder::build () const {
	if (info.surface == VK_NULL_HANDLE) {
		return detail::Error{ SwapchainError::surface_handle_not_provided };
	}

	auto desired_formats = info.desired_formats;
	if (desired_formats.size () == 0) add_desired_formats (desired_formats);
	auto desired_present_modes = info.desired_present_modes;
	if (desired_present_modes.size () == 0) add_desired_present_modes (desired_present_modes);

	auto surface_support_ret = detail::query_surface_support_details (info.physical_device, info.surface);
	if (!surface_support_ret.has_value ())
		return detail::Error{ SwapchainError::failed_query_surface_support_details,
			surface_support_ret.vk_result () };
	auto surface_support = surface_support_ret.value ();

	uint32_t image_count = surface_support.capabilities.minImageCount + 1;
	if (surface_support.capabilities.maxImageCount > 0 && image_count > surface_support.capabilities.maxImageCount) {
		image_count = surface_support.capabilities.maxImageCount;
	}
	VkSurfaceFormatKHR surface_format = detail::find_surface_format (surface_support.formats, desired_formats);

	VkExtent2D extent =
	    detail::find_extent (surface_support.capabilities, info.desired_width, info.desired_height);

	uint32_t image_array_layers = info.array_layer_count;
	if (surface_support.capabilities.maxImageArrayLayers < info.array_layer_count)
		image_array_layers = surface_support.capabilities.maxImageArrayLayers;
	if (info.array_layer_count == 0) image_array_layers = 1;

	uint32_t queue_family_indices[] = { info.graphics_queue_index, info.present_queue_index };


	VkPresentModeKHR present_mode =
	    detail::find_present_mode (surface_support.present_modes, desired_present_modes);

	VkSurfaceTransformFlagBitsKHR pre_transform = info.pre_transform;
	if (info.pre_transform == static_cast<VkSurfaceTransformFlagBitsKHR> (0))
		pre_transform = surface_support.capabilities.currentTransform;

	VkSwapchainCreateInfoKHR swapchain_create_info = {};
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	detail::setup_pNext_chain (swapchain_create_info, info.pNext_chain);
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
	VkResult res = detail::vulkan_functions ().fp_vkCreateSwapchainKHR (
	    info.device, &swapchain_create_info, info.allocation_callbacks, &swapchain.swapchain);
	if (res != VK_SUCCESS) {
		return detail::Error{ SwapchainError::failed_create_swapchain, res };
	}
	swapchain.device = info.device;
	swapchain.image_format = surface_format.format;
	swapchain.extent = extent;
	auto images = swapchain.get_images ();
	if (!images) {
		return detail::Error{ SwapchainError::failed_get_swapchain_images };
	}
	swapchain.image_count = static_cast<uint32_t> (images.value ().size ());
	swapchain.allocation_callbacks = info.allocation_callbacks;
	return swapchain;
}
detail::Result<std::vector<VkImage>> Swapchain::get_images () {
	std::vector<VkImage> swapchain_images;

	auto swapchain_images_ret = detail::get_vector<VkImage> (
	    swapchain_images, detail::vulkan_functions ().fp_vkGetSwapchainImagesKHR, device, swapchain);
	if (swapchain_images_ret != VK_SUCCESS) {
		return detail::Error{ SwapchainError::failed_get_swapchain_images, swapchain_images_ret };
	}
	return swapchain_images;
}
detail::Result<std::vector<VkImageView>> Swapchain::get_image_views () {

	auto swapchain_images_ret = get_images ();
	if (!swapchain_images_ret) return swapchain_images_ret.error ();
	auto swapchain_images = swapchain_images_ret.value ();

	std::vector<VkImageView> views{ swapchain_images.size () };

	for (size_t i = 0; i < swapchain_images.size (); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
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

		VkResult res = detail::vulkan_functions ().fp_vkCreateImageView (
		    device, &createInfo, allocation_callbacks, &views[i]);
		if (res != VK_SUCCESS)
			return detail::Error{ SwapchainError::failed_create_swapchain_image_views, res };
	}
	return views;
}
void Swapchain::destroy_image_views (std::vector<VkImageView> const& image_views) {
	for (auto& image_view : image_views) {
		detail::vulkan_functions ().fp_vkDestroyImageView (device, image_view, allocation_callbacks);
	}
}
SwapchainBuilder& SwapchainBuilder::set_old_swapchain (VkSwapchainKHR old_swapchain) {
	info.old_swapchain = old_swapchain;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_old_swapchain (Swapchain const& swapchain) {
	info.old_swapchain = swapchain.swapchain;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_desired_extent (uint32_t width, uint32_t height) {
	info.desired_width = width;
	info.desired_height = height;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_desired_format (VkSurfaceFormatKHR format) {
	info.desired_formats.insert (info.desired_formats.begin (), format);
	return *this;
}
SwapchainBuilder& SwapchainBuilder::add_fallback_format (VkSurfaceFormatKHR format) {
	info.desired_formats.push_back (format);
	return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_format_selection () {
	info.desired_formats.clear ();
	add_desired_formats (info.desired_formats);
	return *this;
}

SwapchainBuilder& SwapchainBuilder::set_desired_present_mode (VkPresentModeKHR present_mode) {
	info.desired_present_modes.insert (info.desired_present_modes.begin (), present_mode);
	return *this;
}
SwapchainBuilder& SwapchainBuilder::add_fallback_present_mode (VkPresentModeKHR present_mode) {
	info.desired_present_modes.push_back (present_mode);
	return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_present_mode_selection () {
	info.desired_present_modes.clear ();
	add_desired_present_modes (info.desired_present_modes);
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_allocation_callbacks (VkAllocationCallbacks* callbacks) {
	info.allocation_callbacks = callbacks;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_image_usage_flags (VkImageUsageFlags usage_flags) {
	info.image_usage_flags = usage_flags;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::add_image_usage_flags (VkImageUsageFlags usage_flags) {
	info.image_usage_flags = info.image_usage_flags | usage_flags;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::use_default_image_usage_flags () {
	info.image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_image_array_layer_count (uint32_t array_layer_count) {
	info.array_layer_count = array_layer_count;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_clipped (bool clipped) {
	info.clipped = clipped;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_create_flags (VkSwapchainCreateFlagBitsKHR create_flags) {
	info.create_flags = create_flags;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_pre_transform_flags (VkSurfaceTransformFlagBitsKHR pre_transform_flags) {
	info.pre_transform = pre_transform_flags;
	return *this;
}
SwapchainBuilder& SwapchainBuilder::set_composite_alpha_flags (VkCompositeAlphaFlagBitsKHR composite_alpha_flags) {
	info.composite_alpha = composite_alpha_flags;
	return *this;
}

void SwapchainBuilder::add_desired_formats (std::vector<VkSurfaceFormatKHR>& formats) const {
	formats.push_back ({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
	formats.push_back ({ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
}
void SwapchainBuilder::add_desired_present_modes (std::vector<VkPresentModeKHR>& modes) const {
	modes.push_back (VK_PRESENT_MODE_MAILBOX_KHR);
	modes.push_back (VK_PRESENT_MODE_FIFO_KHR);
}
} // namespace vkb