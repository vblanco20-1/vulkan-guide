---
layout: default
title: Performance and frame analysis - Metaphor: ReFantazio
parent: Extra Chapter
nav_order: 1
---

# Intro 
When trying the new Atlus game Metaphor: Refantazio, it came to me that the performance on steam deck was sub-standard for the kind of graphics this game has. While it has a great art-style, the performance does not meet the techniques seen on it, as it seems to come from persona 5 engine which ran on a PS3. For this reason, i captured a few different scenes, and analyzed the result, to try to see what is the cause for such a performance.

My focus here is on the Steam Deck gpu in particular, and how the rendering techniques seen in this game work for it. The game focuses on PS5 and PC, and does not target Steam Deck in particular, but the Steam Deck provides a good proxy for a mid range/low end GPU, and it has very nice performance measuring tools we can use on it. The workflow explained here is usable for vkguide code too, and you can see how the techniques run on-device.

# Performance analysis workflow
The analysis will be done with 2 main tools. First one is RenderDoc https://renderdoc.org , and the second one is Radeon GPU Profiler https://gpuopen.com/rgp/ . Renderdoc will let us replay the frame drawcall by drawcall, and check what exactly happens during the frame, while Radeon Profiler will give us the timings for the commands executed and information about them. By comparing the same capture on both programs, we can see what goes on and what specific draws are slow/fast on them. 

The Steam Deck uses a Van Gogh intergrated GPU by AMD, as part of a chip alongside the CPU. It uses the same memory for both CPU and GPU. Its main weakness is that its a fairly small gpu (as all integrated gpus are), and that its memory bandwidth is fairly bad as it uses normal DDR ram instead of the faster GDDR seen on dedicated GPUs. Its fairly similar to something like the PS4 GPU in architecture due to this shared memory scheme. While the GPU is small, its a modern GPU, and supports most of the latest features in vulkan. Both versions of the vkguide tutorial work on it unchanged.

The Steam Deck renders at a low resolution of 1280x800p which lets it still run somewhat OK even with its low memory bandwidth. 

To test the performance of this game, ive captured 3 different scenes from the game. The first playable area of a desert, a rest area in a forest that is tiny, and the main city at night. All of these are found within a couple hours of starting the demo.

Keep in mind this article is written on 08/10/2024 and on the demo version of the game. Its possible that the developers improve things from here on the final version, or on patches later.
The game does not use vulkan natively, but the translation layers in the steamdeck Proton system translate it all to Vulkan, which we can analyze. There is a bit of strange stuff in renderdoc due to this, but its fine and these layers are very well optimized.

# Frame analysis
Lets begin with the game, checking what exactly goes on in a frame, and how the game renders its world.

Here are images of the 3 scenes for the analysis. We will focus on the City one as its more interesting, but the other 2 will be used to compare performance in different areas. The game has no graphical settings other than ambient occlusion on/off, we are keeping it on. Native resolution at 100% scaling

The desert area looks like this. We have a open space, with a bunch of transparent dust effects, and a terrain, with the main character on it.
![map]({{site.baseurl}}/diagrams/metafor/desert.png)
Runs at ~45 FPS

The rest area in the forest looks like this. Very small map, with a few characters sitting at a bonfire
![map]({{site.baseurl}}/diagrams/metafor/rest_forest.png)
Runs at ~40 FPS

Lastly, we have the City. Large map, with tons of objects on it, and a lot of characters walking around. We also have a bit of text on it
![map]({{site.baseurl}}/diagrams/metafor/city.png)
Runs at ~30 FPS

These 3 areas run slow for different reasons, but the have some parts in common. In other zones like dungeons or combat scenes, the game keeps 60 FPS

Capture the scene in renderdoc using steam deck debug/dev utilities, and lets look at that City to see what the game is doing.

We can see this in renderdoc
![map]({{site.baseurl}}/diagrams/metafor/renderdoc_full_city.png)
There are a LOT of draw commands in here. The final output, a 1280x800 image mostly as we could expect. 

The game begins with a compute pass where it runs a bunch of compute shaders. 
![map]({{site.baseurl}}/diagrams/metafor/rdc_compute.png)

What these shaders do, i have no idea, as we dont have access to the original shaders or any debug information that could give us a hint. What we can see, is that they are shaders that do work purely on data buffers, and not textures. Most likely, they are something like GPU particle calculations and skinning calculations for the characters. We are not going to see ANY further usage of compute shaders in this engine.

