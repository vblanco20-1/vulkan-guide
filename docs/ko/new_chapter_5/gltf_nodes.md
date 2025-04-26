---
layout: default
title: GLTF Scene Nodes
parent: "5. GLTF loading - KO"
grand_parent: "Korean VKGuide"
nav_order: 5
---

GLTF를 불러오는 작업을 두 단계로 나눌 것입니다. 첫 번째는 노드와 메시를 처리하는 것이고, 두 번째는 머테리얼과 텍스쳐를 처리하는 것입니다.

기존의 GLTF 로딩 함수는 제거되고, 메시 뿐만 아니라 씬 노드를 직접 불러올 수 있는 새로운 함수로 대체될 예정입니다. 이전 함수는 복사/붙여넣기나 디버깅 용도로 남겨둘 수 있지만, 더 이상 사용되지는 않을 것입니다.

먼저 해야 할 일은 LoadedGLTF 클래스를 정의하는 것입니다. 이 클래스는 하나의 GLTF 파일에 포함된 모든 자원을 담고 있습니다. 이 클래스가 소멸될 때 메시 버퍼와 텍스쳐도 함께 해제됩니다. 이를 통해 개별 리소스를 따로 관리하지 않고, 보다 간단한 방식으로 그룹 단위로 자원을 관리할 수 있게 됩니다. 이 방식의 목표는 GLTF 파일 하나를 전체 레벨에 필요한 텍스쳐, 메시, 객체들을 포함하는 단위로 불러오는 것입니다. 사용자는 또 다른 GLTF 파일을 캐릭터나 오브젝트 용도로 불러오고, 이를 게임 도중 유지할 수도 있습니다. 하나의 씬을 LoadedGLTF 클래스 하나로 불러온다고 해서, 그 안에 있는 개별 노드들의 Draw() 함수를 호출할 수 없다는 의미는 아닙니다.

vk_loader.h에 다음 클래스를 추가하세요.

```cpp
struct LoadedGLTF : public IRenderable {

    // storage for all the data on a given glTF file
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree order
    std::vector<std::shared_ptr<Node>> topNodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocatorGrowable descriptorPool;

    AllocatedBuffer materialDataBuffer;

    VulkanEngine* creator;

    ~LoadedGLTF() { clearAll(); };

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);

private:

    void clearAll();
};
```

GLTF의 오브젝트들은 이름을 갖고 있으므로, 모든 오브젝트를 unordered map에 저장할 것입니다. 모든 항목은 shared_ptr를 통해 관리됩니다. 이렇게 하면 이름이 없는 노드가 존재하더라도 안전하게 처리할 수 있습니다. 전체 GLTF 파일은 shared_ptr간의 연결로 구성된 그래프 구조를 형성하며, 참조가 유지되는 동안 자체적으로 메모리에 살아있게 됩니다.
 
우선 메시 버퍼를 담는 mesh의 맵이 있습니다. 이는 메시만을 불러오던 이전 코드에서 사용하던 것과 동일합니다. 다음으로, 파일 내 변환 트리로부터 생성된 sceneNode 맵이 있으며, 각각의 노드는 계층 구조를 형성합니다. 그 외에도 texture, material 맵이 있습니다. 텍스쳐와 머테리얼은 분리되어 있는데, 동일한 텍스쳐가 여러 머테리얼에서 공통으로 사용되는 경우가 많기 때문입니다. GLTF 파일 포맷 역시 이 둘을 분리하여 정의하고 있습니다.

또한 부모 노드가 없는 루트 노드들을 별도의 벡터에 저장합니다. 이를 통해 LoadedGLTF 클래스의 Draw 함수에서 이 루트 노드들로부터 재귀적으로 Draw()를 수행할 수 있습니다. 이 방식은 에디터에서 씬 노드를 계층적으로 표시할 때도 유용합니다.

