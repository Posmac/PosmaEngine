#version 450 core

//binding 0 (perVertex)
layout(location = 0) in vec4 localPosition;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

//binding 1 (perInstance)
layout(location = 3) in vec4 im1;
layout(location = 4) in vec4 im2;
layout(location = 5) in vec4 im3;
layout(location = 6) in vec4 im4;

layout(set = 0, binding = 0) uniform PerFrameBuffer
{
	mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ViewProjectionMatrix;
    float Time;
} perFrameBuffer;

//layout(set = 0, binding = 1) uniform MeshBuffer
//{
//	mat4 ModelMatrix;
//} meshBuffer;

layout (location = 0) out vec4 WorldPosition;
layout (location = 1) out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;

	mat4 instanceMatrix = mat4(im1, im2, im3, im4);
	WorldPosition = instanceMatrix * localPosition;
	gl_Position = perFrameBuffer.ViewProjectionMatrix * WorldPosition;
}