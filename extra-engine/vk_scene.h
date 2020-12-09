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
#include "material_system.h"

template<typename T>
struct Handle {
	uint32_t handle;
};

struct MeshObject;
struct Mesh;
struct GPUObjectData;
namespace vkutil { struct Material; }
namespace vkutil { struct ShaderPass; }

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



struct RenderObject {

	Handle<DrawMesh> meshID;
	Handle<vkutil::Material> material;

	uint32_t updateIndex;
	uint32_t customSortKey{0};

	vkutil::PerPassData<int32_t> passIndices;

	glm::mat4 transformMatrix;

	RenderBounds bounds;
};

struct GPUInstance {
	uint32_t objectID;
	uint32_t batchID;
};


class RenderScene {
public:
	struct PassMaterial {
		VkDescriptorSet materialSet;
		vkutil::ShaderPass* shaderPass;

		bool operator==(const PassMaterial& other) const
		{
			return materialSet == other.materialSet && shaderPass == other.shaderPass;
		}
	};
	struct PassObject {
		PassMaterial material;
		Handle<DrawMesh> meshID;
		Handle<RenderObject> original;
		uint32_t customKey;
	};
	struct RenderBatch {
		Handle<PassObject> object;
		uint64_t sortKey;

		bool operator==(const RenderBatch& other) const
		{
			return object.handle == other.object.handle && sortKey == other.sortKey;
		}
	};
	struct IndirectBatch {
		Handle<DrawMesh> meshID;
		PassMaterial material;
		uint32_t first;
		uint32_t count;
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

		std::vector<PassObject> objects;

		std::vector<Handle<PassObject>> reusableObjects;

		std::vector<Handle<PassObject>> objectsToDelete;

		
		AllocatedBuffer<uint32_t> compactedInstanceBuffer;

		AllocatedBuffer<GPUIndirectObject> drawIndirectBuffer;

		AllocatedBuffer<GPUInstance> instanceBuffer;

		AllocatedBuffer<GPUIndirectObject> clearIndirectBuffer;

		PassObject* get(Handle<PassObject> handle);

		MeshpassType type;

		bool needsIndirectRefresh = true;
		bool needsInstanceRefresh = true;
	};

	void init();

	Handle<RenderObject> register_object(MeshObject* object);

	void register_object_batch(MeshObject* first, uint32_t count);

	void update_transform(Handle<RenderObject> objectID,const glm::mat4 &localToWorld);
	void update_object(Handle<RenderObject> objectID);
	
	void fill_objectData(GPUObjectData* data);
	void fill_indirectArray(GPUIndirectObject* data, MeshPass& pass);
	void fill_instancesArray(GPUInstance* data, MeshPass& pass);

	void write_object(GPUObjectData* target, Handle<RenderObject> objectID);
	
	void clear_dirty_objects();

	void build_batches();

	void merge_meshes(class VulkanEngine* engine);

	void refresh_pass(MeshPass* pass);

	void build_indirect_batches(MeshPass* pass, std::vector<IndirectBatch>& outbatches, std::vector<RenderScene::RenderBatch>& inobjects);
	RenderObject* get_object(Handle<RenderObject> objectID);
	DrawMesh* get_mesh(Handle<DrawMesh> objectID);

	vkutil::Material *get_material(Handle<vkutil::Material> objectID);

	std::vector<RenderObject> renderables;
	std::vector<DrawMesh> meshes;
	std::vector<vkutil::Material*> materials;

	std::vector<Handle<RenderObject>> dirtyObjects;

	MeshPass* get_mesh_pass(MeshpassType name);

	MeshPass _forwardPass;
	MeshPass _transparentForwardPass;
	MeshPass _shadowPass;

	std::unordered_map<vkutil::Material*, Handle<vkutil::Material>> materialConvert;
	std::unordered_map<Mesh*, Handle<DrawMesh>> meshConvert;

	Handle<vkutil::Material> getMaterialHandle(vkutil::Material* m);
	Handle<DrawMesh> getMeshHandle(Mesh* m);
	

	AllocatedBuffer<Vertex> mergedVertexBuffer;
	AllocatedBuffer<uint32_t> mergedIndexBuffer;

	AllocatedBuffer<GPUObjectData> objectDataBuffer;
};

