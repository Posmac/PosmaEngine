#version 450

#include "visibility_utils.glsl"

layout(location=0) out vec4 VisibilityBufferRT;

layout(location=0) flat in uint InstanceID;
//layout(location=1) flat in uint VertexID;

void main()
{
	VisibilityBufferRT = unpackUnorm4x8(calculateOutputVIID(gl_PrimitiveID, InstanceID));
}