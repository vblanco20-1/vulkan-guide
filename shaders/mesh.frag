#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1 ) in vec3 inColor;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	float lightValue = max(dot(inNormal, vec3(0.3f,-1.f,0.3f)), 0.3f);

	outFragColor = vec4(inColor *lightValue ,1.0f);
	//outFragColor = vec4(inColor * dot(inNormal, vec3(0.f,1.f,0.f),1.0f);
}

