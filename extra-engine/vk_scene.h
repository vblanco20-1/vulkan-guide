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
struct RenderObject;
struct Mesh;
struct GPUObjectData;

struct IndirectObject {
	VkDrawIndexedIndirectCommand command;
	uint32_t objectID;
};
enum class PassTypeFlags : uint8_t {
	Forward = 1 << 0,
	Prepass = 1 << 1,
	DirectionalShadow = 1 << 2
};

struct RenderObject2 {

	Handle<Mesh> meshID;
	Handle<Material> material;

	uint32_t updateIndex;
	uint32_t customSortKey{0};

	glm::mat4 transformMatrix;

	RenderBounds bounds;
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
	};
	struct RenderBatch {
		Handle<RenderObject2> object;
		uint64_t sortKey;
	};

	struct MeshPass {
		std::vector<RenderScene::IndirectBatch> batches;

		std::vector<Handle<RenderObject2>> unbatchedObjects;

		std::vector<RenderScene::RenderBatch> flat_batches;
	};

	Handle<RenderObject2> register_object(RenderObject* object, PassTypeFlags passes);

	void register_object_batch(RenderObject* first, uint32_t count, PassTypeFlags passes);

	void update_transform(Handle<RenderObject2> objectID,const glm::mat4 &localToWorld);
	void update_object(Handle<RenderObject2> objectID);
	
	void fill_objectData(GPUObjectData* data);
	void fill_indirectArray(IndirectObject* data);

	void build_batches();

	void refresh_pass(MeshPass* pass);

	RenderObject2* get_object(Handle<RenderObject2> objectID);
	Mesh* get_mesh(Handle<Mesh> objectID);
	Material* get_material(Handle<Material> objectID);

	std::vector<RenderObject2> renderables;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	std::vector<Handle<RenderObject2>> dirtyObjects;

	std::array<MeshPass,8> meshPasses;

	std::unordered_map<Material*, Handle<Material>> materialConvert;
	std::unordered_map<Mesh*, Handle<Mesh>> meshConvert;

	Handle<Material> getMaterialHandle(Material* m);
	Handle<Mesh> getMeshHandle(Mesh* m);


	AllocatedBuffer uploadBuffer[2];

	AllocatedBuffer objectDataBuffer;
	AllocatedBuffer indirectDrawBuffer;
};
