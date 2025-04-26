---
layout: default
title: Push Constants and new shaders
parent:  "2. Drawing with Compute"
nav_order: 9
---

컴퓨트 셰이더를 실행해 표시할 수단이 있으며, 엔진에 디버그 UI를 추가하는 기능도 있습니다. 이제 UI를 통해 셰이더로 데이터를 전송하고 상호작용해보겠습니다.

셰이더에 데이터를 전송하는 데 푸시상수를 사용할 것입니다. 푸시상수는 적은 양의 데이터를 GPU로 보낼 수 있는 Vulkan의 고유 기능입니다. 이 데이터를 적게 유지하는 것이 중요한데, 대부분의 드라이버가 데이터가 특정 바이트 수 이하일 때 가장 빠른 경로를 찾아주기 때문입니다(자세한 크기는 GPU 제조사 문서를 참고하세요). 주 용례는 객체마다의 인덱스 혹은 행렬을 전달하는 것입니다. 만약 보내야 하는 데이터가 꽤 많다면 다음 챕터에서 사용할 다른 시스템을 사용해야 합니다.

푸시 상수는 파이프라인을 생성할 때 구성합니다. 코드를 단순하게 유지하고 많은 수정을 피하기 위해 푸시상수에 기본적으로 4개의 vec4를 사용하겠습니다. 16개의 float는 셰이더에서 충분합니다.

프로젝트의 셰이더 폴더에 여러 컴퓨트 셰이더가 있습니다. 이를 바꿔 사용할 수 있으며, 튜토리얼에서는 단순한 색상에만 집중하겠지만 여러 데모 셰이더를 적용해 볼 수 있습니다.

모든 컴퓨트 셰이더가 같은 레이아웃을 공유하기 때문에 UI에 드롭다운을 추가하는 것으로 사용할 파이프라인을 선택할 수 있습니다. 이렇게 하면 런타임에 다양한 컴퓨트 셰이더를 번갈아 사용할 수 있습니다.

푸시상수를 적용할 예제에 사용할 셰이더는 다음과 같습니다. 이는 2개의 색상을 Y좌표에 따라 혼합하여 수직 그라디언트를 만듭니다.

셰이더 폴더의 gradient_color.comp에서 확인할 수 있습니다.

```c
#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba16f,set = 0, binding = 0) uniform image2D image;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data1;
 vec4 data2;
 vec4 data3;
 vec4 data4;
} PushConstants;

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 size = imageSize(image);

    vec4 topColor = PushConstants.data1;
    vec4 bottomColor = PushConstants.data2;

    if(texelCoord.x < size.x && texelCoord.y < size.y)
    {
        float blend = float(texelCoord.y)/(size.y); 
    
        imageStore(image, texelCoord, mix(topColor,bottomColor, blend));
    }
}
```

대부분 이전 글에서 작성했던 그라디언트 셰이더와 동일합니다. 4개의 `vec4`를 담는 푸시상수 블록을 추가했으며 이로부터 상단과 하단의 색상을 불러옵니다. `data3`와 `data4`는 실제로 사용되지는 않지만 셰이더의 푸시상수 범위보다 넓다는 이유로 검증 레이어가 경고를 발생하는 것을 방지하기 위해 추가했습니다.

푸시상수 범위를 구성하기 위해 파이프라인 레이아웃을 수정할 필요가 있습니다. 먼저 이러한 푸시 상수를 직접적으로 나타내는 구조체를 `vk_engine.h`에 추가합니다.

```cpp
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};
```

푸시상수 범위를 설정하기 위해 `init_pipelines`의 시작 부분에서 파이프라인 레이아웃의 생성하는 코드를 수정해야 합니다. 아래는 수정된 버전입니다.

<!-- codegen from tag comp_pipeline_pc on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
VkPipelineLayoutCreateInfo computeLayout{};
computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
computeLayout.pNext = nullptr;
computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
computeLayout.setLayoutCount = 1;

