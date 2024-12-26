#version 450 core

vec4 position[3] = 
{
    vec4(-1.0, -1.0, 0.0, 1.0),  
    vec4(-1.0,  3.0, 0.0, 1.0),  
    vec4( 3.0, -1.0, 0.0, 1.0)   
};

vec2 texCoords[3] = 
{
    vec2(0.0, 0.0),  
    vec2(0.0, 2.0),  
    vec2(2.0, 0.0)   
};

layout(location = 0) out vec2 TexCoords;

void main()
{
    TexCoords = texCoords[gl_VertexIndex];
    gl_Position = position[gl_VertexIndex];
}