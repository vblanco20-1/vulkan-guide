---
layout: default
title: Scene Management
parent:  "3. Drawing meshes"
nav_order: 20
---

We have arbitrary mesh rendering with depth attachments and materials, but at the moment, each object is hardcoded.
We are going to refactor the rendering a little bit to render an array of objects. While it's a very simple way of setting up a scene, it will allow you to start making interesting things, like simple 3d games.

The way it's going to work is that we are going to have a RenderObject struct, which holds the data needed for a single draw. Mesh, Matrix, and Material. We will have an array of those, and just render each of them in order.

To store the materials and meshes, we are going to store them in an unordered_map. One of the main features an unordered_map has, is that it keeps pointers to the objects working well, so we can have a hashmap of materials by name, and then just store pointers to them.

A Material will just be a Pipeline pointer + PipelineLayout, for now.

vk_engine.h
```cpp
//note that we store the VkPipeline and layout by value, not pointer.
//They are 64 bit handles to internal driver structures anyway so storing pointers to them isn't very useful


struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

struct RenderObject {
	Mesh* mesh;

	Material* material;

	glm::mat4 transformMatrix;
};
```
In the VulkanEngine class, let's add a few members and functions to handle those.

```cpp
//add unordered_map to the headers on top
#include <unordered_map>

class VulkanEngine {
public:

//default array of renderable objects
std::vector<RenderObject> _renderables;

std::unordered_map<std::string,Material> _materials;
std::unordered_map<std::string,Mesh> _meshes;
//functions

//create material and add it to the map
Material* create_material(VkPipeline pipeline, VkPipelineLayout layout,const std::string& name);

//returns nullptr if it can't be found
Material* get_material(const std::string& name);

//returns nullptr if it can't be found
Mesh* get_mesh(const std::string& name);

//our draw function
void draw_objects(VkCommandBuffer cmd,RenderObject* first, int count);
};
```

and their implementations
```cpp
Material* VulkanEngine::create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name)
{
	Material mat;
	mat.pipeline = pipeline;
	mat.pipelineLayout = layout;
	_materials[name] = mat;
	return &_materials[name];
}

Material* VulkanEngine::get_material(const std::string& name)
{
	//search for the object, and return nullptr if not found
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


Mesh* VulkanEngine::get_mesh(const std::string& name)
{
	auto it = _meshes.find(name);
	if (it == _meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


void VulkanEngine::draw_objects(VkCommandBuffer cmd,RenderObject* first, int count)
{
	//empty for now
}
```


We are adding the 2 maps for materials and meshes, and a draw_objects function.
We do not have a single draw-mesh function. In a renderer, there is almost never a case where you render only one object, and we want to do sorting on the function, so it's better if our draw function takes an array of objects to draw.

Let's move our triangle and the monkey meshes so that they get registered in the maps, and make sure that the materials for them also get registered, so that we can use them when rendering.

```cpp
void VulkanEngine::load_meshes()
{
	_triangleMesh._vertices.resize(3);

	_triangleMesh._vertices[0].position = { 1.f,1.f, 0.5f };
	_triangleMesh._vertices[1].position = { -1.f,1.f, 0.5f };
	_triangleMesh._vertices[2].position = { 0.f,-1.f, 0.5f };

	_triangleMesh._vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
	_triangleMesh._vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
	_triangleMesh._vertices[2].color = { 0.f,1.f, 0.0f }; //pure green


	_monkeyMesh.load_from_obj("../../assets/monkey_smooth.obj");


    upload_mesh(_triangleMesh);
	upload_mesh(_monkeyMesh);

	//note that we are copying them. Eventually we will delete the hardcoded _monkey and _triangle meshes, so it's no problem now.
	_meshes["monkey"] = _monkeyMesh;
	_meshes["triangle"] = _triangleMesh;
}
```

on the load_pipelines function we are also going to put the last pipeline created, the mesh one, into the maps.
```cpp
void VulkanEngine::init_pipelines()
{
//other code ....

	VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout));

	pipelineBuilder._pipelineLayout = _meshPipelineLayout;


    //build the mesh pipeline
    _meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	create_material(_meshPipeline, _meshPipelineLayout, "defaultmesh");
}
```

With the materials and meshes added to the map, we can create the renderobjects.

