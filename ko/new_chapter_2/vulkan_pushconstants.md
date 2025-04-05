---
layout: default
lang: ko
title: Push Constants and new shaders
parent:  "2. 컴퓨트 셰이더 그리기"
nav_order: 9
---

We have a way to run compute shaders to display, and a way to add debug-UI to the engine. Lets use that to send data to the shaders through the UI, and have an interactive thing.
컴퓨트 셰이더를 실행해 표시할 수단이 필요합니다. 또한 엔진에 디버깅 UI를 추가할 수단이 필요합니다. UI를 통해 셰이더로 데이터를 전송하고 상호작용해보겠습니다.

We will be using PushConstants to send data to the shader. PushConstants are a feature unique to vulkan that allows for some small amount of data to be sent to the GPU. Keeping this data small is important, as most drivers will have a fast-path if the data is below some bytes (consult gpu vendor documentation). Its main use case is to send some per-object indexes or a couple matrices that change for every object. If you have data that is bigger than a handful of floats or ints, you should be using other systems that we will show next chapter.
셰이더에 데이터를 전송하는 데 푸시상수를 사용할 것입니다. 푸시상수는 적은 양의 데이터를 GPU로 보낼 수 있는 Vulkan의 고유 기능입니다. 대부분의 드라이버가 데이터가 몇 바이트 밑인지에 따라 가장 빠른 경로를 찾아주기 때문에 데이터를 적게 유지하는 것은 중요합니다. 주 용례는 객체별로 변하는 객체별 인덱스 혹은 행렬을 전달하는 것입니다. 만약 보내야 하는 데이터가 꽤 많다면 다음 챕터에서 사용할 다른 시스템을 사용해야 합니다.

Push constants are configured when you create a pipeline layout. To keep things simple and not have to change too much code, we are going to default our pushconstants for compute effects to 4 vec4 vectors. 16 floats will be enough for playing around with the shaders.
푸시 상수는 파이프라인을 생성할 때 구성합니다. 코드를 단순하게 유지하고 많이 바꾸지 않기 위해 푸시상수에 기본 vec4를 사용하겠습니다. 16개의 float는 셰이더에서 충분합니다.

In the project shader folder, there are multiple compute shaders you will be able to swap around. We will focus on a simple color gradient one, but you can try the other demo shaders the project comes with.
프로젝트의 셰이더 폴더에 여러 컴퓨트 셰이더가 있습니다. 이를 바꿔 사용할 수 있으며, 튜토리얼에서는 단순한 색상에만 집중하겠지만 여러 데모 셰이더를 적용해 볼 수 있습니다.

As all of our compute shaders will share the same layout, we are going to also add a drop-down to the UI to select which pipeline to use. This way we can flip between the different compute shaders at runtime to test them.
모든 컴퓨트 셰이더가 같은 레이아웃을 공유하기 때문에 UI에 드래그앤드롭을 하는 것으로 사용할 파이프라인을 선택할 수 있습니다. 이 방법은 런타임에 다양한 컴퓨트 셰이더를 번갈아 사용할 수 있습니다.

The shader we are going to use to demonstrate pushconstants is this. It will blend between 2 colors by Y coordinate, making a vertical gradient.
푸시상수를 적용할 예제에 사용할 셰이더는 다음과 같습니다. 이는 2개의 색상을 Y좌표에 따라 혼합하여 수직 기울기를 만듭니다.

Its found under gradient_color.comp in the shader folder
셰이더 폴더의 gradient_color.comp에서 확인할 수 있습니다.

