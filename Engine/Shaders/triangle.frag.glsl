#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout(set = 0, binding = 1) uniform FragmentBuffer
{
	vec4 color;
} fubo;

layout(push_constant) uniform Constants 
{
	float Time;
} PushConstants;

void main()
{
	AttachmentColor = fubo.color * sin(PushConstants.Time);
}