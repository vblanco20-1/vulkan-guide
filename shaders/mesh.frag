#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	float lightValue = max(dot(inNormal, vec3(0.3f,1.f,0.3f)), 0.7f);

	vec3 color = inColor * texture(albedoTex,inUV).xyz;

	outFragColor = vec4(color *lightValue ,1.0f);
	//outFragColor = vec4(texture(albedoTex,inUV).xyz ,1.0f);
	//outFragColor = vec4(inColor * dot(inNormal, vec3(0.f,1.f,0.f),1.0f);
}

