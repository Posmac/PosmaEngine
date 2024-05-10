#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout (location = 0) in vec4 WorldPosition;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 ClipPos;

//is changed very frequently (material)
layout(set = 2, binding = 0) uniform sampler2D Albedo;

//shadow 
layout(set = 3, binding = 0) uniform ShadowsBuffer
{
    mat4 DirectionalViewProjectionMatrix;
} shadowBuffer;

layout(set = 3, binding = 1) uniform sampler2D DirectionalShadowMap;

float ShadowCalculation(vec2 texCoords, float currentDepth)
{
    texCoords = texCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(DirectionalShadowMap, texCoords).r; 
    // check whether current frag pos is in shadow
    float shadow = currentDepth <= closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec4 ndc = shadowBuffer.DirectionalViewProjectionMatrix * WorldPosition;
    ndc /= ndc.w;

    float shadow = ShadowCalculation(ndc.xy, ndc.z);

	AttachmentColor = texture(Albedo, TexCoord) * shadow;
}