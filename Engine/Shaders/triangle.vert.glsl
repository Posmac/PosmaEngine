#version 450 core

layout(location = 0) in vec4 localPosition;
layout(location = 1) in vec2 texCoord;

layout(set = 0, binding = 0) uniform VertexBuffer
{
	vec4 offset;
} ubo;

layout (location = 0) out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;
	gl_Position = ubo.offset + localPosition;
}