The next thing we see, is multiple depth-only passes. These are cascaded shadowmaps. 3 of them. Supiciously far too many commands here for what we see, but we will check that later.

Here are the different shadow maps. Each of them is a 2048x2048 D32S8 format texture. 
![map]({{site.baseurl}}/diagrams/metafor/cascades.png)

We can see the characters on one of the 3 cascades if one pays attention, and we see that the last cascade draws most of the map.

After rendering the shadows, the game renders the main environment into a set of GBuffers, using a classic deferred rendering scheme
![map]({{site.baseurl}}/diagrams/metafor/deferred_env.png)

The one in the full image is a RGBA 8 bit color, that contains the main texture of the objects. Then we have another gbuffer that stores material propeties at format RGBA 8-bit, which seem to be roughness and metallic. The windows here can be seen on a pink color, so it seems it also selects between multiple materials. 
Next we have a normal gbuffer of format RGBA16f, which is a considerable mistake, as its using a considerably overkill format for normals. Normally a gbuffer normal will encode as RGB 8-bit (no alpha), or RG 16 (2 channels). So the game using RGBA16f is using twice the memory than it needs to. On a platform as constrained as the steamdeck, this is not good. 
Not pictured, but there is also a scene-depth texture using 32 bit float. This contains the distance towards the camera, and its a copy of the z-buffer. For this, there is no need to draw it here, as one could do it as a separate step where we load from the native depth buffer into this distance gbuffer, thus saving a lot of pixel write bandwidth on this.

The next pass we see, it is a depth-only pass where it draws the characters and other non-environment objects into the depth buffer. This combines the characters into the depth buffer for processing later.
![map]({{site.baseurl}}/diagrams/metafor/chardepth.png)

Now we begin with the deferred lighting. This seems to be bugged, as it first begins with drawing each light as a sphere, and setting the pixels that light covers into a stencil value. Once this is done for every light, it then draws the lights again, and their pixel shader applies the light to the pixels the sphere covers.

On here, that first pass is not used on the second, so this first pass setting the stencil values is completely useless. This is likely a bug on the developer side, and its going to cost us significant performance on the deferred lighting, which we will check later.

![map]({{site.baseurl}}/diagrams/metafor/deferred_light.png)
On this image we can see what a light does. A sphere is drawn that covers the bounds of the pointlight, and it applies light to the pixels inside it. There is no culling for the lights, so every single pointlight on the scene will draw its sphere, even if its offscreen.

Once all of the point lights are rendered, it applies a full-screen quad that draws the global light of the scene.
![map]({{site.baseurl}}/diagrams/metafor/deferred_global.png)

We are going to see a LOT of these full-screen quads. The game doesnt use compute for drawing, so it does all of its postprocessing as fullscreen quads of layers and layers, completely murdering the poor steamdeck bandwidth.

After the global light, we see another 3 full-screen quads one after another. One of them draws a sky/background using a lower resolution image, the other applies the background on top of the full resolution one, and the 3rd applies global fog. These are all subtle, so they look very similar to the image above.

Now, it draws the characters on top of the fully rendered background we have. Interesting enough, the characters are much brighter than the environment in this pass. Then the characters are drawn again, to draw their outlines. After that, it looks like this.
![map]({{site.baseurl}}/diagrams/metafor/characters.png)

Note the MUCH darker background compared to the characters.

Right after, there is another fullscreen pass that darkens the characters a bit. Several more fullscreen quads follow for various subtle effects. One of the more interesting one is a downsample of the scene that will be used for bloom and some visual effects later. 

There is a chain of like 8 full screen passes to compose this fancy lens flare effect. These are low resolution at least.
![map]({{site.baseurl}}/diagrams/metafor/lightfx.png)

All these get integrated back into the main scene, which no longer has the characters so bright vs the background.

![map]({{site.baseurl}}/diagrams/metafor/bloom.png)

On the image, ive raised the contrast so that we can clearly see the bloom effect on the windows and lights.

Next big pass is vfx and transparents, which results on this.
![map]({{site.baseurl}}/diagrams/metafor/transparents.png)

The transparent objects are drawn offscreen into a RGBA16f texture, and get composited right after. But of course, not without passing through a few postprocessing passes with fullscreen quads to blur them a bit.

