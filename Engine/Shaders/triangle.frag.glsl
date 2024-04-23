#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec4 WorldPosition;
layout (location = 1) in vec2 TexCoord;

//is changed very frequently
layout(set = 2, binding = 0) uniform sampler2D Albedo;

//shadow 
layout(set = 3, binding = 0) uniform ShadowsBuffer
{
    mat4 DirectionalViewProjectionMatrix;
} shadowBuffer;

layout(set = 3, binding = 1) uniform sampler2D DirectionalShadowMap;

void main()
{
    vec4 ndc = shadowBuffer.DirectionalViewProjectionMatrix * WorldPosition;
	ndc /= ndc.w;

    vec4 projCoords = ndc * 0.5 + 0.5;

    float shadowMapDepth = texture(DirectionalShadowMap, projCoords.xy).r; 
    float depth = ndc.z < shadowMapDepth ? 1.0 : 0.0;

	AttachmentColor = texture(Albedo, TexCoord.xy) * depth; 
}