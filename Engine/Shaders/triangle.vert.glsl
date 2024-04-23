#version 450 core

//binding 0 (perVertex)
layout(location = 0) in vec4 localPosition;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

//binding 1 (perInstance)
layout(location = 3) in vec4 meshToParentRow1;
layout(location = 4) in vec4 meshToParentRow2;
layout(location = 5) in vec4 meshToParentRow3;
layout(location = 6) in vec4 meshToParentRow4;

layout(set = 0, binding = 0) uniform GlobalBuffer
{
	mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ViewProjectionMatrix;
    float Time;
} globalBuffer;

layout(set = 1, binding = 0) uniform ModelData
{
	mat4 ModelToWorldMatrix;
} modelData;

layout (location = 0) out vec4 WorldPosition;
layout (location = 1) out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;

	mat4 meshToModel = mat4(meshToParentRow1, meshToParentRow2, meshToParentRow3, meshToParentRow4);
	mat4 modelToWorld = modelData.ModelToWorldMatrix * meshToModel;
	WorldPosition = modelToWorld * localPosition;
	gl_Position = globalBuffer.ViewProjectionMatrix * WorldPosition;
}