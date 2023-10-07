#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec4 WorldPosition;
layout (location = 1) in vec2 TexCoord;

layout(set = 0, binding = 1) uniform sampler2D DirShadowMap;

layout(set = 0, binding = 2) uniform ShadowBuffer
{
	mat4 DirLightMatrix;
} shadowData;

//is changed very frequently
layout(set = 1, binding = 0) uniform sampler2D Albedo;

void main()
{
	vec4 texCoords = shadowData.DirLightMatrix * WorldPosition;
	texCoords /= texCoords.w;

	vec2 uv = texCoords.xy * 0.5 + 0.5;
	float shadow = texture(DirShadowMap, uv).x;

	float isInShadow = texCoords.z > shadow ? 0.0 : 1.0;

	AttachmentColor = texture(Albedo, TexCoord) * isInShadow; 
}