GLTF 포맷에 맞춰 VkSamplers 배열도 포함합니다. 이 배열은 일반적으로 소수의 항목만 가질 것입니다. 물론 샘플러를 전역으로 해싱하여 엔진에서 공유하도록 구현할 수도 있지만, GLTF 하나가 Vulkan 자원을 전부 관리한다는 개념을 유지하면 엔진측 로직이 더 단순해집니다.

그리고 GLTF 파일 전용의 디스크립터 풀도 생성합니다. 이렇게 하면 개별 디스크립터 셋을 직접 관리하지 않아도 되며, 파일에 속한 머테리얼의 디스크립터 셋들을 한꺼번에 해제하는 것도 간단해집니다.

materialDataBuffer는 GLTFMetallicRoughness 머테리얼 데이터와 같은 형식으로 전체 머테리얼 정보를 담을 것입니다. 이 버퍼는 모든 머테리얼 데이터를 하나의 큰 버퍼에 담아 처리합니다.

그 외에는 소멸자 함수, Draw 함수, 그리고 VulkanEngine 인스턴스를 내부에 저장하여 clearAll() 호출 시 자원을 올바르게 해제할 수 있도록 합니다. 만약 포인터 저장을 피하고 싶다면 싱글톤 패턴을 사용하는 것도 고려할 수 있습니다.

vk_loader.cpp에 새로운 코드를 작성합시다. 처음부터 새로 구현할 것이며, 머테리얼 관련 설정은 일단 기본값으로 두겠습니다.

```cpp
std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(VulkanEngine* engine,std::string_view filePath)
{
    fmt::print("Loading GLTF: {}", filePath);

    std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
    scene->creator = engine;
    LoadedGLTF& file = *scene.get();

    fastgltf::Parser parser {};

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
    // fastgltf::Options::LoadExternalImages;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    fastgltf::Asset gltf;

    std::filesystem::path path = filePath;

    auto type = fastgltf::determineGltfFileType(&data);
    if (type == fastgltf::GltfType::glTF) {
        auto load = parser.loadGLTF(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else if (type == fastgltf::GltfType::GLB) {
        auto load = parser.loadBinaryGLTF(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else {
        std::cerr << "Failed to determine glTF container" << std::endl;
        return {};
    }
}
```

파일을 불러오는 것부터 시작하겠습니다. 이번에는 단순히 메시만을 불러오던 이전 코드보다 더 범용적인 구조를 가질 것이기 때문에, GLTF와 GLB간 형식을 구분하여 처리할 수 있도록 형식 확인 로직을 추가합니다. 그 외의 대부분의 로직은 기존의 메시 전용 코드와 거의 동일합니다.

<!-- codegen from tag load_2 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    // we can stimate the descriptors we will need accurately
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 } };

    file.descriptorPool.init(engine->_device, gltf.materials.size(), sizes);
```

다음으로는 필요한 디스크립터의 개수를 대략적으로 추산하여 디스크립터 풀을 초기화합니다. 풀의 용량을 초과하는 경우에는 확장 가능한 풀을 사용하기 때문에 필요에 따라 VkDescriptorPool을 추가로 생성하여 처리할 수 있습니다.

이제 샘플러를 불러옵니다. GLTF 샘플러는 Vulkan과는 호환되지 않는 OpenGL 기반의 숫자 및 속성을 사용하므로, 이를 Vulkan 형식에 맞게 변환하는 함수를 작성해야 합니다.

<!-- codegen from tag filters on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
VkFilter extract_filter(fastgltf::Filter filter)
{
    switch (filter) {
    // nearest samplers
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
        return VK_FILTER_NEAREST;

    // linear samplers
    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode extract_mipmap_mode(fastgltf::Filter filter)
{
    switch (filter) {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;

    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}
```

