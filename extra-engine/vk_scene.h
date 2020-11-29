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

struct DrawMesh {
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
	bool isMerged;

	Mesh* original;
};

enum class PassTypeFlags : uint8_t {
	None = 0,
	Forward = 1 << 0,
	Prepass = 1 << 1,
	DirectionalShadow = 1 << 2
};

struct RenderObject {

	Handle<DrawMesh> meshID;
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
		Handle<DrawMesh> meshID;
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
	struct Multibatch {
		uint32_t first;
		uint32_t count;
	};
	struct MeshPass {

		std::vector<RenderScene::Multibatch> multibatches;

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

	void write_object(GPUObjectData* target, Handle<RenderObject> objectID);
	
	void clear_dirty_objects();

	void build_batches();

	void merge_meshes(class VulkanEngine* engine);

	void refresh_pass(MeshPass* pass);

	RenderObject* get_object(Handle<RenderObject> objectID);
	DrawMesh get_mesh(Handle<DrawMesh> objectID);
	Material* get_material(Handle<Material> objectID);

	std::vector<RenderObject> renderables;
	std::vector<Material*> materials;
	std::vector<DrawMesh> meshes;

	std::vector<Handle<RenderObject>> dirtyObjects;

	MeshPass _forwardPass;
	MeshPass _shadowPass;

	std::unordered_map<Material*, Handle<Material>> materialConvert;
	std::unordered_map<Mesh*, Handle<DrawMesh>> meshConvert;

	Handle<Material> getMaterialHandle(Material* m);
	Handle<DrawMesh> getMeshHandle(Mesh* m);

	AllocatedBufferUntyped uploadBuffer[2];

	AllocatedBuffer<Vertex> mergedVertexBuffer;
	AllocatedBuffer<uint32_t> mergedIndexBuffer;

	AllocatedBuffer<GPUObjectData> objectDataBuffer;	
};

inline PassTypeFlags operator&(PassTypeFlags A, PassTypeFlags B)
{
	uint8_t uA = (uint8_t)A;
	uint8_t uB = (uint8_t)B;

	return (PassTypeFlags)(uA & uB);
}
inline PassTypeFlags operator|(PassTypeFlags A, PassTypeFlags B)
{
	uint8_t uA = (uint8_t)A;
	uint8_t uB = (uint8_t)B;

	return (PassTypeFlags)(uA | uB);
}