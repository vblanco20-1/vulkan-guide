---
layout: default
title: Setting up Materials
parent: "New 4. Textures and Engine Architecture"
nav_order: 10
---


Lets begin with setting up the structures we need to build MaterialData and the GLTF shaders we use.

```cpp
enum class MaterialPass :uint8_t {
    MainColor,
    Transparent,
    Other
};
struct MaterialPipeline {
	VkPipeline pipeline;
	VkPipelineLayout layout;
};
struct MaterialData {
    MaterialPipeline* pipeline;
    VkDescriptorSet materialSet;
    MaterialPass passType;
};
```

This is the structs we need for the material data. MaterialData will hold a raw pointer (non owning) into its MaterialPipeline which contains the real pipeline. It holds a descriptor set too.

For creating those objects, we are going to wrap the logic into a struct, as VulkanEngine is getting too big, and we will want to have multiple materials later.

```cpp
struct GLTFMetallic_Roughness {
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout;

    struct MaterialConstants {
		glm::vec4 colorFactors;
		glm::vec4 metal_rough_factors;
        //padding, we need it for uniform buffers
		glm::vec4 extra[14];
    };

    struct MaterialResources {
        AllocatedImage colorImage; 
        VkSampler colorSampler;
        AllocatedImage metalRoughImage;
        VkSampler metalRoughSampler;
        VkBuffer dataBuffer; 
        uint32_t dataBufferOffset;
    };

    DescriptorWriter writer;

    void build_pipelines(VulkanEngine* engine);
    void clear_resources(VkDevice device);

    MaterialData write_material(VkDevice device,MaterialPass pass,const MaterialResources& resources , DescriptorAllocatorGrowable& descriptorAllocator);
};
```

We will hold the 2 pipelines we will be using for now, one for transparent draws, and other for opaque (and alpha-masked). And the descriptor set layout for the material.

We have a struct for the material constants, which will be written into uniform buffers later. The parameters we want for now are the colorFactors, which are used to multiply the color texture, and the metal_rough factors, which have metallic and roughness parameters on r and b components, plus two more that are used in other places.

We have also a bunch of vec4s for padding. In vulkan, when you want to bind a uniform buffer, it needs to meet a minimum requirement for its alignment. 256 bytes is a good default alignment for this which all the gpus we target meet, so we are adding those vec4s to pad the structure to 256 bytes. 

When we create the descriptor set, there are some textures we want to bind, and the uniform buffer with the color factors and other properties. We will hold those in the MaterialResources struct, so that its easy to send them to the write_material function.
 
The `build_pipelines` function will compile the pipelines. `clear_resources` will delete everything, and write_material is where we will create the descriptor set and return a fully built MaterialData struct we can then use when rendering.

Lets look at the implementation of those functions.

```cpp
void GLTFMetallic_Roughness::build_pipelines(VulkanEngine* engine)
{
	VkShaderModule meshFragShader;
	if (!vkutil::load_shader_module("../../shaders/mesh.frag.spv", engine->_device, &meshFragShader)) {
		fmt::println("Error when building the triangle fragment shader module");
	}

	VkShaderModule meshVertexShader;
	if (!vkutil::load_shader_module("../../shaders/mesh.vert.spv", engine->_device, &meshVertexShader)) {
		fmt::println("Error when building the triangle vertex shader module");
	}

	VkPushConstantRange matrixRange{};
	matrixRange.offset = 0;
	matrixRange.size = sizeof(GPUDrawPushConstants);
	matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    DescriptorLayoutBuilder layoutBuilder;
    layoutBuilder.add_binding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	layoutBuilder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    materialLayout = layoutBuilder.build(engine->_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

	VkDescriptorSetLayout layouts[] = { engine->_gpuSceneDataDescriptorLayout,
        materialLayout };

	VkPipelineLayoutCreateInfo mesh_layout_info = vkinit::pipeline_layout_create_info();
	mesh_layout_info.setLayoutCount = 2;
	mesh_layout_info.pSetLayouts = layouts;
	mesh_layout_info.pPushConstantRanges = &matrixRange;
	mesh_layout_info.pushConstantRangeCount = 1;

	VkPipelineLayout newLayout;
	VK_CHECK(vkCreatePipelineLayout(engine->_device, &mesh_layout_info, nullptr, &newLayout));

    opaquePipeline.layout = newLayout;
    transparentPipeline.layout = newLayout;

	// build the stage-create-info for both vertex and fragment stages. This lets
	// the pipeline know the shader modules per stage
	PipelineBuilder pipelineBuilder;
	pipelineBuilder.set_shaders(meshVertexShader, meshFragShader);
	pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	pipelineBuilder.set_multisampling_none();
	pipelineBuilder.disable_blending();
	pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

	//render format
	pipelineBuilder.set_color_attachment_format(engine->_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(engine->_depthImage.imageFormat);

	// use the triangle layout we created
	pipelineBuilder._pipelineLayout = newLayout;

	// finally build the pipeline
    opaquePipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);

	// create the transparent variant
	pipelineBuilder.enable_blending_additive();

	pipelineBuilder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

	transparentPipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);
	
	vkDestroyShaderModule(engine->_device, meshFragShader, nullptr);
	vkDestroyShaderModule(engine->_device, meshVertexShader, nullptr);
}
```

`build_pipelines` is similar to the init_pipelines functions we had on VulkanEngine. We load the fragment and vertex shader and compile the pipelines. We are creating the pipeline layout in here too, and we are also creating 2 pipelines with the same pipeline builder. First we create the opaque pipeline, and then we enable blending and create the transparent pipeline. Once the pipelines are clear, we can destroy the shader modules.