이 두 함수는 vk_loader.cpp 파일 내의 전역 함수입니다. 외부에서는 필요하지 않을 것입니다. Vulkan에서는 샘플러 필터와 밉맵 모드가 분리되어 있으므로, GLTF의 필터 옵션을 기반으로 각각을 따로 추출해야 합니다. `extract_filter()`는 `VK_FILTER_NEAREST` 혹은 `VK_FILTER_LINEAR`와 같은 필터링 방식을 반환합니다. `extract_mipmap_mode()`에서는 `VK_SAMPLER_MIPMAP_MODE_NEAREST` 혹은 `VK_SAMPLER_MIPMAP_MODE_LINEAR`와 같은 밉맵 방식을 반환합니다. NEAREST는 단일 밉맵 레벨을 선택하여 블렌딩이 없이 사용하는 방식이며, Linear는 여러 밉맵 레벨을 블렌딩하여 부드럽게 전환합니다.

이제 GLTF 파일로부터 샘플러를 불러올 수 있습니다.

<!-- codegen from tag load_samplers on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp

    // load samplers
    for (fastgltf::Sampler& sampler : gltf.samplers) {

        VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, .pNext = nullptr};
        sampl.maxLod = VK_LOD_CLAMP_NONE;
        sampl.minLod = 0;

        sampl.magFilter = extract_filter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
        sampl.minFilter = extract_filter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        sampl.mipmapMode= extract_mipmap_mode(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        VkSampler newSampler;
        vkCreateSampler(engine->_device, &sampl, nullptr, &newSampler);

        file.samplers.push_back(newSampler);
    }
```

`VkSamplerCreateInfo`구조체를 생성하고, maxLOD와 minLOD값을 기본값으로 설정한 뒤, 앞서 정의한 extract 함수를 사용해 관련 설정을 지정합니다. 지난 챕터에서의 기본 샘플러와는 달리, 여기서는 LOD 범위를 명시적으로 지정하여 밉맵을 사용할 수 있도록 합니다. 생성된 VkSamplers들은 LoadedGLTF 구조체에 직접 저장됩니다.

이제 메시를 불러오기 전에, 구조체를 담기 위한 배열들을 먼저 생성할 것입니다. GLTF 파일에서는 대부분의 작업이 인덱스를 기반으로 이루어지므로, 인덱스를 통해 객체를 추적하고 저장할 수단이 필요합니다. 예를 들어, 어떤 메시노드가 특정 메시를 참조할 때에는 메시 이름이 아닌 인덱스로 연결됩니다.

<!-- codegen from tag load_arrays on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    // temporal arrays for all the objects to use while creating the GLTF data
    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<AllocatedImage> images;
    std::vector<std::shared_ptr<GLTFMaterial>> materials;
```

이제 올바른 순서에 따라 파일을 불러와야 합니다. MeshNode는 메시에, mesh는 머테리얼에, 머테리얼은 텍스쳐에 의존합니다. 따라서 텍스쳐부터 시작하여 올바른 순서로 생성해야 합니다. 텍스쳐부터 시작합시다. 텍스쳐의 경우 실제 이미지는 추후 불러올 것이기 때문에 엔진에 존재하는 기본 텍스처를 복사해 사용할 것입니다. 이미지를 불러오는 데 실패했을 경우를 대비해 체크보드 텍스쳐를 사용하겠습니다.

```cpp
// load all textures
for (fastgltf::Image& image : gltf.images) {
   
    images.push_back(engine->_errorCheckerboardImage);
}
```

머테리얼은 머테리얼 파라미터를 담는 버퍼의 크기를 사전에 계산해야 합니다. 현재는 하나의 머테리얼 타입만 사용하므로 특별한 문제는 없으며, 유니폼 버퍼 구조체의 크기에 머테리얼 개수를 곱해 필요한 전체 버퍼 크기를 구하면 됩니다.

