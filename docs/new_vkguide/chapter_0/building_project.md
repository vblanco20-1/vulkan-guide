---
layout: default
title: Building Project
parent: "New 0. Initial Setup"
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


Next: [Chapter 0 code walkthrough]({{ site.baseurl }}{% link docs/new_vkguide/chapter-0/code_walkthrough.md %})