You will note we have 2 new shaders, This is the code for them. Now that we will be rendering materials properly, we need to create completely new shaders for all of this.

We will be using #includes in our shaders this time, as the input will be used on both fragment and vertex shaders.

input_structures.glsl looks like this
```c
layout(set = 0, binding = 0) uniform  SceneData{   

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 1, binding = 0) uniform GLTFMaterialData{   

	vec4 colorFactors;
	vec4 metal_rough_factors;
	
} materialData;

layout(set = 1, binding = 1) uniform sampler2D colorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;
```

We have one uniform for scene-data. this will contain the view matrices and a few extras. This will be the global descriptor set.

Then we have 3 bindings for set 1 for the material. We have the uniform for the material constants, and 2 textures.


mesh.vert looks like this.
```c
#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "input_structures.glsl"

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
	mat4 render_matrix;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	
	vec4 position = vec4(v.position, 1.0f);

	gl_Position =  sceneData.viewproj * PushConstants.render_matrix *position;

	outNormal = (PushConstants.render_matrix * vec4(v.normal, 0.f)).xyz;
	outColor = v.color.xyz * materialData.colorFactors.xyz;	
	outUV.x = v.uv_x;
	outUV.y = v.uv_y;
}
```

We have the same vertex logic we had before, but this time we multiply it by the matrices when calculating the position. We also set the correct vertex color parameters and the UV.
For the normals, we multiply the vertex normal with the render matrix alone, no camera.

mesh.frag looks like this

```c
#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	float lightValue = max(dot(inNormal, sunlightDirection.xyz), 0.1f);

	vec3 color = inColor * texture(colorTex,inUV).xyz;
    vec3 ambient = color * ambientColor.xyz;


	outFragColor = vec4(color * lightValue * sunlightColor.w + ambient ,1.0f);
}
```

We are doing a very basic lighting shader. This will lets us render the meshes in a bit better way. We calculate the surface color by multiplying the vertex color with the texture, and then we do a simple light model where we have a single sunlight and an ambient light.

This is the kind of lighting you would see on very old games, simple function with 1 hardcoded light and very basic multiplying for light formula. We will be improving this later, but we need something that has a small amount of light calculation to display the materials better.

Lets go back to the `GLTFMetallic_Roughness` and fill the write_material function that will create the descriptor sets and set the parameters.

```cpp
MaterialData GLTFMetallic_Roughness::write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator)
{
	MaterialData matData;
	matData.passType = pass;
	if (pass == MaterialPass::Transparent) {
		matData.pipeline = &transparentPipeline;
	}
	else {
		matData.pipeline = &opaquePipeline;
	}

	matData.materialSet = descriptorAllocator.allocate(device,materialLayout);
    
   
    writer.clear();
    writer.write_buffer(0,resources.dataBuffer,sizeof(MaterialConstants),resources.dataBufferOffset,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.write_image(1, resources.colorImage.imageView, resources.colorSampler,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.write_image(2, resources.metalRoughImage.imageView, resources.metalRoughSampler,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    writer.update_set(device,matData.materialSet);

    return matData;
}
```

Depending on what the material pass is, we will select either the opaque or the transparent pipeline for it, then we allocate the descriptor set, and we write it using the images and buffer from MaterialResources.

Lets create a default material we can use for testing as part of the load sequence of the engine.

Lets first add the material structure to VulkanEngine, and a MaterialData struct to use for default. 

```cpp
MaterialData defaultData;
GLTFMetallic_Roughness metalRoughMaterial;
```

From init-pipelines we call the build_pipelines function on the material structure to compile it.

```cpp
void VulkanEngine::init_pipelines()
{
    metalRoughMaterial.build_pipelines(this);
}
```

Now, at the end of init_default_data(), we create the default MaterialData struct using the basic textures we just made. Like we did with the temporal buffer for scene data, we are going to allocate the buffer and then put it into a deletion queue, but its going to be the global deletion queue. We wont need to access the default material constant buffer at any point after creating it

```cpp
GLTFMetallic_Roughness::MaterialResources materialResources;
//default the material textures
materialResources.colorImage = _whiteImage;
materialResources.colorSampler = _defaultSamplerLinear;
materialResources.metalRoughImage = _whiteImage;
materialResources.metalRoughSampler = _defaultSamplerLinear;

//set the uniform buffer for the material data
AllocatedBuffer materialConstants = create_buffer(sizeof(GLTFMetallic_Roughness::MaterialConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

//write the buffer
GLTFMetallic_Roughness::MaterialConstants* sceneUniformData = (GLTFMetallic_Roughness::MaterialConstants*)materialConstants.allocation->GetMappedData();
sceneUniformData->colorFactors = glm::vec4{1,1,1,1};
sceneUniformData->metal_rough_factors = glm::vec4{1,0.5,0,0};

_mainDeletionQueue.push_function([=, this]() {
	destroy_buffer(materialConstants);
});

materialResources.dataBuffer = materialConstants.buffer;
materialResources.dataBufferOffset = 0;

//create the material instance
defaultData = metalRoughMaterial.write_material(_device,MaterialPass::MainColor,materialResources, globalDescriptorAllocator);
```

We are going to fill the parameters of the material on MaterialResources with the default white image. Then we create a buffer to hold the material color, and add it for deletion. Then we call write_material to create the descriptor set and initialize that defaultData material properly.
 
^nextlink
 
{% include comments.html term="Vkguide 2 Beta Comments" %}