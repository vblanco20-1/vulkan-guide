---
layout: default
title: Setting up render pipeline
parent: "3. Graphics Pipelines - KO"
grand_parent: "Korean VKGuide"
nav_order: 4
---

## VkGraphicsPipelineCreateInfo
그래픽스 파이프라인을 만드는 과정은 컴퓨트 파이프라인을 만들 때 보다 꽤 많은 작업이 추가로 필요합니다. 컴퓨트 파이프라인에서는 단일 셰이더 모듈과 파이프라인만 필요했습니다. 따라서 어떠한 추상 계층도 필요가 없었지만, 그래픽스 파이프라인은 설정해야 할 옵션이 상당히 많기 때문에 이를 단순화하지 않으면 생성 과정이 상당히 복잡해질 것입니다.

이러한 이유로, `PipelineBuilder`라는 구조체를 추가하여 이러한 모든 옵션을 추적하려 합니다. 이 구조체는 가능한 많은 항목을 기본값으로 유지하면서, 우리가 원하는 기능만 활성화/비활성화하는 단순한 함수를 제공할 것입니다. 튜토리얼에서는 사용할 설정이 많지 않기 때문에 이러한 복잡성을 줄이는 것이 유용합니다.

파이프라인의 이러한 옵션 중 일부는 동적으로 설정될 수 있으며, 이는 파이프라인을 바인딩할 때와 그리기 명령을 기록할 때 해당 옵션을 지정할 수 있다는 의미입니다. 예를 들어 viewport를 동적으로 지정할 예정인데, 만약 이를 굽는(baked in) 방식으로 설정하면 렌더링 해상도를 변경하고 싶을 때마다 새로운 파이프라인을 생성해야 하기 때문입니다.

Builder를 작성하기 전에, 무엇을 채워야 하는지 먼저 살펴봅시다. 컴퓨트 파이프라인을 생성할 때와 같이 그래픽스 파이프라인은 `VkGraphicsPipelineCreateInfo`구조체를 채워야 합니다.

```cpp
typedef struct VkGraphicsPipelineCreateInfo {
    VkStructureType                                  sType;
    const void*                                      pNext;
    VkPipelineCreateFlags                            flags;
    uint32_t                                         stageCount;
    const VkPipelineShaderStageCreateInfo*           pStages;
    const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
    const VkPipelineTessellationStateCreateInfo*     pTessellationState;
    const VkPipelineViewportStateCreateInfo*         pViewportState;
    const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
    const VkPipelineDynamicStateCreateInfo*          pDynamicState;
    VkPipelineLayout                                 layout;
    VkRenderPass                                     renderPass;
    uint32_t                                         subpass;
    VkPipeline                                       basePipelineHandle;
    int32_t                                          basePipelineIndex;
} VkGraphicsPipelineCreateInfo;
```

그래픽스 파이프라인의 관한 정보는 다음 링크에서 세부 사항을 확인할 수 있습니다.
* [VkGraphicsPipelineCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkGraphicsPipelineCreateInfo)


`stageCount`와 `pStages`는 파이프라인의 각 셰이더 단계에 해당하는 셰이더 모듈 정보를 담는 `VkPipelineShaderStageCreateInfo`를 포함합니다. 여기에는 정점 셰이더와 프래그먼트 셰이더를 전달할 것입니다.

`VkPipelineVertexInputStateCreateInfo`는 정점 버퍼에 사용할 정점 속성 입력 구성을 담습니다. 이를 올바르게 설정하면 정점 셰이더는 정점 속성을 최적화된 방식으로 입력받을 수 있습니다. 하지만 우리는 이를 사용하지는 않을 것입니다. 셰이더에 데이터의 배열을 직접 전달하여 인덱싱하여 사용할 것입니다. 이 방식은 성능을 향상시키고 데이터 압축이 적용된 더 복잡한 정점 포맷을 처리할 수 있습니다. 이러한 방식은 일반적으로 정점 풀링(vertex pulling)이라고 불리며, 고정 하드웨어 방식의 정점 입력과 동일한 작업을 수행하더라도, 현대 GPU에서는 거의 동일한 성능을 보입니다.

`VkPipelineInputAssemblyStateCreateInfo`는 삼각형 토폴로지를 담습니다. 이 구조체를 통해 파이프라인이 삼각형, 점, 선 중 어떤 도형을 그릴 지 설정할 수 있습니다.

