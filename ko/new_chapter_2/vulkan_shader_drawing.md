---
layout: default
lang: ko
title: Vulkan Shaders
parent:  "2. 컴퓨트 셰이더 그리기"
nav_order: 3
---

## GPU에서 코드 실행하기
We have the render loop implemented now, so the next step is to draw something.
이제 렌더링 루프가 구현되었습니다. 다음 단계는 무언가를 그리는 것입니다.

Instead of drawing geometry just yet, we are going to use Compute Shaders to write data into a image, and then display that image to the screen. Compute shaders to calculate images is a very common use case that you can see from complex postprocessing chains in engines. Its also common to do when the programmer is going to do raytracing or other non-geometry drawing.
아직 도형을 그리는 것 대신에 컴퓨트 셰이더를 사용해 이미지를 작성한 후, 이를 화면에 표시해보겠습니다. 이미지를 계산하는 용도의 컴퓨트 셰이더는 엔진의 복잡한 후처리에서 흔히 볼 수 있는 사용 사례입니다. 개발자가 레이트레이싱이나 도형이 아닌 다른 것을 그릴 때에도 흔히 볼 수 있습니다. 

## VkPipeline
In vulkan, to execute code on the gpu, we need to setup a pipeline. There are two types of pipelines, Graphics and Compute. The Compute pipelines are much simpler, because they only require the data for the shader code, and the layout for the descriptors used for data bindings. Graphics pipelines, on the other hand, have to configure a considerable amount of state for all of the fixed function hardware in the GPU such as color blending, depth testing, or geometry formats. We will use them next chapter.
Vulkan에서는 GPU에서 코드를 실행하기 위해 파이프라인을 설정해야 합니다. 그래픽스와 컴퓨트 두 종류의 파이프라인이 있습니다. 컴퓨트 파이프라인은 꽤 단순합니다. 셰이더 코드에 전달할 데이터, 데이터 바인딩에 사용할 디스크립터(descriptor)의 레이아웃만 필요합니다. 반면 그래픽스 파이프라인은 색상 혼합, 깊이 테스트, 도형 포맷과 같은 GPU의 모든 고정 함수 하드웨어를 상당한 양의 상태를 구성해야 합니다ㅇ.

Both types of pipelines share the shader modules and the layouts, which are built in the same way. 
두 파이프라인은 같은 방식으로 만들어지는 셰이더 모듈과 레이아웃을 공유합니다.

## VkShaderModule
A VkShaderModule is a processed shader file. We create it from a pre-compiled SpirV file. 
In vulkan, unlike in opengl, the driver does not accept shader code in GLSL directly. There are extensions that allow it, but its not standard. So we need to compile our GLSL files ahead of time into a compiled spirv file. As part of the vulkan SDK, we have the glslangValidator program, which is used to compile GLSL into spirv. If you look at the main project CMakeLists.txt on the vkguide codebase, you can see the section where it grabs all of the shader files from the /shaders/ folder, and adds them as custom commands into a compile target.
VkShaderModule은 처리된 셰이더 파일입니다. 전처리된 SpirV 파일을 통해 생성할 수 있습니다. OpenGL과 달리 Vulkan에서는 GLSL 셰이더 코드를 직접 받지 않습니다. 이를 허용하는 확장도 있지만 표준은 아닙니다. 따라서 GLSL 파일을 미리 컴파일된 SpirV파일로 컴파일할 필요가 있습니다. Vulkan SDK에는 glslangValidator가 들어있는데, 이는 GLSL을 SpirV로 컴파일하는 데 사용됩니다. 만약 vkguide 코드에서 메인 프로젝트의 CMakeLists.txt를 보았다면 /shaders/ 폴더에서 모든 셰이더 파일을 가져와 컴파일 타겟으로 추가하는 커스텀 명령을 볼 수 있습니다. 

Whenever you want to compile the shaders on this tutorial, you need to build the Shaders target. this will compile all your shader files. Due to cmake limitations, when you add new shader files, you need to re-run the cmake configuration step so that cmake can pick them. On other engines, a common thing to do is to have an executable or a .bat or other similar script that compiles the shaders automatically as the engine opens. 
이 튜토리얼에서 셰이더를 컴파일하는 언제든 셰이더 타겟을 구성할 필요가 있습니다. 이는 모든 셰이더 파일을 컴파일 할 것입니다. cmake 제한으로 인해 새로운 셰이더 파일을 추가할 때 cmake 구성 단계를 재실행하여 cmake가 해당 셰이더 파일을 가져올 수 있도록 해야 합니다. 다른 엔진에서는 이를 수행하는 흔한 방법은 실행가능한 파일 혹은 .bat, 혹은 다른 스크립트 파일을 사용해 셰이더를 엔진이 열리면 자동으로 컴파일 하도록 합니다.

