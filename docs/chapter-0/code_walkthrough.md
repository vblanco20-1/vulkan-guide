---
layout: default
title: Code Walkthrough
parent: "0.. Initial Setup"
nav_order: 3
---

# Walkthrough
Because we are starting this chapter with an already made code skeleton, we are going to see what it actually does.

The files are all stored in the project/src/ folder

- vk_engine.h/cpp : This will be the main class for the engine, and where most of the code of the tutorial will go
- vk_initializers.h/cpp : Vulkan initialization of types gets very verbose, so we will create some small helpers here. There are a lot of them, so it will be its own thing
- vk_types.h : As the tutorial continues, we will add the "basic" types such as Vertex definitions here.
- main.cpp : Entry point for the code. Has nothing but just calls into vk_engine code

Physical design (how the project is laid out) is very important for complex projects. We will call a pair of matched .h/.cpp files a "module". It doesn't necessarily have to be only one class, or a class at all. We will have them be generally standalone when possible. The reason for calling them modules is that they will likely *be* modules once C++20 modules are widely adopted.

vk_types is completely standalone (depends on nothing other than vulkan), and so will be the vk_initializers component. Once they grow, you can safely keep them for your own projects as your own small abstractions.
vk_engine will be the "end point" of almost everything. It will depend on most parts of the project.

Whenever possible, we will try to keep the headers of each component as lightweight as possible. The lighter the headers are, the faster your program will compile, and this is crucial when dealing with C++

# Code

main.cpp
```cpp
#include <vk_engine.h>

int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.init();

	engine.run();

	engine.cleanup();

	return 0;
}
```

We start with something simple, main.cpp. We do nothing here except immediately call into the Vulkan engine methods. 

In the future, this could be a good place to set some configuration parameters brought from the command line arguments at argc/argv or a settings file.

vk_types.h holds this

```cpp
#pragma once
#include <vulkan/vulkan.h>
```

We include the main header for Vulkan, which is the `<vulkan/vulkan.h>` you can see. That will include all the Vulkan function definitions and types that we will need for everything.
`#pragma once` is a preprocessor directive that tells the compiler to never include this twice into the same file. It's equivalent to include guards, but cleaner.

vk_init.h looks like this

```cpp
#pragma once

#include <vk_types.h>

namespace vkinit {
}
```

We include the vk_types header, which brings Vulkan itself (we will need it), and we declare a namespace for the functions we will add here later.

Finally, we get into vk_engine.h, the main class

```cpp

#pragma once

#include <vk_types.h>

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

    struct SDL_Window* _window{ nullptr };
	
	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();
};
```

As with vk_init, we include vk_types. We already need a Vulkan type in VkExtent2D.
The Vulkan engine will be the core of everything we will be doing.
We have a flag to know if the engine is initialized, a frame number integer (very useful!), and the size of the window we are going to open, in pixels. 

The declaration `struct SDL_Window* _window;` is of special interest. Note the `struct` at the beginning. This is called a forward-declaration, and its what allows us to have the `SDL_Window `pointer in the class, without including SDL on the Vulkan engine header. This variable holds the window that we create for the application.

With the header seen, lets go to the cpp files.

vk_engine.cpp line 1
```cpp
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_types.h>
#include <vk_initializers.h>
```
Unlike in the other files, in this one we include a few more things.
We include both `<SDL.h>` and `<SDL_vulkan.h>`. SDL.h holds the main SDL library data  for opening a window and input, while SDL_vulkan.h holds the Vulkan-specific flags and functionality for opening a Vulkan-compatible window and other Vulkan-specific things.

vk_engine.cpp, line 10

```cpp
void VulkanEngine::init()
{
    // We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
    //create blank SDL window for our application
	_window = SDL_CreateWindow(
		"Vulkan Engine", //window title
		SDL_WINDOWPOS_UNDEFINED, //window position x (don't care)
		SDL_WINDOWPOS_UNDEFINED, //window position y (don't care)
		_windowExtent.width,  //window width in pixels
		_windowExtent.height, //window height in pixels
		window_flags 
	);
	
	//everything went fine
	_isInitialized = true;
}
```
Here we see our first proper code, in the shape of creating a SDL window.
The first thing we do is init the SDL library. The SDL library contains quite a few things, so we have to send a flag of what do we want to use. SDL_INIT_VIDEO tells SDL that we want the main windowing functionality. That also includes basic input events like keys or mouse.

Once SDL has been initialized, we use it to create a window. The window is stored on the `_window` member for later use.

Because SDL is a C library, it does not support constructors and destructors, and things have to get deleted manually. 

If the window is created, it also has to be destroyed.

vk_engine.cpp, line 29
```cpp
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		SDL_DestroyWindow(_window);
	}
}
```
In a similar way that we did `SDL_CreateWindow`, we need to do `SDL_DestroyWindow` This will destroy the window for the program.
Over time, we will add more logic into this cleanup function. While its not completely necessary to cleanup properly, as the OS will delete everything for us anyway when the program terminates, its good practice to do it.

vk_engine.cpp, line 37
```cpp
void VulkanEngine::draw()
{
	//nothing yet
}
```

Our draw function is empty for now, but here is where we will add the rendering code.

vk_engine.cpp, line 42
```cpp
void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//close the window when user clicks the X button or alt-f4s
			if (e.type == SDL_QUIT) bQuit = true;
		}

		draw();
	}
}
```

This is our application main loop. We have an endless loop in the `while()`, that is only stopped when SDL receives the SDL_QUIT event

On every iteration of the inner loop, we do SDL_PollEvent. This will ask SDL for all of the events the OS has sent to the application during the last frame. In here, we can check for things like keyboard events, mouse movement, window moving, minimization, and many others. For now we are only interested on the SDL_QUIT event. This event is called when the OS requests that the window needs to be closed.

And finally, every iteration of the main loop we call `draw();`

We now have seen how to open a window with SDL, and basically not much else.

There is really only one thing that can be added to this at this point, and is experimenting with the SDL events. 

As an exercise, read the documentation of SDL2 and try to get keypress events, using `std::cout` to log them.

Now we can move forward to the first chapter, and get a render loop going.

Next: [Initializing Vulkan]({{ site.baseurl }}{% link docs/chapter-1/vulkan_init_flow.md %})


{% include comments.html term="0. Comments" %}




