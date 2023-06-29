#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec2 TexCoords;

layout(set = 0, binding = 2) uniform sampler2D Albedo;

void main()
{
	AttachmentColor = texture(Albedo, TexCoords); 
}