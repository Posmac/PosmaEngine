#version 450 core

layout(location = 0) in vec3 localPosition;

void main()
{
	gl_Position = vec4(localPosition, 1.0);
}