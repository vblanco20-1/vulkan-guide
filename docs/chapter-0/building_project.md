---
layout: default
title: Building Project
parent: Chapter 0
nav_order: 2
---

# Building the project

Start by syncing git project at <https://github.com/vblanco20-1/vulkan-guide/>
The all-chapters branch contains the example code for the entire project, and the starting-point branch contains the initial empty project setup. I recomend you sync both branches to different folders, and if at one point you get stuck, check how the code works in the all-chapters code.

You will need Visual Studio to build the project. The code in the project should work multiplatform, but this guide is windows-centric.


To start, grab CMake, as its the build system we are going to use. <https://cmake.org/>

Once CMake is installed, use CMake-gui to open the project root CMakeLists. Press the Configure button.
At this point, it should give you an error about missing SDL 2.

SDL2 is a library dependency that we need to get separately from the rest.

You can grab it here <https://www.libsdl.org/download-2.0.php> . I recomend you just take the development libraries.
Once you have unzipped SDL somewhere, put the root path in the SDL2_dir variable of CMake.
Note, this is not the /libs or similar folder, but the root folder of SDL itself.

Only one thing left, and thats the Vulkan SDK. Get it from here and install it <https://vulkan.lunarg.com/sdk/home>
It doesnt matter where its installed. Once its installed and the PC is rebooted, CMake will pick up Vulkan automatically.

Now that the dependencies are set, you can proceed with CMake and Configure then Generate the project files for Visual Studio (or your IDE of choice). 

Cmake should look like this:
 

![cmake]({{site.baseurl}}/assets/images/Cmakesetup.png)

From Visual Studio, select the vulkan-guide target, set as Startup Project, and you can now compile and execute it by just hitting F5 (start debugging)

![vs]({{site.baseurl}}/assets/images/vs_compile.png)

If you get the message of SDL2 dll missing, go to your sdl folder, lib directory, and grab the dlls from there. Paste them on vulkan_guide/bin/Debug/ folder. (Or Release) . The dll has to be by the side of the vulkan_guide.exe

![dll]({{site.baseurl}}/assets/images/sdl_dll.png)


Next: [Chapter 0 code walkthrough]({{ site.baseurl }}{% link docs/chapter-0/code_walkthrough.md %})