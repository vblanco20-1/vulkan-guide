---
layout: default
title: A thin abstraction for descriptor sets
parent: Extra Chapter
nav_order: 33
---

Creating and managing descriptor sets is one of the most painful things about vulkan. Creating an abstraction that simplifies is is really important, and will improve the workflow a lot.

I'm going to show you a simple way to create a thin abstraction over descriptor sets, that makes it easier to handle them.

The end result will look like this.

```cpp
VkDescriptorSet GlobalSet;
vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, _descriptorAllocator)
	.bind_buffer(0, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT )
	.bind_buffer(1, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_FRAGMENT_BIT)
	.build(GlobalSet);

VkDescriptorSet ObjectDataSet;
vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, _descriptorAllocator)
	.bind_buffer(0, &objectBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
	.build(ObjectDataSet);

VkDescriptorSet ImageSet;
vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, _descriptorAllocator)
		.bind_image(0, &imageBufferInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(ImageSet);
```

The implementation of this abstraction will be completely standalone, and you can find its code on the Engine branch of the example code.
* https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/vk_descriptors.h
* https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/vk_descriptors.cpp

The engine uses this abstraction instead of creating descriptors manually.

The abstraction is composed of 3 modular parts.
* DescriptorAllocator: manages allocation of descriptor sets. Will keep creating new descriptor pools once they get filled. Can reset the entire thing and reuse pools.
* DescriptorLayoutCache: caches DescriptorSetLayouts to avoid creating duplicated layouts.
* DescriptorBuilder: Uses the 2 objects above to allocate and write a descriptor set and its layout automatically.

Note that both the layout and the allocator won't be threadsafe, but they could be made so relatively easily. The descriptor builder is fully stateless so it would be threadsafe by default.

We are going to put the 3 classes into `vk_descriptors.h` and `vk_descriptors.cpp`. They won't depend on the rest of the engine, only on vulkan.h and STL.

## Descriptor Allocator
Explained in chapter 4, descriptors have to be allocated from a pool. You need to manage this pool, and create more as needed, also reusing them whenever possible if you allocate descriptors every frame.

We are going to make a basic abstraction where it allocates pools of 1000 descriptors at a time, and when the pool is filled, it will allocate new pools. When the allocator gets reset, it will reset all of its pools, and move them to a list of reusable pools. It will only allocate new pools if there are no reusable pools left. The design of this allocator is a simplified version on the threadsafe one that its on https://github.com/vblanco20-1/Vulkan-Descriptor-Allocator

We will have multiple allocators in the engine. One of them will be for "persistent" descriptors, and one allocator per frame to use for dynamically allocated descriptors.

Let's start with the class itself

```cpp
class DescriptorAllocator {
	public:

		struct PoolSizes {
			std::vector<std::pair<VkDescriptorType,float>> sizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
			};
		};

		void reset_pools();
		bool allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);

		void init(VkDevice newDevice);

		void cleanup();

		VkDevice device;
	private:
		VkDescriptorPool grab_pool();

		VkDescriptorPool currentPool{VK_NULL_HANDLE};
		PoolSizes descriptorSizes;
		std::vector<VkDescriptorPool> usedPools;
		std::vector<VkDescriptorPool> freePools;
	};
```

We will store multipliers of descriptor types in the PoolSizes struct. The idea is that it's a multiplier on the number of descriptor sets allocated for the pools.

For example, if you set `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER` to `4.f` in there, it means that when a pool for 1000 descriptors is allocated, the pool will have space for 4000 combined image descriptors. The numbers in there as default are a reasonable default, but you can improve memory usage of this allocator significantly if you tweak it to what your project uses.

The rest of the functions are simple. We have a `init()` and `cleanup()` function to start and destroy the allocator, and `reset_pools()` and `allocate()` functions.

`reset_pools()` will reset all the DescriptorPools held inside the system, and move them to the `freePools` array, where they can be reused later.
`allocate()` will perform the descriptor set allocator. It returns false if there is a big error.

`usedPools;` will hold the pools that are "active" in the allocator, and have descriptors allocated in them. `freePools` stores completely reset pools for reuse.

We can now start focusing on the implementation of the class. We will begin with `init()` and `cleanup()` functions.
```cpp
void DescriptorAllocator::init(VkDevice newDevice)
{
	device = newDevice;
}

void DescriptorAllocator::cleanup()
{
	//delete every pool held
	for (auto p : freePools)
	{
		vkDestroyDescriptorPool(device, p, nullptr);
	}
	for (auto p : usedPools)
	{
		vkDestroyDescriptorPool(device, p, nullptr);
	}
}
```
On the init we only need to set the device. For the cleanup we are going to iterate both descriptor pool arrays, and destroy every pool.


Lets fill the `createPool()` function, where new pools are allocated.