```c
#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba16f,set = 0, binding = 0) uniform image2D image;

//push constants block / 푸시상수 입니다.
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

Its mostly the same as the gradient shader we had from last article. We have added a push constant block containing 4 vec4s, and we are loading top and bottom color from it. data3 and data4 are not used, but we have them in there to avoid the validation layers complaining that we have a push-constants range larger than we have in the shader.
대부분 이전 글에서 작성했던 그라디언트 셰이더와 같습니다. 4개의 vec4를 담는 푸시상수 블록을 추가했으며 이로부터 상단과 하단의 색상을 불러옵니다. data3와 data4는 사용되지 않지만 셰이더에서 푸시상수 범위보다 넓다고 검증 레이어가 오류를 뱉는 것을 방지하기 위해 추가했습니다.

We now need to change the pipeline layout creation to configure the pushconstants range. Lets first create a structure that mirrors those pushconstants directly into vk_engine.h.
푸시상수 범위를 구성하기 위해 파이프라인 레이아웃의 생성을 바꿀 필요는 없습니다. 먼저 이러한 푸시 상수를 직접적으로 나타내는 구조체를 vk_engine.h에 추가합니다.

```cpp
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};
```

To set the push constant ranges, we need to change the code that creates the pipeline layout at the start of init_pipelines. the new version looks like this
푸시상수 범위를 설정하기 위해 init_pipelines의 시작지점에 파이프라인 레이아웃의 생성 지점을 바꿀 필요가 있습니다. 새로운 버전은 아래와 같습니다.

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

We need to add a VkPushConstantRange to the pipeline layout info. A PushConstantRange holds an offset, which we will keep at 0, and then a size plus the stage flags. For size we will use our cpp version of the structure, as that matches. And for stage flags its going to be compute because its the only stage we have right now.
VkPushConstantRange를 파이프라인 레이아웃 info 구조체에 추가합니다. 푸시상수 범위는 오프셋을 담으며(여기서는 0으로 설정합니다.), 크기에 더해 실행 단계 플래그를 담습니다. 크기가 일치하도록 우리 구조체의 cpp 버전을 사용할 것입니다. 그리고 실행 단계 플래그는 우리가 실행하는 유일한 단계인 컴퓨트일 것입니다.

After that, just change the shader to be compiled to be the new one
이 이후에, 셰이더를 바꾸기만 하면 됩니다.

```cpp
VkShaderModule computeDrawShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &computeDrawShader))
{
	std::cout << "Error when building the colored mesh shader" << std::endl;
}
```

This is all we need to add pushconstants to a shader. lets now use them from the render loop
이는 셰이더로 푸시상수를 전달하기 위해 필요한 모든 것입니다. 이제 이를 렌더링 루프에서 사용해봅시다.

<!-- codegen from tag draw_pc on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// bind the gradient drawing compute pipeline / 컴퓨트 파이프라인을 그리는 그라디언트를 바인딩합니다.
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

	// bind the descriptor set containing the draw image for the compute pipeline / 컴퓨트 파이프라인에 사용할 그릴 이미지를 담는 디스크립터 셋을 바인딩합니다.
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	ComputePushConstants pc;
	pc.data1 = glm::vec4(1, 0, 0, 1);
	pc.data2 = glm::vec4(0, 0, 1, 1);

	vkCmdPushConstants(cmd, _gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &pc);
	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it / 컴퓨트 파이프라인 dispatch를 실행합니다. 16 * 16개의 워크그룹을 사용할 것이므로 이로 나눕니다.
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

To update pushconstants, we call VkCmdPushConstants. it requires the pipeline layout, an offset for the data to be written to (we use just offset 0), and the size of the data + the pointer to copy. It also requires the shader stage flags as one can update pushconstants for different stages on different commands.
푸시 상수를 업데이트 하기 위해 VkCmdPushConstants를 호출합니다. 이는 파이프라인 레이아웃, 작성할 데이터의 오프셋(여기서는 0입니다.) 그리고 데이터의 크기와 복사할 포인터를 요구합니다. 또한 푸시상수를 업데이트할 셰이더 단계 플래그도 요구합니다.

This is all. If you run the program at this moment, you will see a gradient of red to blue.
이것으로 끝났습니다. 이 시점에서 프로그램을 실행시킨다면 빨간 색과 파란 색으로 기울기를 볼 수 있습니다.

## Imgui editable parameters / Imgui 편집 가능한 매개변수

We are hardcoding the colors right now, but we can do better than that by adding a small window using imgui with those as editable colors.
지금은 하드코딩된 이미지를 전달하고 있지만, imgui에 작은 창을 추가하여 색상을 바꿔볼 수 도 있습니다.

We want to store an array of compute pipelines we will be drawing, alongside one of those ComputePushConstant structs for their value. This way we will be able to switch between different compute shaders.
그 값으로 사용할 ComputePushConstant 구조체 하나와 함께 그릴 컴퓨트 파이프라인의 배열을 담아야 합니다. 이 방법은 다양한 컴퓨트 셰이더를 전환할 수 있게 합니다.

Lets add a struct to vk_engine.h with that

```cpp
struct ComputeEffect {
    const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};
