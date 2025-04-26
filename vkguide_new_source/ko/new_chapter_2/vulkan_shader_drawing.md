---
layout: default
title: Vulkan Shaders
parent:  "2. Drawing with Compute"
nav_order: 3
---

## GPU에서 코드 실행하기
이제 렌더링 루프가 구현되었으니, 다음 단계는 실제로 무언가를 그리는 것입니다.

도형을 바로 그리는 것보다는 컴퓨트 셰이더를 사용해 이미지에 데이터를 기록하고, 이를 화면에 표시해보겠습니다. 이미지를 계산하는 용도의 컴퓨트 셰이더는 엔진의 복잡한 후처리에서 흔히 볼 수 있는 사용 사례입니다. 또한 개발자가 레이트레이싱이나 도형이 아닌 다른 것을 그릴 때에도 흔히 볼 수 있습니다. 

## VkPipeline
Vulkan에서는 GPU에서 코드를 실행하기 위해 파이프라인을 설정해야 합니다. 그래픽스와 컴퓨트 두 종류의 파이프라인이 있습니다. 컴퓨트 파이프라인은 꽤 단순합니다. 셰이더 코드에 전달할 데이터, 데이터 바인딩에 사용할 디스크립터 레이아웃만 필요합니다. 반면 그래픽스 파이프라인은 블렌딩, 깊이 테스트, 도형 포맷 등 GPU의 모든 고정 기능 하드웨어를 위한 상당한 상태를 구성해야 합니다. 그래픽스 파이프라인은 다음 장에서 다룰 예정입니다.

두 파이프라인은 셰이더 모듈과 레이아웃을 동일한 방식으로 생성하며, 이를 공유할 수 있습니다.

## VkShaderModule
`VkShaderModule`은 전처리된 셰이더 파일입니다. 컴파일된 SpirV 파일을 통해 생성할 수 있습니다. OpenGL과 달리 Vulkan에서는 GLSL 셰이더 코드를 직접 전달할 수 없습니다. 이를 가능케하는 확장도 존재하기는 하지만 표준은 아닙니다. 따라서 GLSL 파일을 미리 컴파일하여 SpirV파일로 만들어야 합니다. Vulkan SDK에는 glslangValidator가 포함되어 있으며, 이를 사용해 GLSL을 SpirV로 컴파일할 수 있습니다. vkguide 코드에서 메인 프로젝트의 CMakeLists.txt를 보면 /shaders/ 폴더에서 모든 셰이더 파일을 가져와 컴파일 타겟으로 추가하는 커스텀 명령을 볼 수 있습니다. 

이 튜토리얼에서 셰이더를 컴파일하려면 셰이더 타겟을 빌드해야 합니다. 이는 작업은 모든 셰이더 파일을 컴파일 합니다. CMake의 한계로 인해 새로운 셰이더 파일을 추가하면 CMake 설정 단계를 재실행하여 CMake가 해당 셰이더 파일을 가져올 수 있도록 해야 합니다. 다른 엔진에서는 셰이더 컴파일을 자동화하기 위해 엔진이 시작될 때 자동으로 실행가능한 파일 혹은 .bat, 혹은 다른 스크립트 파일을 사용하는 것이 일반적입니다.