`VkPipelineTessellationStateCreateInfo`는 고정 기능 테셀레이션을 구성합니다. 이를 사용하지 않을 것이므로 null로 설정할 것입니다.

`VkPipelineViewportStateCreateInfo`는 픽셀이 렌더링될 뷰포트의 정보를 담습니다. 이는 파이프라인이 어떤 영역의 픽셀에 그릴지를 설정할 수 있습니다. 우리는 동적 상태를 사용할 것이므로 이를 기본값으로 설정할 것입니다.

`VkPipelineRasterizationStateCreateInfo`는 정점 셰이더와 프래그먼트 셰이더 간 삼각형을 어떻게 래스터화할지에 대한 정보를 담습고 있습니다. 이는 깊이 바이어스(그림자 렌더링에 사용됩니다), 와이어프레임과 솔리드 렌더링 전환, 그리고 뒷면 렌더링 여부 등의 옵션을 제공합니다.

`VkPipelineMultisampleStateCreateInfo`는 멀티 샘플링 안티 앨리어싱을 구성할 수 있도록 합니다. 이는 삼각형의 경계에서 프래그먼트를 여러 번 래스터화함으로서 렌더링 품질을 향상시키는 기법입니다. 안티 앨리어싱을 적용하지 않을 것이므로 기본값으로 설정하지만, 추후 이를 적용하는 것도 고려할 것입니다.

`VkPipelineDepthStencilStateCreateInfo`는 깊이 테스트와 스텐실 설정 정보를 담습니다.

`VkPipelineColorBlendStateCreateInfo`는 블렌딩과 어태치 먼트 쓰기 설정을 담습니다. 투명한 삼각형을 렌더링하거나 다양한 블렌딩 설정에 사용됩니다.

`VkPipelineDynamicStateCreateInfo`는 동적 상태를 구성하는 구조체입니다. Vulkan 파이프라인의 가장 큰 단점 중 하나는 파이프라인 설정이 생성 시에 "하드코딩" 된다는 점입니다. 예를 들어 깊이 테스트를 켜거나 끄는 기능을 원한다면 2개의 파이프라인이 필요할 것입니다. 뷰포트도 마찬가지입니다. 렌더 타겟의 크기를 바꾸려면 모든 파이프라인을 재구성해야 합니다. 파이프라인을 생성하는 과정은 매우 무거운 작업이기 때문에 파이프라인의 수를 가능한 한 줄이는 것이 성능적으로 중요합니다. 이러한 이유로, Vulkan 파이프라인의 일부 상태는 동적으로 설정될 수 있으며, 이 때 설정된 옵션은 렌더링 명령을 기록할 때 런타임에 변경될 수 있습니다. 어떤 동적 상태를 지원하는지는 GPU 제조사, 드라이버 버전, 기타 환경에 따라 달라질 수 있습니다. 대부분의 GPU가 뷰포트와 가위 설정에 대한 동적 상태를 지원하므로, 우리는 해당 항목을 동적 상태로 설정할 것입니다. 이를 통해 파이프라인 생성 시점에 이미지 해상도를 고정할 필요 없이 유연하게 대응할 수 있습니다.

`VkGraphicsPipelineCreateInfo`는 컴퓨트 파이프라인을 구성할 때와 동일한 `VkPipelineLayout`을 인자로 받습니다.

또한 `VkRenderPass`와 서브패스 인덱스도 받지만, 동적 렌더링을 사용할 것이기 때문에 이를 구성하지는 않을 것입니다. 즉, `VkRenderPass`와 관련된 모든 절차를 건너뛸 것입니다. 대신, `VkGraphicsPipelineCreateInfo`를 `pNext`에 연결한 `VkPipelineRenderingCreateInfo`로 확장해야 합니다. 이 구조체는 파이프라인에 사용할 어태치먼트 포맷의 목록을 담습니다.


이제 빌더 함수를 작성해봅시다. 모든 파이프라인 코드는 `vk_pipeline.s.h/cpp`에 있습니다. 챕터 코드를 확인하려면 shared 폴더에서 확인할 수 있습니다.

