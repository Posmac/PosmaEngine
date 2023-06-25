#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout(set = 0, binding = 1) uniform FragmentBuffer
{
	vec4 color;
} fubo;

layout(set = 0, binding = 2) uniform sampler2D Texture;
layout(set = 0, binding = 3) uniform sampler2D CobbleTexture;

layout(push_constant) uniform Constants 
{
	float Time;
} PushConstants;

layout (location = 0) in vec2 TexCoords;

void main()
{
	AttachmentColor = texture(CobbleTexture, TexCoords) + texture(Texture, TexCoords); 
}