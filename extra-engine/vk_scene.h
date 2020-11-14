// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_scene.h>

#include <vk_mesh.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <array>
#include <vector>
#include <unordered_map>

template<typename T>
struct Handle {
	uint32_t handle;
};

struct Material;
struct MeshObject;
struct Mesh;
struct GPUObjectData;

struct GPUIndirectObject {
	VkDrawIndexedIndirectCommand command;
	uint32_t objectID;
	uint32_t batchID;
};


enum class PassTypeFlags : uint8_t {
	Forward = 1 << 0,
	Prepass = 1 << 1,
	DirectionalShadow = 1 << 2
};

struct RenderObject {

	Handle<Mesh> meshID;
	Handle<Material> material;

	uint32_t updateIndex;
	uint32_t customSortKey{0};

	glm::mat4 transformMatrix;

	RenderBounds bounds;
};

struct GPUInstance {
	uint32_t objectID;
	uint32_t batchID;
};


class RenderScene {
public:
	struct IndirectBatch {
		Handle<Mesh> meshID;
		Handle<Material> material;

		uint32_t first;
		uint32_t count;

		glm::vec3 AABBMin;
		glm::vec3 AABBMax;

		std::vector<Handle<RenderObject>> objects;
	};
	struct RenderBatch {
		Handle<RenderObject> object;
		uint64_t sortKey;
	};

	struct MeshPass {
		std::vector<RenderScene::IndirectBatch> batches;

		std::vector<Handle<RenderObject>> unbatchedObjects;

		std::vector<RenderScene::RenderBatch> flat_batches;

		
		AllocatedBuffer<uint32_t> compactedInstanceBuffer;

		AllocatedBuffer<GPUIndirectObject> drawIndirectBuffer;

		AllocatedBuffer<GPUInstance> instanceBuffer;

		AllocatedBuffer<GPUIndirectObject> clearIndirectBuffer;

		bool needsIndirectRefresh = true;
		bool needsInstanceRefresh = true;
	};

	Handle<RenderObject> register_object(MeshObject* object, PassTypeFlags passes);

	void register_object_batch(MeshObject* first, uint32_t count, PassTypeFlags passes);

	void update_transform(Handle<RenderObject> objectID,const glm::mat4 &localToWorld);
	void update_object(Handle<RenderObject> objectID);
	
	void fill_objectData(GPUObjectData* data);
	void fill_indirectArray(GPUIndirectObject* data);
	void fill_instancesArray(GPUInstance* data);

	
	void clear_dirty_objects();

	void build_batches();

	void refresh_pass(MeshPass* pass);

	RenderObject* get_object(Handle<RenderObject> objectID);
	Mesh* get_mesh(Handle<Mesh> objectID);
	Material* get_material(Handle<Material> objectID);

	std::vector<RenderObject> renderables;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	std::vector<Handle<RenderObject>> dirtyObjects;

	MeshPass _forwardPass;

	std::unordered_map<Material*, Handle<Material>> materialConvert;
	std::unordered_map<Mesh*, Handle<Mesh>> meshConvert;

	Handle<Material> getMaterialHandle(Material* m);
	Handle<Mesh> getMeshHandle(Mesh* m);

	AllocatedBufferUntyped uploadBuffer[2];

	AllocatedBuffer<GPUObjectData> objectDataBuffer;	
};