Once composited into the main scene, they look like this.
![map]({{site.baseurl}}/diagrams/metafor/transparent_composite.png)

We are almost done with the frame now. The next thing is user interface and things like speech bubbles, with results on the final scene.

![map]({{site.baseurl}}/diagrams/metafor/fulldraw.png)

When rendering the text, it does 1 drawcall of a single quad per letter, which is suboptimal. A lot of the UI does this, and its quite a few draws in total.

All of these rendering passes here are at 720p resolution, while the steam deck is at 800p, so as the last thing the frame does, it copies it to the swapchain, getting the final look with the black borders at the top and bottom. Those borders are not artificial or drawn over, the game just doesnt render at the aspect ratio of the steam deck, and chooses to do all its rendering in the common 720p resolution and then copies to the final image.

We now have gone through the whole frame, and already identified a few strange parts that could be a performance bottleneck, or not. Renderdoc is not a profiler, but a system to understand whats going on. To really profile it, we need to continue in Radeon GPU Profiler, doing a capture at the same location

# Analyzing the performance

After loading the capture, we get greeted with this timeline. 
![map]({{site.baseurl}}/diagrams/metafor/rgp_global.png)

At the top, we have the wavefront occupancy view. This is a global chart of the "usage" of the GPU. Bigger bars mean more of the gpu is being used, and their color depends on what the gpu is executing at that point. We can see that for the first half, we are under very low usage, specially in the first 14 miliseconds where its all vertex shader invocations for some reason. Then there is an area with low occupancy but a mix of vertex and fragment shaders, and then we have 2 blocks of heavy fragment shader usage. The frame begins with 1 ms worth of compute shaders, which are those misterious compute shaders we saw when we saw it on renderdoc.

Now we need to try to identify what those parts of the scene actually do, so lets check the command stream, and try to sync it with renderdoc.

Seeing it, all of those initial vertex shader sections are from the shadow rendering, where it spends a total of 12.5 miliseconds to render the 3 cascades. We can even see those 3 "sections" of the drawn shadow images. So what exactly is going on here? A game that runs at 60 fps needs to draw its whole scene in 16.6 miliseconds, and we have shadows here, just 3 cascades, in 12.5, completely bloating the budget we have for the frame,.

If we select the area of those shadows, we can see on the right what RGP says

![map]({{site.baseurl}}/diagrams/metafor/shadows.png)

So aparently, we have all of it as primitive shader invocation, which is basically vertex shader. Very few shader invocations are being done executing pixel shaders, only 15%. Lets go back to renderdoc, and see in more detail what are those shadows drawing.

Something interesting comes to view, and it is that the 3 shadows are rendering the exact same objects. But each of the cascades is rendering a considerably different image. One of them is close to the characters and only renders characters + a bit of ground, while the far scale one draws most of the city. Turns out, that the game does not do any sort of culling for its shadow casting. Even the small shadow cascade gets thrown every single object in the map at it. This is crazy, and its what results on the timeline we are seeing on RGP. The shadow cascades are being sent so many geometry, most of it off-screen, that the entire execution is completely bottlenecked on pure triangle processing in the vertex shaders. 

With correct culling in here, the only shadow cascade that would be expensive to draw would be the far distance one, while the other 2 would be very cheap to draw. An stimate is that it would probably make the time for shadows to go from 12.5 miliseconds down to 4 miliseconds. Still expensive, but then it could also render the shadows at a lower resolution, as 3 2k resolution shadows is a bit on the expensive side. Even then, the biggest bottleneck is the raw amount of triangles thrown at the GPU for these shadows.

Comparing with the captures on the other 2 zones, it looks like the same happens. This is a engine-wide issue and happens on every map, it seems.

The next zone we can identify is the gbuffer environment draw. This one also has low occupancy overall, but a less extreme vertex bottleneck. It takes around 4 miliseconds to draw the background, and another 2 miliseconds to draw the characters. 

On the background, there are some draws that are slow in a strange way. All others are faster. 2 draws, on this city scene, take 1 milisecond by themselves. Looking at them in renderdoc, they are terrain draws, and have a different and more complex shader than the other meshes. We dont really know what the shader is doing, but radeon profiler tells us that it has low occupancy, with it being at 10/16 on the fragment shader, and 12/16 on the vertex shader. 

![map]({{site.baseurl}}/diagrams/metafor/rgp_occupancy.png)
