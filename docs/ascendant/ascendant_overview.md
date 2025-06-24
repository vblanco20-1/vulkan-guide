---
layout: default
title: Project Ascendant Overview
parent: Project Ascendant
nav_order: 1
auto_comments: Project Ascendant
---

Showcase video made from an early version, missing a lot of current features.
<iframe width="560" height="315" src="https://www.youtube.com/embed/ugsaEypPfn4?si=HkYnio8GM-auuGOV" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

Project Ascendant is a side project created to prototype a large scale voxel RPG. The code was not meant to be used for a shipping game, but to have a framework to try rendering features and procedural generation techniques, as game engines are normally not a good fit for this sort of exploration. 

Project Ascendant is inspired by Minecraft and Daggerfall, and it spawns the player into a procedurally generated landscape with various biomes and features such as cities and ruins. It creates animals and enemies in the map, and you can gather resources, craft them into other ones, and fight enemies with sword, bow, or magic. The game mechanics are basic, as most of the exploration was done around the graphics techniques and procedural generation, with the main "at the ground" gameplay mechanics being mostly there to make sure the architecture works. 

The codebase continues directly after the last chapter of VkGuide 2.0, and started as a possible chapter 6 and 7 to add draw indirect and lighting to the tutorial. 


![map]({{site.baseurl}}/diagrams/ascendant/grass.jpg)

# Graphics Features
* GPU driven voxel renderer, with near and far voxels to control LOD. Far voxels use a point-draw system, Near voxels are meshed using an algorithm similar to SurfaceNets (its not cubes!)
* Autobatching mesh renderer, with multiple materials and multiple meshes, tries to draw everything instanced. Not indirect but can be upgraded to do so if needed.
* Animated mesh support with very high efficiency being able to animate hundreds of meshes per milisecond per core with real GLTF animations
* Deferred renderer with compute based lighting
* LPV dynamic global illumination
* Froxel raymarched volumetric system
* Bloom, screenspace contact shadows, SSAO, and other lighting features
* Vulkan 1.3 exclusively. Runs on SteamDeck at high performance
* Particle systems using effekseer
* Targetting 2 km view range

# Engine features
* Absurdly over-engineered procedural generation system that scales with as many cores as you have with SIMD accelerated noises thanks to FastNoise
* Jolt physics support
* SQLite based save storage to save the generated worlds and game-state for save/load
* Parallel worlds/dimensions, used to go from overworld into generated sub-maps with interior dungeons instantly
* Opens into game in a couple seconds
* Lua based data management to define block/gameobject prototypes
* SoLoud used for audio for 3d audio
* 15 seconds compile time from scratch for the whole code outside of 3rd party dependencies
* Template system to create prefabs for buildings/trees/etc that the procedural generation can then spawn in the world
* Double-based object locations for large scale worlds.
* Hybrid ECS game object model designed around very high object counts

# Game features
* Enemy AI with pathfinding and sensing to either run from the player or attack him
* Day/night system
* Neutral NPCs at towns you can talk to and they give procedural quests or have inventories to sell you
* Inventory system
* First person combat system with multiple item types and effects. Sword combat, blocking, projectile weapons like bows, magic based on equipable runes
* Can gather plants/blocks/etc to inventory, and use them to craft new items
* Procedural terrain based on biomes + erosion simulation + placement of Points of Interest according to rules, which maps into the procedural quest system too, while still being fast


![map]({{site.baseurl}}/diagrams/ascendant/mountain_forest.jpg)

The goal of the project was exploration of procedural techniques and data systems, as those can be very annoying to iterate with when using Unreal Engine which is what most of the tech will be ported to later when beginning full development. The reason the game will be "ported" into Unreal Engine is because it will have console support and multiplayer, and doing those on a custom engine will take years. Unity is not considered due to licensing issues, and Godot is not ready for something of this scale, might as well go full custom there and take the extra years of development.

Over the coming weeks, I will be writing articles on sections of the engine that will be of interest to people, specially coming from VKGuide tutorial, but also outside of it if you are interested in game engine development in a "real" way, with an actual game project, not just a rendering engine. I will be covering architectural details, graphics techniques, game object architectures, and procedural generation.





{: .fs-6 .fw-300 }