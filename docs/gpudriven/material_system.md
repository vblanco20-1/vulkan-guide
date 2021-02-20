---
layout: default
title: Material System
parent: GPU Driven Rendering
nav_order: 8
---

For the engine, we start to have multiple passes of rendering, so ' time to create an actual material system.
Material systems are notoriously hard to architect in rendering engines, due to their need to balance many things (flexibility, ease of use, batching), with many different parts of the engine.

In the engine, the system used is a fairly simple one, but does its job for now. It's not the best solution, but it's a solution that works in this use case.


The material system can be found on the `material_system.h/cpp` pair, but it also depends on the descriptor system and shader reflection system.

It's based on the same core logic as the original material system shown in the main tutorial chapters. A material will contain the pipeline and shaders, alongside a descriptor set for slot 2 which will be used for textures.

## Shader Effect
```cpp
struct ShaderEffect {
    VkPipelineLayout builtLayout;
	std::array<VkDescriptorSetLayout, 4> setLayouts;

    struct ShaderStage {
		ShaderModule* shaderModule;
		VkShaderStageFlagBits stage;
	};

	std::vector<ShaderStage> stages;

    //others omitted
}
```

Implemented in `vk_shaders.h`, a shader effect is a struct that groups a set of shaders that would compose a pipeline, and takes care of handling its descriptor set layouts and pipeline layouts. In a way, a shader effect holds all of the shader related state that a pipeline needs to be built.

To create a shader effect, we fill its shader stages, and then let it build the pipeline layouts and grab the required reflection data that we can then use for other features. 

A given material will have multiple Shader Effects, according to the pipelines where it maps to. The shader effect for a depth-only shadow rendering isnt the same as the shader effect for a forward pass. And a textured material has a different effect than a untextured one.

A Shader Pass is essentially the built version of a Shader Effect, where it stores the built pipeline

```cpp
struct ShaderPass {
		ShaderEffect* effect{ nullptr };
		VkPipeline pipeline{ VK_NULL_HANDLE };
		VkPipelineLayout layout{ VK_NULL_HANDLE };
	};
```

```cpp
//default effects	
ShaderEffect* texturedLit = build_effect(engine,  "tri_mesh_ssbo_instanced.vert.spv" ,"textured_lit.frag.spv" );
ShaderEffect* defaultLit = build_effect(engine, "tri_mesh_ssbo_instanced.vert.spv" , "default_lit.frag.spv" );
ShaderEffect* opaqueShadowcast = build_effect(engine, "tri_mesh_ssbo_instanced_shadowcast.vert.spv","");

//passes
ShaderPass* texturedLitPass = build_shader(engine->_renderPass,forwardBuilder, texturedLit);
ShaderPass* defaultLitPass = build_shader(engine->_renderPass, forwardBuilder, defaultLit);
ShaderPass* opaqueShadowcastPass = build_shader(engine->_shadowPass,shadowBuilder, opaqueShadowcast);
```


## Effect Template
```cpp
struct EffectTemplate {
	PerPassData<ShaderPass*> passShaders;
		
	ShaderParameters* defaultParameters;
	assets::TransparencyMode transparency;
};
```


To gather the pipelines required for multiple passes, we gather them in a Effect Template. This is kind of like the *master* material. Other materials are created from it. For example one effect template is the LitTexturedOpaque template, which is a material template for a material that receives light and has textures, and it also can render on the shadow pass.

In the material system, we begin by creating a few templates that we will then use as base for the invidual materials.

```cpp
	{
		EffectTemplate defaultTextured;
        //no transparent pass
		defaultTextured.passShaders[MeshpassType::Transparency] = nullptr;
        //default opaque shadowpass
		defaultTextured.passShaders[MeshpassType::DirectionalShadow] = opaqueShadowcastPass;
        //textured lit for main view
		defaultTextured.passShaders[MeshpassType::Forward] = texturedLitPass;

		defaultTextured.defaultParameters = nullptr;
		defaultTextured.transparency = assets::TransparencyMode::Opaque;

		templateCache["texturedPBR_opaque"] = defaultTextured;
	}
```

A possible improvement over this is that Effect Templates could be created from file configuration, and that way it would be easier to configure what shaders are used on the different types.

A Effect Template also contains a ShaderParameters struct. This is so materials can have a uniform buffer of data to index from the shader, for things like material color. The ShaderParameters struct in the Effect Template is the default one.

## Material

```cpp
	struct Material {
		EffectTemplate* original;
		PerPassData<VkDescriptorSet> passSets;
		
		std::vector<SampledTexture> textures;

		ShaderParameters* parameters;		
	};
```
Finally, we have the Material itself.
The material will hold a pointer to its Effect Template parent, and will also hold the descriptor sets neccesary for rendering.