```cpp
class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
   
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
    VkPipelineRasterizationStateCreateInfo _rasterizer;
    VkPipelineColorBlendAttachmentState _colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo _multisampling;
    VkPipelineLayout _pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo _depthStencil;
    VkPipelineRenderingCreateInfo _renderInfo;
    VkFormat _colorAttachmentformat;

	PipelineBuilder(){ clear(); }

    void clear();

    VkPipeline build_pipeline(VkDevice device);
}
```

파이프라인 빌더는 우리가 추적해야 할 대부분의 상태 정보와 색상 어태치먼트 포맷의 배열, 그리고 셰이더 단계 정보를 담을 것입니다. 실제 CreateInfo 구조체는 `build_pipeline()` 함수에서 채워질 것입니다. `clear()`함수는 모든 상태를 초기값 혹은 비어있는 상태로 설정할 것입니다. 파이프라인 빌더의 생성자가 이 함수를 호출할 것이지만, 수동적으로 초기화가 필요할 때를 대비해 별도의 초기화 함수로 분리해두는 편이 유용합니다.

먼저 `clear()` 함수를 작성해봅시다.

<!-- codegen from tag pipe_clear on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::clear()
{
    // clear all of the structs we need back to 0 with their correct stype

    _inputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

    _rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

    _colorBlendAttachment = {};

    _multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

    _pipelineLayout = {};

    _depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

    _renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

    _shaderStages.clear();
}
```

모든 구조체의 `.sType`을 여기서 명시적으로 설정해주고 있습니다. 그리고 나머지 필드들은 0으로 설정합니다. 이는 C++20 구조체 초기화 문법을 활용한 것으로, 중괄호에서 명시하지 않은 필드는 자동으로 0으로 설정됩니다. Vulkan의 대부분의 Info 구조체는 모든 필드가 0으로 설정되어 있어도 유효한 기본 상태가 되도록 설계되어 있기 때문에 이러한 방식은 매우 적절하게 동작합니다.

이제 `build_pipeline` 함수를 작성합시다. 먼저 구성되지 않아 누락된 info 구조체중 일부를 설정하는 것부터 시작하겠습니다. 

<!-- codegen from tag build_pipeline_1 on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
VkPipeline PipelineBuilder::build_pipeline(VkDevice device)
{
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // setup dummy color blending. We arent using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    // completely clear VertexInputStateCreateInfo, as we have no need for it
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

```

먼저 `VkPipelineViewportStateCreateInfo`를 뷰포트 개수만 채웁니다. 동적 뷰포트 상태에서는 뷰포트나 스텐실 옵션을 채울 필요는 없습니다.

그 후 `VkPipelineColorBlendStateCreateInfo`에서 블렌딩 로직을 기본값으로 채웁니다(우리는 사용하지 않을 것입니다). 그리고 하나의 어태치먼트 블렌딩 작업을 위한 `VkPipelineColorBlendAttachmentState`를 연결합니다. 하나의 어태치먼트 렌더링만을 지원할 것이므로 이정도로 충분합니다. 여러 어태치먼트에 그릴 필요가 있다면 `VkPipelineColorBlendAttachmentState`의 배열을 사용할 수 있습니다.

함수를 계속해 `VkGraphicsPipelineCreateInfo`를 채워봅시다.

<!-- codegen from tag build_pipeline_2 on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    // connect the renderInfo to the pNext extension mechanism
    pipelineInfo.pNext = &_renderInfo;

    pipelineInfo.stageCount = (uint32_t)_shaderStages.size();
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &_rasterizer;
    pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.layout = _pipelineLayout;

```

준비한 모든 구조체를 파이프라인 빌더에 연결하고 `_renderInfo`를 `pNext`에 추가합니다.

다음은 동적 상태를 설정하겠습니다.

<!-- codegen from tag build_pipeline_3 on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
    VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;

    pipelineInfo.pDynamicState = &dynamicInfo;
```

동적 상태를 설정하는 것은 `VkPipelineDynamicStateCreateInfo`구조체를 VkDynamicState 열거형의 배열로 채우는 것으로 이루어집니다. 여기서는 2개의 동적 상태만 사용할 것입니다.

파이프라인을 구성하기 위해 필요한 모든 과정을 마쳤으므로, 생성함수를 호출해봅시다.

<!-- codegen from tag build_pipeline_4 on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
    // its easy to error out on create graphics pipeline, so we handle it a bit
    // better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
            nullptr, &newPipeline)
        != VK_SUCCESS) {
        fmt::println("failed to create pipeline");
        return VK_NULL_HANDLE; // failed to create graphics pipeline
    } else {
        return newPipeline;
    }
