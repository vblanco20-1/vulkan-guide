#version 450

#extension GL_EXT_scalar_block_layout : require

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;

//push constants block
layout( push_constant ) uniform constants
{
	mat4 render_matrix;
} PushConstants;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(set = 0, binding = 0) readonly buffer VertexBuffer{   

	Vertex vertices[];
} vertexBuffer;

void main() 
{
	Vertex v = vertexBuffer.vertices[gl_VertexIndex];

	mat4 model = PushConstants.render_matrix;
	vec4 position = vec4(v.position, 1.0f);

	gl_Position = model *position;	

	outNormal = (model * vec4(v.normal, 0.f)).xyz;
	outColor = v.color.xyz;
}

