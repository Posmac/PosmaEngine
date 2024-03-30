#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec4 WorldPosition;
layout (location = 1) in vec2 TexCoord;

//is changed very frequently
layout(set = 1, binding = 0) uniform sampler2D Albedo;

//shadow 
layout(set = 2, binding = 0) uniform ShadowsBuffer
{
    mat4 DirectionalViewProjectionMatrix;
} shadowBuffer;

layout(set = 2, binding = 1) uniform sampler2D DirectionalShadowMap;

void main()
{
    vec4 texCoords = shadowBuffer.DirectionalViewProjectionMatrix * WorldPosition;
	texCoords /= texCoords.w;

    vec2 uv = texCoords.xy * 0.5 + 0.5;

    float depth = texture(DirectionalShadowMap, TexCoord).r; 
    depth = texCoords.z > depth ? 1.0 : 0.0;       

	AttachmentColor = texture(Albedo, TexCoord) * depth; 
}