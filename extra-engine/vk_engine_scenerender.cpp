#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_types.h>
#include <vk_initializers.h>
#include <vk_descriptors.h>

#include "vk_textures.h"
#include "vk_shaders.h"

#include "Tracy.hpp"
#include "TracyVulkan.hpp"
#include "vk_profiler.h"


glm::vec4 normalizePlane(glm::vec4 p)
{
	return p / glm::length(glm::vec3(p));
}

void VulkanEngine::execute_compute_cull(VkCommandBuffer cmd, RenderScene::MeshPass& pass,CullParams& params )
{
	if (_config.freezeCulling) return;
	
	if (pass.batches.size() == 0) return;
	TracyVkZone(_graphicsQueueContext, cmd, "Cull Dispatch");
	VkDescriptorBufferInfo objectBufferInfo = _renderScene.objectDataBuffer.get_info();

	VkDescriptorBufferInfo dynamicInfo = get_current_frame().dynamicDataBuffer.get_info();
	dynamicInfo.range = sizeof(GPUCameraData);

	VkDescriptorBufferInfo instanceInfo = pass.instanceBuffer.get_info();

	VkDescriptorBufferInfo finalInfo = pass.compactedInstanceBuffer.get_info();

	VkDescriptorBufferInfo indirectInfo = pass.drawIndirectBuffer.get_info();

	VkDescriptorImageInfo depthPyramid;
	depthPyramid.sampler = _depthSampler;
	depthPyramid.imageView = _depthPyramid._defaultView;
	depthPyramid.imageLayout = VK_IMAGE_LAYOUT_GENERAL;


	VkDescriptorSet COMPObjectDataSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &objectBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.bind_buffer(1, &indirectInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.bind_buffer(2, &instanceInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.bind_buffer(3, &finalInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.bind_image(4, &depthPyramid, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
		.bind_buffer(5, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.build(COMPObjectDataSet);


	glm::mat4 projection = params.projmat;
	glm::mat4 projectionT = transpose(projection);

	glm::vec4 frustumX = normalizePlane(projectionT[3] + projectionT[0]); // x + w < 0
	glm::vec4 frustumY = normalizePlane(projectionT[3] + projectionT[1]); // y + w < 0

	DrawCullData cullData = {};
	cullData.P00 = projection[0][0];
	cullData.P11 = projection[1][1];
	cullData.znear = 0.1f;
	cullData.zfar = params.drawDist;
	cullData.frustum[0] = frustumX.x;
	cullData.frustum[1] = frustumX.z;
	cullData.frustum[2] = frustumY.y;
	cullData.frustum[3] = frustumY.z;
	cullData.drawCount = pass.flat_batches.size();
	cullData.cullingEnabled = params.frustrumCull;
	cullData.lodEnabled = false;
	cullData.occlusionEnabled = params.occlusionCull;
	cullData.lodBase = 10.f;
	cullData.lodStep = 1.5f;
	cullData.pyramidWidth = depthPyramidWidth;
	cullData.pyramidHeight = depthPyramidHeight;
	cullData.viewMat = params.viewmat;//get_view_matrix();

	cullData.AABBcheck = params.aabb;
	cullData.aabbmin_x = params.aabbmin.x;
	cullData.aabbmin_y = params.aabbmin.y;
	cullData.aabbmin_z = params.aabbmin.z;

	cullData.aabbmax_x = params.aabbmax.x;
	cullData.aabbmax_y = params.aabbmax.y;
	cullData.aabbmax_z = params.aabbmax.z;

	if (params.drawDist > 10000)
	{cullData.distanceCheck = false; }
	else{
		cullData.distanceCheck = true;
	}

	//copy from the cleared indirect buffer into the one we will use on rendering. This one happens every frame
	VkBufferCopy indirectCopy;
	indirectCopy.dstOffset = 0;
	indirectCopy.size = pass.batches.size() * sizeof(GPUIndirectObject);
	indirectCopy.srcOffset = 0;
	vkCmdCopyBuffer(cmd, pass.clearIndirectBuffer._buffer, pass.drawIndirectBuffer._buffer, 1, &indirectCopy);

	{
		VkBufferMemoryBarrier barrier = vkinit::buffer_barrier(pass.drawIndirectBuffer._buffer, _graphicsQueueFamily);
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		uploadBarriers.push_back(barrier);
		

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);
	}

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _cullPipeline);

	vkCmdPushConstants(cmd, _cullLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(DrawCullData), &cullData);

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _cullLayout, 0, 1, &COMPObjectDataSet, 0, nullptr);

	
	vkCmdDispatch(cmd, (pass.flat_batches.size() / 256)+1, 1, 1);


	//barrier the 2 buffers we just wrote for culling, the indirect draw one, and the instances one, so that they can be read well when rendering the pass
	{
		VkBufferMemoryBarrier barrier = vkinit::buffer_barrier(pass.compactedInstanceBuffer._buffer, _graphicsQueueFamily);
		barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		


		VkBufferMemoryBarrier barrier2 = vkinit::buffer_barrier(pass.drawIndirectBuffer._buffer, _graphicsQueueFamily);	
		barrier2.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		barrier2.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		


		VkBufferMemoryBarrier barriers[] = { barrier,barrier2 };

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, 0, 0, nullptr, 2, barriers, 0, nullptr);
	}
	if (_config.outputIndirectBufferToFile)
	{
		uint32_t offset = get_current_frame().debugDataOffsets.back();
		VkBufferCopy debugCopy;
		debugCopy.dstOffset = offset;
		debugCopy.size = pass.batches.size() * sizeof(GPUIndirectObject);
		debugCopy.srcOffset = 0;
		vkCmdCopyBuffer(cmd, pass.drawIndirectBuffer._buffer, get_current_frame().debugOutputBuffer._buffer, 1, &debugCopy);
		get_current_frame().debugDataOffsets.push_back(offset + debugCopy.size);
		get_current_frame().debugDataNames.push_back("Cull Indirect Output");
	}
}

void VulkanEngine::ready_mesh_draw(VkCommandBuffer cmd)
{
	
	TracyVkZone(_graphicsQueueContext, get_current_frame()._mainCommandBuffer, "Data Refresh");
	ZoneScopedNC("Draw Upload", tracy::Color::Blue);

	//upload object data to gpu
	
	if (_renderScene.dirtyObjects.size() > 0)
	{
		ZoneScopedNC("Refresh Object Buffer", tracy::Color::Red);

		//if 80% of the objects are dirty, then just reupload the whole thing
		if (_renderScene.dirtyObjects.size() >= _renderScene.renderables.size() * 0.8)
		{
			AllocatedBuffer<GPUObjectData> newBuffer = create_buffer(sizeof(GPUObjectData) * _renderScene.renderables.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

			GPUObjectData* objectSSBO = map_buffer(newBuffer);
			_renderScene.fill_objectData(objectSSBO);
			unmap_buffer(newBuffer);

			get_current_frame()._frameDeletionQueue.push_function([=]() {

				vmaDestroyBuffer(_allocator, newBuffer._buffer, newBuffer._allocation);
				});

			//copy from the uploaded cpu side instance buffer to the gpu one
			VkBufferCopy indirectCopy;
			indirectCopy.dstOffset = 0;
			indirectCopy.size = _renderScene.renderables.size() * sizeof(GPUObjectData);
			indirectCopy.srcOffset = 0;
			vkCmdCopyBuffer(cmd, newBuffer._buffer, _renderScene.objectDataBuffer._buffer, 1, &indirectCopy);
		}
		else {
			//update only the changed elements

			std::vector<VkBufferCopy> copies;
			copies.reserve(_renderScene.dirtyObjects.size());

			uint32_t buffersize = sizeof(GPUObjectData) * _renderScene.dirtyObjects.size();
			uint32_t vec4size = sizeof(glm::vec4);
			uint32_t intsize = sizeof(uint32_t);
			uint32_t wordsize = sizeof(GPUObjectData) / sizeof(uint32_t);
			uint32_t uploadSize = _renderScene.dirtyObjects.size() * wordsize * intsize;
			AllocatedBuffer<GPUObjectData> newBuffer = create_buffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
			AllocatedBuffer<uint32_t> targetBuffer = create_buffer(uploadSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

			get_current_frame()._frameDeletionQueue.push_function([=]() {

				vmaDestroyBuffer(_allocator, newBuffer._buffer, newBuffer._allocation);
				vmaDestroyBuffer(_allocator, targetBuffer._buffer, targetBuffer._allocation);
				});

			uint32_t* targetData = map_buffer(targetBuffer);
			GPUObjectData* objectSSBO = map_buffer(newBuffer);
			uint32_t launchcount = _renderScene.dirtyObjects.size() * wordsize;
			{
				ZoneScopedNC("Write dirty objects", tracy::Color::Red);
				uint32_t sidx = 0;
				for (int i = 0; i < _renderScene.dirtyObjects.size(); i++)
				{
					_renderScene.write_object(objectSSBO + i, _renderScene.dirtyObjects[i]);


					uint32_t dstOffset = wordsize * _renderScene.dirtyObjects[i].handle;

					for (int b = 0; b < wordsize; b++ )
					{
						uint32_t tidx = dstOffset + b;
						targetData[sidx] = tidx;
						sidx++;
					}
				}
				launchcount = sidx;
			}
			unmap_buffer(newBuffer);
			unmap_buffer(targetBuffer); 
			
			VkDescriptorBufferInfo indexData = targetBuffer.get_info();

			VkDescriptorBufferInfo sourceData = newBuffer.get_info(); 

			VkDescriptorBufferInfo targetInfo = _renderScene.objectDataBuffer.get_info();

			VkDescriptorSet COMPObjectDataSet;
			vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
				.bind_buffer(0, &indexData, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(1, &sourceData, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(2, &targetInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build(COMPObjectDataSet);

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _sparseUploadPipeline);

			
			vkCmdPushConstants(cmd, _sparseUploadLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uint32_t), &launchcount);

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _sparseUploadLayout, 0, 1, &COMPObjectDataSet, 0, nullptr);

			vkCmdDispatch(cmd, ((launchcount) / 256) + 1, 1, 1);
		}

		VkBufferMemoryBarrier barrier = vkinit::buffer_barrier(_renderScene.objectDataBuffer._buffer, _graphicsQueueFamily);
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			
		uploadBarriers.push_back(barrier);
		_renderScene.clear_dirty_objects();
	}

	RenderScene::MeshPass* passes[2] = { &_renderScene._forwardPass,&_renderScene._shadowPass };
	for (int p = 0; p < 2; p++)
	{
		auto& pass = *passes[p];

		//if the pass has changed the batches, need to reupload them
		if (pass.needsIndirectRefresh && pass.batches.size() > 0)
		{
			ZoneScopedNC("Refresh Indirect Buffer", tracy::Color::Red);

			AllocatedBuffer<GPUIndirectObject> newBuffer = create_buffer(sizeof(GPUIndirectObject) * pass.batches.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

			GPUIndirectObject* indirect = map_buffer(newBuffer);
			_renderScene.fill_indirectArray(indirect,pass);
			unmap_buffer(newBuffer);

			if (pass.clearIndirectBuffer._buffer != VK_NULL_HANDLE)
			{
				AllocatedBufferUntyped deletionBuffer = pass.clearIndirectBuffer;
				//add buffer to deletion queue of this frame
				get_current_frame()._frameDeletionQueue.push_function([=]() {

					vmaDestroyBuffer(_allocator, deletionBuffer._buffer, deletionBuffer._allocation);
					});
			}

			pass.clearIndirectBuffer = newBuffer;
			pass.needsIndirectRefresh = false;
		}


		if (pass.needsInstanceRefresh && pass.flat_batches.size() >0)
		{
			ZoneScopedNC("Refresh Instancing Buffer", tracy::Color::Red);

			AllocatedBuffer<GPUInstance> newBuffer = create_buffer(sizeof(GPUInstance) * pass.flat_batches.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

			GPUInstance* instanceData = map_buffer(newBuffer);
			_renderScene.fill_instancesArray(instanceData, pass);
			unmap_buffer(newBuffer);

			get_current_frame()._frameDeletionQueue.push_function([=]() {

				vmaDestroyBuffer(_allocator, newBuffer._buffer, newBuffer._allocation);
				});

			//copy from the uploaded cpu side instance buffer to the gpu one
			VkBufferCopy indirectCopy;
			indirectCopy.dstOffset = 0;
			indirectCopy.size = pass.flat_batches.size() * sizeof(GPUInstance);
			indirectCopy.srcOffset = 0;
			vkCmdCopyBuffer(cmd, newBuffer._buffer, pass.instanceBuffer._buffer, 1, &indirectCopy);

			VkBufferMemoryBarrier barrier = vkinit::buffer_barrier(pass.instanceBuffer._buffer, _graphicsQueueFamily);
			barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			uploadBarriers.push_back(barrier);

			pass.needsInstanceRefresh = false;
		}
	}

	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, uploadBarriers.size(), uploadBarriers.data(), 0, nullptr);//1, &readBarrier);
	uploadBarriers.clear();
}

void VulkanEngine::draw_objects_forward(VkCommandBuffer cmd, RenderScene::MeshPass& pass)
{
	ZoneScopedNC("DrawObjects", tracy::Color::Blue);
	//make a model view matrix for rendering the object
	//camera view
	glm::mat4 view = get_view_matrix();
	//camera projection
	glm::mat4 projection = get_projection_matrix();


	GPUCameraData camData;
	camData.proj = projection;
	camData.view = view;
	camData.viewproj = projection * view;

	_sceneParameters.sunlightShadowMatrix =_mainLight.get_projection() * _mainLight.get_view();


	float framed = (_frameNumber / 120.f);
	_sceneParameters.ambientColor = glm::vec4{ 0.5 };
	_sceneParameters.sunlightColor = glm::vec4{ 1.f };
	_sceneParameters.sunlightDirection = glm::vec4(_mainLight.lightDirection * 1.f,1.f);

	//push data to dynmem
	uint32_t camera_data_offsets[3];
	uint32_t scene_data_offset;

	uint32_t dyn_offset = 0;

	char* dynData;
	vmaMapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation, (void**)&dynData);

	camera_data_offsets[0] = dyn_offset;
	memcpy(dynData, &camData, sizeof(GPUCameraData));
	dyn_offset += sizeof(GPUCameraData);
	dyn_offset = pad_uniform_buffer_size(dyn_offset);

	dynData += dyn_offset;

	scene_data_offset = dyn_offset;
	memcpy(dynData, &_sceneParameters, sizeof(GPUSceneData));

	vmaUnmapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation);

	VkDescriptorBufferInfo objectBufferInfo = _renderScene.objectDataBuffer.get_info();

	VkDescriptorBufferInfo dynamicInfo = get_current_frame().dynamicDataBuffer.get_info();
	dynamicInfo.range = sizeof(GPUSceneData);

	VkDescriptorBufferInfo instanceInfo = pass.compactedInstanceBuffer.get_info();


	VkDescriptorImageInfo shadowImage;
	shadowImage.sampler = _shadowSampler;

	shadowImage.imageView = _shadowImage._defaultView;
	shadowImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkDescriptorSet GlobalSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT)
		.bind_buffer(1, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT| VK_SHADER_STAGE_FRAGMENT_BIT)
		.bind_image(2, &shadowImage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(GlobalSet);

	VkDescriptorSet ObjectDataSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &objectBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.bind_buffer(1, &instanceInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build(ObjectDataSet);
	vkCmdSetDepthBias(cmd, 0, 0, 0);
	execute_draw_commands(cmd, pass, ObjectDataSet, camera_data_offsets, scene_data_offset, GlobalSet);
}


void VulkanEngine::execute_draw_commands(VkCommandBuffer cmd, RenderScene::MeshPass& pass, VkDescriptorSet ObjectDataSet, uint32_t* camera_data_offsets, uint32_t scene_data_offset, VkDescriptorSet GlobalSet)
{
	{
		ZoneScopedNC("Draw Commit", tracy::Color::Blue4);
		Mesh* lastMesh = nullptr;
		VkPipeline lastPipeline{ VK_NULL_HANDLE };
		VkPipelineLayout lastLayout{ VK_NULL_HANDLE };
		VkDescriptorSet lastMaterialSet{ VK_NULL_HANDLE };

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmd, 0, 1, &_renderScene.mergedVertexBuffer._buffer, &offset);

		vkCmdBindIndexBuffer(cmd, _renderScene.mergedIndexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);

		stats.objects = pass.flat_batches.size();
		for (int i = 0; i < pass.multibatches.size(); i++)
		{
			auto& multibatch = pass.multibatches[i];
			auto& instanceDraw = pass.batches[multibatch.first];

			VkPipeline newPipeline = instanceDraw.material.shaderPass->pipeline;
			VkPipelineLayout newLayout = instanceDraw.material.shaderPass->layout;
			VkDescriptorSet newMaterialSet = instanceDraw.material.materialSet;

			Mesh* drawMesh = _renderScene.get_mesh(instanceDraw.meshID).original;

			if (newPipeline != lastPipeline)
			{
				lastPipeline = newPipeline;
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, newLayout, 1, 1, &ObjectDataSet, 0, nullptr);

				//update dynamic binds
				uint32_t dynamicBinds[] = { camera_data_offsets[0],scene_data_offset };
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, newLayout, 0, 1, &GlobalSet, 2, dynamicBinds);
			}
			if (newMaterialSet != lastMaterialSet)
			{
				lastMaterialSet = newMaterialSet;
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, newLayout, 2, 1, &newMaterialSet, 0, nullptr);
			}

			bool merged = _renderScene.get_mesh(instanceDraw.meshID).isMerged;
			if (merged)
			{
				if (lastMesh != nullptr)
				{
					VkDeviceSize offset = 0;
					vkCmdBindVertexBuffers(cmd, 0, 1, &_renderScene.mergedVertexBuffer._buffer, &offset);

					vkCmdBindIndexBuffer(cmd, _renderScene.mergedIndexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);
					lastMesh = nullptr;
				}
			}
			else if (lastMesh != drawMesh) {

				//bind the mesh vertex buffer with offset 0
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmd, 0, 1, &drawMesh->_vertexBuffer._buffer, &offset);

				if (drawMesh->_indexBuffer._buffer != VK_NULL_HANDLE) {
					vkCmdBindIndexBuffer(cmd, drawMesh->_indexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);
				}
				lastMesh = drawMesh;
			}

			bool bHasIndices = drawMesh->_indices.size() > 0;
			if (!bHasIndices) {
				stats.draws++;
				stats.triangles += (drawMesh->_vertices.size() / 3) * instanceDraw.count;
				vkCmdDraw(cmd, drawMesh->_vertices.size(), instanceDraw.count, 0, instanceDraw.first);
			}
			else {
				stats.triangles += (drawMesh->_indices.size() / 3) * instanceDraw.count;

				vkCmdDrawIndexedIndirect(cmd, pass.drawIndirectBuffer._buffer, multibatch.first * sizeof(GPUIndirectObject), multibatch.count, sizeof(GPUIndirectObject));

				stats.draws++;
				stats.drawcalls += instanceDraw.count;
			}
		}
	}
}
void VulkanEngine::draw_objects_shadow(VkCommandBuffer cmd, RenderScene::MeshPass& pass)
{
	ZoneScopedNC("DrawObjects", tracy::Color::Blue);
	
	glm::mat4 view = _mainLight.get_view();

	glm::mat4 projection = _mainLight.get_projection();

	GPUCameraData camData;
	camData.proj = projection;
	camData.view = view;
	camData.viewproj = projection * view;
	
	//push data to dynmem
	uint32_t camera_data_offsets[3];
	uint32_t scene_data_offset;

	uint32_t dyn_offset = 2048;

	char* dynData;
	vmaMapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation, (void**)&dynData);
	dynData += dyn_offset;

	camera_data_offsets[0] = dyn_offset;
	memcpy(dynData, &camData, sizeof(GPUCameraData));
	dyn_offset += sizeof(GPUCameraData);
	dyn_offset = pad_uniform_buffer_size(dyn_offset);

	dynData += dyn_offset;

	scene_data_offset = dyn_offset;
	memcpy(dynData, &_sceneParameters, sizeof(GPUSceneData));

	vmaUnmapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation);

	VkDescriptorBufferInfo objectBufferInfo = _renderScene.objectDataBuffer.get_info();

	VkDescriptorBufferInfo dynamicInfo = get_current_frame().dynamicDataBuffer.get_info();
	dynamicInfo.range = sizeof(GPUSceneData);

	VkDescriptorBufferInfo instanceInfo = pass.compactedInstanceBuffer.get_info();


	VkDescriptorSet GlobalSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT)
		.build(GlobalSet);

	VkDescriptorSet ObjectDataSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &objectBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.bind_buffer(1, &instanceInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build(ObjectDataSet);

	vkCmdSetDepthBias(cmd, _config.shadowBias, 0, _config.shadowBiasslope);
	execute_draw_commands(cmd, pass, ObjectDataSet, camera_data_offsets, scene_data_offset, GlobalSet);
}



