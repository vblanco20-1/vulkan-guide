---
layout: default
title: Compute based Culling
parent: GPU Driven Rendering
nav_order: 12
---

## Cull Compute Core

With draw indirect explained and the entire engine explained, the last part that makes everything work is the compute-based culling.
The entire thing is contained in `indirect_cull.comp` . 

As explained in the last article, we use the culling shader to build the final list for rendering. The shader looks like this (simplified).

```glsl
void main() 
{		
	uint gID = gl_GlobalInvocationID.x;
	if(gID < cullData.drawCount)
	{
		//grab object ID from the buffer
		uint objectID = instanceBuffer.Instances[gID].objectID;		
		
		//check if object is visible
		bool visible  = IsVisible(objectID);		
		
		if(visible)
		{
			//get the index of the draw to insert into
			uint batchIndex = instanceBuffer.Instances[gID].batchID;

			//atomic-add to +1 on the number of instances of that draw command
			uint countIndex = atomicAdd(drawBuffer.Draws[batchIndex].instanceCount,1);

			//write the object ID into the instance buffer that maps from gl_instanceID into ObjectID
			uint instanceIndex = drawBuffer.Draws[batchIndex].firstInstance + countIndex;
			finalInstanceBuffer.IDs[instanceIndex] = objectID;
		}
	}
}
```

The `instanceBuffer` is `AllocatedBuffer<GPUInstance> instanceBuffer;` from the last article. It stores ObjectID + BatchID (draw indirect ID)

From ObjectID we can calculate if said object ID is visible or not. Once we have decided that the object is visible, we need to add it to the draw indirect command for that object.

At the start of the compute shader, the drawBuffer has all the draw data for vertex count and instance count, but instanceCount is set to 0. The compute shader does an atomic +1 to it, and uses it to "reserve" a slot, which it then stores in the `finalInstanceBuffer`. finalInstanceBuffer is then used in the vertex shaders to access ObjectID.

This is a way to sidestep the fact that we arent using DrawIndirectCount.
If we were using DrawIndirectCount, another possibility is that each object has its own draw indirect command, and then the commands array is compacted with the surviving objects. There are quite a few ways to structure this part, the best to use depends on what you are doing in the engine and your target hardware.




## Frustum Culling function