VkPushConstantRange pushConstant{};
pushConstant.offset = 0;
pushConstant.size = sizeof(ComputePushConstants) ;
pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

computeLayout.pPushConstantRanges = &pushConstant;
computeLayout.pushConstantRangeCount = 1;

VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));
```

`VkPushConstantRange`를 `VkPipelineLayoutCreateInfo` 구조체에 추가합니다. 푸시상수 범위는 오프셋을 담으며(여기서는 0으로 설정합니다.), 크기와 실행 단계 플래그를 지정합니다. 크기에는 우리 구조체의 cpp 버전을 사용할 것이고, 실행 단계 플래그는 우리가 실행하는 유일한 단계인 컴퓨트(compute)일 것입니다.

이후에 셰이더를 바꾸기만 하면 됩니다.

```cpp
VkShaderModule computeDrawShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &computeDrawShader))
{
	std::cout << "Error when building the colored mesh shader" << std::endl;
}
```

이것으로 셰이더에 푸시상수를 전달하기 위해 필요한 모든 작업이 완료되었습니다. 이제 이를 렌더링 루프에서 사용해봅시다.

<!-- codegen from tag draw_pc on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// bind the gradient drawing compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

	// bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	ComputePushConstants pc;
	pc.data1 = glm::vec4(1, 0, 0, 1);
	pc.data2 = glm::vec4(0, 0, 1, 1);

	vkCmdPushConstants(cmd, _gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &pc);
	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

푸시 상수를 업데이트 하기 위해 `VkCmdPushConstants`를 호출합니다. 이 함수는 파이프라인 레이아웃, 작성할 데이터의 오프셋(여기서는 0입니다.) 그리고 데이터의 크기와 복사할 포인터를 요구합니다. 또한 서로 다른 셰이더 단계에 대해 푸시상수를 업데이트할 수 있기 때문에 셰이더 단계 플래그도 필요합니다.

이것으로 모든 설정이 끝났습니다. 이 시점에서 프로그램을 실행시킨다면 빨간 색과 파란 색으로 이어지는 그라디언트를 볼 수 있습니다.

## ImGui 매개변수

지금은 하드코딩된 색상을 전달하고 있지만, ImGui에 작은 창을 추가하여 색상을 바꿔볼 수 도 있습니다.

그 값으로 사용할 `ComputePushConstant` 구조체 하나와 함께 그릴 컴퓨트 파이프라인의 배열을 담아야 합니다. 이렇게 하면 다양한 컴퓨트 셰이더를 전환할 수 있게 됩니다.

`vk_engine.h`에 해당 구조체를 추가합시다.

```cpp
struct ComputeEffect {
    const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};
