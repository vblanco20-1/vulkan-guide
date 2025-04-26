---
layout: default
title: Vulkan Usage - KO
nav_order: 2
parent: "introduction - ko"
grand_parent: "Korean VKGuide"
---

# Vulkan 핵심 객체와 활용 방법

- `VkInstance` : Vulkan 문맥(context)으로, GPU 드라이버에 접근하는 데 사용됩니다.
- `VkPhysicalDevice` : GPU 그 자체입니다. 기능, 호환성, 메모리 크기, 그 이외의 여러 GPU 세부 사항을 조회하는 데 사용됩니다.
- `VkDevice` : GPU를 실제로 실행하는데 사용할 "논리적인" GPU 문맥입니다.
- `VkBuffer` : GPU가 접근할 수 있는 메모리 묶음입니다.
- `VkImage` : 읽고 쓸 수 있는 텍스쳐입니다.
- `VkPipeline` : 그리기에 필요한 GPU 상태를 담고 있는 개겣입니다.(ex. 셰이더, 래스터화, 깊이 설정)
- `VkRenderPass` : 렌더링할 이미지의 정보를 담고있습니다. 모든 그리기 명령은 렌더패스(renderpass) 내부에서 수행됩니다.
- `VkFrameBuffer` : 렌더패스의 타겟 이미지를 담습니다. 구버전 vkguide에서만 사용됩니다.
- `VkCommandBuffer` : GPU 명령을 기록합니다. (드라이버가 아닌)GPU에서 실행되는 모든 명령은 `VkCommandBuffer`에 기록됩니다.
- `VkQueue` : 명령의 실행 지점입니다. GPU는 다른 속성의 큐 모음을 갖습니다. 어떤 큐는 그래픽 명령만 받으며, 다른 어떤 큐는 메모리 명령만 받습니다. 예를 들어 커맨드 버퍼는 큐에 제출되어 실행되는데, 이는 GPU로 렌더링 명령을 전달합니다.
- `VkDescriptorSet` : `VkBuffer`자원이나 `VkImage`텍스쳐와 같은 데이터를 셰이더로 연결하는 데 필요한 정보를 담습니다. GPU에서의 포인터라고 생각해도 좋습니다.
- `VkSwapchainKHR` : 화면의 이미지를 담는데 사용됩니다. 렌더링 한 것을 시각적으로 볼 수 있는 창에 렌더링 할 수 있게 합니다. `KHR`접미사는 Vulkan 확장의 객체임을 나타냅니다. 여기서는 `Vk_KHR_swapchain`입니다.
- `VkSemaphore` : GPU간 명령 동기화에 사용됩니다. 여러 커맨드 버퍼가 있을 때 하나가 실행되고 그 다음 것이 실행하는 것을 보장하기 위해 사용됩니다.
- `VkFence` : GPU와 CPU를 동기화하는 데 사용됩니다. 커맨드 버퍼가 GPU에서 실행이 완료되었는지를 확인할 때 사용합니다.

# 고수준 Vulkan 애플리케이션 흐름

## 엔진 초기화
먼저, 모든 것을 초기화합니다. Vulkan을 초기화하기 위해선 `VkInstance`를 생성하는 것부터 시작합니다. `VkInstance`로부터 컴퓨터의 사용 가능한 `VkPhysicalDevice` 핸들의 목록을 조회합니다. 예를 들어, 컴퓨터가 전용 GPU와 통합 그래픽스를 갖는다면, 각각 `VkPhysicalDevice`가 될 수 있습니다.  `VkPhysicalDevice`로부터 사용 가능한 기능과 제한 사항을 조회한 후 `VkDevice`를 생성합니다. `VkDevice`에서는 커맨드를 실행하게 해주는 `VkQueue` 핸들을 얻을 수 있습니다. 그 후, `VkSwapchainKHR`을 초기화합니다. `VkQueue` 핸들로 커맨드 버퍼를 할당할 수 있는 객체인 `VkCommandPool`을 생성할 수 있습니다.

