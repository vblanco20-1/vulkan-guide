---
layout: default
title: Executing Vulkan Commands
parent:  "1. Initializing Vulkan - KO"
nav_order: 20
---


![cmake]({{site.baseurl}}/diagrams/vkcommands.png)

## Vulkan 명령 실행
OpenGL이나 DirectX11 이전 버전과는 달리 Vulkan에서는 모든 GPU 명령이 커맨드 버퍼(Command buffer)를 통해 전달되며, 큐(Queue)를 통해 실행됩니다.

명령을 실행하는 일반적인 흐름은 다음과 같습니다.:
- `VkCommandPool`로부터 `VkCommandBuffer`를 할당합니다.
- `VkCmdXXXXX` 함수를 사용하여 명령을 커맨드 버퍼에 기록합니다.
- `VkQueue`로 커맨드 버퍼를 제출해 명령을 실행을 시작합니다.

같은 커맨드 버퍼를 여러 번 제출하는 것도 가능합니다. 많은 튜토리얼과 예제에서는 흔히 커맨드 버퍼를 한번만 작성하여 렌더링 루프에서 매 프레임마다 제출하는 방식을 사용합니다. 하지만 이 튜토리얼에서는 매 프레임마다 명령을 작성할 것이며, 이는 실제 렌더링 엔진의 동작 방식과 더 유사하기 때문입니다.

Vulkan에서 명령을 기록하는 것은 상대적으로 비용이 적게 듭니다. 일반적으로 비용이 많이 드는 작업은 VkQueueSubmit 호출로, 이 과정에서 드라이버가 커맨드 버퍼의 유효성을 검사하고 GPU에서 실행하기 때문입니다.

커맨드 버퍼에서 매우 중요한 점은 병렬로 기록할 수 있다는 것입니다. 여러 쓰레드에서 서로 다른 커맨드 버퍼를 안전하게 기록할 수 있습니다. 이를 위해 각 쓰레드는 최소한 하나의 `VkCommandPool`과 하나의 `VkCommandBuffer`를 가져야 하며, 각 쓰레드가 자신만의 고유한 커맨드 버퍼와 커맨드 풀(Command Pool)을 사용해야 합니다. 올바르게 설정하면 각 쓰레드에서 커맨드 버퍼를 제출할 수 있습니다. `vkQueueSubmit`이 쓰레드 안전(thread-safe)하지 않기 때문에, 하나의 큐에는 한번에 하나의 쓰레드만 명령을 제출할 수 있습니다. 대규모 엔진에서는 흔히 백그라운드 쓰레드에서 제출을 처리하며, 메인 렌더링 루프 쓰레드가 실행될 수 있도록 합니다.

## VkQueue
Vulkan에서의 큐는 GPU의 실행 포트입니다. 모든 GPU는 사용가능한 여러 큐를 가지며, 서로 다른 명령을 동시에 실행할 수도 있습니다. 서로 다른 큐에 제출된 명령들은 동시에 실행될 수 있습니다. 이는 메인 프레임 루프와 직접적으로 연관되지 않는 백그라운드 작업을 수행할 때 매우 유용합니다. 이를 위해 백그라운드 작업 전용 `VkQueue`를 생성하여 일반적인 렌더링 작업과 분리할 수 있습니다.

Vulkan의 모든 큐는 큐 패밀리로부터 파생됩니다. 큐 패밀리는 특정 유형의 큐 그룹이며, 지원하는 명령의 종류가 각기 다릅니다.

