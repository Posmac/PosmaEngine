#version 450 core

layout(location = 0) out vec4 AttachmentColor;

layout(location = 0) in vec2 TexCoords;

//gbuffer
layout(set = 0, binding = 0) uniform sampler2D AlbedoTarget;
layout(set = 0, binding = 1) uniform sampler2D NormalTarget;
layout(set = 0, binding = 2) uniform sampler2D DepthTarget;
layout(set = 0, binding = 3) uniform sampler2D WorldPositionTarget;
layout(set = 0, binding = 4) uniform sampler2D EmissionTarget;
layout(set = 0, binding = 5) uniform sampler2D SpecularGlossTarget;
layout(set = 0, binding = 6) uniform sampler2D RoughtMetallTarget;

//shadow 
layout(set = 1, binding = 0) uniform ShadowsBuffer
{
    mat4 DirectionalViewProjectionMatrix;
} shadowBuffer;

layout(set = 1, binding = 1) uniform sampler2D DirectionalShadowMap;

float ShadowCalculation(vec2 texCoords, float currentDepth)
{
    texCoords = texCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(DirectionalShadowMap, texCoords).r; 
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec4 worldPosition = texture(WorldPositionTarget, TexCoords);
    vec4 ndc = shadowBuffer.DirectionalViewProjectionMatrix * worldPosition;
    ndc /= ndc.w;

    float shadow = ShadowCalculation(ndc.xy, ndc.z);

    AttachmentColor = texture(AlbedoTarget, TexCoords) * shadow;
}