```cpp
VkDescriptorPool createPool(VkDevice device, const DescriptorAllocator::PoolSizes& poolSizes, int count, VkDescriptorPoolCreateFlags flags)
	{
		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(poolSizes.sizes.size());
		for (auto sz : poolSizes.sizes) {
			sizes.push_back({ sz.first, uint32_t(sz.second * count) });
		}
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = flags;
		pool_info.maxSets = count;
		pool_info.poolSizeCount = (uint32_t)sizes.size();
		pool_info.pPoolSizes = sizes.data();

		VkDescriptorPool descriptorPool;
		vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

		return descriptorPool;
	}
```

The `createPool()` function will convert our multiplier array into the proper `VkDescriptorPoolSize` array, and then use that to allocate the pool.


Lets also fill the `grab_pool()` function, where pools are grabbed as needed.

```cpp
VkDescriptorPool DescriptorAllocator::grab_pool()
{
	//there are reusable pools availible
	if (freePools.size() > 0)
	{
		//grab pool from the back of the vector and remove it from there.
		VkDescriptorPool pool = freePools.back();
		freePools.pop_back();
		return pool;
	}
	else
	{
		//no pools availible, so create a new one
		return createPool(device, descriptorSizes, 1000, 0);
	}
}
```

On the function, we reuse a descriptor pool if it's availible, and if we don't have one, we then create a new pool to hold 1000 descriptors. The 1000 count is arbitrary. Could also create growing pools or different sizes.

With those functions coded, we can now focus on the main `allocate` function, which is where the meat of the allocator is.

```cpp
bool DescriptorAllocator::allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
	{
		//initialize the currentPool handle if it's null
		if (currentPool == VK_NULL_HANDLE){

			currentPool = grab_pool();
			usedPools.push_back(currentPool);
		}

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;

		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorPool = currentPool;
		allocInfo.descriptorSetCount = 1;

		//try to allocate the descriptor set
		VkResult allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);
		bool needReallocate = false;

		switch (allocResult) {
		case VK_SUCCESS:
			//all good, return
			return true;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			//reallocate pool
			needReallocate = true;
			break;
		default:
			//unrecoverable error
			return false;
		}

		if (needReallocate){
			//allocate a new pool and retry
			currentPool = grab_pool();
			usedPools.push_back(currentPool);

			allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);

			//if it still fails then we have big issues
			if (allocResult == VK_SUCCESS){
				return true;
			}
		}

		return false;
	}
```

In the function, we begin by initializing the `currentPool` handle if it's null, and reusing or allocating a new descriptor pool.

After that, we try to allocate the descriptor using `vkAllocateDescriptorSets()`. After that call, we check the error code.
If there is no error `VK_SUCCESS`, everything went fine and we can return from the function.
If the error is either `VK_ERROR_FRAGMENTED_POOL` or `VK_ERROR_OUT_OF_POOL_MEMORY`, we need to allocate a newer pool, so we continue.
If the error is not one of those memory errors, something is really wrong, so we stop the function and return false

On memory error we try to grab another pool, and then reallocate.
If we still have errors on the second reallocation, which is from a cleanly allocated/reused pool, something is really wrong, so we let it continue and return false.

With the main allocation function written, only thing left is to implement the `reset()` function.
```cpp
void DescriptorAllocator::reset_pools(){
	//reset all used pools and add them to the free pools
	for (auto p : usedPools){
		vkResetDescriptorPool(device, p, 0);
		freePools.push_back(p);
	}

	//clear the used pools, since we've put them all in the free pools
	usedPools.clear();

	//reset the current pool handle back to null
	currentPool = VK_NULL_HANDLE;
}
```

On the reset, we call `vkResetDescriptorPool` on every used pool, and then move them to the `freePools` array for reuse. We also set the currentPool handle to null so that the allocation function tries to grab a pool on the next allocation.

This simple allocator won't be the most optimal, but if you use it right by setting the proper size multipliers, it will be optimal.
If you have common descriptor set shapes, it might be a good idea to have an allocator just for those descriptors. For example an allocator only for sets that hold textures.


Next thing is the LayoutCache


## Descriptor Set Layout Cache

Trying to reuse descriptor set layouts through a codebase manually can be very tricky. A better way of doing it is to have a cache, and then when creating a new descriptor set layout, if that specific layout has already been created, we can reuse it from the cache.

The cache will be a simple unordered_map. Its not the most efficient hashmap for this use case, but it will work fine as an example. Consider replacing it with a better hashmap if you use this in a bigger project.

The way the cache is going to work is a very common thing in Vulkan. A lot of objects can be cached in this exact way, for example pipeline layouts, or render passes. This descriptor layout cache acts as a nice example of how to implement this sort of caching.

The class definition will look like this:

