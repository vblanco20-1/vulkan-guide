---
layout: default
title: Textures
parent: "New 4. Textures and Engine Architecture"
nav_order: 2
---

We already showed how to use images when we did compute based rendering, but there are things about images we still need to deal with, specially how to use them in graphics shaders for rendering and display. We will begin here by creating a set of default textures for our engine, and then load a texture from a file.

First, we need to add functions to the VulkanEngine class to deal with creating images.

