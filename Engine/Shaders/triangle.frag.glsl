#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec4 WorldPosition;
layout (location = 1) in vec2 TexCoord;

//is changed very frequently
layout(set = 1, binding = 0) uniform sampler2D Albedo;

void main()
{
	AttachmentColor = texture(Albedo, TexCoord); 
}