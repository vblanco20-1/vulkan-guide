---
layout: default
title: Meshes and Camera
parent: "New 4. Textures and Engine Architecture"
nav_order: 14
---

We will begin by setting up the new draw loop using the RenderObjects explained last chapter. We were harcoding the rendering on the mesh list loaded from GLTF, but now we will convert that list into RenderObjects and then draw that. We cant load textures from GLTF yet so we will be using the default material. 

We will begin creating the architecture by defining the scene-node classes as explained in the article before.

```cpp
struct DrawContext; //forward declaration
// base class for a renderable dynamic object
class IRenderable {

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};

// implementation of a drawable scene node.
// the scene node can hold children and will also keep a transform to propagate
// to them
struct Node : public IRenderable {

    // parent pointer must be a weak pointer to avoid circular dependencies
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 localTransform;
    glm::mat4 worldTransform;

    void refreshTransform(const glm::mat4& parentMatrix)
    {
        worldTransform = parentMatrix * localTransform;
        for (auto c : children) {
            c->refreshTransform(worldTransform);
        }
    }

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
    {
        // draw children
        for (auto& c : children) {
            c->Draw(topMatrix, ctx);
        }
    }
};
```

The node will be the first IRenderable we have. We will be building the node tree using smart pointers. For the parent pointer, we store it as weak_ptr to avoid circular dependencies. The children will be stored as shared pointer. 

The Node class will hold the object matrix for the transforms. Both local and world transform. The world transform needs to be updated, so whenever the local Transform  gets changed, refreshTransform must be called. This will recursively go down the node tree and make sure the matrices are on their correct places.

The draw function will do nothing, only call Draw() on children. 

This base node class does nothing, so we need to add a MeshNode class that displays a mesh.

```cpp
struct MeshNode : public Node {

    std::shared_ptr<MeshAsset> mesh;

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};
```

The MeshNode holds a pointer to a mesh asset, and overrides the draw function to add commands into the draw context.

Lets write the DrawContext too. 
```cpp
struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;
    
    MaterialInstance* material;

    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
    std::vector<RenderObject> OpaqueSurfaces;
};
```
The draw context is just a list of RenderObject structures, for now.
The RenderObject is the core of our rendering. The engine itself will not call any vulkan functions on the node classes, and the renderer is going to take the array of RenderObjects from the context, built every frame (or cached), and execute a single vulkan draw function for each.

With those defined, the Draw() function of the meshnode looks like this

```cpp
void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    glm::mat4 nodeMatrix = topMatrix * worldTransform;

    for (auto& s : mesh->surfaces) {
        RenderObject def;
        def.indexCount = s.count;
        def.firstIndex = s.startIndex;
        def.indexBuffer = mesh->meshBuffers.indexBuffer.buffer;
        def.material = &s.material->data;

		def.transform = nodeMatrix;
		def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;
       
        ctx.OpaqueSurfaces.push_back(def);
    }

    // recurse down
    Node::Draw(topMatrix, ctx);
}
```

A mesh can have multiple surfaces with different materials, so we will loop the surfaces of the mesh, and add the resulting RenderObjects to the list.
Note how we are dealing with the matrix. We are not inserting the objects directly from the node WorldTransform, but multiplying that by the TopMatrix. This means that if the Draw() function gets called multiple times, we can draw the same multiple times with different transforms. Very useful if we want to render the same objects multiple times, which is a common thing to do.

The last thing we need here is to add the object draw loop into VulkanEngine class so that a DrawContext can be processed and turned into real vulkan calls.

To do that, remove the code relevant to that rectangle hardcoded mesh and the code that used to draw the monkey head. We will be replacing that. All the code on draw_geometry after the first triangle draw gets removed.

To hold the draw list, we add the DrawContext structure into the VulkanEngine class. We will also add a ` update_scene()` function where we will call the draw functions outside of the vulkan render loop. Also a hashmap of Nodes that will contain the meshes we load. This function will also handle the logic like setting up the camera.

```cpp
class VulkanEngine{
    DrawContext mainDrawContext;
    std::unordered_map<std::string, shared_ptr<Node*>> loadedNodes;

    void update_scene();
}
```

We will add the code to the renderer on draw_geometry, right after creating the GPUSceneData descriptor set, so that we can bind it.

