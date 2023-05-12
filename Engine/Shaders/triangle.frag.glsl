#version 450 core

layout(location = 0) out vec4 AttachmentColor;

//layout(binding = 0) uniform FragmentBuffer
//{
//	vec4 color;
//} ubo;

void main()
{
	AttachmentColor = vec4(1, 0, 1, 1);
}