With the culling core shown, now we need to actually decide what to do with the cull functions themselves.
The cull here is based on the one shown by Arseny in his open source Niagara stream, you can find the original [here](https://github.com/zeux/niagara). The one in the engine is a small tweak to that.

The first thing we are going to do is frustrum culling.

```glsl
bool IsVisible(uint objectIndex)
{
	//grab sphere cull data from the object buffer
	vec4 sphereBounds = objectBuffer.objects[objectIndex].spherebounds;

	vec3 center = sphereBounds.xyz;
	center = (cullData.view * vec4(center,1.f)).xyz;
	float radius = sphereBounds.w;
	
	bool visible = true;

	//frustrum culling
	visible = visible && center.z * cullData.frustum[1] - abs(center.x) * cullData.frustum[0] > -radius;
	visible = visible && center.z * cullData.frustum[3] - abs(center.y) * cullData.frustum[2] > -radius;

	if(cullData.distCull != 0)
	{// the near/far plane culling uses camera space Z directly
		visible = visible && center.z + radius > cullData.znear && center.z - radius < cullData.zfar;
	}	

	visible = visible || cullData.cullingEnabled == 0;

	return visible;
}
```

For all of the `cullData`, that's written from the Cpp when calling the compute shader. It holds the frustrum data and the configuration for the culling.

We begin by grabbing the object sphereBounds from the objectIndex. The spherebounds are calculated every time the object moves, or is initialized. 

Once we have a sphere, we translate it into view space, and then check it against the frustrum. 
If the checks pass, then all is good, and we can return visible to use when writing the draw indirect commands.

Frustrum culling will easily cut half the objects, but we can go much further. 

## Occlusion Culling
We want to avoid rendering objects that won't be visible at all due to them being behind other objects. To do that, we are going to implement occlusion culling using the depth buffer from the last frame. This is a very common technique with the downside of having 1 frame of latency. Some engines instead render a few bigger objects, and then use that depth buffer to do culling.

The normal depth buffer is too detailed for doing efficient culling, so we need to convert it into a depth pyramid.
The idea of a depth pyramid is that we build a mipmap chain for the depth buffer in a way that the depth values are allways the maximum depth of that region. That way, we can look up directly into a mipmap so that pixel size is similar to object size, and it gives us a fairly accurate approximation.

The first thing to do is that we need to store the depth image after the main renderpass in the same way as we do with shadow map, and then we copy it into the depth pyramid.

```cpp
//forward pass renders depth into this
AllocatedImage _depthImage;
//pyramid depth used for culling
AllocatedImage _depthPyramid;

//special cull sampler
VkSampler _depthSampler;
//image view for each mipmap of the depth pyramid
VkImageView depthPyramidMips[16] = {};
```

To build the depth pyramid, we will use a compute shader to copy from each mipmap into the next, doing reduction.

```cpp
for (int32_t i = 0; i < depthPyramidLevels; ++i)
	{
		VkDescriptorImageInfo destTarget;
		destTarget.sampler = _depthSampler;
		destTarget.imageView = depthPyramidMips[i];
		destTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkDescriptorImageInfo sourceTarget;
		sourceTarget.sampler = _depthSampler;

		//for te first iteration, we grab it from the depth image
		if (i == 0)
		{
			sourceTarget.imageView = _depthImage._defaultView;
			sourceTarget.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		//afterwards, we copy from a depth mipmap into the next
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

		//execute downsample compute shader
		vkCmdPushConstants(cmd, _depthReduceLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(reduceData), &reduceData);
		vkCmdDispatch(cmd, getGroupCount(levelWidth, 32), getGroupCount(levelHeight, 32), 1);


		//pipeline barrier before doing the next mipmap
		VkImageMemoryBarrier reduceBarrier = vkinit::image_barrier(_depthPyramid._image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &reduceBarrier);
	}
```

The compute shader looks like this.

```glsl
void main()
{
	uvec2 pos = gl_GlobalInvocationID.xy;

	// Sampler is set up to do min reduction, so this computes the minimum depth of a 2x2 texel quad
	float depth = texture(inImage, (vec2(pos) + vec2(0.5)) / imageSize).x;

	imageStore(outImage, ivec2(pos), vec4(depth));
}
```

The real trick on it is the sampler for the texture. In here, we are using a commonly found extension that will calculate the minimum of a 2x2 texel quad, instead of averaging the values like a LINEAR mipmap does.
The sampler will also be used in the cull shader, and it's created like this.

```cpp
	VkSamplerCreateInfo createInfo = {};

	//fill the normal stuff
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = VK_FILTER_LINEAR;
	createInfo.minFilter = VK_FILTER_LINEAR;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	createInfo.minLod = 0;
	createInfo.maxLod = 16.f;

	//add a extension struct to enable Min mode
	VkSamplerReductionModeCreateInfoEXT createInfoReduction = {};

	createInfoReduction.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT
	createInfoReduction.reductionMode = VK_SAMPLER_REDUCTION_MODE_MIN;
	createInfo.pNext = &createInfoReduction;	

	
	VK_CHECK(vkCreateSampler(_device, &createInfo, 0, &_depthSampler));
```

This is a extension that will have to be enabled, but its supported everywhere, even on switch. In vulkan 1.2, it's a default feature. [Spec](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_EXT_sampler_filter_minmax.html)


With the depth pyramid written, we can finally use it in the cull shader.

```glsl
	//frustum stuff from before

	visible = visible || cullData.cullingEnabled == 0;

	//flip Y because we access depth texture that way
	center.y *= -1;

	if(visible && cullData.occlusionEnabled != 0)
	{
		//project the cull sphere into screenspace coordinates
		vec4 aabb;
		if (projectSphere(center, radius, cullData.znear, cullData.P00, cullData.P11, aabb))
		{	
			float width = (aabb.z - aabb.x) * cullData.pyramidWidth;
			float height = (aabb.w - aabb.y) * cullData.pyramidHeight;

			//find the mipmap level that will match the screen size of the sphere
			float level = floor(log2(max(width, height)));

			//sample the depth pyramid at that specific level
			float depth = textureLod(depthPyramid, (aabb.xy + aabb.zw) * 0.5, level).x;

			float depthSphere =cullData.znear / (center.z - radius);

			//if the depth of the sphere is in front of the depth pyramid value, then the object is visible
			visible = visible && depthSphere >= depth;
		}
	}
```

We are finding the AABB that covers the sphere in screen space, and then accessing the depth pyramid at that point, in the mipmap where the size of the AABB is similar to a pixel. 

This depth pyramid logic is very similar if not almost exactly the same as the cull system used in unreal engine. In there, they don't have draw indirect, so instead they do the cull in a shader, and output into an array of visible objects. This array is then read from the CPU to know what objects are visible or not.

With this last piece of the puzzle, the engine can now render huge scenes at really high perf, because it will only render whatever is visible on the screen, but without a roundtrip to the CPU. 

There will be a single frame of latency on the depth pyramid, but it can be worked around it by making the culling spheres a bit bigger to account for the disconnect, like unreal engine does, where they have 3-4 frames of latency on the culling. Another possible implementation is that the depth from last frame is reprojected and combined with some very big objects from the new frame. Assassins Creed talks about that in their presentation, and Dragon Age inquisition does something similar.

## Culling and Transparency sorting.

Sadly, doing culling and sorting at the same time is a really hard problem, so in the engine we do no sorting at all for transparent objects.
The culling uses GPU atomics, whose order depends on how the threads are executed in the GPU hardware. This means it has no stability at all for sorting, and the final layout of the rendering will be different.

Even then, there are ways of working around it.
If instead of doing the draw indirect using instancing we have 1 draw command per object, and set its instance count to 0 if culled, we can keep the order. But if we do that, we will have 0 sized draws which will still cost performance in the engine, so it's not that good of a solution.

Another possibility is to sort in the gpu itself, but gpu sorting is a nontrivial operation, so we arent doing it in the tutorial due to it being off scope.

The last possibility is that we could have order-independent transparency. This would mean that our transparent objects do not need any sorting at all, at the cost of a significantly more expensive rendering operation.