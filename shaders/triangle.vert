#version 450

layout (location = 0) out vec3 outColor;

void main() 
{
	if(gl_VertexIndex == 0)
	{
		gl_Position = vec4(1.f,1.f, 0.1f, 1.0f);
		outColor = vec3(1.0f, 0.0f, 0.0f);
	}
	else if(gl_VertexIndex == 1)
	{
		gl_Position = vec4(-1.f,1.f, 0.1f, 1.0f);
		outColor = vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		gl_Position = vec4(0.f,-1.f, 0.1f, 1.0f);
		outColor = vec3(0.0f, 0.0f, 1.0f);
	}
}