<!-- codegen from tag load_buffer on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    // create buffer to hold the material data
    file.materialDataBuffer = engine->create_buffer(sizeof(GLTFMetallic_Roughness::MaterialConstants) * gltf.materials.size(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    int data_index = 0;
    GLTFMetallic_Roughness::MaterialConstants* sceneMaterialConstants = (GLTFMetallic_Roughness::MaterialConstants*)file.materialDataBuffer.info.pMappedData;
```

매핑된 포인터를 sceneMaterialConstants에 저장하여 머테리얼 데이터를 쓸 수 있도록 하겠습니다.

이제 머테리얼을 불러오는 반복문을 작성하겠습니다.

<!-- codegen from tag load_material on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    for (fastgltf::Material& mat : gltf.materials) {
        std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
        materials.push_back(newMat);
        file.materials[mat.name.c_str()] = newMat;

        GLTFMetallic_Roughness::MaterialConstants constants;
        constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
        constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
        constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
        constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

        constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
        constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
        // write material parameters to buffer
        sceneMaterialConstants[data_index] = constants;

        MaterialPass passType = MaterialPass::MainColor;
        if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            passType = MaterialPass::Transparent;
        }

        GLTFMetallic_Roughness::MaterialResources materialResources;
        // default the material textures
        materialResources.colorImage = engine->_whiteImage;
        materialResources.colorSampler = engine->_defaultSamplerLinear;
        materialResources.metalRoughImage = engine->_whiteImage;
        materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

        // set the uniform buffer for the material data
        materialResources.dataBuffer = file.materialDataBuffer.buffer;
        materialResources.dataBufferOffset = data_index * sizeof(GLTFMetallic_Roughness::MaterialConstants);
        // grab textures from gltf file
        if (mat.pbrData.baseColorTexture.has_value()) {
            size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

            materialResources.colorImage = images[img];
            materialResources.colorSampler = file.samplers[sampler];
        }
        // build material
        newMat->data = engine->metalRoughMaterial.write_material(engine->_device, passType, materialResources, file.descriptorPool);

        data_index++;
    }
```

먼저 MaterialConstants를 채우고, GLTF의 머테리얼 정보에서 기본 색상, metallic, roughness 요소를 불러옵니다.

이후 MaterialResources 구조체를 채웁니다. 텍스쳐와 샘플러는 기본 흰색 텍스쳐로 설정합니다. 그리고 materialDataBuffer를 올바른 데이터 오프셋으로 연결합니다. GLTF 머테리얼에서 텍스쳐는 선택 사항이므로, 설정되지 않은 경우 보통 흰색이 기본값입니다. 만약 색상 텍스쳐가 존재한다면 텍스쳐와 샘플러를 인덱스를 통해 연결합니다. 또한 머테리얼이 투명한지 확인한 후, 이 경우 MaterialPass를 Transparent로 설정합니다.

모든 준비가 완료되면, 인자들을 metalRoughMaterial 클래스에 전달하여 머테리얼을 생성합니다.

다음은 메시를 불러오는 것입니다. 이전의 메시를 불러오는 코드와 거의 유사하지만, 차이점은 메시를 다른 방식으로 저장한다는 점입니다.

```cpp
// use the same vectors for all meshes so that the memory doesnt reallocate as
// often
std::vector<uint32_t> indices;
std::vector<Vertex> vertices;

for (fastgltf::Mesh& mesh : gltf.meshes) {
    std::shared_ptr<MeshAsset> newmesh = std::make_shared<MeshAsset>();
    meshes.push_back(newmesh);
    file.meshes[mesh.name.c_str()] = newmesh;
    newmesh->name = mesh.name;

    // clear the mesh arrays each mesh, we dont want to merge them by error
    indices.clear();
    vertices.clear();

    for (auto&& p : mesh.primitives) {
        GeoSurface newSurface;
        newSurface.startIndex = (uint32_t)indices.size();
        newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

        size_t initial_vtx = vertices.size();

        // load indexes
        {
            fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
            indices.reserve(indices.size() + indexaccessor.count);

            fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                [&](std::uint32_t idx) {
                    indices.push_back(idx + initial_vtx);
                });
        }

        // load vertex positions
        {
            fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
            vertices.resize(vertices.size() + posAccessor.count);

            fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                [&](glm::vec3 v, size_t index) {
                    Vertex newvtx;
                    newvtx.position = v;
                    newvtx.normal = { 1, 0, 0 };
                    newvtx.color = glm::vec4 { 1.f };
                    newvtx.uv_x = 0;
                    newvtx.uv_y = 0;
                    vertices[initial_vtx + index] = newvtx;
                });
        }

        // load vertex normals
        auto normals = p.findAttribute("NORMAL");
        if (normals != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                [&](glm::vec3 v, size_t index) {
                    vertices[initial_vtx + index].normal = v;
                });
        }

        // load UVs
        auto uv = p.findAttribute("TEXCOORD_0");
        if (uv != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                [&](glm::vec2 v, size_t index) {
                    vertices[initial_vtx + index].uv_x = v.x;
                    vertices[initial_vtx + index].uv_y = v.y;
                });
        }

        // load vertex colors
        auto colors = p.findAttribute("COLOR_0");
        if (colors != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                [&](glm::vec4 v, size_t index) {
                    vertices[initial_vtx + index].color = v;
                });
        }

        if (p.materialIndex.has_value()) {
            newSurface.material = materials[p.materialIndex.value()];
        } else {
            newSurface.material = materials[0];
        }

        newmesh->surfaces.push_back(newSurface);
    }

    newmesh->meshBuffers = engine->uploadMesh(indices, vertices);
}
```

차이점은 마지막에 머테리얼 인덱스를 처리하는 부분입니다. 머테리얼이 없다면 파일의 첫 번째 머테리얼을 기본값으로 사용합니다. 비록 머테리얼이 선택 사항이지만, 머테리얼 인덱스를 갖고 있지 않는 경우는 드물기 때문에 특별히 신경 쓰지 않아도 됩니다.


이제 노드를 불러오겠습니다.

<!-- codegen from tag load_nodes on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    // load all nodes and their meshes
    for (fastgltf::Node& node : gltf.nodes) {
        std::shared_ptr<Node> newNode;

        // find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the meshnode class
        if (node.meshIndex.has_value()) {
            newNode = std::make_shared<MeshNode>();
            static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
        } else {
            newNode = std::make_shared<Node>();
        }

        nodes.push_back(newNode);
        file.nodes[node.name.c_str()];

        std::visit(fastgltf::visitor { [&](fastgltf::Node::TransformMatrix matrix) {
                                          memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
                                      },
                       [&](fastgltf::Node::TRS transform) {
                           glm::vec3 tl(transform.translation[0], transform.translation[1],
                               transform.translation[2]);
                           glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
                               transform.rotation[2]);
                           glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

                           glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                           glm::mat4 rm = glm::toMat4(rot);
                           glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                           newNode->localTransform = tm * rm * sm;
                       } },
            node.transform);
    }
```

노드를 불러오는 작업은 두 단계로 나뉩니다. 첫 번째는 노드가 메시를 포함하는지 여부에 따라 기본 Node 클래스 또는 MeshNode 클래스로 노드를 생성합니다. 그 후, GLTF의 변환 정보를 불러와 이를 GLTF에서 사용하는 최종 변환 행렬로 변환하여 로컬 행렬을 계산합니다.

노드가 모두 불러와졌다면 씬 그래프를 구성하기 위한 부모 자식 관계를 설정해야 합니다.

<!-- codegen from tag load_graph on file E:\ProgrammingProjects\vulkan-guide-2\chapter-5/vk_loader.cpp --> 
```cpp
    // run loop again to setup transform hierarchy
    for (int i = 0; i < gltf.nodes.size(); i++) {
        fastgltf::Node& node = gltf.nodes[i];
        std::shared_ptr<Node>& sceneNode = nodes[i];

        for (auto& c : node.children) {
            sceneNode->children.push_back(nodes[c]);
            nodes[c]->parent = sceneNode;
        }
    }

    // find the top nodes, with no parents
    for (auto& node : nodes) {
        if (node->parent.lock() == nullptr) {
            file.topNodes.push_back(node);
            node->refreshTransform(glm::mat4 { 1.f });
        }
    }
    return scene;
```

먼저 모든 노드를 순회하여 자식 노드가 있는지 확인하고, 부모/자식 포인터를 설정합니다. 그런 다음 다시 한번 노드를 순회하면서 부모를 갖지 않는 노드를 찾아 topNodes 배열에 추가하고 변환 정보를 갱신합니다. 지난 챕터에서 설명했듯이 refreshTransform 함수는 부모-자식 관계를 기반으로 월드 행렬을 재귀적으로 계산하기 때문에, 어떤 노드든 부모가 설정되어 있다면 그 자식 노드들도 함께 갱신됩니다. 

이제 전체 씬을 불러왔습니다. 이제 렌더링하기 위해 Draw() 함수를 채워보겠습니다.

```cpp
void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->Draw(topMatrix, ctx);
    }
}
```

Draw함수는 단순히 topNodes를 순회하며 각 노드의 Draw를 호출할 뿐입니다. 이 호출은 자식 노드들에게도 전파됩니다.

clearAll() 함수를 지금은 비워두겠습니다. 아직은 텍스쳐 처리가 제대로 구현되지 않았으므로 남겨두겠습니다.

이제 이를 VulkanEngine 클래스에 연결합니다.

불러온 GLTF를 이름을 키로 하여 unordered map에 저장하겠습니다.
```cpp
 std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;
