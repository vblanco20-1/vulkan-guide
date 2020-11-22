#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vOctNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 texCoord;
layout (location = 2) out vec3 outNormal;
layout(location = 3) out vec4 ShadowCoord;
layout(set = 0, binding = 0) uniform  CameraBuffer{   
    mat4 view;
    mat4 proj;
	mat4 viewproj;

} cameraData;


layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
	mat4 sunlightShadowMatrix;
} sceneData;

struct ObjectData{
	mat4 model;
vec4 spherebounds;
vec4 extents;
}; 


vec3 OctNormalDecode(vec2 f)
{
    f = f * 2.0 - 1.0;
 
    // https://twitter.com/Stubbesaurus/status/937994790553227264
    vec3 n = vec3( f.x, f.y, 1.0 - abs( f.x ) - abs( f.y ) );
    float t = clamp( -n.z,0.f,1.f );
	
	n.x += n.x >= 0.0f ? -t : t;
    n.y += n.y >= 0.0f ? -t : t;

    return normalize( n );
}

//all object matrices
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{   

	ObjectData objects[];
} objectBuffer;

//all object indices
layout(set = 1, binding = 1) readonly buffer InstanceBuffer{   

	uint IDs[];
} instanceBuffer;

void main() 
{	
	uint index = instanceBuffer.IDs[gl_InstanceIndex];
	
	vec3 vNormal = OctNormalDecode(vOctNormal);

	mat4 modelMatrix = objectBuffer.objects[index].model;
	mat4 transformMatrix = (cameraData.viewproj * modelMatrix);
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outNormal = normalize((modelMatrix * vec4(vNormal,0.f)).xyz);
	outColor = vColor;
	texCoord = vTexCoord;

	ShadowCoord = sceneData.sunlightShadowMatrix * (modelMatrix* vec4(vPosition, 1.0f)  );
}
