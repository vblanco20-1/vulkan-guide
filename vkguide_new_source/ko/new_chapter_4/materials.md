---
layout: default
title: Setting up Materials
parent: "4. Textures and Engine Architecture"
nav_order: 12
---


이제 MaterialInstance를 구성하고, 우리가 사용할 GLTF 셰이더에 필요한 구조체들을 정의하는 작업부터 시작해보겠습니다.

다음 구조체들을 vk_types.h에 추가합니다.

^code mat_types shared/vk_types.h

이 구조체들은 머테리얼 데이터에 필요한 구조체들입니다. MaterialInstance는 실제 파이프라인을 담는 MaterialPipeline을 가리키는 포인터를 담습니다(소유하지는 않습니다). 또한, 디스크립터 셋도 함께 담고 있습니다.

이러한 객체들을 생성하는 로직은 VulkanEngine이 점점 커지고 있기 때문에 별도의 구조체로 감싸서 분리할 예정입니다. 나중에는 다양한 머테리얼들을 사용할 수 있어야 하기 때문입니다.

다음을 vk_engine.h에 추가합니다.

^code gltfmat chapter-4/vk_engine.h

현재로서는 두 파이프라인을 보유하게 됩니다. 하나는 투명한 객체를 그릴 때, 다른 하나는 불투명한 객체와 알파마스킹된 객체를 그릴 때 사용합니다. 그리고 머테리얼을 위한 디스크립터 셋 레이아웃도 포함됩니다.

머테리얼 상수를 위한 구조체도 있습니다. 이 구조체는 나중에 유니폼 버퍼에 쓰일 것입니다. 지금 필요한 파라미터는 다음과 같습니다.
- `colorFactors`는 색상 텍스쳐에 곱해질 색상 값입니다. 
- `metal_rough_factors`는 metallic과 roughness 값을 각각 r과 b컴포넌트에 저장합니다.
- 그 외에도 다른 용도로 사용되는 두 가지 추가 값이 포함됩니다. 
 
이외에도 여러 vec4들이 추가되어 있는데, 이는 패딩(padding)을 위한 것입니다. Vulkan에서는 유니폼 버퍼를 바인딩할 때는 최소 정렬 규칙을 만족해야 합니다. 보통 256바이트 정렬을 기본값으로 사용하며, 우리가 목표로 하는 모든 GPU에서 이를 지원합니다. 따라서 이 구조체의 크기를 2566바이트로 맞추기 위해 vec4들을 추가합니다.

디스크립터 셋을 생성할 때는 몇 가지 텍스쳐와 함께, 앞서 언급한 `colorFactors` 및 기타 속성을 담고 있는 유니폼 버퍼를 바인딩해야 합니다. 이러한 자원들을 MaterialResources 구조체에 담아 `write_material` 함수에 전달하기 쉽게 구성합니다.
 
`build_pipelines` 함수는 파이프라인을 컴파일 합니다. `clear_resources`는 관련된 모든 자원을 정리하고, `write_material`은 디스크립터 셋을 생성하고 렌더링 시 사용할 수 있는 완전한 MaterialInstance 구조체를 반환합니다.

이제 이 함수들의 구현부를 살펴봅시다.

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

`build_pipelines`는 VulkanEngine에서의 `init_pipelines` 함수와 유사한 방식으로 동작합니다. 프래그먼트와 정점 셰이더를 불러와 파이프라인으로 컴파일합니다. 이 과정에서 파이프라인 레이아웃도 생성하며, 동일한 PipelineBuilder를 사용해 2개의 파이프라인을 생성하고 있습니다. 먼저 불투명 렌더링을 위한 파이프라인을 생성하고, 그후 블렌딩을 활성화하여 투명 렌더링을 위한 파이프라인을 생성합니다. 파이프라인을 모두 생성한 후에는 셰이더 모듈을 파괴해도 문제없습니다.


2개의 새로운 셰이더가 추가되었음을 확인할 수 있습니다. 아래에 해당 셰이더들의 코드가 포함되어 있습니다. 이제부터는 머테리얼 기반으로 렌더링을 수행할 것이므로, 기존 셰이더 대신 완전히 새로운 셰이더를 사용해야 합니다.

