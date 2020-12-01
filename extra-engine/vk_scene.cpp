#include <vk_scene.h>
#include <vk_engine.h>
#include "Tracy.hpp"

Handle<RenderObject> RenderScene::register_object(MeshObject* object)
{
	RenderObject newObj;
	newObj.bounds = object->bounds;
	newObj.transformMatrix = object->transformMatrix;
	newObj.material = getMaterialHandle(object->material);
	newObj.meshID = getMeshHandle(object->mesh);
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = object->customSortKey;
	Handle<RenderObject> handle;
	handle.handle = renderables.size();
	
	renderables.push_back(newObj);

	if (object->bDrawForwardPass)
	{
		if (object->material->forwardEffect)
		{
			_forwardPass.unbatchedObjects.push_back(handle);
		}
	}
	if (object->bDrawShadowPass)
	{
		if (object->material->shadowEffect)
		{
			_shadowPass.unbatchedObjects.push_back(handle);
		}
	}

	update_object(handle);
	return handle;
}

void RenderScene::register_object_batch(MeshObject* first, uint32_t count)
{
	renderables.reserve(count);

	for (uint32_t i = 0; i < count; i++) {
		register_object(&(first[i]));
	}
}

void RenderScene::update_transform(Handle<RenderObject> objectID, const glm::mat4& localToWorld)
{
	get_object(objectID)->transformMatrix = localToWorld;
	update_object(objectID);
}


void RenderScene::update_object(Handle<RenderObject> objectID)
{
	if (get_object(objectID)->updateIndex == (uint32_t)-1)
	{
		get_object(objectID)->updateIndex = dirtyObjects.size();
		dirtyObjects.push_back(objectID);
	}
}

void RenderScene::write_object(GPUObjectData* target, Handle<RenderObject> objectID)
{
	RenderObject* renderable = get_object(objectID);
	GPUObjectData object;

	object.modelMatrix = renderable->transformMatrix;
	object.origin_rad = glm::vec4(renderable->bounds.origin, renderable->bounds.radius);
	object.extents = glm::vec4(renderable->bounds.extents, renderable->bounds.valid ? 1.f : 0.f);

	memcpy(target, &object, sizeof(GPUObjectData));
}

void RenderScene::fill_objectData(GPUObjectData* data)
{
	
	for(int i = 0; i < renderables.size(); i++)
	{
		Handle<RenderObject> h;
		h.handle = i;
		write_object(data + i, h);
	}
}


void RenderScene::fill_indirectArray(GPUIndirectObject* data)
{	
	int dataIndex = 0;
	for (int i = 0; i < _forwardPass.batches.size(); i++) {

		auto batch = _forwardPass.batches[i];

		data[dataIndex].command.firstInstance = batch.first;//i;
		data[dataIndex].command.instanceCount = 0;
		data[dataIndex].command.firstIndex = get_mesh(batch.meshID).firstIndex;
		data[dataIndex].command.vertexOffset = get_mesh(batch.meshID).firstVertex;
		data[dataIndex].command.indexCount = get_mesh(batch.meshID).indexCount;
		data[dataIndex].objectID = 0;
		data[dataIndex].batchID = i;

		dataIndex++;
	}
}

void RenderScene::fill_instancesArray(GPUInstance* data)
{
	
	int dataIndex = 0;
	for (int i = 0; i < _forwardPass.batches.size(); i++) {

		auto batch = _forwardPass.batches[i];

		for (auto objID : batch.objects)
		{			
			data[dataIndex].objectID = objID.handle;
			data[dataIndex].batchID = i;
			dataIndex++;
		}
	}
}

void RenderScene::clear_dirty_objects()
{
	for (auto obj : dirtyObjects)
	{
		get_object(obj)->updateIndex = (uint32_t)-1;
	}
	dirtyObjects.clear();
}

void RenderScene::build_batches()
{
	refresh_pass(&_forwardPass);
	refresh_pass(&_shadowPass);
}