```cpp
class DescriptorLayoutCache {
	public:
		void init(VkDevice newDevice);
		void cleanup();

		VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info);

		struct DescriptorLayoutInfo {
			//good idea to turn this into a inlined array
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			bool operator==(const DescriptorLayoutInfo& other) const;

			size_t hash() const;
		};



	private:

		struct DescriptorLayoutHash		{

			std::size_t operator()(const DescriptorLayoutInfo& k) const{
				return k.hash();
			}
		};

		std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
		VkDevice device;
};
```

We will have a similar flow to the allocator, with the `init(device)` and `cleanup()` functions.
The main function is `create_descriptor_layout`, which mimics `vkCreateDescriptorSetLayout` directly, taking a `VkDescriptorSetLayoutCreateInfo` struct.

The cache will work with `DescriptorLayoutCache::DescriptorLayoutInfo` structs, which hold a vector of `VkDescriptorSetLayoutBinding`. This is what we will use in the hashmap, so we need hashing and equality check implementations.

The `std::unordered_map` can receive a custom hash implementation as the 3rd parameter in the template arguments. For that, we need to specifically create a hasher object `DescriptorLayoutHash`. This will be a empty struct that implements `operator()` and just forwards it to `DescriptorLayoutInfo::hash()`

To put an object as the key in a hashmap, we need to make it have equality and hash function, which is why we implement `operator==` and the hasher object.

The idea of this cache is that we will translate the raw `VkDescriptorSetLayoutCreateInfo` struct into our `DescriptorLayoutCache::DescriptorLayoutInfo`, and then look it up in the hashmap to see if there is already a VkDescriptorSetLayout created for that specific layout description.

Lets start with the init and cleanup functions

```cpp
void DescriptorLayoutCache::init(VkDevice newDevice){
	device = newDevice;
}
void DescriptorLayoutCache::cleanup(){
	//delete every descriptor layout held
	for (auto pair : layoutCache){
		vkDestroyDescriptorSetLayout(device, pair.second, nullptr);
	}
}
```

They work in almost the exact same way. For the cleanup function, we go trhough all stored layouts in the hashmap, and destroy them all.

Lets implement the create layout function now.
```cpp
VkDescriptorSetLayout DescriptorLayoutCache::create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info){
	DescriptorLayoutInfo layoutinfo;
	layoutinfo.bindings.reserve(info->bindingCount);
	bool isSorted = true;
	int lastBinding = -1;

	//copy from the direct info struct into our own one
	for (int i = 0; i < info->bindingCount; i++) {
		layoutinfo.bindings.push_back(info->pBindings[i]);

		//check that the bindings are in strict increasing order
		if (info->pBindings[i].binding > lastBinding){
			lastBinding = info->pBindings[i].binding;
		}
		else{
			isSorted = false;
		}
	}
	//sort the bindings if they aren't in order
	if (!isSorted){
		std::sort(layoutinfo.bindings.begin(), layoutinfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b ){
				return a.binding < b.binding;
		});
	}

	//try to grab from cache
	auto it = layoutCache.find(layoutinfo);
	if (it != layoutCache.end()){
		return (*it).second;
	}
	else {
		//create a new one (not found)
		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(device, info, nullptr, &layout);

		//add to cache
		layoutCache[layoutinfo] = layout;
		return layout;
		}
	}
```

The first thing we do in the function is to copy the bindings from `VkDescriptorSetLayoutCreateInfo` into `DescriptorLayoutInfo` vector.
We check if the bindings are in perfect ascending order, and if they aren't, we use std::sort to sort them.
We want the bindings ordered because it makes equality check work much better.

To use `std::sort` we need to include `<algorithm>`. std::sort uses `operator<` by default, but we can override that logic by using a lambda to implement a custom sorting.

With it sorted, we can now try to find it in the hashmap. If we find it, we return the already created layout. If we cant find it, we create a new layout, add it to the cache, and then return it.

The hashmap doesn't work without the operators, so now we implement them.
For equality, we are going to do this
```cpp
bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const{
	if (other.bindings.size() != bindings.size()){
		return false;
	}
	else {
		//compare each of the bindings is the same. Bindings are sorted so they will match
		for (int i = 0; i < bindings.size(); i++) {
			if (other.bindings[i].binding != bindings[i].binding){
				return false;
			}
			if (other.bindings[i].descriptorType != bindings[i].descriptorType){
				return false;
			}
			if (other.bindings[i].descriptorCount != bindings[i].descriptorCount){
				return false;
			}
			if (other.bindings[i].stageFlags != bindings[i].stageFlags){
				return false;
			}
		}
		return true;
	}
}
```

We first compare if the bindings vector is the same size, and if it is, we compare each of the bindings in there. This is why we need to have them sorted, so testing equality doesn't require a more complex loop.

