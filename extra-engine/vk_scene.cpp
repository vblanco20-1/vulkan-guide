#include <vk_scene.h>
#include <vk_engine.h>
#include "Tracy.hpp"

Handle<RenderObject2> RenderScene::register_object(RenderObject* object, PassTypeFlags passes)
{
	RenderObject2 newObj;
	newObj.bounds = object->bounds;
	newObj.transformMatrix = object->transformMatrix;
	newObj.material = getMaterialHandle(object->material);
	newObj.meshID = getMeshHandle(object->mesh);
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = object->customSortKey;
	Handle<RenderObject2> handle;
	handle.handle = renderables.size();
	
	renderables.push_back(newObj);

	if (passes == PassTypeFlags::Forward)
	{
		meshPasses[0].unbatchedObjects.push_back(handle);
	}

	update_object(handle);
	return handle;
}

void RenderScene::register_object_batch(RenderObject* first, uint32_t count, PassTypeFlags passes)
{
	renderables.reserve(count);

	for (uint32_t i = 0; i < count; i++) {
		register_object(&(first[i]), passes);
	}
}

void RenderScene::update_transform(Handle<RenderObject2> objectID, const glm::mat4& localToWorld)
{
	get_object(objectID)->transformMatrix = localToWorld;
	update_object(objectID);
}


void RenderScene::update_object(Handle<RenderObject2> objectID)
{
	if (get_object(objectID)->updateIndex == (uint32_t)-1)
	{
		get_object(objectID)->updateIndex = dirtyObjects.size();
		dirtyObjects.push_back(objectID);
	}
}

void RenderScene::fill_objectData(GPUObjectData* data)
{
	static int ntimes = 0;
	ntimes++;
	if (ntimes > 3) return;
	for(int i = 0; i < renderables.size(); i++)
	{
		GPUObjectData object;
		const RenderObject2 &renderable = renderables[i];

		object.modelMatrix = renderable.transformMatrix;
		object.origin_rad = glm::vec4(renderable.bounds.origin, renderable.bounds.radius);
		object.extents = glm::vec4(renderable.bounds.extents, renderable.bounds.valid? 1.f : 0.f);

		memcpy(data + i, &object, sizeof(GPUObjectData));
	}
}

void RenderScene::fill_instanceArray(uint32_t* data)
{
	static int ntimes = 0;
	ntimes++;
	if (ntimes > 3) return;
	for (int i = 0; i < meshPasses[0].flat_batches.size(); i++)
	{
		data[i] = meshPasses[0].flat_batches[i].object.handle;
	}
}

void RenderScene::fill_indirectArray(IndirectObject* data)
{
	static int ntimes = 0;
	ntimes++;
	if (ntimes > 3) return;
	for (int i = 0; i < meshPasses[0].flat_batches.size(); i++) {
		RenderObject2* obj = get_object(meshPasses[0].flat_batches[i].object);

		data[i].command.firstInstance = i;
		data[i].command.instanceCount = 1;
		data[i].command.firstIndex = 0;
		data[i].command.vertexOffset = 0;
		data[i].command.indexCount = get_mesh(obj->meshID)->_indices.size();
		data[i].objectID = meshPasses[0].flat_batches[i].object.handle;
	}
}

void RenderScene::build_batches()
{
	refresh_pass(&meshPasses[0]);
}

void RenderScene::refresh_pass(MeshPass* pass)
{
	{
		pass->flat_batches.clear();
		ZoneScopedNC("Fill DrawList", tracy::Color::Blue2);
		for (int i = 0; i < pass->unbatchedObjects.size(); i++) {
			RenderObject2* object = get_object(pass->unbatchedObjects[i]);
		
			{
				RenderScene::RenderBatch newCommand;

				newCommand.object = pass->unbatchedObjects[i];

				//pack mesh id and material into 32 bits
				uint64_t meshmat = object->material.handle << 32 | object->meshID.handle;

				newCommand.sortKey = meshmat;

				pass->flat_batches.push_back(newCommand);
			}
		}
	}

	{
		ZoneScopedNC("Draw Sort", tracy::Color::Blue1);
		std::sort(pass->flat_batches.begin(), pass->flat_batches.end(), [](const RenderScene::RenderBatch& A, const RenderScene::RenderBatch& B) {
			return A.sortKey < B.sortKey;
			});
	}

	{
		ZoneScopedNC("Draw Merge", tracy::Color::Blue);

		pass->batches.clear();

		RenderScene::IndirectBatch newBatch;
		newBatch.first = 0;
		newBatch.count = 0;
		newBatch.material = get_object(pass->flat_batches[0].object)->material;
		newBatch.meshID = get_object(pass->flat_batches[0].object)->meshID;

		pass->batches.push_back(newBatch);

		for (int i = 0; i < pass->flat_batches.size(); i++) {
			RenderObject2* obj = get_object(pass->flat_batches[i].object);
			RenderScene::IndirectBatch* back = &pass->batches.back();

			if (obj->meshID.handle == back->meshID.handle
				&& obj->material.handle == back->material.handle)
			{
				back->count++;
				back->AABBMax = glm::max(back->AABBMax, obj->bounds.origin + obj->bounds.extents);
				back->AABBMin = glm::min(back->AABBMin, obj->bounds.origin - obj->bounds.extents);
			}
			else {

				newBatch.first = i;
				newBatch.count = 1;
				newBatch.material = obj->material;
				newBatch.meshID = obj->meshID;
				newBatch.AABBMax = obj->bounds.origin + obj->bounds.extents;
				newBatch.AABBMin = obj->bounds.origin - obj->bounds.extents;

				pass->batches.push_back(newBatch);
			}
		}
	}
}

RenderObject2* RenderScene::get_object(Handle<RenderObject2> objectID)
{
	return &renderables[objectID.handle];
}

Mesh* RenderScene::get_mesh(Handle<Mesh> objectID)
{
	return meshes[objectID.handle];
}

Material* RenderScene::get_material(Handle<Material> objectID)
{
	return materials[objectID.handle];
}

Handle<Material> RenderScene::getMaterialHandle(Material* m)
{
	Handle<Material> handle;
	auto it = materialConvert.find(m);
	if (it == materialConvert.end())
	{
		uint32_t index = materials.size();
		 materials.push_back(m);

		 handle.handle = index;
		 materialConvert[m] = handle;
	}
	else {
		handle = (*it).second;
	}
	return handle;
}

Handle<Mesh> RenderScene::getMeshHandle(Mesh* m)
{
	Handle<Mesh> handle;
	auto it = meshConvert.find(m);
	if (it == meshConvert.end())
	{
		uint32_t index = meshes.size();
		meshes.push_back(m);

		handle.handle = index;
		meshConvert[m] = handle;
	}
	else {
		handle = (*it).second;
	}
	return handle;
}