GPU는 각기 다른 큐 패밀리를 지원합니다. 예를 들어 NVIDIA GT 750TI의 경우, 이 [링크](https://vulkan.gpuinfo.org/displayreport.php?id=8859#queuefamilies)에서 확인할 수 있습니다. 이 GPU에는 2개의 큐 패밀리가 있으며, 하나는 모든 기능을 지원하는 16개의 큐로 이루어져 있으며, 다른 하나는 전송(Transfer)만을 지원하는 1개의 큐로 이루어진 큐 패밀리가 있습니다. 반면, 고성능 AMD GPU는 [여기](https://vulkan.gpuinfo.org/displayreport.php?id=24407#queuefamilies)를 참고할 수 있습니다.이 GPU에는 5개의 큐 패밀리가 있으며, 각 유형별로 최대 2개의 큐만 존재합니다. 1개의 큐는 모든 기능을 지원하고, 계산(Compute) 및 전송(Transfer) 작업을 지원하는 2개의 큐, 2개의 전송 전용 큐, 2개의 표시(Present) 전용 큐가 있습니다. 이처럼, 각 GPU가 지원하는 큐 패밀리는 상당히 다를 수 있습니다.

대부분의 엔진에서는 3개의 큐 패밀리를 사용하는 것이 일반적입니다. 하나는 프레임을 그리는 데 사용하고, 다른 하나는 비동기 계산(compute), 그리고 다른 하나는 데이터 전송입니다. 이 튜토리얼에서는 단순화를 위해 하나의 큐에서 모든 명령을 실행하겠습니다.

## VkCommandPool
`VkCommandPool`은 `VkDevice`를 기반으로 생성되며, 해당 커맨드 풀이 명령을 생성할 큐 패밀리의 인덱스가 필요합니다.

`VkCommandPool`을 `VkCommandBuffer`의 할당기라고 생각할 수 있습니다. 하나의 풀에서 원하는 만큼 `VkCommandBuffer`를 할당할 수 있지만, 한 번에 하나의 쓰레드만 명령을 기록할 수 있습니다. 만약 멀티쓰레드 환경에서 명령을 기록하고자 한다면 추가적인 `VkCommandPool` 객체가 필요합니다. 이러한 이유로, 해당 커맨드 풀과 쌍으로 구성할 것입니다.

## VkCommandBuffer

GPU의 모든 명령은 `VkCommandBuffer`에 기록됩니다. GPU의 작업을 수행하는 모든 함수는 커맨드 버퍼가 `VkQueueSubmit`을 통해 GPU로 제출될 때까지 실행되지 않습니다.

커맨드 버퍼는 준비(Ready) 상태에서부터 시작합니다. 준비 상태에서는 `vkBeginCommandBuffer()`를 호출해 기록(Recording) 상태로 변경할 수 있습니다. 이 상태에서 `vkCmdXXXXX`함수를 호출해 명령을 기록할 수 있습니다. 모든 명령을 기록하고 나면 `vkEndCommandBuffer()`를 호출하여 실행가능한(Executable) 상태로 변경하면 해당 커맨드 버퍼를 GPU에 제출할 수 있습니다.

커맨드 버퍼를 제출하려면 제출할 명령과 큐를 지정하여 `vkQueueSubmit()`를 호출합니다. 또한, `vkQueueSubmit`은 여러 개의 커맨드 버퍼를 한 번에 제출할 수도 있습니다. 제출한 모든 커맨드 버퍼는 보류(Pending) 상태로 변환됩니다.

커맨드 버퍼가 제출되면 여전히 "유효(alive)"하며 GPU에서 처리되고 있습니다. 이 시점에서는 커맨드 버퍼를 리셋하는 것이 안전하지 않습니다. 커맨드 버퍼를 재설정하고 재사용 하기 전에, GPU가 해당 커맨드 버퍼의 모든 명령을 완료했는 지 확인해야 합니다.

커맨드 버퍼를 리셋하기 위해서 `vkResetCommandBuffer()`를 사용합니다.

커맨드 버퍼가 실행중인 동안 다음 프레임을 계속 그릴 수 있도록 더블 버퍼링을 수행할 것입니다. 이렇게 하면 GPU가 한 버퍼에서 렌더링 및 처리를 수행하는 동안, 다른 버퍼에서 명령을 기록할 수 있습니다.

커맨드 버퍼의 생명주기에 대한 더 자세한 정보는 다음 Vulkan 사양의 관련 글을 참고하세요.

<https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap6.html#commandbuffers-lifecycle>.


Next: [ Setting up Vulkan commands]({{ site.baseurl }}{% link docs/ko/new_chapter_1/vulkan_commands_code.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}


