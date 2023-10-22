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

const vec2 offsets[9] = vec2[](
    vec2( -1, -1 ),
    vec2( -1, 0 ),
    vec2( -1, 1 ),
    vec2( 0, -1 ),
    vec2( 0, 0 ),
    vec2( 0, 1 ),
    vec2( 1, -1 ),
    vec2( 1, 0 ),
    vec2( 1, 1 )
);

void main()
{
	vec4 texCoords = shadowData.DirLightMatrix * WorldPosition;
	texCoords /= texCoords.w;

	vec2 uv = texCoords.xy * 0.5 + 0.5;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(DirShadowMap, 0);

	for(int i = 0; i < 9; i++)
	{
	    float pcfDepth = texture(DirShadowMap, uv + offsets[i]).r; 
	    shadow += texCoords.z > pcfDepth ? 1.0 : 0.0;        
	}
	shadow /= 9.0;

	AttachmentColor = texture(Albedo, TexCoord) * (1.0 - shadow); 
}