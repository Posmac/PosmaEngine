#version 450

layout(set = 0, binding = 0) uniform GlobalBuffer
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ViewProjectionMatrix;
    float DeltaTime;
} globalBuffer;

struct Particle {
    vec2 Position;
    vec2 Velocity;
    vec4 Color;
};

layout(std140, set = 0, binding = 1) readonly buffer ParticleSSBOIn {
   Particle particlesIn[ ];
};

layout(std140, set = 0, binding = 2) buffer ParticleSSBOOut {
   Particle particlesOut[ ];
};

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() 
{
    uint index = gl_GlobalInvocationID.x;  

    Particle particleIn = particlesIn[index];

    particlesOut[index].Position = particleIn.Position + particleIn.Velocity.xy * globalBuffer.DeltaTime;
    particlesOut[index].Velocity = particleIn.Velocity;
}