We are going to add a new function to VulkanEngine, `init_scene`, to create a bunch of renderobjects, and call it at the end of `init()`

```cpp
void VulkanEngine::init()
{
	//other ....

	init_pipelines();

	load_meshes();

	init_scene();

	//everything went fine
	_isInitialized = true;
}

```

Now, in our init_scene, we are going to create a monkey as the first object, and more triangles around it. A lot of triangles
```cpp
void VulkanEngine::init_scene()
{
	RenderObject monkey;
	monkey.mesh = get_mesh("monkey");
	monkey.material = get_material("defaultmesh");
	monkey.transformMatrix = glm::mat4{ 1.0f };

	_renderables.push_back(monkey);

	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {

			RenderObject tri;
			tri.mesh = get_mesh("triangle");
			tri.material = get_material("defaultmesh");
			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			tri.transformMatrix = translation * scale;

			_renderables.push_back(tri);
		}
	}
}
```
We create 1 monkey, add it as the first thing to the renderables array, and then we create a lot of triangles in a grid, and put them around the monkey.

Next, filling the draw function.

```cpp
void VulkanEngine::draw_objects(VkCommandBuffer cmd,RenderObject* first, int count)
{
	//make a model view matrix for rendering the object
	//camera view
	glm::vec3 camPos = { 0.f,-6.f,-10.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = first[i];

		//only bind the pipeline if it doesn't match with the already bound one
		if (object.material != lastMaterial) {

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
			lastMaterial = object.material;
		}


		glm::mat4 model = object.transformMatrix;
		//final render matrix, that we are calculating on the cpu
		glm::mat4 mesh_matrix = projection * view * model;

		MeshPushConstants constants;
		constants.render_matrix = mesh_matrix;

		//upload the mesh to the GPU via push constants
		vkCmdPushConstants(cmd, object.material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

		//only bind the mesh if it's a different one from last bind
		if (object.mesh != lastMesh) {
			//bind the mesh vertex buffer with offset 0
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->_vertexBuffer._buffer, &offset);
			lastMesh = object.mesh;
		}
		//we can now draw
		vkCmdDraw(cmd, object.mesh->_vertices.size(), 1, 0, 0);
	}
}
```
We first calculate the matrices for the camera itself. Then we iterate each object in the renderables array, and render each of them in order. The loop right now is a simple one with no sorting, but there isn't much need for sorting when the objects in the renderables array are already sorted. You can sort the renderables array by pipeline pointer if you want.
Note how we are checking lastMesh and lastMaterial in the BindVertexBuffers and BindPipeline calls. There is no need to rebind the same vertex buffer over and over between draws, and the pipeline is the same, but we are pushing the constants on every single call.
The loop here is a lot higher performance that you would think. This simple loop will render thousands and thousands of objects with no issue. Binding pipeline is a expensive call, but drawing the same object over and over with different push constants is very fast.

Last thing is to replace the old code in the `draw()` function with calling this function. It should end up like this.

```cpp
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	draw_objects(cmd, _renderables.data(), _renderables.size());

	//finalize the render pass
	vkCmdEndRenderPass(cmd);
```

Now that we have changed the code, you can try to remove the _triangleMesh, _monkeyMesh, and their hardcoded pipelines from Vulkan Engine class. We no longer need any of that, as we have a simple system to manage the meshes and pipelines now.

Feel free to play around with the amount of triangles and monkeys created in the load_scene function, or to change how they are generated. You will find you can reach object counts in the hundreds of thousands before it gets slow if you disable the debug layers and run in release mode. In debug mode with layers it won't be that fast.

Now that we have an engine that actually does something. There are some exercises you can try to do.
- Clean up all the hardcoded pipelines and meshes from VulkanEngine class
- Create multiple pipelines with newer shaders, and use them to render monkeys each with a different material each
- Load more meshes. As long as it's an obj with TRIANGLE meshes, it should work fine. Make sure on export that the obj includes normals and colors
- Add WASD controls to the camera. For that, you would need to modify the camera matrices in the draw functions.
- Sort the renderables array before rendering by Pipeline and Mesh, to reduce number of binds.

In the github repo of the chapters, the chapter-3-scene will have the full refactor done. But not the other optional features.


Next: [Double buffering]({{ site.baseurl }}{% link docs/chapter-4/double_buffering.md %})

{% include comments.html term="Chapter 3 Comments" %}