```

이제 `VulkanEngine` 클래스에 셰이더의 배열을 추가하고, 렌더링 시 사용할 인덱스를 담는 정수도 함께 추가합니다.

```cpp
std::vector<ComputeEffect> backgroundEffects;
int currentBackgroundEffect{0};
```

`init_pipelines`의 코드를 수정해 2개의 이펙트를 생성해 봅시다. 하나는 그라디언트, 다른 하나는 밤하늘 셰이더입니다.

하늘 셰이더는 여기서 설명하기에는 다소 복잡하므로, sky.comp의 코드를 확인해 보는 것도 좋습니다. shadertoy에서 가져와 여기서 컴퓨트 셰이더로 실행하기 위해 약간 바꾸었습니다. 푸시상수의 data1은 하늘 색상의 x/y/z를 담을 것이며, w는 별의 숫자를 제어할 때 사용할 것입니다.

2개의 셰이더가 있으므로 2개의 `VkShaderModule`이 필요합니다.

<!-- codegen from tag comp_pipeline_multi on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
VkShaderModule gradientShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &gradientShader)) {
	fmt::print("Error when building the compute shader \n");
}

VkShaderModule skyShader;
if (!vkutil::load_shader_module("../../shaders/sky.comp.spv", _device, &skyShader)) {
	fmt::print("Error when building the compute shader \n");
}

VkPipelineShaderStageCreateInfo stageinfo{};
stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
stageinfo.pNext = nullptr;
stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
stageinfo.module = gradientShader;
stageinfo.pName = "main";

VkComputePipelineCreateInfo computePipelineCreateInfo{};
computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
computePipelineCreateInfo.pNext = nullptr;
computePipelineCreateInfo.layout = _gradientPipelineLayout;
computePipelineCreateInfo.stage = stageinfo;

ComputeEffect gradient;
gradient.layout = _gradientPipelineLayout;
gradient.name = "gradient";
gradient.data = {};

//default colors
gradient.data.data1 = glm::vec4(1, 0, 0, 1);
gradient.data.data2 = glm::vec4(0, 0, 1, 1);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline));

//change the shader module only to create the sky shader
computePipelineCreateInfo.stage.module = skyShader;

ComputeEffect sky;
sky.layout = _gradientPipelineLayout;
sky.name = "sky";
sky.data = {};
//default sky parameters
sky.data.data1 = glm::vec4(0.1, 0.2, 0.4 ,0.97);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline));

//add the 2 background effects into the array
backgroundEffects.push_back(gradient);
backgroundEffects.push_back(sky);

//destroy structures properly
vkDestroyShaderModule(_device, gradientShader, nullptr);
vkDestroyShaderModule(_device, skyShader, nullptr);
_mainDeletionQueue.push_function([=]() {
	vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
	vkDestroyPipeline(_device, sky.pipeline, nullptr);
	vkDestroyPipeline(_device, gradient.pipeline, nullptr);
});

```

pipelines 함수를 수정했습니다. 기존의 파이프라인 레이아웃은 그대로 유지하면서, 이제 두 개의 서로 다른 파이프라인을 생성하여 ComputeEffect 벡터에 저장합니다. 또한 각 이펙트에 기본값 데이터를 설정합니다.

이제 이 기능을 위한 ImGui 디버그 창을 추가할 수 있습니다. 해당 코드는 run()함수 내부에 들어가며 기존의 데모 이펙트 호출을 새로운 UI 로직으로 대체할 것입니다.

<!-- codegen from tag imgui_bk on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
		ImGui::NewFrame();
		
		if (ImGui::Begin("background")) {
			
			ComputeEffect& selected = backgroundEffects[currentBackgroundEffect];
		
			ImGui::Text("Selected effect: ", selected.name);
		
			ImGui::SliderInt("Effect Index", &currentBackgroundEffect,0, backgroundEffects.size() - 1);
		
			ImGui::InputFloat4("data1",(float*)& selected.data.data1);
			ImGui::InputFloat4("data2",(float*)& selected.data.data2);
			ImGui::InputFloat4("data3",(float*)& selected.data.data3);
			ImGui::InputFloat4("data4",(float*)& selected.data.data4);
		}
		ImGui::End();

		ImGui::Render();

```

먼저 선택된 컴퓨트 이펙트를 배열 인덱싱을 통해 가져옵니다. 그 후 `ImGui::Text`를 사용해 이펙트 이름을 출력하며, 편집할 정수 슬라이더와 `float4` 입력을 제공합니다.

마지막으로 할 일은 렌더링 루프를 수정해 셰이더를 선택하여 그에 대응하는 데이터를 사용하도록 하는 것입니다.

<!-- codegen from tag draw_multi on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	ComputeEffect& effect = backgroundEffects[currentBackgroundEffect];

	// bind the background compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);

	// bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	vkCmdPushConstants(cmd, _gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);
	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

크게 바뀐 것은 없습니다. 컴퓨트 이펙트 배열에 연결하고 해당 항목으로부터 푸시상수를 업로드해주기만 하면 됩니다.

애플리케이션을 실행하여 디버그 창을 통해 셰이더를 선택하고 파라미터를 수정해보세요.

Next: [Chapter 3: The graphics pipeline]({{ site.baseurl }}{% link docs/ko/new_chapter_3/render_pipeline.md %})

{% include comments.html term="Vkguide 2 Beta Comments" %}
