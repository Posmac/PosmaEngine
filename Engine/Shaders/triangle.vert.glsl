#version 450 core

layout(location = 0) in vec3 localPosition;

layout(set = 0, binding = 0) uniform VertexBuffer
{
	vec4 offset;
} ubo;

void main()
{
	gl_Position = ubo.offset + vec4(localPosition, 1.0);
}