```cpp
	for (const RenderObject& draw : mainDrawContext.OpaqueSurfaces) {

		vkCmdBindPipeline(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->pipeline);
		vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,draw.material->pipeline->layout, 0,1, &globalDescriptor,0,nullptr );
		vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,draw.material->pipeline->layout, 1,1, &draw.material->materialSet,0,nullptr );

		vkCmdBindIndexBuffer(cmd, draw.indexBuffer,0,VK_INDEX_TYPE_UINT32);

		GPUDrawPushConstants pushConstants;
		pushConstants.vertexBuffer = draw.vertexBufferAddress;
		pushConstants.worldMatrix = draw.transform;
		vkCmdPushConstants(cmd,draw.material->pipeline->layout ,VK_SHADER_STAGE_VERTEX_BIT,0, sizeof(GPUDrawPushConstants), &pushConstants);

		vkCmdDrawIndexed(cmd,draw.indexCount,1,draw.firstIndex,0,0);
	}
```

When the RenderObject was designed, it was meant to directly convert into a single draw command on vulkan. So there is no logic other than directly binding the stuff and calling VkCmdDraw. We are binding the data every draw which is inneficient but we will fix that later. 

Last thing is going to be using the mesh load we loaded last chapter to create some Nodes, and then drawing them so they add the meshes into the draw context. The loadGltfMeshes is not loading material properly, but we can give it the default material. 

```cpp
	for (auto& m : testMeshes) {

		std::shared_ptr<MeshNode> newNode = std::make_shared<MeshNode>();
		newNode->mesh = std::make_shared<MeshAsset>(m);

		newNode->localTransform = glm::mat4{ 1.f };
		newNode->worldTransform = glm::mat4{ 1.f };

		for (auto& s : newNode->mesh->surfaces) {
			s.material = std::make_shared<GLTFMaterial>(defaultData);
		}

		loadedNodes[m.name] = std::move(newNode);
	}
```

For each of the test meshes, we create a new MeshNode, and copy the mesh asset into a shared ptr for that node. Then we do similar with the default material.

This is because normally we would not be loading objects like this, but directly loading the nodes, meshes, and materials from GLTF properly. on there, multiple nodes can refer to the same mesh, and multiple meshes to the same material, so the shared_ptrs are needed, even if they look pointless in this case.

Lets create the update_scene() function. We are also moving the camera logic we had on the monkeyhead last chapter into here.

```cpp
void VulkanEngine::update_scene()
{
	mainDrawContext.OpaqueSurfaces.clear();

	for (auto&& m : loadedNodes) {
		m.second->Draw(glm::mat4{1.f}, mainDrawContext);
	}

	sceneData.view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	sceneData.proj = glm::perspective(glm::radians(70.f), (float)_windowExtent.width / (float)_windowExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	sceneData.proj[1][1] *= -1;
	sceneData.viewproj = sceneData.proj * sceneData.view;
}
```

We begin by clearing the render objects from the draw context, then looping other the loadedNodes and calling Draw on each. This adds their commands into the draw context. Then we set up the camera math.

This function gets called at the very start of the draw() function, before waiting on the frame fences.

```cpp
void VulkanEngine::draw()
{
	update_scene();

	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
}
```

If you draw the engine now, you will see that the objects from the mesh asset are being drawn. 

Now, to demonstrate it, we are going to manipulate the nodes and drawing a bit.

First, we are going to draw the objects more times by grabbing the Node that draws a cube, and making it draw a line made of cubes. We have the Nodes stored on the hashmap, so we can access and render them individually however we want to.

On the update_scene() function.
```cpp
	for (int x = -3; x < 3; x++) {

		glm::mat4 scale = glm::scale(glm::vec3{0.2});
		glm::mat4 translation =  glm::translate(glm::vec3{x, 1, 0});

		loadedNodes["Cube"]->Draw(translation * scale, mainDrawContext);
	}
```

We make the cubes smaller, and give them a translation from left to right of the screen. Then we call Draw on them. Every time Draw is called it will add the RenderObject into the context with a different matrix, so it we can render the objects multiple times on different positions. 

This concludes chapter 4. On the next chapter, we will upgrade the gltf loader to load scenes with textures and multiple objects, and set up a interactive FPS camera.

Next: [Chapter 5: Interactive Camera]({{ site.baseurl }}{% link docs/new_vkguide/chapter_5/interactive_camera.md %})


{% include comments.html term="Vkguide 2 Beta Comments" %}
