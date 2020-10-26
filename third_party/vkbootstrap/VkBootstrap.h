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

#pragma once

#include <cassert>

#include <vector>
#include <system_error>

#include <vulkan/vulkan.h>


namespace vkb {

namespace detail {

struct Error {
	std::error_code type;
	VkResult vk_result = VK_SUCCESS; // optional error value if a vulkan call failed
};

template <typename T> class Result {
	public:
	Result (const T& value) : m_value{ value }, m_init{ true } {}
	Result (T&& value) : m_value{ std::move (value) }, m_init{ true } {}

	Result (Error error) : m_error{ error }, m_init{ false } {}

	Result (std::error_code error_code, VkResult result = VK_SUCCESS)
	: m_error{ error_code, result }, m_init{ false } {}

	~Result () { destroy (); }
	Result (Result const& expected) : m_init (expected.m_init) {
		if (m_init)
			new (&m_value) T{ expected.m_value };
		else
			m_error = expected.m_error;
	}
	Result (Result&& expected) : m_init (expected.m_init) {
		if (m_init)
			new (&m_value) T{ std::move (expected.m_value) };
		else
			m_error = std::move (expected.m_error);
		expected.destroy ();
	}

	Result& operator= (const T& expect) {
		destroy ();
		m_init = true;
		new (&m_value) T{ expect };
		return *this;
	}
	Result& operator= (T&& expect) {
		destroy ();
		m_init = true;
		new (&m_value) T{ std::move (expect) };
		return *this;
	}
	Result& operator= (const Error& error) {
		destroy ();
		m_init = false;
		m_error = error;
		return *this;
	}
	Result& operator= (Error&& error) {
		destroy ();
		m_init = false;
		m_error = error;
		return *this;
	}
	// clang-format off
	const T* operator-> () const { assert (m_init); return &m_value; }
	T*       operator-> ()       { assert (m_init); return &m_value; }
	const T& operator* () const& { assert (m_init);	return m_value; }
	T&       operator* () &      { assert (m_init); return m_value; }
	T&&      operator* () &&	 { assert (m_init); return std::move (m_value); }
	const T&  value () const&    { assert (m_init); return m_value; }
	T&        value () &         { assert (m_init); return m_value; }
	const T&& value () const&&   { assert (m_init); return std::move (m_value); }
	T&&       value () &&        { assert (m_init); return std::move (m_value); }

    std::error_code error() const { assert (!m_init); return m_error.type; }
    VkResult vk_result() const { assert (!m_init); return m_error.vk_result; }
	// clang-format on


	bool has_value () const { return m_init; }
	explicit operator bool () const { return m_init; }

	private:
	void destroy () {
		if (m_init) m_value.~T ();
	}
	union {
		T m_value;
		Error m_error;
	};
	bool m_init;
};

} // namespace detail

enum class InstanceError {
	vulkan_unavailable,
	vulkan_version_unavailable,
	vulkan_version_1_1_unavailable,
	vulkan_version_1_2_unavailable,
	failed_create_instance,
	failed_create_debug_messenger,
	requested_layers_not_present,
	requested_extensions_not_present,
	windowing_extensions_not_present,
};
enum class PhysicalDeviceError {
	no_surface_provided,
	failed_enumerate_physical_devices,
	no_physical_devices_found,
	no_suitable_device,
};
enum class QueueError {
	present_unavailable,
	graphics_unavailable,
	compute_unavailable,
	transfer_unavailable,
	queue_index_out_of_range,
	invalid_queue_family_index
};
enum class DeviceError {
	failed_create_device,
};
enum class SwapchainError {
	surface_handle_not_provided,
	failed_query_surface_support_details,
	failed_create_swapchain,
	failed_get_swapchain_images,
	failed_create_swapchain_image_views,
};

std::error_code make_error_code (InstanceError instance_error);
std::error_code make_error_code (PhysicalDeviceError physical_device_error);
std::error_code make_error_code (QueueError queue_error);
std::error_code make_error_code (DeviceError device_error);
std::error_code make_error_code (SwapchainError swapchain_error);

const char* to_string_message_severity (VkDebugUtilsMessageSeverityFlagBitsEXT s);
const char* to_string_message_type (VkDebugUtilsMessageTypeFlagsEXT s);

const char* to_string (InstanceError err);
const char* to_string (PhysicalDeviceError err);
const char* to_string (QueueError err);
const char* to_string (DeviceError err);
const char* to_string (SwapchainError err);

// Gathers useful information about the available vulkan capabilities, like layers and instance extensions.
// Use this for enabling features conditionally, ie if you would like an extension but can use a fallback if
// it isn't supported but need to know if support is available first.
struct SystemInfo {
	private:
	SystemInfo ();