The hash looks like this.

```cpp
size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const{
		using std::size_t;
		using std::hash;

		size_t result = hash<size_t>()(bindings.size());

		for (const VkDescriptorSetLayoutBinding& b : bindings)
		{
			//pack the binding data into a single int64. Not fully correct but it's ok
			size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

			//shuffle the packed binding data and xor it with the main hash
			result ^= hash<size_t>()(binding_hash);
		}

		return result;
	}
```

We will begin the hash by hashing the number of bindings that we have in the layout info. After that, we compress the data of each binding into a size_t, and xor that one with the hash. While the packing we do is not really the best, it doesn't matter that much.

The descriptor cache is now live. You can use similar code to cache almost any other vulkan object. Some recommended ones are pipelines themselves and render passes.

Last thing to implement is the descriptor builder itself.

## Descriptor Builder

The descriptor builder will follow a builder pattern. We will start it with "begin()" call, and then do the other calls to bind objects to it.
When bulding the descriptor set, it will automatically generate the descriptor layout (using the cache), and then allocate and write the descriptor set.

The class declaration looks like this.
```cpp
class DescriptorBuilder {
public:
	static DescriptorBuilder begin(DescriptorLayoutCache* layoutCache, DescriptorAllocator* allocator );

	DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);
	DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

	bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
	bool build(VkDescriptorSet& set);
private:

	std::vector<VkWriteDescriptorSet> writes;
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	DescriptorLayoutCache* cache;
	DescriptorAllocator* alloc;
	};
```

On the `begin()` function we request an allocator and a layout cache. This builder can be used without them, but it works much better when it's all together.

Next we have the bind functions, one for buffers, and another for images. They are very similar.

And last, 2 build functions. One that returns layout, and another that doesn't. You don't always need the layout, so it's nice to not have it in the call.

The data in the class will be the cache and allocator that we store, alongside 2 vectors. One for descriptor writes, and another for layout bindings. We will use those to create the cache and write the descriptor data.

Lets start with the begin call.
```cpp
vkutil::DescriptorBuilder DescriptorBuilder::begin(DescriptorLayoutCache* layoutCache, DescriptorAllocator* allocator){

	DescriptorBuilder builder;

	builder.cache = layoutCache;
	builder.alloc = allocator;
	return builder;
}

```
The begin call is different from the `init()` calls in the other objects. This one creates a new descriptor builder, initializes the parameters, and returns it.

The `bind_buffer()` and `bind_image()` are almost exactly the same. We only need to look at the `bind_buffer()` one to see the logic.

```cpp
	vkutil::DescriptorBuilder& DescriptorBuilder::bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
	{
		//create the descriptor binding for the layout
		VkDescriptorSetLayoutBinding newBinding{};

		newBinding.descriptorCount = 1;
		newBinding.descriptorType = type;
		newBinding.pImmutableSamplers = nullptr;
		newBinding.stageFlags = stageFlags;
		newBinding.binding = binding;

		bindings.push_back(newBinding);

		//create the descriptor write
		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = type;
		newWrite.pBufferInfo = bufferInfo;
		newWrite.dstBinding = binding;

		writes.push_back(newWrite);
		return *this;
	}
```

The `bind_buffer()` function does 2 main things. It creates a new descriptor layout binding from the parameters, and it creates a descriptor write that will write that same binding.
We store both of them in the 2 arrays.

Lets look at the main `build()` function now.

```cpp
bool DescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout){
	//build layout first
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;

	layoutInfo.pBindings = bindings.data();
	layoutInfo.bindingCount = bindings.size();

	layout = cache->create_descriptor_layout(&layoutInfo);

	//allocate descriptor
	bool success = alloc->allocate(&set, layout);
	if (!success) { return false; };

	//write descriptor
	for (VkWriteDescriptorSet& w : writes) {
		w.dstSet = set;
	}

	vkUpdateDescriptorSets(alloc->device, writes.size(), writes.data(), 0, nullptr);

	return true;
}
```
We begin by filling in the layoutInfo and creating the layout. Because we have been storing the bindings in the vector, we just need to hook it to the Info struct. We also use the cache to create the layout, to avoid duplicate creations.

With the layout created, we can use the descriptor allocator to allocate a new descriptor set.

Then we can write into it. We need to go through the array of writes, and make sure that they point to the newly created descriptor set. Once that's done, we can finally call `vkUpdateDescriptorSets` with that array, setting the data in the set.

That's it, now you have a thin abstraction over descriptor sets and their layouts, that makes it much easier to handle them at runtime.
You can look at how the code in the engine uses it.

The `draw_objects()` function in the engine branch uses this abstractions, so you can check how they are used. https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/vk_engine.cpp



{% include comments.html term="Descriptor Abstraction Comments" %}







