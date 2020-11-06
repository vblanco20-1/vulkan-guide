//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 inNormal;
//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;


void main() 
{	
	vec3 color = inColor.xyz;

	float lightAngle = clamp(dot(inNormal, sceneData.sunlightDirection.xyz),0.f,1.f);
	vec3 lightColor = sceneData.sunlightColor.xyz * lightAngle;

	outFragColor = vec4(color * lightColor,1.0f);
	outFragColor = vec4(inColor + sceneData.ambientColor.xyz,1.0f);
}