	public:
    // Use get_system_info to create a SystemInfo struct. This is because loading vulkan could fail.
	static detail::Result<SystemInfo> get_system_info ();
	static detail::Result<SystemInfo>  get_system_info (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr);

	// Returns true if a layer is available
	bool is_layer_available (const char* layer_name) const;
	// Returns true if an extension is available
	bool is_extension_available (const char* extension_name) const;

	std::vector<VkLayerProperties> available_layers;
	std::vector<VkExtensionProperties> available_extensions;
	bool validation_layers_available = false;
	bool debug_utils_available = false;
};


class InstanceBuilder;
class PhysicalDeviceSelector;

struct Instance {
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
	VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;

	PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr = nullptr;

	private:
	bool headless = false;
	uint32_t instance_version = VK_MAKE_VERSION (1, 0, 0);

	friend class InstanceBuilder;
	friend class PhysicalDeviceSelector;
};

void destroy_instance (Instance instance); // release instance resources

class InstanceBuilder {
	public:
	// Default constructor, will load vulkan.
	explicit InstanceBuilder ();
	// Optional: Can use your own PFN_vkGetInstanceProcAddr
	explicit InstanceBuilder (PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr);

	// Create a VkInstance. Return an error if it failed.
	detail::Result<Instance> build () const;

	// Sets the name of the application. Defaults to "" if none is provided.
	InstanceBuilder& set_app_name (const char* app_name);
	// Sets the name of the engine. Defaults to "" if none is provided.
	InstanceBuilder& set_engine_name (const char* engine_name);
	// Sets the (major, minor, patch) version of the application.
	InstanceBuilder& set_app_version (uint32_t major, uint32_t minor, uint32_t patch = 0);
	// Sets the (major, minor, patch) version of the engine.
	InstanceBuilder& set_engine_version (uint32_t major, uint32_t minor, uint32_t patch = 0);
	// Require a vulkan instance API version. Will fail to create if this version isn't available.
	InstanceBuilder& require_api_version (uint32_t major, uint32_t minor, uint32_t patch = 0);
	// Prefer a vulkan instance API version. If the desired version isn't available, it will use the highest version available.
	InstanceBuilder& desire_api_version (uint32_t major, uint32_t minor, uint32_t patch = 0);

	// Adds a layer to be enabled. Will fail to create an instance if the layer isn't available.
	InstanceBuilder& enable_layer (const char* layer_name);
	// Adds an extension to be enabled. Will fail to create an instance if the extension isn't available.
	InstanceBuilder& enable_extension (const char* extension_name);

	// Headless Mode does not load the required extensions for presentation. Defaults to true.
	InstanceBuilder& set_headless (bool headless = true);

	// Enables the validation layers. Will fail to create an instance if the validation layers aren't available.
	InstanceBuilder& enable_validation_layers (bool require_validation = true);
	// Checks if the validation layers are available and loads them if they are.
	InstanceBuilder& request_validation_layers (bool enable_validation = true);

	// Use a default debug callback that prints to standard out.
	InstanceBuilder& use_default_debug_messenger ();
	// Provide a user defined debug callback.
	InstanceBuilder& set_debug_callback (PFN_vkDebugUtilsMessengerCallbackEXT callback);
	// Set what message severity is needed to trigger the callback.
	InstanceBuilder& set_debug_messenger_severity (VkDebugUtilsMessageSeverityFlagsEXT severity);
	// Add a message severity to the list that triggers the callback.
	InstanceBuilder& add_debug_messenger_severity (VkDebugUtilsMessageSeverityFlagsEXT severity);
	// Set what message type triggers the callback.
	InstanceBuilder& set_debug_messenger_type (VkDebugUtilsMessageTypeFlagsEXT type);
	// Add a message type to the list of that triggers the callback.
	InstanceBuilder& add_debug_messenger_type (VkDebugUtilsMessageTypeFlagsEXT type);

