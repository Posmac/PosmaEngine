#version 450 core

layout(location = 0) in vec4 localPosition;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 0) uniform PerFrameBuffer
{
	mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ViewProjectionMatrix;
    float Time;
} perFrameBuffer;

layout(set = 0, binding = 1) uniform MeshBuffer
{
	mat4 ModelMatrix;
} meshBuffer;

layout (location = 0) out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;
	gl_Position = perFrameBuffer.ViewProjectionMatrix * meshBuffer.ModelMatrix * localPosition;
}