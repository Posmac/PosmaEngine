#version 450 core 

layout(location=0) out vec4 AlbedoRT;
layout(location=1) out vec4 NormalRT;
layout(location=2) out vec4 DepthRT;
layout(location=3) out vec4 WorldPosRT;
layout(location=4) out vec4 EmissionRT;
layout(location=5) out vec4 SpecularGlossRT;
layout(location=6) out vec4 RoughMetallicRT;

layout(location=0) in vec4 WorldPosition;
layout(location=1) in vec4 WorldNormal;
layout(location=2) in vec4 ClipPos;
layout(location=3) in vec2 TexCoords;

//is changed very frequently (material)
layout(set = 2, binding = 0) uniform sampler2D Albedo;
layout(set = 2, binding = 1) uniform sampler2D Normal;
layout(set = 2, binding = 2) uniform sampler2D Metallic;
layout(set = 2, binding = 3) uniform sampler2D Roughness;

void main()
{
    DepthRT = vec4(gl_FragCoord.z, 0, 0, 1);
    EmissionRT = vec4(1, 0, 0, 1);
    SpecularGlossRT = vec4(0, 1, 0, 1);
    RoughMetallicRT = vec4(0, 0, 1, 1);
    NormalRT = WorldNormal;
    WorldPosRT = WorldPosition;
	AlbedoRT = texture(Albedo, TexCoords);
}