	// Disable some validation checks.
	// Checks: All, and Shaders
	InstanceBuilder& add_validation_disable (VkValidationCheckEXT check);

	// Enables optional parts of the validation layers.
	// Parts: best practices, gpu assisted, and gpu assisted reserve binding slot.
	InstanceBuilder& add_validation_feature_enable (VkValidationFeatureEnableEXT enable);

	// Disables sections of the validation layers.
	// Options: All, shaders, thread safety, api parameters, object lifetimes, core checks, and unique handles.
	InstanceBuilder& add_validation_feature_disable (VkValidationFeatureDisableEXT disable);

	// Provide custom allocation callbacks.
	InstanceBuilder& set_allocation_callbacks (VkAllocationCallbacks* callbacks);

	private:
	struct InstanceInfo {
		// VkApplicationInfo
		const char* app_name = nullptr;
		const char* engine_name = nullptr;
		uint32_t application_version = 0;
		uint32_t engine_version = 0;
		uint32_t required_api_version = VK_MAKE_VERSION (1, 0, 0);
		uint32_t desired_api_version = VK_MAKE_VERSION (1, 0, 0);

		// VkInstanceCreateInfo
		std::vector<const char*> layers;
		std::vector<const char*> extensions;
		VkInstanceCreateFlags flags = 0;
		std::vector<VkBaseOutStructure*> pNext_elements;

		// debug callback
		PFN_vkDebugUtilsMessengerCallbackEXT debug_callback = nullptr;
		VkDebugUtilsMessageSeverityFlagsEXT debug_message_severity =
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		VkDebugUtilsMessageTypeFlagsEXT debug_message_type =
		    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		// validation features
		std::vector<VkValidationCheckEXT> disabled_validation_checks;
		std::vector<VkValidationFeatureEnableEXT> enabled_validation_features;
		std::vector<VkValidationFeatureDisableEXT> disabled_validation_features;

		// Custom allocator
		VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;

		bool request_validation_layers = false;
		bool enable_validation_layers = false;
		bool use_debug_messenger = false;
		bool headless_context = false;

		PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr = nullptr;
	} info;
};

VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

void destroy_debug_utils_messenger(VkInstance const instance, VkDebugUtilsMessengerEXT const messenger, VkAllocationCallbacks* allocation_callbacks = nullptr);

// ---- Physical Device ---- //
class PhysicalDeviceSelector;
class DeviceBuilder;

struct PhysicalDevice {
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkPhysicalDeviceFeatures features{};
	VkPhysicalDeviceProperties properties{};
	VkPhysicalDeviceMemoryProperties memory_properties{};

	// Has a queue family that supports compute operations but not graphics nor transfer.
	bool has_dedicated_compute_queue () const;
	// Has a queue family that supports transfer operations but not graphics nor compute.
	bool has_dedicated_transfer_queue () const;

	// Has a queue family that supports transfer operations but not graphics.
	bool has_separate_compute_queue () const;
	// Has a queue family that supports transfer operations but not graphics.
	bool has_separate_transfer_queue () const;

	// Advanced: Get the VkQueueFamilyProperties of the device if special queue setup is needed
	std::vector<VkQueueFamilyProperties> get_queue_families () const;

	private:
	std::vector<const char*> extensions_to_enable;
	std::vector<VkQueueFamilyProperties> queue_families;
	bool defer_surface_initialization = false;
	friend class PhysicalDeviceSelector;
	friend class DeviceBuilder;
};

enum class PreferredDeviceType {
	other = 0,
	integrated = 1,
	discrete = 2,
	virtual_gpu = 3,
	cpu = 4
};

class PhysicalDeviceSelector {
	public:
	// Requires a vkb::Instance to construct, needed to pass instance creation info.
	explicit PhysicalDeviceSelector (Instance const& instance);

	detail::Result<PhysicalDevice> select () const;

	// Set the surface in which the physical device should render to.
	PhysicalDeviceSelector& set_surface (VkSurfaceKHR surface);
	// Set the desired physical device type to select. Defaults to PreferredDeviceType::discrete.
	PhysicalDeviceSelector& prefer_gpu_device_type (PreferredDeviceType type = PreferredDeviceType::discrete);
	// Allow selection of a gpu device type that isn't the preferred physical device type. Defaults to true.
	PhysicalDeviceSelector& allow_any_gpu_device_type (bool allow_any_type = true);

