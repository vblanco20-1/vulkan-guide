---
layout: default
title: "4. Buffers, Shader input/output"
nav_order: 106
has_children: true
parent: Legacy VkGuide, Vulkan 1.1
permalink: /docs/chapter_4

auto_comments: Chapter 4 Comments
---
![chapter4]({{site.baseurl}}/diagrams/chapter4.png)
# Chapter 4 : Shader input/output

In this chapter, we explain how to send data into shaders using Descriptor sets, and how to setup the different types of buffers offered by Vulkan.
We will create descriptor sets to use uniform buffers for our camera data, and we will also set dynamic uniforms and storage-buffers to render a full scene in an efficient way.

{: .fs-6 .fw-300 }