## 에셋 초기화
핵심 구조체들이 초기화 되고 나면, 렌더링할 대상에 필요한 자원을 초기화해야합니다. 머테리얼(Material)이 로딩되면 `VkPipeline`의 집합을 생성합니다. 이는 머테리얼을 렌더링하는 데 필요한 셰이더 조합과 파라미터를 담습니다. 메시(Mesh)를 렌더링 하기 위해 메시의 정점 데이터를 `VkBuffer` 자원으로, 그리고 메시의 텍스쳐를 `VkImage` 자원으로 업로드해야 합니다. 이 때 이미지가 "읽을 수 있는" 레이아웃임을 분명히 해야합니다. 또한 주요 렌더링 패스에 대해 `VkRenderPass` 객체를 만들어야 합니다. 예를 들어, 주요 렌더링하는데 사용할 `VkRenderPass`가 있을 수 있고, 그림자 패스에 사용할 `VkRenderPass`가 있을 수 있습니다. 특히 파이프라인 생성은 꽤 비싼 편이기 때문에, 실제 엔진에서는 이러한 작업은 백그라운드 쓰레드에서 병렬화 되어 처리될 수 있습니다. 

## 렌더링 루프
이제 렌더링할 준비가 끝났습니다. 먼저 `VkSwapchainKHR`에 렌더링할 이미지를 요청해야 합니다. 그리고 `VkCommandPool`로부터 `VkCommandBuffer`를 할당하거나 이미 할당된 커맨드 버퍼를 재사용해 커맨드를 작성합니다. 다음으로는 `VkRenderPass`를 시작해서 렌더링을 시작합니다. 이는 일반적인 `VkRenderPass` 혹은 동적 렌더링을 사용해 수행할 수 있습니다. 렌더패스는 스왑체인으로부터 받아온 이미지에 렌더링을 수행하도록 지정합니다. 그리고 `VkPipeline`, (셰이더 파라미터에 필요한)`VkDescriptorSet`, 정점 버퍼를 바인딩하고 그리기 호출을 실행할 루프를 만듭니다. 렌더패스에서 그리기가 끝났으면 `VkRenderPass`를 끝냅니다. 렌더링 할 것이 남아있지 않다면 `VkCommandBuffer`도 끝냅니다. 마지막으로 렌더링하기 위해 커맨드 버퍼를 큐로 제출합니다. 그러면 GPU에서 커맨드 버퍼의 명령들의 실행이 시작될 것입니다. 만약 렌더링한 결과를 화면에 표시하고 싶다면, 렌더링한 이미지를 화면에 "표시(present)"해야합니다. 아직 렌더링이 끝나지 않았을 수 있기 때문에, 세마포어(semaphore)를 사용해 렌더링이 끝날 때 까지 화면에 이미지를 표시하는 작업을 대기시켜야 합니다.

Vulkan에서의 렌더링 루프 의사코드입니다.

```cpp
// 스왑체인에 렌더링할 이미지의 인덱스를 요청합니다.
int image_index = request_image(mySwapchain);

// 커맨드 버퍼를 생성합니다.
VkCommandBuffer cmd = allocate_command_buffer();

// 커맨드 버퍼를 초기화합니다.
vkBeginCommandBuffer(cmd, ... );

// 스왑체인으로부터 받아온 이미지 인덱스로 새로운 렌더패스를 시작합니다.
// 각 프레임 버퍼는 스왑체인의 이미지를 참조합니다.
vkCmdBeginRenderPass(cmd, main_render_pass, framebuffers[image_index] );

// 모든 객체를 렌더링합니다.
for(object in PassObjects){

    // 객체를 렌더링하는 데 사용되는 구성과 셰이더를 바인딩합니다.
    vkCmdBindPipeline(cmd, object.pipeline);
    
    // 객체를 렌더링하는 데 사용되는 정점과 인덱스 버퍼를 바인딩합니다.
    vkCmdBindVertexBuffers(cmd, object.VertexBuffer,...);
    vkCmdBindIndexBuffer(cmd, object.IndexBuffer,...);

    // 셰이더 입력으로 사용되는 객체의 디스크립터 셋을 바인딩합니다.
    vkCmdBindDescriptorSets(cmd, object.textureDescriptorSet);
    vkCmdBindDescriptorSets(cmd, object.parametersDescriptorSet);

    // 그립니다.
    vkCmdDraw(cmd,...);
}

// 렌더패스와 커맨드 버퍼를 끝냅니다.
vkCmdEndRenderPass(cmd);
vkEndCommandBuffer(cmd);


// 커맨드 버퍼를 제출하고 GPU에서 실행합니다.
vkQueueSubmit(graphicsQueue, cmd, ...);

// 렌더링한 이미지를 화면에 표시합니다.
// renderSemaphore는 `cmd`가 실행될 때 까지 이미지를 표시하지 않는 것을 보장합니다.
vkQueuePresent(graphicsQueue, renderSemaphore);
```

Next: [Project files and libraries]({{ site.baseurl }}{% link docs/ko/introduction/project_libs.md %})

{% include comments.html term="Vkguide 2 Korean Comments" %}