It is also possible to use HLSL instead of GLSL to code Vulkan shaders. A lot of projects like doing that, but we wont be doing it as part of this tutorial. If you want to look at how to do that, you can check in this official vulkan site [HLSL In Vulkan](https://docs.vulkan.org/guide/latest/hlsl.html)
Vulkan 셰이더의 코드로 GLSL 대신 HLSL을 사용하는 것도 가능합니다. 많은 프로젝트가 이를 하고 있지만 이 튜토리얼에서는 다루지 않을 것입니다. 어떻게 작동하는지 더 많은 정보를 확인하고 싶다면 공식 Vulkan 사이트를 확인할 수 있습니다.

## Descriptor sets
To give data to the shaders, we need to setup some bindings. In vulkan, binding objects like images and buffers to the shader requires Descriptor Sets.
Think of a single descriptor as a handle or pointer into a resource. That resource being a Buffer or a Image, and also holds other information, such as the size of the buffer, or the type of sampler if it's for an image.
A `VkDescriptorSet` is a pack of those pointers that are bound together. Vulkan does not allow you to bind individual resources in shaders.
Descriptor sets are allocated from a `VkDescriptorPool`, using a `VkDescriptorLayout` that contains the information about what that descriptor set holds (for example, 2 images). Once a descriptor set is allocated, you can update its data using `vkUpdateDescriptorSets`, which takes an array of `VkWriteDescriptorSet`. Once you have a fully configured descriptor set, you can bind it to your pipeline using `VkBindDescriptorSets`, and its data will be available in the shader. 
셰이더에 데이터를 전달하기 위해 바인딩을 설정할 필요가 있습니다. Vulkan에서는 이미지와 버퍼를 셰이더로 바인딩하는 것은 디스크립터 셋(DescriptorSet)이 필요합니다. 단일 디스크립터를 자원에 대한 핸들 혹은 포인터라고 생각해도 무방합니다. 이는 버퍼 혹은 이미지 자원이며 버퍼의 크기나 이미지의 샘플러(sampler) 타입과 같은 정보도 담고 있습니다. `VkDescriptorSet`은 함께 바인딩된 이러한 포인터들의 묶음입니다. Vulkan은 셰이더로 개별 리소스를 바인딩할 수 없게 했습니다. 디스크립터 셋은 `VkDescriptorSetLayout`을 사용해 `VkDescriptorPool`로부터 할당됩니다. `VkDescriptorSetLayout`은 디스크립터 셋이 담는 것의 정보를 담고 있습니다.(예를 들어, 이미지 2개가 있을 수 있습니다.) 디스크립터 셋이 할당되면 `vkUpdateDescriptorSets`를 사용해 업데이트 할 수 있으며, 이는 `VkWriteDescriptorSet`의 배열을 받습니다. 디스크립터 셋을 완전히 구성하면 파이프라인에 `VkBindDescriptorSets`를 사용해 바인딩할 수 있으며 셰이더에서 데이터를 사용할 수 있습니다.

<div class="mxgraph" style="max-width:100%;border:1px solid transparent;" data-mxgraph="{&quot;highlight&quot;:&quot;#0000ff&quot;,&quot;nav&quot;:true,&quot;resize&quot;:true,&quot;toolbar&quot;:&quot;zoom layers lightbox&quot;,&quot;edit&quot;:&quot;_blank&quot;,&quot;xml&quot;:&quot;&lt;mxfile host=\&quot;app.diagrams.net\&quot; modified=\&quot;2020-10-02T16:02:40.267Z\&quot; agent=\&quot;5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\&quot; etag=\&quot;G0Tl161wDfz5oobdgIFU\&quot; version=\&quot;13.7.7\&quot; type=\&quot;device\&quot;&gt;&lt;diagram id=\&quot;6hGFLwfOUW9BJ-s0fimq\&quot; name=\&quot;Page-1\&quot;&gt;7Vpdk5owFP01PG4HCCA+6qptZ7Yzne706zFKRLrR0BBX6a9vAkEgQdc6wLRs9UFyE0hyzj03N0ED3G+PbymMNx9IgLBhm8HRADPDti0bjPiPsKS5xfNAbghpFMhGpeEx+oWk0ZTWfRSgpNaQEYJZFNeNK7LboRWr2SCl5FBvtia43msMQ6QZHlcQ69avUcA2udW3R6X9HYrCTdGz5Y3zmiVcPYWU7HeyP8MGzkx88+otLJ4lJ5psYEAOlU7B3AD3lBCWX22P9wgLbAvY8vsWZ2pP46Zox665AXhyHCwt5o4CDoUsEso2JCQ7iOeldZrND4knmLxUtnkgJOZGixt/IMZSySvcM8JNG7bFsnZNduyeYEKzHsHcE19uTxglT6hSMxuLL6/R5yWnmpA9XcmRS3bEBFREuaciskWMprwBRRiy6LnONZQuE57anW79SCLeq21K93ZdSZ50bsc0649gkIaIybtK8CeUwrTSLBYNkgv9mM39LG5rzy/yERSlCialKfOPZl+R8D5DvJfQfXmaoWRFo5gR+oiY5kqlowjWD5uIoccYZnQdeOBQnCLCuEI9tE3TdpucwluY/KO70Tr7XHKWZ0QZOl70DVkLnDqWYCTLhzIcWIWIN5VQ4Cm+UHWnChMXgbZvESWPJLGoXWN0nIgQ2IZQOYA0/Sbuf+MWxe/ycVlhdqyVUlnqSOC+XAoyfXWk+atJ8jU5TDAmK8iQYXuYD2+6pDUSvZ97EdYzcO6SDPIJb2D58TEDoKjnVyE7QaI4wQNc8rW2RhLEUbjj1ysOHeK4ToWbR3wxm8iKbRQEeeBGvFu4zJ4naJIRiD/cnRruTI5NeoPlN9JSOKempdNqLDso1p6zGrsz3wBzbNV0dmfnxVtDdNGErNcJYhqffxbwdIarAe8jT0gGHPFO0a2PiAf8AaUhwO4nD/HGdcY0JlrKQ0ZKXgHcy3mI2r4YV1t5SIFvRZdmHkqn+/UaiaBbqnRACnU9hQfnSoXaLSi06GsYCgX9KNT3blPoLaIAmiisVymKaxP1VkRhaUj+w6JwehKFun12uhOFo4nCzkXxfitOgGwPboVz75aJ+HkVAjmdVvQiEDAkgbj9CGQE6oy5oDuBuJpAwGtcNRzQoyiAfpD2juAg0fDlk2HdbfWFbhK51W9XNNfz4Izc+urt6zy4XdEw1miYfl4s5p+qxzdnTmJe8PUWkNE2dw0eCrqCxtF3WO8/TN7O/wpkfHAmf6gg4zUA08Y5haMnFH+Rz2gLR9MGsQkadYG5CRpPg2aZirNQkW+8CM7wQ93IUgTdsFHpTM/6ijPNuXH/s5MHldHL7DQppw123CElyY48ye06SQbq0YrbWZLsXH4PkL34VCX0AFOy11+IvrAGqCJigss23NtSgo81vjLftVRYbwo/eqalHNgOZmNhmeO6W/a5sSi4On8GOByc1dXU6xNnS8O5fqw0XJivjRutwKzvQsArcWdw7cvXVnDWNzXZsolo8j83NPSteNMhRUu5IS+W//3LU5XyD5Zg/hs=&lt;/diagram&gt;&lt;/mxfile&gt;&quot;}"></div>
<script type="text/javascript" src="https://viewer.diagrams.net/js/viewer-static.min.js"></script>

In this chapter we will connect a draw image to the compute shader so that the shader can write into it. Over the tutorial, we will be writing some abstractions that simplify this flow.
A pipeline can have multiple slots to bind a few descriptor sets. The vulkan spec guarantees that we will have at least 4 sets, so that will be what we will target in the tutorial. According to gpu vendors, each descriptor set slot has a cost, so the less we have, the better. Later in the tutorial we will have Descriptor Set #0 to always bind some global scene data, which will contain some uniform buffers, and some special textures. Descriptor Set #1 will be used for per-object data.
이 챕터에서는 그릴 이미지를 컴퓨트 셰이더에 연결하여 셰이더가 작성할 수 있게 할 것입니다. 튜토리얼에 걸쳐 이 흐름을 단순화하는 추상화를 작성할 것입니다. 파이프라인은 몇 개의 디스크립터 셋을 바인딩할 수 있는 여러 슬롯을 갖습니다. Vulkan 사양은 적어도 4개의 디스크립터 셋을 보장하며, 이 튜토리얼에서 목표로할 것입니다. GPU 제조사에 따라 각 디스크립터 셋 슬롯은 비용이 들 수 있습니다. 따라서 가능한 적은 편이 좋습니다. 이후의 튜토리얼에서는 디스크립터 셋 0번에는 항상 유니폼 버퍼, 몇몇 특수 텍스쳐를 포함하는 전역 씬 데이터를 바인딩할 것이고, 디스크립터 셋 1번에는 오브젝트별 데이터에 사용하겠습니다.

## 푸시 상수
Apart from Descriptor Sets, which are used to connect the shaders to buffers and images, vulkan offers an option to directly write a few bytes of data while recording the command buffer. PushConstants is a mechanic unique to vulkan which reserves a very small amount of memory to be bound directly to the shader. This one is fast, but its also very limited in space and must be written as you encode the commands. Its main use is that you can give it some per-object data, if your objects dont need much of it, but according to gpu vendors, their best use case is to send some indexes to the shader to be used to access some bigger data buffers. 
셰이더에 버퍼와 이미지를 전달하는 데 사용되는 디스크립터 셋과는 별도로, Vulkan은 커맨드 버퍼를 작성하면서 직접 몇 바이트의 데이터를 작성할 수 있는 선택지를 제시합니다. 푸시 상수는 Vulkan의 고유한 기법으로 셰이더로 직접 바인딩할 적은 양의 메모리를 보존합니다. 이는 빠르지만 매우 제한된 공간만 사용할 수 있으며 명령에 인코딩되어 작성되어야만 합니다. 주로 객체가 많은 데이터를 요구하지않을 때, 객체별 데이터에 사용할 수 있습니다. 하지만 GPU 제조사에 따르면 최적의 용례는 셰이더로 인덱스를 전달해 더 큰 데이터 버퍼에 접근할 때 사용하는 것입니다.

## 파이프라인 레이아웃
A shader has some inputs it needs, and this is what a `VkPipelineLayout` is made of. To create one, we must give it the `VkDescriptorSetLayout` of the descriptor slots it needs, and PushConstantRange defining its push constant usage.
PipelineLayouts for a graphics and compute pipeline are made in the same way, and they must be created before the pipeline itself. 
셰이더는 필요한 입력을 가지며, 이는 `VkPipelineLayout`이 만드는 것입니다. 파이프라인 레이아웃을 만들기 위해 `VkDescriptorSetLayout`과 푸시 상수에 사용되는 PushConstantRage 정의가 필요합니다. 그래픽스 파이프라인과 컴퓨트 파이프라인의 파이프라인 레이아웃은 같은 방식으로 만들어지며, 파이프라인이 생성되기 전에 생성되어야 합니다.

## 컴퓨트 파이프라인
To build a compute pipeline, we need to create first the pipeline layout for it, and then hook a single shader module for its code. Once its built, we can execute the compute shader by first calling `VkCmdBindPipeline` and then calling `VkCmdDispatch`.
컴퓨트 파이프라인을 구성하기 위해 파이프라인 레이아웃을 먼저 만들어야 합니다. 그리고 셰이더 모듈을 연결합니다. 파이프라인이 만들어지면 컴퓨트 셰이더를 `VkCmdBindPipeline`를 호출하고 `VkCmdDispatch`를 호출함으로써 실행할 수 있습니다.

Compute shaders have a specific programming model. When we call vkCmdDispatch, we give vulkan a number of work groups to launch in 3 dimensions in a X * Y * Z fashion. In our case here, we will be using it to draw a image, so we only use 2 of those dimensions, that way we can execute one workgroup per group of pixels in the image.
컴퓨트 셰이더는 특수한 프로그래밍 모델입니다. VkCmdDispatch를 호출할 때 Vulkan에게 몇개의 작업 그룹을 할당하고 x, y, z 세 차원으로 시작합니다. 우리의 경우 이미지를 그릴 것이므로 2개의 차원만 사용합니다. 이미지의 픽셀마다 작업 그룹을 실행하는 것입니다.

Inside the shader itself, we can see  `layout (local_size_x = 16, local_size_y = 16) in;` By doing that, we are setting the size of a single workgroup. This means that for every work unit from the vkCmdDispatch, we will have 16x16 lanes of execution, which works well to write into a 16x16 pixel square.
셰이더 내부에서는 `layout (local_size_x = 16, local_size_y = 16) in;`을 볼 수 있습니다. 이를 통해 단일 작업 그룹을 설정할 수 있습니다. 이는 vkCmdDispatch로부터 받는 모든 작업 그룹에 대해 16 * 16 단위로 실행할 것이라는 뜻입니다. 이는 16 * 16 픽셀의 사각형을 작성할 때 유용합니다. 

On the shader code, we can access what the lane index is through `gl_LocalInvocationID` variable. There is also `gl_GlobalInvocationID` and `gl_WorkGroupID`.  By using those variables we can find out what pixel exactly do we write from each lane.
셰이더 코드에서는 `gl_LocalInvocationID` 변수를 통해 현재 라인 인덱스에 접근할 수 있습니다. `gl_GlobalInvocationID`와 `gl_WorkGroupID`도 있습니다. 이러한 변수들을 사용하여 현재 라인에서 작성할 픽셀을 정확히 찾을 수 있습니다.

Lets now set all of this up in the code and get our first shaders working.
이제 실제 코드를 살펴보며 어떻게 셰이더가 작동하는 지 알아보겠습니다.

다음 글 : [ Vulkan 셰이더 코드]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_shader_code.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