	// Require that a physical device supports presentation. Defaults to true.
	PhysicalDeviceSelector& require_present (bool require = true);

	// Require a queue family that supports compute operations but not graphics nor transfer.
	PhysicalDeviceSelector& require_dedicated_compute_queue ();
	// Require a queue family that supports transfer operations but not graphics nor compute.
	PhysicalDeviceSelector& require_dedicated_transfer_queue ();

	// Require a queue family that supports compute operations but not graphics.
	PhysicalDeviceSelector& require_separate_compute_queue ();
	// Require a queue family that supports transfer operations but not graphics.
	PhysicalDeviceSelector& require_separate_transfer_queue ();

	// Require a memory heap from VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT with `size` memory available.
	PhysicalDeviceSelector& required_device_memory_size (VkDeviceSize size);
	// Prefer a memory heap from VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT with `size` memory available.
	PhysicalDeviceSelector& desired_device_memory_size (VkDeviceSize size);

	// Require a physical device which supports a specific extension.
	PhysicalDeviceSelector& add_required_extension (const char* extension);
	// Require a physical device which supports a set of extensions.
	PhysicalDeviceSelector& add_required_extensions (std::vector<const char*> extensions);

	// Prefer a physical device which supports a specific extension.
	PhysicalDeviceSelector& add_desired_extension (const char* extension);
	// Prefer a physical device which supports a set of extensions.
	PhysicalDeviceSelector& add_desired_extensions (std::vector<const char*> extensions);

	// Prefer a physical device that supports a (major, minor) version of vulkan.
	PhysicalDeviceSelector& set_desired_version (uint32_t major, uint32_t minor);
	// Require a physical device that supports a (major, minor) version of vulkan.
	PhysicalDeviceSelector& set_minimum_version (uint32_t major, uint32_t minor);

	// Require a physical device which supports the features in VkPhysicalDeviceFeatures.
	PhysicalDeviceSelector& set_required_features (VkPhysicalDeviceFeatures features);

	// Used when surface creation happens after physical device selection.
	// Warning: This disables checking if the physical device supports a given surface.
	PhysicalDeviceSelector& defer_surface_initialization ();

	// Ignore all criteria and choose the first physical device that is available.
	// Only use when: The first gpu in the list may be set by global user preferences and an application may wish to respect it.
	PhysicalDeviceSelector& select_first_device_unconditionally (bool unconditionally = true);

	private:
	struct SystemInfo {
		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		bool headless = false;
	} system_info;

	struct PhysicalDeviceDesc {
		VkPhysicalDevice phys_device = VK_NULL_HANDLE;
		std::vector<VkQueueFamilyProperties> queue_families;

		VkPhysicalDeviceFeatures device_features{};
		VkPhysicalDeviceProperties device_properties{};
		VkPhysicalDeviceMemoryProperties mem_properties{};
	};
	PhysicalDeviceDesc populate_device_details (VkPhysicalDevice phys_device) const;

	struct SelectionCriteria {
		PreferredDeviceType preferred_type = PreferredDeviceType::discrete;
		bool allow_any_type = true;
		bool require_present = true;
		bool require_dedicated_transfer_queue = false;
		bool require_dedicated_compute_queue = false;
		bool require_separate_transfer_queue = false;
		bool require_separate_compute_queue = false;
		VkDeviceSize required_mem_size = 0;
		VkDeviceSize desired_mem_size = 0;

		std::vector<const char*> required_extensions;
		std::vector<const char*> desired_extensions;

		uint32_t required_version = VK_MAKE_VERSION (1, 0, 0);
		uint32_t desired_version = VK_MAKE_VERSION (1, 0, 0);

		VkPhysicalDeviceFeatures required_features{};

		bool defer_surface_initialization = false;
		bool use_first_gpu_unconditionally = false;
	} criteria;

	enum class Suitable { yes, partial, no };

	Suitable is_device_suitable (PhysicalDeviceDesc phys_device) const;
};

// ---- Queue ---- //
enum class QueueType { present, graphics, compute, transfer };

// ---- Device ---- //

struct Device {
	VkDevice device = VK_NULL_HANDLE;
	PhysicalDevice physical_device;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	std::vector<VkQueueFamilyProperties> queue_families;
	VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;

