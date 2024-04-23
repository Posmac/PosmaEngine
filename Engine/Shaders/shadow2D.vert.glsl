#version 450 core

//binding 0 (perVertex)
layout(location = 0) in vec4 localPosition;
layout(location = 1) in vec4 localNormal;
layout(location = 2) in vec2 localTexcoords;

//binding 1 (perInstance)
layout(location = 3) in vec4 meshToParentRow1;
layout(location = 4) in vec4 meshToParentRow2;
layout(location = 5) in vec4 meshToParentRow3;
layout(location = 6) in vec4 meshToParentRow4;

layout(set = 0, binding = 0) uniform PerViewBuffer
{
    mat4 ViewProjectionMatrix;
} perViewBuffer;

layout(set = 1, binding = 0) uniform ModelData
{
	mat4 ModelToWorldMatrix;
} modelData;

void main()
{
	mat4 meshToModel = mat4(meshToParentRow1, meshToParentRow2, meshToParentRow3, meshToParentRow4);
	mat4 modelToWorld = modelData.ModelToWorldMatrix * meshToModel;
	gl_Position = perViewBuffer.ViewProjectionMatrix * modelToWorld * localPosition;
}