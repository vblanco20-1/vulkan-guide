---
layout: default
title: Toggling shaders
parent:  "2. The graphics pipeline, Hello Triangle"
nav_order: 15
---

Right now, we have 2 different shader sets for our triangle, but we do them hardcoded at compile time, and cant switch between them.
Lets create a system that lets us toggle the shaders for our triangle by pressing the Spacebar key.


## SDL Input events
Because we want to use the spacebar, we need to run code whenever the key is pressed. Thankfully, the SDL library we are using can do that easily. 

Lets first add a integer variable in VulkanEngine to use for choosing between the shaders.

```cpp
int _selectedShader{ 0 };
```
We will use 0 for red triangle, and 1 for colored triangle. Later we will add more shaders.

If you look into the `VulkanEngine::run()` function, you can see that we have the event loop
```cpp
//Handle events on queue
while (SDL_PollEvent(&e) != 0)
{
	//close the window when user alt-f4s or clicks the X button			
	if (e.type == SDL_QUIT) bQuit = true;
}
```
We are using the event loop right now to close the window, but its in here where we can also check for keyboard events, or for mouse events.
To detect the keyboard event, we need to change that code into:
```cpp
while (SDL_PollEvent(&e) != 0)
{
	if (e.type == SDL_QUIT)
	{ 
		bQuit = true;
	}
	else if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			_selectedShader += 1;
			if(_selectedShader > 1)
			{
				_selectedShader = 0;
			}
		}
	}
}
```

SDL_QUIT isnt the only event, we also have `SDL_KEYDOWN` and `SDL_KEYUP` along others. if the event type is `SDL_KEYDOWN`, then its a key PRESSED event. It will get called whenever the key is pressed, and if the key is kept pressed, the event will get called repeatadly a few times per second (this depends on the OS). 

Once we know that the event type is a key down event, we can check which key it is by looking at key.keysym.sym variable. If you look at SDL documentation, you will see more keycodes. The one we want for spacebar is `SDLK_SPACE`

We then rotate the _selectedShader integer, so it goes from 0 to 1, and then back to 0. 

Now that the input is done, we can toggle the shaders in the code.

## Multiple pipelines

Right now we have exactly 1 pipeline, but we can have as many as we want. As we are going to have 2 pipelines, we are going to create a variable for the second one in VulkanEngine class

```cpp
	VkPipeline _trianglePipeline;
	VkPipeline _redTrianglePipeline;
```

We are going to keep the `_trianglePipeline` from the last chapter to hold the pipeline for the colored triangle, and we are going to put the red version in `_redTrianglePipeline`

Lets make the code in `init_pipelines()` compile the shader modules for both pipelines.

```cpp
void VulkanEngine::init_pipelines()
{
	//compile colored triangle modules
	VkShaderModule triangleFragShader;
	if (!load_shader_module("../../shaders/colored_triangle.frag.spv", &triangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else {
		std::cout << "Triangle fragment shader succesfully loaded" << std::endl;
	}

	VkShaderModule triangleVertexShader;
	if (!load_shader_module("../../shaders/colored_triangle.vert.spv", &triangleVertexShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else {
		std::cout << "Triangle vertex shader succesfully loaded" << std::endl;
	}

	//compile red triangle modules
	VkShaderModule redTriangleFragShader;
	if (!load_shader_module("../../shaders/triangle.frag.spv", &redTriangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else {
		std::cout << "Red Triangle fragment shader succesfully loaded" << std::endl;
	}

	VkShaderModule redTriangleVertShader;
	if (!load_shader_module("../../shaders/triangle.vert.spv", &redTriangleVertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else {
		std::cout << "Red Triangle vertex shader succesfully loaded" << std::endl;
	}

	//other code ....
}
```

We only had to add the shader modules for the extra shaders. Now we can compile the pipeline with those.
Because we use the Pipeline Builder abstraction, creating pipelines that are very similar, but only change in shaders or a couple variables, is very simple.

We are going to overwrite the shader module variables in the builder, and make it build again.

```cpp
void VulkanEngine::init_pipelines()
{
	// other code ....

	//finally build the pipeline
	_trianglePipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	//clear the shader stages for the builder
	pipelineBuilder._shaderStages.clear();

	//add the other shaders
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, &redTriangleVertShader));

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, &redTriangleFragShader));

	//build the red triangle pipeline
	_redTrianglePipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
}
```

In this case, the pipeline configuration is all exactly the same, except for the shader stages, where we use different shader modules. So we clear the shader stages stored in the builder, and then replace them.

We now have the 2 different pipelines initialized, so we can implement the selection code.

## Switching pipeline at runtime

To change shaders, we only need to change what is sent to vkCmdBindPipeline, everything else is the same, so we are going to add a branch to select the pipeline.

```cpp
void VulkanEngine::draw()
{
	// other code ....
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);


	//once we start adding rendering commands, they will go here
	if(_selectedShader == 0)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);
	}
	else
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _redTrianglePipeline);
	}
	
	vkCmdDraw(cmd, 3, 1, 0, 0);

	//finalize the render pass
	vkCmdEndRenderPass(cmd);

	// other code ....
}
```

If we now run the application, we should be able to toggle between the 2 shaders that we have and see it display accordingly.

Next: [Cleanup]({{ site.baseurl }}{% link docs/chapter-2/cleanup.md %})

{% include comments.html term="Chapter 2 Comments" %}