또한 이번에는 셰이더 코드에 #include 지시문을 사용할 예정입니다. 동일한 입력 구조가 정점 셰이더와 프래그먼트 셰이더 양쪽 모두에서 사용되기 때문입니다.

input_structures.glsl은 다음과 같습니다.
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

씬 데이터용 유니폼이 하나 있으며, 뷰 행렬과 몇 가지 추가 정보를 포함합니다. 이 유니폼은 전역 디스크립터 셋으로 사용됩니다.

그후 머테리얼을 위한 디스크립터 셋(set 1)에는 총 3개의 바인딩이 있으며, 각각 머테리얼 상수를 위한 유니폼 하나와 텍스쳐 두 개로 구성됩니다.


mesh.vert는 다음과 같습니다.
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

이전과 동일한 정점 처리 로직을 사용하지만, 이번에는 위치를 계산할 때 행렬을 곱해줍니다. 또한 정점 색상과 UV를 올바르게 설정하고 있습니다. 법선의 경우 카메라 행렬은 사용하지 않고 render_matrix만 곱합니다.

mesh.frag는 다음과 같습니다.

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
	float lightValue = max(dot(inNormal, sceneData.sunlightDirection.xyz), 0.1f);

	vec3 color = inColor * texture(colorTex,inUV).xyz;
	vec3 ambient = color *  sceneData.ambientColor.xyz;

	outFragColor = vec4(color * lightValue *  sceneData.sunlightColor.w + ambient ,1.0f);
}
```

여기서 매우 기본적인 조명 셰이더를 사용합니다. 이를 통해 메시를 조금 더 나은 방식으로 렌더링할 수 있습니다. 정점 색상과 텍스쳐를 곱해 표면 색을 계산한 뒤, 하나의 태양광과 환경광만을 사용하는 단순한 조명 모델을 적용합니다.

이 방식은 오래된 게임에서 볼 수 있는 종류의 조명 모델로, 하나의 하드코딩된 조명과 아주 단순한 곱셈 기반 조명 방식을 사용합니다. 나중에 개선할 예정이지만, 현재는 머테리얼을 좀 더 잘 보여주기 위한 최소한의 조명 계산이 필요합니다.

이제 `GLTFMetallic_Roughness`로 돌아가 디스크립터 셋을 생성하고 파라미터를 설정하는 write_material 함수를 구현해봅시다.

^code write_mat chapter-4/vk_engine.cpp

materaiPass에 따라 투명과 불투명 파이프라인 중 선택한 후, 디스크립터 셋을 할당합니다. 이후 MaterialResource에 있는 이미지와 버퍼를 사용해 디스크립터 셋을 작성합니다.

이제 엔진의 로딩 시퀀스를 테스트할 수 있는 기본 머테리얼을 생성합시다.
 
먼저 VulkanEngine에 머테리얼 구조체를 추가하고, 기본값으로 사용할 MaterialInstance 구조체도 정의합시다.

```cpp
MaterialInstance defaultData;
GLTFMetallic_Roughness metalRoughMaterial;
```

init_pipelines() 함수의 마지막에서 머테리얼 구조체에 대해  build_pipelines함수를 호출하여 파이프라인을 컴파일합니다. 

```cpp
void VulkanEngine::init_pipelines()
{
	//rest of initializing functions

    metalRoughMaterial.build_pipelines(this);
}
```

이제 `init_default_data()`함수의 마지막에서, 앞서 만든 기본 텍스쳐들을 사용하여 기본 MaterialInstance 구조체를 생성합니다. 씬 데이터를 위한 임시 버퍼에서 했던 것처럼, 버퍼를 할당한 후 삭제 큐에 등록할 것인데, 이번에는 전역 삭제 큐에 추가합니다. 기본 머테리얼 상수 버퍼는 생성 이후에는 더 이상 접근할 필요가 없습니다.

^code default_mat chapter-4/vk_engine.cpp

MaterialResources에 있는 머테리얼의 파라미터를 기본 흰색 이미지로 채울 것입니다. 이후 머테리얼 색상을 저장할 버퍼를 생성하고 삭제 큐에 등록합니다. 그런 다음 write_material 함수를 호출하여 디스크립터 셋을 생성하고, 기본 머테리얼을 올바르게 초기화합니다.
 
^nextlink
 
{% include comments.html term="Vkguide 2 Beta Comments" %}