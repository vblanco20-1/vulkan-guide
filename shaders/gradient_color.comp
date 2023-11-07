#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba16f,set = 0, binding = 0) uniform image2D image;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data1;
 vec4 data2;
 vec4 data3;
 vec4 data4;
} PushConstants;

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 size = imageSize(image);

    vec4 topColor = PushConstants.data1;
    vec4 bottomColor = PushConstants.data2;

    if(texelCoord.x < size.x && texelCoord.y < size.y)
    {
        float blend = float(texelCoord.y)/(size.y); 
    
        imageStore(image, texelCoord, mix(topColor,bottomColor, blend));
    }
}