```

Now lets add an array of them to the VulkanEngine class, with an integer to hold the index to use when drawing
이제 VulkanEngine 클래스에 그릴 때 사용할 인덱스를 담는 정수와 함께 셰이더들의 배열을 추가합니다.

```cpp
std::vector<ComputeEffect> backgroundEffects;
int currentBackgroundEffect{0};
```

Lets change the code on init_pipelines to create 2 of these effects. One will be the gradient we just did, the other is a pretty star-night sky shader.
init_pipelines의 코드를 변경해 이중 2개의 이펙트를 생성합니다. 하나는 그라디언트, 다른 하나는 별-밤 하늘 셰이더입니다.

The sky shader is too complicated to explain here, but feel free to check the code on sky.comp. Its taken from shadertoy and adapted slightly to run as a compute shader in here. data1 of the pushconstant will contain sky color x/y/z, and then w can be used to control the amount of stars.
하늘 셰이더는 여기서 설명하기에는 꽤 복잡하지만, sky.comp의 코드를 확인해 보는 것으로 충분합니다. shadertoy에서 가져와 여기서 컴퓨트 셰이더로 실행하기 위해 약간 바꾸었습니다. 푸시상수의 data1은 하늘 색상의 x/y/z를 담을 것이며, w는 별의 숫자를 제어할 때 사용할 수 있습니다.

With 2 shaders, we need to create 2 different VkShaderModule.
2개의 셰이더가 있으므로 2개의 VkShaderModule이 필요합니다.

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

//default colors / 기본 색상
gradient.data.data1 = glm::vec4(1, 0, 0, 1);
gradient.data.data2 = glm::vec4(0, 0, 1, 1);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline));

//change the shader module only to create the sky shader / 하늘 셰이더를 생성할 때에만 셰이더 모듈을 변경합니다.
computePipelineCreateInfo.stage.module = skyShader;

ComputeEffect sky;
sky.layout = _gradientPipelineLayout;
sky.name = "sky";
sky.data = {};
//default sky parameters / 기본 하늘 파라미터입니다.
sky.data.data1 = glm::vec4(0.1, 0.2, 0.4 ,0.97);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline));

//add the 2 background effects into the array / 배열에 2개의 백그라운드 효과를 추가합니다.
backgroundEffects.push_back(gradient);
backgroundEffects.push_back(sky);

//destroy structures properly / 구조체를 제대로 파괴합니다.
vkDestroyShaderModule(_device, gradientShader, nullptr);
vkDestroyShaderModule(_device, skyShader, nullptr);
_mainDeletionQueue.push_function([=]() {
	vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
	vkDestroyPipeline(_device, sky.pipeline, nullptr);
	vkDestroyPipeline(_device, gradient.pipeline, nullptr);
});

```

We have changed the pipelines function. We keep the pipeline layout from before, but now we create 2 different pipelines, and store them into the ComputeEffect vector. We also give the effects some default data.

Now we can add the imgui debug window for this. This goes on run() function. We will replace the demo effect call with the new ui logic

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

First we grab the selected compute effect by indexing into the array. Then we use Imgui::Text to display the effect name, and then we have int slider and float4 input for the edits.
먼저 선택된 컴퓨트 이펙트를 배열의 인덱싱을 통해 가져옵니다. 그 후 Imgui::Text를 사용해 효과 이름을 출력하며, 편집할 정수 슬라이더와 float4 입력을 갖습니다.

Last we need to do is to change the render loop to select the shader selected with its data

<!-- codegen from tag draw_multi on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	ComputeEffect& effect = backgroundEffects[currentBackgroundEffect];

	// bind the background compute pipeline / 백그라운드 컴퓨트 파이프라인을 바인딩합니다.
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);

	// bind the descriptor set containing the draw image for the compute pipeline / 컴퓨트 파이프라인에 그릴 이미지를 포함하는 디스크립터 셋을 바인딩합니다.
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	vkCmdPushConstants(cmd, _gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);
	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it / 컴퓨트 파이프라인 dispatch를 실행합니다. 16 * 16 작업 그룹 크기를 사용하므로 이로 나눕니다.
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

Not much of a change, we are just hooking into the compute effect array and uploading the pushconstants from there.
변화가 많지는 않습니다. 컴퓨트 이펙트 배열과 푸시상수 업로드를 연결해주었을 뿐입니다.

Try to run the app now, and you will see a debug window where it lets you select the shader, and edit its parameters.
애플리케이션을 실행하여 디버그 창을 통해 셰이더를 선택하고 파라미터를 수정해보세요.

다음 글 : [3장 : 그래픽스 파이프라인]({{ site.baseurl }}{% link docs/new_chapter_3/render_pipeline.md %})

{% include comments.html term="Vkguide 2 Beta Comments" %}