```

그리고 이것으로 주요 생성 함수는 끝났습니다. 이제 실제로 옵션들을 제대로 설정해야 합니다. 현재는 모든 파이프라인 설정이 사실상 비어 있어서, 필요한 옵션이 누락되어 그대로 실행하면 오류가 발생할 것입니다.

<!-- codegen from tag set_shaders on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
    _shaderStages.clear();

    _shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));

    _shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}
```

정점 셰이더와 프래그먼트 셰이더를 설정하는 함수를 추가해봅시다. 컴퓨트 파이프라인을 구성할 때 사용했던 방식과 마찬가지로, 이 셰이더들을 적절한 정보와 함께 `_shaderStages` 배열에 추가합니다.

다음은 입력 토폴로지를 설정하는 함수를 추가합니다.

<!-- codegen from tag set_topo on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_input_topology(VkPrimitiveTopology topology)
{
    _inputAssembly.topology = topology;
    // we are not going to use primitive restart on the entire tutorial so leave
    // it on false
    _inputAssembly.primitiveRestartEnable = VK_FALSE;
}
```

`VkPrimitiveTopology`는 `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST`, `VK_PRIMITIVE_TOPOLOGY_POINT_LIST` 등의 옵션이 있습니다. PrimitiveRestart는 triangle strip이나 line strip에 사용되지만 여기서는 사용하지 않을 것입니다.

래스터화 상태는 꽤 크므로 이를 몇 개의 옵션으로 나누겠습니다.

<!-- codegen from tag set_poly on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_polygon_mode(VkPolygonMode mode)
{
    _rasterizer.polygonMode = mode;
    _rasterizer.lineWidth = 1.f;
}
```

`lineWidth`를 기본적으로 1.f로 설정해야 합니다. 그 후 와이어 프레임, 솔리드 렌더링, 그리고 점 렌더링 중 어떤 방식으로 렌더링할지를 제어하는 polygon 모드를 설정합니다.

<!-- codegen from tag set_cull on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace)
{
    _rasterizer.cullMode = cullMode;
    _rasterizer.frontFace = frontFace;
}
```

컬링 모드는 전면 방향과 후면 컬링에 사용할 컬링 모드를 설정합니다.

다음은 멀티샘플링 상태입니다. 기본적으로는 멀티샘플링이 비활성화된 구조체를 사용할 것입니다. 이후 안티앨리어싱을 위해 다양한 멀티샘플링 레벨을 활성화하는 함수를 추가할 수 있습니다.

<!-- codegen from tag set_multisample on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_multisampling_none()
{
    _multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling defaulted to no multisampling (1 sample per pixel)
    _multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampling.minSampleShading = 1.0f;
    _multisampling.pSampleMask = nullptr;
    // no alpha to coverage either
    _multisampling.alphaToCoverageEnable = VK_FALSE;
    _multisampling.alphaToOneEnable = VK_FALSE;
}
```

다음은 블렌딩 모드를 위한 함수를 추가하겠습니다.

<!-- codegen from tag set_noblend on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::disable_blending()
{
    // default write mask
    _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // no blending
    _colorBlendAttachment.blendEnable = VK_FALSE;
}
```

`disable_blending()`함수는 `blendEnable`을 `false`로 설정하지만, 올바른 `writeMask`도 설정해야 합니다. 이후 더 많은 블렌딩 모드를 추후 추가할 것이기 때문에 여기서 올바른 `colorWriteMask`를 설정하여 픽셀 출력이 어태치먼트로 정확히 기록되도록 할 것입니다.

이제 포맷을 연결하고, 깊이 테스트와 색상 어태치먼트를 위한 함수를 추가하겠습니다.

<!-- codegen from tag set_formats on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::set_color_attachment_format(VkFormat format)
{
    _colorAttachmentformat = format;
    // connect the format to the renderInfo  structure
    _renderInfo.colorAttachmentCount = 1;
    _renderInfo.pColorAttachmentFormats = &_colorAttachmentformat;
}

void PipelineBuilder::set_depth_format(VkFormat format)
{
    _renderInfo.depthAttachmentFormat = format;
}
```