```

이제 무언가를 불러와봅시다. 프로젝트에 포함되어 있으며 1500개의 메시를 그리는 커다란 씬을 그려보겠습니다. 디버그 모드에서는 불러오는 데 1초에서 2초정도 소요될 수 있으므로, 잠시 기다려 주세요.

다음 코드를 init() 함수의 끝에 추가하세요.
```cpp
    std::string structurePath = { "..\\..\\assets\\structure.glb" };
    auto structureFile = loadGltf(this,structurePath);

    assert(structureFile.has_value());

    loadedScenes["structure"] = *structureFile;
```

해당 에셋을 불러와 이후에 사용할 수 있도록 해시맵에 저장합니다.

이후에는 update_scene() 함수에서 Draw()를 호출할 때 이 에셋을 사용하게 됩니다.

```cpp
	loadedScenes["structure"]->Draw(glm::mat4{ 1.f }, mainDrawContext);
```
이 씬은 크기가 크기 때문에, 서로 다른 위치에 여러 번 그려 벤치마크 용도로 활용할 수 있습니다.

씬을 정리할 때는, 적절한 시점에 초기화 해야 합니다. clear()함수에 다음 코드를 추가합니다. vkDeviceWaitIdle 호출 직후 Vulkan 정리 작업의 초반에 해시 맵을 초기화할 것입니다. 이 작업은 모든 씬 데이터를 정리하게 됩니다.

```cpp
 // make sure the gpu has stopped doing its things
 vkDeviceWaitIdle(_device);

 loadedScenes.clear();
```

이제 프로젝트를 실행해 보고 맵을 둘러보세요. 커다란 씬이지만 이제 카메라로 둘러볼 수 있습니다. 카메라의 기본 위치를 다음과 같이 변경하면 적절한 초기 위치가 됩니다.
```
 mainCamera.position = glm::vec3(30.f, -00.f, -085.f);
```
init()함수에서 이 값을 설정해주세요.

다음 글에서 텍스쳐를 불러오는 것으로 이 과정을 마무리해 보겠습니다.

Next: [ GLTF Textures]({{ site.baseurl }}{% link docs/ko/new_chapter_5/gltf_textures.md %})  

{% include comments.html term="Vkguide 2 Korean Comments" %}


