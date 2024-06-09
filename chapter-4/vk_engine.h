// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include "vk_mem_alloc.h"
#include <deque>
#include <functional>
#include "vk_descriptors.h"
#include "vk_loader.h"

class VulkanEngine;

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

struct FrameData {
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	DeletionQueue _deletionQueue;
	DescriptorAllocatorGrowable _frameDescriptors;
};

constexpr unsigned int FRAME_OVERLAP = 2;

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

//> gltfmat
struct GLTFMetallic_Roughness {
	MaterialPipeline opaquePipeline;
	MaterialPipeline transparentPipeline;

	VkDescriptorSetLayout materialLayout;

	struct MaterialConstants {
		glm::vec4 colorFactors;
		glm::vec4 metal_rough_factors;
		//padding, we need it anyway for uniform buffers
		glm::vec4 extra[14];
	};

	struct MaterialResources {
		AllocatedImage colorImage;
		VkSampler colorSampler;
		AllocatedImage metalRoughImage;
		VkSampler metalRoughSampler;
		VkBuffer dataBuffer;
		uint32_t dataBufferOffset;
	};

	DescriptorWriter writer;

	void build_pipelines(VulkanEngine* engine);
	void clear_resources(VkDevice device);

	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
};
//< gltfmat

//> renderobject
struct RenderObject {
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;

	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
	std::vector<RenderObject> OpaqueSurfaces;
};
//< renderobject
//> meshnode
struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};
//< meshnode

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;	

	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	VkExtent2D _swapchainExtent;
	VkExtent2D _drawExtent;
	float renderScale = 1.f;
	DescriptorAllocatorGrowable globalDescriptorAllocator;

	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	std::vector<VkFramebuffer> _framebuffers;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;
	VkDescriptorSetLayout _singleImageDescriptorLayout;

	DeletionQueue _mainDeletionQueue;

	VmaAllocator _allocator; //vma lib allocator

	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;

	GPUMeshBuffers rectangle;
	std::vector<std::shared_ptr<MeshAsset>> testMeshes;
		// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

	VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;
	//draw resources
	DrawContext mainDrawContext;
	GPUSceneData sceneData;
	MaterialInstance defaultData;
	
	GLTFMetallic_Roughness metalRoughMaterial;

	AllocatedImage _drawImage;
	AllocatedImage _depthImage;

	std::unordered_map<std::string, std::shared_ptr<Node>> loadedNodes;

	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect{ 0 };

	//material stuff
	VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;
	VkDescriptorSetLayout _gltfMatDescriptorLayout;

	VkPipeline _gltfDefaultOpaque;
	VkPipeline _gltfDefaultTranslucent;
	VkPipelineLayout _gltfPipelineLayout;

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	void draw_background(VkCommandBuffer cmd);

	void draw_geometry(VkCommandBuffer cmd);

	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

	void update_scene();

	//run main loop
	void run();

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);

	void destroy_buffer(const AllocatedBuffer& buffer);
	void destroy_image(const AllocatedImage& img);

	bool resize_requested{ false };
	bool freeze_rendering{ false };
private:

	void init_vulkan();

	void init_swapchain();
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
	void resize_swapchain();

	void init_commands();
	
	void init_background_pipelines();

	void init_pipelines();
	
	void init_triangle_pipeline();
	void init_mesh_pipeline();

	void init_descriptors();

	void init_sync_structures();
	
	void init_imgui();

	void init_default_data();
};