색상 어태치먼트의 경우 파이프라인이 색상 어태치먼트 배열을 원하기 때문에 포인터를 전달합니다. 이는 여러 이미지를 한번에 그리는 디퍼드 렌더링과 같은 곳에서 유용하지만, 지금은 필요하지 않으므로 기본적으로 하나의 색상 포맷만을 사용합니다.

마지막으로, 깊이 테스트 로직을 비활성화하는 함수가 필요합니다.

<!-- codegen from tag depth_disable on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::disable_depthtest()
{
    _depthStencil.depthTestEnable = VK_FALSE;
    _depthStencil.depthWriteEnable = VK_FALSE;
    _depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;
}
```

파이프라인 빌더의 모든 기본 기능이 채워졌으므로, 이제 삼각형을 그릴 수 있습니다. 이번에는 정점 셰이더에서 하드코딩된 정점 위치를 사용하고, 출력은 단색으로 설정할 것입니다

다음은 사용할 셰이더입니다.

colored_triangle.vert
<!-- codegen from tag all on file E:\ProgrammingProjects\vulkan-guide-2\shaders/colored_triangle.vert --> 
```cpp
#version 450

layout (location = 0) out vec3 outColor;

void main() 
{
	//const array of positions for the triangle
	const vec3 positions[3] = vec3[3](
		vec3(1.f,1.f, 0.0f),
		vec3(-1.f,1.f, 0.0f),
		vec3(0.f,-1.f, 0.0f)
	);

	//const array of colors for the triangle
	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(00.f, 0.0f, 1.0f)  //blue
	);

	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex], 1.0f);
	outColor = colors[gl_VertexIndex];
}
```

colored_triangle.frag
<!-- codegen from tag all on file E:\ProgrammingProjects\vulkan-guide-2\shaders/colored_triangle.frag --> 
```cpp
#version 450

//shader input
layout (location = 0) in vec3 inColor;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	//return red
	outFragColor = vec4(inColor,1.0f);
}
```



정점 셰이더에는 하드코딩된 위치 배열이 있으며, `gl_VertexIndex`를 통해 해당 배열에 인덱싱합니다. 이 방식은 컴퓨트 셰이더에서의 LocalThreadID와 유사한 방식으로 작동합니다. 정점 셰이더가 호출될 때마다 서로 다른 인덱스가 주어지며, 이를 활용해 정점을 처리하고 고정 함수 변수인 `gl_Position`에 값을 기록합니다. 배열의 길이가 3이기 때문에, 만약 3개 이상의 정점(1개 이상의 삼각형)을 렌더링한다면 에러가 발생할 것입니다.

프래그먼트 셰이더에서는 `layout = 0`에 출력값을 선언하고(이는 렌더패스에서 설정한 어태치먼트에 연결됩니다), 단순히 하드코딩된 빨간색을 출력합니다.

이제 삼각형을 그릴 때 필요한 파이프라인과 파이프라인 레이아웃을 만들겠습니다. 새로운 셰이더 파일을 추가했으므로 CMake 프로젝트를 재빌드하여 Shaders 타겟을 빌드해야 합니다.

`VulkanEngine` 클래스에 `init_triangle_pipeline()`함수를 추가하고, 파이프라인과 파이프라인 레이아웃을 담는 멤버도 추가할 것입니다. 

```cpp
VkPipelineLayout _trianglePipelineLayout;
VkPipeline _trianglePipeline;

void init_triangle_pipeline();
```

`init_triangle_pipeline()`을 `init_pipelines()`함수에서 호출할 것입니다.

이제 해당 함수를 작성해보겠습니다. 컴퓨트 셰이더에서와 마찬가지로 함수를 두 셰이더를 불러오는 것으로 시작할 것입니다. 다만 이번에는 더 많은 셰이더를 사용합니다.

<!-- codegen from tag triangle_shaders on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle.frag.spv", _device, &triangleFragShader)) {
		fmt::print("Error when building the triangle fragment shader module");
	}
	else {
		fmt::print("Triangle fragment shader succesfully loaded");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle.vert.spv", _device, &triangleVertexShader)) {
		fmt::print("Error when building the triangle vertex shader module");
	}
	else {
		fmt::print("Triangle vertex shader succesfully loaded");
	}
	
	//build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_trianglePipelineLayout));
```