	detail::Result<uint32_t> get_queue_index (QueueType type) const;
	// Only a compute or transfer queue type is valid. All other queue types do not support a 'dedicated' queue index
	detail::Result<uint32_t> get_dedicated_queue_index (QueueType type) const;

	detail::Result<VkQueue> get_queue (QueueType type) const;
	// Only a compute or transfer queue type is valid. All other queue types do not support a 'dedicated' queue
	detail::Result<VkQueue> get_dedicated_queue (QueueType type) const;
};

// For advanced device queue setup
struct CustomQueueDescription {
	explicit CustomQueueDescription (uint32_t index, uint32_t count, std::vector<float> priorities);
	uint32_t index = 0;
	uint32_t count = 0;
	std::vector<float> priorities;
};

void destroy_device (Device device);

class DeviceBuilder {
	public:
	// Any features and extensions that are requested/required in PhysicalDeviceSelector are automatically enabled.
	explicit DeviceBuilder (PhysicalDevice physical_device);

	detail::Result<Device> build () const;

	// For Advanced Users: specify the exact list of VkDeviceQueueCreateInfo's needed for the application.
	// If a custom queue setup is provided, getting the queues and queue indexes is up to the application.
	DeviceBuilder& custom_queue_setup (std::vector<CustomQueueDescription> queue_descriptions);

	// Add a structure to the pNext chain of VkDeviceCreateInfo.
	// The structure must be valid when DeviceBuilder::build() is called.
	template <typename T> DeviceBuilder& add_pNext (T* structure) {
		info.pNext_chain.push_back (reinterpret_cast<VkBaseOutStructure*> (structure));
		return *this;
	}

	// Provide custom allocation callbacks.
	DeviceBuilder& set_allocation_callbacks (VkAllocationCallbacks* callbacks);

	private:
	struct DeviceInfo {
		VkDeviceCreateFlags flags = 0;
		std::vector<VkBaseOutStructure*> pNext_chain;
		PhysicalDevice physical_device;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		bool defer_surface_initialization = false;
		std::vector<VkQueueFamilyProperties> queue_families;
		VkPhysicalDeviceFeatures features{};
		std::vector<const char*> extensions_to_enable;
		std::vector<CustomQueueDescription> queue_descriptions;
		VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;
	} info;
};

// ---- Swapchain ---- //
struct Swapchain {
	VkDevice device = VK_NULL_HANDLE;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	uint32_t image_count = 0;
	VkFormat image_format = VK_FORMAT_UNDEFINED;
	VkExtent2D extent = { 0, 0 };
	VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;

	// Returns a vector of VkImage handles to the swapchain.
	detail::Result<std::vector<VkImage>> get_images ();

	// Returns a vector of VkImageView's to the VkImage's of the swapchain.
	// VkImageViews must be destroyed.
	detail::Result<std::vector<VkImageView>> get_image_views ();
	void destroy_image_views (std::vector<VkImageView> const& image_views);
};

void destroy_swapchain (Swapchain const& swapchain);

class SwapchainBuilder {
	public:
	explicit SwapchainBuilder (Device const& device);
	explicit SwapchainBuilder (Device const& device, VkSurfaceKHR const surface);
	explicit SwapchainBuilder (VkPhysicalDevice const physical_device, VkDevice const device, VkSurfaceKHR const surface, int32_t graphics_queue_index = -1, int32_t present_queue_index = -1);

	detail::Result<Swapchain> build () const;

	// Set the oldSwapchain member of VkSwapchainCreateInfoKHR.
	// For use in rebuilding a swapchain.
	SwapchainBuilder& set_old_swapchain (VkSwapchainKHR old_swapchain);
	SwapchainBuilder& set_old_swapchain (Swapchain const& swapchain);


	// Desired size of the swapchain. By default, the swapchain will use the size
	// of the window being drawn to.
	SwapchainBuilder& set_desired_extent (uint32_t width, uint32_t height);

	// When determining the surface format, make this the first to be used if supported.
	SwapchainBuilder& set_desired_format (VkSurfaceFormatKHR format);
	// Add this swapchain format to the end of the list of formats selected from.
	SwapchainBuilder& add_fallback_format (VkSurfaceFormatKHR format);
	// Use the default swapchain formats. This is done if no formats are provided.
	SwapchainBuilder& use_default_format_selection ();