void RenderScene::merge_meshes(VulkanEngine* engine)
{
	ZoneScopedNC("Mesh Merge", tracy::Color::Magenta)
	size_t total_vertices = 0;
	size_t total_indices = 0;

	for (auto& m : meshes)
	{
		m.firstIndex = total_indices;
		m.firstVertex = total_vertices;

		total_vertices += m.vertexCount;
		total_indices += m.indexCount;

		m.isMerged = true;
	}

	mergedVertexBuffer = engine->create_buffer(total_vertices * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VMA_MEMORY_USAGE_GPU_ONLY);

	mergedIndexBuffer = engine->create_buffer(total_indices * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	engine->immediate_submit([&](VkCommandBuffer cmd)
	{
		for (auto& m : meshes)
		{
			VkBufferCopy vertexCopy;
			vertexCopy.dstOffset = m.firstVertex * sizeof(Vertex);
			vertexCopy.size = m.vertexCount * sizeof(Vertex);
			vertexCopy.srcOffset = 0;

			vkCmdCopyBuffer(cmd, m.original->_vertexBuffer._buffer, mergedVertexBuffer._buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy;
			indexCopy.dstOffset = m.firstIndex * sizeof(uint32_t);
			indexCopy.size = m.indexCount * sizeof(uint32_t);
			indexCopy.srcOffset = 0;

			vkCmdCopyBuffer(cmd, m.original->_indexBuffer._buffer, mergedIndexBuffer._buffer, 1, &indexCopy);
		}
	});
}


void RenderScene::refresh_pass(MeshPass* pass)
{
	pass->needsIndirectRefresh = true;
	pass->needsInstanceRefresh = true;
	{
		pass->flat_batches.clear();
		ZoneScopedNC("Fill DrawList", tracy::Color::Blue2);
		for (int i = 0; i < pass->unbatchedObjects.size(); i++) {
			RenderObject* object = get_object(pass->unbatchedObjects[i]);
		
			{
				RenderScene::RenderBatch newCommand;

				newCommand.object = pass->unbatchedObjects[i];

				//pack mesh id and material into 32 bits
				uint32_t meshmat = (uint64_t(object->material.handle) << 10) | uint64_t(object->meshID.handle);

				//pack mesh id and material into 64 bits
				//uint64_t meshmat = uint64_t(meshmat) | object->customSortKey << 32;//object->material.handle << 32 | object->meshID.handle;

				newCommand.sortKey = uint64_t(meshmat) | (uint64_t(object->customSortKey) << 32);

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
			RenderObject* obj = get_object(pass->flat_batches[i].object);
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
			pass->batches.back().objects.push_back(pass->flat_batches[i].object);			
		}

		

		//flatten batches into multibatch
		Multibatch newbatch;
		pass->multibatches.clear();

		
		newbatch.count = 1;
		newbatch.first = 0;

		for (int i = 1; i < pass->batches.size(); i++)
		{
			IndirectBatch* joinbatch = &pass->batches[newbatch.first];
			IndirectBatch* batch = &pass->batches[i];

			bool bSameMat = joinbatch->material.handle == batch->material.handle;
			bool bCompatibleMesh = get_mesh(joinbatch->meshID).isMerged;

			if (!bSameMat || !bCompatibleMesh)
			{
				pass->multibatches.push_back(newbatch);
				newbatch.count = 1;
				newbatch.first = i;
			}
			else {
				newbatch.count++;
			}
		}
		pass->multibatches.push_back(newbatch);
	}
}

RenderObject* RenderScene::get_object(Handle<RenderObject> objectID)
{
	return &renderables[objectID.handle];
}

DrawMesh RenderScene::get_mesh(Handle<DrawMesh> objectID)
{
	return meshes[objectID.handle];
}

OldMaterial* RenderScene::get_material(Handle<OldMaterial> objectID)
{
	return materials[objectID.handle];
}

Handle<OldMaterial> RenderScene::getMaterialHandle(OldMaterial* m)
{
	Handle<OldMaterial> handle;
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

Handle<DrawMesh> RenderScene::getMeshHandle(Mesh* m)
{
	Handle<DrawMesh> handle;
	auto it = meshConvert.find(m);
	if (it == meshConvert.end())
	{
		uint32_t index = meshes.size();

		DrawMesh newMesh;
		newMesh.original = m;
		newMesh.firstIndex = 0;
		newMesh.firstVertex = 0;
		newMesh.vertexCount = m->_vertices.size();
		newMesh.indexCount = m->_indices.size();

		meshes.push_back(newMesh);

		handle.handle = index;
		meshConvert[m] = handle;
	}
	else {
		handle = (*it).second;
	}
	return handle;
}
