#version 450 core

//binding 0 (perVertex)
layout(location = 0) in vec4 localPosition;

//binding 1 (perInstance)
layout(location = 1) in vec4 im1;
layout(location = 2) in vec4 im2;
layout(location = 3) in vec4 im3;
layout(location = 4) in vec4 im4;

layout(set = 0, binding = 0) uniform PerViewBuffer
{
    mat4 ViewProjectionMatrix;
} perViewBuffer;

void main()
{
	mat4 instanceMatrix = mat4(im1, im2, im3, im4);
	gl_Position = perViewBuffer.ViewProjectionMatrix * instanceMatrix * localPosition;
}