A material holds a vector of the textures it uses, as this will be used to build the descriptor sets used in here.


## Material assets
When loading a scene, we need to also load the materials for the different objects in it.
This is an extension of the asset system explained in Extra chapter, and is part of the GLTF loading code.

A material asset is a small asset that contains a embedded json with the material parameters and the base effect to use when building the material (maps to a Effect Template)

```cpp
struct MaterialInfo {
		std::string baseEffect;
		std::unordered_map<std::string, std::string> textures; //name -> path
		std::unordered_map<std::string, std::string> customProperties;
		TransparencyMode transparency;
	};
```

The material info will be loaded when loading a prefab, and the material will be initialized.

## Caching system

To create a material, you fill a MaterialInfo struct, and request the material by name to the material system.

```cpp
struct MaterialData {
		std::vector<SampledTexture> textures;
		ShaderParameters* parameters;
		std::string baseTemplate;
	};
```

```cpp
{
		vkutil::MaterialData texturedInfo;
		texturedInfo.baseTemplate = "texturedPBR_opaque";
		texturedInfo.parameters = nullptr;

		vkutil::SampledTexture whiteTex;
		whiteTex.sampler = smoothSampler;
		whiteTex.view = _loadedTextures["white"].imageView;

		texturedInfo.textures.push_back(whiteTex);

		vkutil::Material* newmat = _materialSystem->build_material("textured", texturedInfo);
	}
```

It's incredibly common with most GLTF and FBX files that you will have the same materials under different names. This is even more common when loading multiple prefabs, where it's likely that some materials are the same. 
To improve this, the material system is very heavily cached.
The `build_material` function is a lie, and will first try to find if there is a material that is the same to what you want to create. It will only create the material and properly build the texture descriptors if it's a unique combination. This way, materials get merged constantly, which makes it much better to use from a draw indirect batching standpoint.

```cpp
vkutil::Material* vkutil::MaterialSystem::build_material(const std::string& materialName, const MaterialData& info)
{
	Material* mat;
	//search material in the cache first in case it's already built
	auto it = materialCache.find(info);
	if (it != materialCache.end())
	{
        //material found, just return it
		mat = (*it).second;
		materials[materialName] = mat;
	}
	else {

		//need to build the material
		Material *newMat = new Material();
		newMat->original = &templateCache[ info.baseTemplate];
		newMat->parameters = info.parameters;
		//not handled yet
		newMat->passSets[MeshpassType::DirectionalShadow] = VK_NULL_HANDLE;
		newMat->textures = info.textures;
	
        //build descriptor set
		auto& db = vkutil::DescriptorBuilder::begin(engine->_descriptorLayoutCache, engine->_descriptorAllocator);

		for (int i = 0; i < info.textures.size(); i++)
		{
			VkDescriptorImageInfo imageBufferInfo;
			imageBufferInfo.sampler = info.textures[i].sampler;
			imageBufferInfo.imageView = info.textures[i].view;
			imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			db.bind_image(i, &imageBufferInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}		
			
		db.build(newMat->passSets[MeshpassType::Forward]);


		LOG_INFO("Built New Material {}", materialName);
		//add material to cache
		materialCache[info] = (newMat);
		mat = newMat;
		materials[materialName] = mat;
	}

	return mat;
}
```

Pipelines are already cached because pipelines are created with the Effect Template, and the amount of unique Effect Templates is very low.

## Rendering
The material system is deeply tied to how the mesh pass works, but the important thing is that the material is "unpacked" when you register a renderable with a given meshpass.
If you have a mesh and register it with a Depth pass, the depth pass will first check if the material has a depth effect, and if it does, it will directly store the final pipeline and descriptor set information.

```cpp
if (object->bDrawShadowPass)
{
	if (object->material->original->passShaders[MeshpassType::DirectionalShadow])
	{
        //add object to shadow pass
		_shadowPass.unbatchedObjects.push_back(handle);
	}
}
```

The only thing that is needed for rendering is the pipeline ID and the descriptor set ID, for that reason the mesh renderer in `vk_scene.cpp` will "unpack" the layers of the material, and eventually only store what is needed. By doing that, the materials that share the same pipeline ID are "merged" together when the system sorts the meshes, and will batch much better together.

Because the shadow pass uses allways the same default opaque-shadow effect, and it has no textures (null descriptor set), the shadow pass in the engine will allways render in a single drawcall. Even if the scene is composed of multiple different materials, when it's time to render, the system will see that pipeline is allways the same, so it will only be one drawcall.





{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}