	// When determining the present mode, make this the first to be used if supported.
	SwapchainBuilder& set_desired_present_mode (VkPresentModeKHR present_mode);
	// Add this present mode to the end of the list of present modes selected from.
	SwapchainBuilder& add_fallback_present_mode (VkPresentModeKHR present_mode);
	// Use the default presentation mode. This is done if no present modes are provided.
	SwapchainBuilder& use_default_present_mode_selection ();

	// Set the bitmask of the image usage for acquired swapchain images.
	SwapchainBuilder& set_image_usage_flags (VkImageUsageFlags usage_flags);
	// Add a image usage to the bitmask for acquired swapchain images.
	SwapchainBuilder& add_image_usage_flags (VkImageUsageFlags usage_flags);
	// Use the default image usage bitmask values. This is the default if no image usages
	// are provided. The default is VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
	SwapchainBuilder& use_default_image_usage_flags ();

	// Set the number of views in for multiview/stereo surface
	SwapchainBuilder& set_image_array_layer_count (uint32_t array_layer_count);

	// Set whether the Vulkan implementation is allowed to discard rendering operations that
	// affect regions of the surface that are not visible. Default is true.
	// Note: Applications should use the default of true if they do not expect to read back the content
	// of presentable images before presenting them or after reacquiring them, and if their fragment
	// shaders do not have any side effects that require them to run for all pixels in the presentable image.
	SwapchainBuilder& set_clipped (bool clipped = true);

	// Set the VkSwapchainCreateFlagBitsKHR.
	SwapchainBuilder& set_create_flags (VkSwapchainCreateFlagBitsKHR create_flags);
	// Set the transform to be applied, like a 90 degree rotation. Default is the current transform.
	SwapchainBuilder& set_pre_transform_flags (VkSurfaceTransformFlagBitsKHR pre_transform_flags);
	// Set the alpha channel to be used with other windows in on the system. Default is VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
	SwapchainBuilder& set_composite_alpha_flags (VkCompositeAlphaFlagBitsKHR composite_alpha_flags);

	// Add a structure to the pNext chain of VkSwapchainCreateInfoKHR.
	// The structure must be valid when SwapchainBuilder::build() is called.
	template <typename T> SwapchainBuilder& add_pNext (T* structure) {
		info.pNext_chain.push_back (reinterpret_cast<VkBaseOutStructure*> (structure));
		return *this;
	}

	// Provide custom allocation callbacks.
	SwapchainBuilder& set_allocation_callbacks (VkAllocationCallbacks* callbacks);

	private:
	void add_desired_formats (std::vector<VkSurfaceFormatKHR>& formats) const;
	void add_desired_present_modes (std::vector<VkPresentModeKHR>& modes) const;

	struct SwapchainInfo {
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		std::vector<VkBaseOutStructure*> pNext_chain;
		VkSwapchainCreateFlagBitsKHR create_flags = static_cast<VkSwapchainCreateFlagBitsKHR> (0);
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<VkSurfaceFormatKHR> desired_formats;
		uint32_t desired_width = 256;
		uint32_t desired_height = 256;
		uint32_t array_layer_count = 1;
		VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t graphics_queue_index = 0;
		uint32_t present_queue_index = 0;
		VkSurfaceTransformFlagBitsKHR pre_transform = static_cast<VkSurfaceTransformFlagBitsKHR> (0);
		VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkPresentModeKHR> desired_present_modes;
		bool clipped = true;
		VkSwapchainKHR old_swapchain = VK_NULL_HANDLE;
		VkAllocationCallbacks* allocation_callbacks = VK_NULL_HANDLE;
	} info;
};

} // namespace vkb


namespace std {
template <> struct is_error_code_enum<vkb::InstanceError> : true_type {};
template <> struct is_error_code_enum<vkb::PhysicalDeviceError> : true_type {};
template <> struct is_error_code_enum<vkb::QueueError> : true_type {};
template <> struct is_error_code_enum<vkb::DeviceError> : true_type {};
template <> struct is_error_code_enum<vkb::SwapchainError> : true_type {};
} // namespace std