struct alignas(16) DepthReduceData
{
	glm::vec2 imageSize;
};
inline uint32_t getGroupCount(uint32_t threadCount, uint32_t localSize)
{
	return (threadCount + localSize - 1) / localSize;
}

void VulkanEngine::reduce_depth(VkCommandBuffer cmd)
{
	vkutil::VulkanScopeTimer timer(cmd, _profiler, "Depth Reduce");

	VkImageMemoryBarrier depthReadBarriers[] =
	{
		vkinit::image_barrier(_depthImage._image, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT),
	};

	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, depthReadBarriers);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _depthReducePipeline);

	for (uint32_t i = 0; i < depthPyramidLevels; ++i)
	{
		VkDescriptorImageInfo destTarget;
		destTarget.sampler = _depthSampler;
		destTarget.imageView = depthPyramidMips[i];
		destTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkDescriptorImageInfo sourceTarget;
		sourceTarget.sampler = _depthSampler;
		if (i == 0)
		{
			sourceTarget.imageView = _depthImage._defaultView;
			sourceTarget.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		else {
			sourceTarget.imageView = depthPyramidMips[i - 1];
			sourceTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		}

		VkDescriptorSet depthSet;
		vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
			.bind_image(0, &destTarget, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
			.bind_image(1, &sourceTarget, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build(depthSet);

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _depthReduceLayout, 0, 1, &depthSet, 0, nullptr);


		uint32_t levelWidth = depthPyramidWidth >> i;
		uint32_t levelHeight = depthPyramidHeight >> i;
		if (levelHeight < 1) levelHeight = 1;
		if (levelWidth < 1) levelWidth = 1;

		DepthReduceData reduceData = { glm::vec2(levelWidth, levelHeight) };

		vkCmdPushConstants(cmd, _depthReduceLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(reduceData), &reduceData);
		vkCmdDispatch(cmd, getGroupCount(levelWidth, 32), getGroupCount(levelHeight, 32), 1);


		VkImageMemoryBarrier reduceBarrier = vkinit::image_barrier(_depthPyramid._image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &reduceBarrier);
	}

	VkImageMemoryBarrier depthWriteBarrier = vkinit::image_barrier(_depthImage._image, VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &depthWriteBarrier);

}