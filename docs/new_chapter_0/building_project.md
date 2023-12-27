---
layout: default
title: Building Project
parent: "0. Project Setup"
nav_order: 2
---

# Building the project

Start by syncing the Git project at <https://github.com/vblanco20-1/vulkan-guide/>.
The all-chapters branch contains the example code for the entire project, and the starting-point branch contains the initial empty project setup. I recommend you sync both branches to different folders, and if at one point you get stuck, check how the code works in the all-chapters code.
If you want a direct download to the code without using Git just download from these direct links. 

[All Chapters](https://github.com/vblanco20-1/vulkan-guide/archive/all-chapters-1.3-wip.zip)

[Starting Point](https://github.com/vblanco20-1/vulkan-guide/archive/starting-point-2.zip)


You will need Visual Studio to build the project. The code in the project should work cross-platform, but this guide is windows-centric.

First, install the Vulkan SDK <https://vulkan.lunarg.com/sdk/home>. This will contain the development headers and libraries needed to create Vulkan applications. It is put in a global location, so you don't need to worry about where it gets installed.

Then grab CMake, as it's the build system we are going to use. <https://cmake.org/>.

Once CMake is installed, use CMake-gui to open the project root CMakeLists. Press the Configure button.

If at this point it complains about not finding Vulkan_INCLUDE_DIR or Vulkan_LIBRARY, make sure you've finished installing the Vulkan SDK, then close and open CMake-Gui and try configuring again. 

Now that the dependencies are set, you can proceed with CMake and Configure then Generate the project files for Visual Studio (or your IDE of choice). 

From Visual Studio, select the engine target, set as Startup Project, and you can now compile and execute it by just hitting F5 (start debugging)

![vs]({{site.baseurl}}/assets/images/vs_compile.png)

# Third party libraries
Under the third_party folder, we have several different libraries that we are going to use across the tutorial. Those libraries are all embedded as sourcecode into the folder. Some of them have source code edits or are different from what is on their public repo

This is the list of libraries we are using.

- vulkan SDK: This one is not in the third_party folder, but its a dependency we have. You should have it installed from the explanation above
- vkbootstrap: We will be using this to simplify the vulkan initialization logic. It will do things like select what gpu to use for us, and setup some initial structures.
- VMA: Vulkan Memory Allocator. This is a library that implements high performance memory allocators for GPU structures. We will use that instead of trying to write the allocators ourselves. It will manage our GPU memory
- SDL: Highly multiplatform library for windowing, input, audio, and a few other things. We will be using its windowing and input parts on the tutorial.
- GLM : Math library. Will provide vectors, matrices, and so on.
- {fmt} : String formatting and console output library. We will use it instead of std::cout as it runs significantly faster and gives us great utilities to display strings and other formatting options.
- stb_image: for loading PNG or jpeg images.
- dear imgui: Great UI library very useful for debug interfaces. Will lets us display some timings or add some UI windows.
- fastgltf: High performance GLTF loading library. 


Next: [ Code Walkthrough]({{ site.baseurl }}{% link docs/new_chapter_0/code_walkthrough.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