또한 파이프라인 레이아웃도 생성합니다. 이전의 컴퓨트 셰이더와는 달리, 이번에는 푸시 상수나 디스크립터 바인딩이 전혀 없기 때문에 완전히 비어있는 레이아웃입니다.

이제 앞서 만든 `PipelineBuilder`를 사용해 파이프라인을 생성하겠습니다. 

<!-- codegen from tag triangle_pip_1 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	PipelineBuilder pipelineBuilder;

	//use the triangle layout we created
	pipelineBuilder._pipelineLayout = _trianglePipelineLayout;
	//connecting the vertex and pixel shaders to the pipeline
	pipelineBuilder.set_shaders(triangleVertexShader, triangleFragShader);
	//it will draw triangles
	pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	//filled triangles
	pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
	//no backface culling
	pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	//no multisampling
	pipelineBuilder.set_multisampling_none();
	//no blending
	pipelineBuilder.disable_blending();
	//no depth testing
	pipelineBuilder.disable_depthtest();

	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

	//finally build the pipeline
	_trianglePipeline = pipelineBuilder.build_pipeline(_device);

	//clean structures
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
		vkDestroyPipeline(_device, _trianglePipeline, nullptr);
	});
```

파이프라인이 생성되었으므로, 매 프레임마다 생성되는 커맨드 버퍼에서 삼각형을 그릴 수 있습니다.

백그라운드에 사용된 컴퓨트 셰이더는 GENERAL 레이아웃의 이미지에 작성해야 했지만, 도형(geometry) 렌더링을 수행할 때에는 `COLOR_ATTACHMENT_OPTIMAL`을 사용해야 합니다. 그래픽스 파이프라인에서 GENERAL 레이아웃에 그릴수는 있지만 성능이 낮고, 검증 레이어가 경고 메시지를 출력하게 됩니다. 이러한 그래픽 명령을 담기 위해 새로운 함수 `draw_geometry()`를 만들고, 먼저 렌더링 루프를 업데이트 하겠습니다.

<!-- codegen from tag draw_barriers on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	// transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	draw_background(cmd);

	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	draw_geometry(cmd);

	//transtion the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
```

이제 `draw_geometry` 함수를 채웁시다.

<!-- codegen from tag draw_geo on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
void VulkanEngine::draw_geometry(VkCommandBuffer cmd)
{
	//begin a render pass  connected to our draw image
	VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(_drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkRenderingInfo renderInfo = vkinit::rendering_info(_drawExtent, &colorAttachment, nullptr);
	vkCmdBeginRendering(cmd, &renderInfo);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);

	//set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = _drawExtent.width;
	viewport.height = _drawExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(cmd, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = _drawExtent.width;
	scissor.extent.height = _drawExtent.height;

	vkCmdSetScissor(cmd, 0, 1, &scissor);

	//launch a draw command to draw 3 vertices
	vkCmdDraw(cmd, 3, 1, 0, 0);

	vkCmdEndRendering(cmd);
}
```

삼각형을 그리기 위해 `vkCmdBeginRendering`을 호출해 렌더패스를 시작해야 합니다. 이는 지난 챕터에서의 ImGui에서와 동일하지만, 이번에는 스왑체인 이미지가 아니라 `_drawImage`를 가리킵니다.

`vkCmdBindPipeline`를 호출할 때, 이전처럼 `VK_PIPELINE_BIND_POINT_COMPUTE`이 아니라 `VK_PIPELINE_BIND_POINT_GRAPHICS`를 사용해야 합니다. 이후 뷰포트와 가위를 설정해야 합니다. 이는 파이프라인 생성 시 동적 파이프라인 상태를 사용했기 때문에 필수적으로 설정해야 하는 부분입니다. 설정이 완료되면 `vkCmdDraw`를 호출해 삼각형을 그릴 수 있고, 이후 렌더패스를 종료하여 그리기 작업을 마무리할 수 있습니다.

이 시점에서 프로그램을 실행한다면 컴퓨트 셰이더로 그린 배경 위에 삼각형이 렌더링 된 것을 확인할 수 있습니다.

![triangle]({{site.baseurl}}/diagrams/ColorTri2.png)

Next: [ Mesh buffers]({{ site.baseurl }}{% link docs/ko/new_chapter_3/mesh_buffers.md %})  

{% include comments.html term="Vkguide 2 Korean Comments" %}