Vulkan 셰이더의 코드로 GLSL 대신 HLSL을 사용하는 것도 가능합니다. 많은 프로젝트가 이러한 방식을 선호하지만, 이 튜토리얼에서는 다루지 않습니다. 만약 HLSL을 Vulkan에서 사용하는 방법에 대해 더 알고 싶다면, [HLSL In Vulkan](https://docs.vulkan.org/guide/latest/hlsl.html)를 참고해보세요.

## Descriptor sets 
셰이더에 데이터를 전달하려면 바인딩을 설정해야 합니다. Vulkan에서는 이미지나 버퍼 같은 객체를 셰이더로 바인딩하기 위해 디스크립터 셋(DescriptorSet)이 필요합니다. 하나의 디스크립터는 자원(버퍼나 이미지 등)에 대한 핸들 혹은 포인터라고 생각하면 됩니다. 이는 버퍼의 크기나 이미지의 샘플러(sampler) 타입과 같은 정보도 담고 있습니다. `VkDescriptorSet`은 이러한 포인터들을 묶어놓은 집합입니다. Vulkan은 셰이더로 개별 리소스를 직접 바인딩하는 것을 허용하지 않습니다. 디스크립터 셋은 `VkDescriptorSetLayout`을 기반으로 `VkDescriptorPool`에서 할당됩니다. `VkDescriptorSetLayout`은 디스크립터 셋이 어떤 자원을 담는지를 정의합니다.(예 : 이미지 2개) 디스크립터 셋이 할당되면 `vkUpdateDescriptorSets`를 사용해 데이터를 갱신 할 수 있으며, 이때 `VkWriteDescriptorSet`의 배열을 넘깁니다. 디스크립터 셋을 구성이 완료되면 파이프라인에 `VkBindDescriptorSets`를 사용해 바인딩할 수 있으며 셰이더에서 데이터를 사용할 수 있습니다.

<div class="mxgraph" style="max-width:100%;border:1px solid transparent;" data-mxgraph="{&quot;highlight&quot;:&quot;#0000ff&quot;,&quot;nav&quot;:true,&quot;resize&quot;:true,&quot;toolbar&quot;:&quot;zoom layers lightbox&quot;,&quot;edit&quot;:&quot;_blank&quot;,&quot;xml&quot;:&quot;&lt;mxfile host=\&quot;app.diagrams.net\&quot; modified=\&quot;2020-10-02T16:02:40.267Z\&quot; agent=\&quot;5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\&quot; etag=\&quot;G0Tl161wDfz5oobdgIFU\&quot; version=\&quot;13.7.7\&quot; type=\&quot;device\&quot;&gt;&lt;diagram id=\&quot;6hGFLwfOUW9BJ-s0fimq\&quot; name=\&quot;Page-1\&quot;&gt;7Vpdk5owFP01PG4HCCA+6qptZ7Yzne706zFKRLrR0BBX6a9vAkEgQdc6wLRs9UFyE0hyzj03N0ED3G+PbymMNx9IgLBhm8HRADPDti0bjPiPsKS5xfNAbghpFMhGpeEx+oWk0ZTWfRSgpNaQEYJZFNeNK7LboRWr2SCl5FBvtia43msMQ6QZHlcQ69avUcA2udW3R6X9HYrCTdGz5Y3zmiVcPYWU7HeyP8MGzkx88+otLJ4lJ5psYEAOlU7B3AD3lBCWX22P9wgLbAvY8vsWZ2pP46Zox665AXhyHCwt5o4CDoUsEso2JCQ7iOeldZrND4knmLxUtnkgJOZGixt/IMZSySvcM8JNG7bFsnZNduyeYEKzHsHcE19uTxglT6hSMxuLL6/R5yWnmpA9XcmRS3bEBFREuaciskWMprwBRRiy6LnONZQuE57anW79SCLeq21K93ZdSZ50bsc0649gkIaIybtK8CeUwrTSLBYNkgv9mM39LG5rzy/yERSlCialKfOPZl+R8D5DvJfQfXmaoWRFo5gR+oiY5kqlowjWD5uIoccYZnQdeOBQnCLCuEI9tE3TdpucwluY/KO70Tr7XHKWZ0QZOl70DVkLnDqWYCTLhzIcWIWIN5VQ4Cm+UHWnChMXgbZvESWPJLGoXWN0nIgQ2IZQOYA0/Sbuf+MWxe/ycVlhdqyVUlnqSOC+XAoyfXWk+atJ8jU5TDAmK8iQYXuYD2+6pDUSvZ97EdYzcO6SDPIJb2D58TEDoKjnVyE7QaI4wQNc8rW2RhLEUbjj1ysOHeK4ToWbR3wxm8iKbRQEeeBGvFu4zJ4naJIRiD/cnRruTI5NeoPlN9JSOKempdNqLDso1p6zGrsz3wBzbNV0dmfnxVtDdNGErNcJYhqffxbwdIarAe8jT0gGHPFO0a2PiAf8AaUhwO4nD/HGdcY0JlrKQ0ZKXgHcy3mI2r4YV1t5SIFvRZdmHkqn+/UaiaBbqnRACnU9hQfnSoXaLSi06GsYCgX9KNT3blPoLaIAmiisVymKaxP1VkRhaUj+w6JwehKFun12uhOFo4nCzkXxfitOgGwPboVz75aJ+HkVAjmdVvQiEDAkgbj9CGQE6oy5oDuBuJpAwGtcNRzQoyiAfpD2juAg0fDlk2HdbfWFbhK51W9XNNfz4Izc+urt6zy4XdEw1miYfl4s5p+qxzdnTmJe8PUWkNE2dw0eCrqCxtF3WO8/TN7O/wpkfHAmf6gg4zUA08Y5haMnFH+Rz2gLR9MGsQkadYG5CRpPg2aZirNQkW+8CM7wQ93IUgTdsFHpTM/6ijPNuXH/s5MHldHL7DQppw123CElyY48ye06SQbq0YrbWZLsXH4PkL34VCX0AFOy11+IvrAGqCJigss23NtSgo81vjLftVRYbwo/eqalHNgOZmNhmeO6W/a5sSi4On8GOByc1dXU6xNnS8O5fqw0XJivjRutwKzvQsArcWdw7cvXVnDWNzXZsolo8j83NPSteNMhRUu5IS+W//3LU5XyD5Zg/hs=&lt;/diagram&gt;&lt;/mxfile&gt;&quot;}"></div>
<script type="text/javascript" src="https://viewer.diagrams.net/js/viewer-static.min.js"></script>

이 챕터에서는 렌더링할 이미지를 컴퓨트 셰이더에 연결하여 셰이더가 해당 이미지에 직접 쓸 수 있도록 하겠습니다. 튜토리얼 전반에 걸쳐 이러한 흐름을 단순화하는 추상화를 작성할 것입니다. 파이프라인은 여러 개의 디스크립터 셋을 바인딩할 수 있는 슬롯을 갖습니다. Vulkan 명세에서는 적어도 4개의 디스크립터 셋을 보장하며, 이 튜토리얼에서는 이 수를 기준으로 삼겠습니다. GPU 제조사에 따르면 각 디스크립터 셋 슬롯은 일정한 비용이 들 수 있으므로, 가능한 한 적게 사용하는 것이 좋습니다. 이후의 튜토리얼에서는 디스크립터 셋 0번에는 유니폼 버퍼와 일부 특수 텍스쳐를 포함하는 전역 씬 데이터를 바인딩할 것이고, 디스크립터 셋 1번에는 개별 오브젝트 데이터를 바인딩하는 용도로 사용하겠습니다.

## 푸시 상수
셰이더에 버퍼와 이미지를 전달하는 데 사용되는 디스크립터 셋 외에도, Vulkan은 커맨드 버퍼를 기록하는 과정 중에 직접 몇 바이트의 데이터를 쓸 수 있는 기능을 제공합니다. 이는 푸시 상수(Push Constants)라 불리며, Vulkan에만 존재하는 독특한 방식입니다. 셰이더에 직접 바인딩되는 아주 소량의 메모리 공간을 예약하는 구조로, 속도는 빠르지만 매우 제한된 용량만 사용할 수 있으며, 명령을 인코딩하는 시점에 작성되어야만 합니다. 주로 객체가 많은 데이터를 요구하지않을 때, 객체별 데이터 전달에 사용할 수 있습니다. GPU 제조사에 따르면 이상적인 용례는 셰이더로 인덱스를 전달해 더 큰 데이터 버퍼에 접근할 때 사용하는 것입니다.

## 파이프라인 레이아웃 
셰이더는 다양한 입력이 필요하며, 이러한 입력 구조는 `VkPipelineLayout`이 정의합니다. 파이프라인 레이아웃을 생성하기 위해 `VkDescriptorSetLayout`과 푸시 상수에 사용되는 `PushConstantRange` 정의가 필요합니다. 그래픽스 파이프라인과 컴퓨트 파이프라인의 파이프라인 레이아웃은 같은 방식으로 레이아웃을 생성하며, 실제 파이프라인이 생성되기 전에 먼저 생성되어야 합니다.

## 컴퓨트 파이프라인
컴퓨트 파이프라인을 구성하기 위해서는 파이프라인 레이아웃을 먼저 생성하고, 셰이더 모듈을 연결해야 합니다. 파이프라인이 만들어지면 컴퓨트 셰이더를 `VkCmdBindPipeline`를 호출해 바인딩하고 `VkCmdDispatch`를 호출해 실행할 수 있습니다.

컴퓨트 셰이더는 특수한 프로그래밍 모델을 따릅니다. `VkCmdDispatch` 호출 시, Vulkan에게 X, Y, Z 형태로 작업 그룹의 수를 지정합니다. 이 튜토리얼에서는 이미지를 처리할 것이므로 2개의 차원만 사용합니다. 이미지의 각 픽셀 그룹마다 하나의 작업 그룹이 실행되도록 설정합니다.

셰이더 내부에서는 다음과 같은 코드가 보입니다. `layout (local_size_x = 16, local_size_y = 16) in;`. 이는 각 작업 그룹이 16 * 16 쓰레드로 구성됨을 의미합니다. 즉, vkCmdDispatch로 한 작업 그룹이 호출되면, 16 * 16개의 쓰레드가 병렬로 실행되어  16 * 16 픽셀 사각형을 처리할 수 있습니다. 

셰이더 코드에서는 `gl_LocalInvocationID` 변수를 통해 현재 쓰레드 인덱스에 접근할 수 있습니다. `gl_GlobalInvocationID`와 `gl_WorkGroupID`도 있습니다. 이러한 변수들을 사용하여 각 쓰레드가 어떤 픽셀을 처리할 지 정확히 찾을 수 있습니다.

이제 실제 코드를 살펴보며 어떻게 셰이더가 작동하는 지 알아보겠습니다.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}