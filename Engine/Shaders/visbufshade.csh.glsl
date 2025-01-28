    #version 450

//#include "visibility_utils.glsl"
//unpacks vertex id and instance id
void unpackOutputVBID(uint packedID, out uint primitiveId, out uint instanceId)
{
    instanceId = (packedID >> 23) & 0xFF;
    primitiveId = packedID & 0x007FFFFF;
}

struct Model
{
    uint ModelId;
};

struct Vertex 
{
    vec4 Position;
    vec4 Normal;
    vec2 TexCoord;
    vec2 _padding;
};

struct Instance
{
    mat4x4 Matrix;
};

//input
layout(set = 0, binding = 0) uniform sampler2D visibilityBuffer;

layout(std140, set = 0, binding = 1) uniform GlobalBuffer
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ViewProjectionMatrix;
    vec2 ViewPortSize;
    float DeltaTime;
} globalBuffer;

layout(std430, set = 0, binding = 2) readonly buffer VertexBuffer 
{
    Vertex inVertices[];
};

layout(std430, set = 0, binding = 3) readonly buffer IndexBuffer 
{
    uint inIndices[];
};

layout(std430, set = 0, binding = 4) readonly buffer InstanceBuffer 
{
    Instance inInstances[];
};

//output
layout(set = 0, binding = 5, rgba16f) writeonly uniform image2D gBufferAlbedo;
layout(set = 0, binding = 6, rgba16f) writeonly uniform image2D gBufferNormal;
layout(set = 0, binding = 7, rgba16f) writeonly uniform image2D gBufferPosition;

Vertex[3] LoadTriangleVertices(uint primID)
{
    Vertex[3] vertices;
	// Load vertex data of the 3 control points
	vertices[0] = inVertices[inIndices[primID * 3 + 0]];
	vertices[1] = inVertices[inIndices[primID * 3 + 1]];
	vertices[2] = inVertices[inIndices[primID * 3 + 2]];
	return vertices;
}

//Get clip space coordinates for vertex positions
vec4[3] GetClipSpaceCoordinates(vec4[3] vertices, mat4x4 viewProjMatrix, mat4x4 worldMatrix)
{
    vec4[3] result;
    mat4x4 cumulativeMatrix = viewProjMatrix * worldMatrix;
    result[0] = cumulativeMatrix * vertices[0];
    result[1] = cumulativeMatrix * vertices[1];
    result[2] = cumulativeMatrix * vertices[2];

    result[0].xyz /= result[0].w;
    result[1].xyz /= result[1].w;
    result[2].xyz /= result[2].w;

    return result;
}

//Get barycentric coordinates
vec3 GetBarycentricCoordinates(vec4[3] clipVerts, vec2 position)
{
    vec3 result;
     float det = (clipVerts[0].x * (clipVerts[1].y - clipVerts[2].y)) + 
                (clipVerts[1].x * (clipVerts[2].y - clipVerts[0].y)) +
                (clipVerts[2].x * (clipVerts[0].y - clipVerts[1].y));

    result.x = (clipVerts[1].y - clipVerts[2].y) * (position.x - clipVerts[2].x) + (clipVerts[2].x - clipVerts[1].x) * (position.y - clipVerts[2].y);
    result.x /= det;

    result.y = (clipVerts[2].y - clipVerts[0].y) * (position.x - clipVerts[2].x) + (clipVerts[0].x - clipVerts[2].x) * (position.y - clipVerts[2].y);
    result.y /= det;

    result.z = 1 - result.x - result.y;

    return result;
}

//Interpolate atributes
vec3 Interpolate3dAttribute(vec3[3] attributes, vec3 barycentricCoords, vec3 perspectiveCoefficients, float wInterp)
{
    vec3 result;

    //divide attributes by its w
    vec3 attr0 = attributes[0] / perspectiveCoefficients.x;
    vec3 attr1 = attributes[1] / perspectiveCoefficients.y;
    vec3 attr2 = attributes[2] / perspectiveCoefficients.z;

    //interpoalte a/w
    result = attr0 * barycentricCoords.x + attr1 * barycentricCoords.y + attr2 * barycentricCoords.z;
    
    //return result multiplied by interpolated w
    return result * wInterp;
}

vec2 Interpolate2dAttribute(vec2[3] attributes, vec3 barycentricCoords, vec3 perspectiveCoefficients, float wInterp)
{
    vec2 result;

    vec2 attr0 = attributes[0] / perspectiveCoefficients.x;
    vec2 attr1 = attributes[1] / perspectiveCoefficients.y;
    vec2 attr2 = attributes[2] / perspectiveCoefficients.z;

    result = attr0 * barycentricCoords.x + attr1 * barycentricCoords.y + attr2 * barycentricCoords.z;

    return result * wInterp;
}

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() 
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 screenCoordinates = pixelCoords / globalBuffer.ViewPortSize;

    vec4 visibilityData = texture(visibilityBuffer, screenCoordinates);

    if (visibilityData != vec4(0.0))
	{
        uint primitiveID = 0;
        uint instanceId = 0;
        unpackOutputVBID(packUnorm4x8(visibilityData), primitiveID, instanceId);

        Vertex[3] vertices = LoadTriangleVertices(primitiveID);

        vec4[3] vertexPositions = {
            vertices[0].Position,
            vertices[1].Position,
            vertices[2].Position
        };

        vec4[3] vClip = GetClipSpaceCoordinates(vertexPositions, globalBuffer.ViewProjectionMatrix, inInstances[instanceId].Matrix);
        vec3 perspectiveCoefficients = vec3(vClip[0].w, vClip[1].w, vClip[2].w);
        vec3 barycentricCoords = GetBarycentricCoordinates(vClip, screenCoordinates * 2.0 - 1.0);
        float wInterp = perspectiveCoefficients.x * barycentricCoords.x + perspectiveCoefficients.y * barycentricCoords.y + perspectiveCoefficients.z * barycentricCoords.z;

        vec3[3] positions = {
            vertices[0].Position.xyz,
            vertices[1].Position.xyz,
            vertices[2].Position.xyz
        };

        vec3[3] normals = {
            vertices[0].Normal.xyz,
            vertices[1].Normal.xyz,
            vertices[2].Normal.xyz,
        };

        vec2[3] textureCoords = {
            vertices[0].TexCoord,
            vertices[1].TexCoord,
            vertices[2].TexCoord,
        };

        vec3 position = Interpolate3dAttribute(positions, barycentricCoords, perspectiveCoefficients, wInterp);
        vec3 normal = Interpolate3dAttribute(normals, barycentricCoords, perspectiveCoefficients, wInterp);
        vec2 texturesCoordinates = Interpolate2dAttribute(textureCoords, barycentricCoords, perspectiveCoefficients, wInterp);

        vec4 worldPosition = inInstances[instanceId].Matrix * vec4(position, 1.0);

        imageStore(gBufferAlbedo, pixelCoords, vec4(texturesCoordinates, 0.0, 1.0));
        imageStore(gBufferNormal, pixelCoords, vec4(normal, 1.0));
        imageStore(gBufferPosition, pixelCoords, worldPosition);
    }
    else
    {
        imageStore(gBufferAlbedo, pixelCoords, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        imageStore(gBufferNormal, pixelCoords, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        imageStore(gBufferPosition, pixelCoords, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
}