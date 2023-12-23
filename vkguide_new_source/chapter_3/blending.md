---
layout: default
title: Blending
parent: "New 3. Graphics Pipelines"
nav_order: 20
---

When we created the pipeline builder, we completely skipped over the blending logic, setting it as no blending. Blending is used for transparent objects and some graphical effects, so its important to have it. for that reason, we will make the rectangle we rendered last article transparent.

## Blending in the pipeline
We cant really control blending from shaders, thats property of the pipeline. The gpu hardware itself does the blending mathematics for us, and has a bunch of options. We will be adding 2 new blending modes into the pipeline builder, one will be additive blending, where it just adds the colors, and the other alpha-blend, where it would mix the colors.

Lets add these 2 functions into the pipeline builder

^code alphablend shared/vk_pipelines.cpp


When setting blending options in vulkan, we need to fill the formula on both color and alpha. The parameters work the same on both color and alpha. The formula works like this

```
outColor = srcColor * srcColorBlendFactor <op> dstColor * dstColorBlendFactor;
```
there are a few possible operators, but for the most part we will be doing `VK_BLEND_OP_ADD`, as its the most basic and guaranteed to work. There are many other much more advanced operators, but those come with extensions and we wont use them.

With the formula above, lets explain what the addition blending does.

`VK_BLEND_FACTOR_ONE` sets the blend factor to just 1.0, so no multiplying. `VK_BLEND_FACTOR_DST_ALPHA` on the other hand multiplies it by the alpha of the destination. Our blending ends up as this formula
```c
outColor = srcColor.rgb * 1.0 + dstColor.rgb * dstColor.a
```

The alpha-blend one will look like this instead.

```c
outColor = srcColor.rgb * (1.0 - dst.color.a) + dstColor.rgb * dstColor.a
```
Essentially making it into a lerp controlled by dstColor alpha.

Lets try to use it to see what it does. We dont have alpha set in our shaders, so lets just try the additive one. Change the blending on the `init_mesh_pipeline()` function.

```cpp
//pipelineBuilder.disable_blending();
pipelineBuilder.enable_blending_additive();
```

You should now see the rectangle we had as semi-transparent, letting the color below it show.

If you want, try also using it on the monkey head mesh, and see how it looks. Play around with the blending modes to see what effects they result on.

Before we move to chapter 4, lets implement window resizing.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}