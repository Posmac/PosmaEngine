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

//internal things
struct DerivativesOutput
{
    vec3 dbDx;
    vec3 dbDy;
};

// Get interpolated atrribute

// Engel's barycentric coord partial derivs function. Follows equation from [Schied][Dachsbacher]
// Computes the partial derivatives of point's barycentric coordinates from the projected screen space vertices
DerivativesOutput ComputePartialDerivatives(vec2 v0, vec2 v1, vec2 v2)
{
	DerivativesOutput derivatives;
	float d = 1.0 / determinant(mat2(v2 - v1, v0 - v1));
	derivatives.dbDx = vec3(v1.y - v2.y, v2.y - v0.y, v0.y - v1.y) * d;
	derivatives.dbDy = vec3(v2.x - v1.x, v0.x - v2.x, v1.x - v0.x) * d;
	return derivatives;
}

// Interpolate 2D attributes using the partial derivatives and generates dx and dy for texture sampling.
vec2 Interpolate2DAttributes(mat3x2 attributes, vec3 dbDx, vec3 dbDy, vec2 d)
{
	vec3 attr0 = vec3(attributes[0].x, attributes[1].x, attributes[2].x);
	vec3 attr1 = vec3(attributes[0].y, attributes[1].y, attributes[2].y);
	vec2 attribute_x = vec2(dot(dbDx,attr0), dot(dbDx,attr1));
	vec2 attribute_y = vec2(dot(dbDy,attr0), dot(dbDy,attr1));
	vec2 attribute_s = attributes[0];
	
	vec2 result = (attribute_s + d.x * attribute_x + d.y * attribute_y);
	return result;
}

// Interpolate vertex attributes at point 'd' using the partial derivatives
vec3 Interpolate3DAttributes(mat3 attributes, vec3 dbDx, vec3 dbDy, vec2 d)
{
	vec3 attribute_x = attributes * dbDx;
	vec3 attribute_y = attributes * dbDy;
	vec3 attribute_s = attributes[0];
	
	return (attribute_s + d.x * attribute_x + d.y * attribute_y);
}

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
vec3 Interpolate3dAtribute(vec3[3] attributes, vec3 barycentricCoords, vec3 perspectiveCoefficients)
{
    vec3 result;

    //divide attributes by its w
    vec3 attr0 = attributes[0] / perspectiveCoefficients.x;
    vec3 attr1 = attributes[1] / perspectiveCoefficients.y;
    vec3 attr2 = attributes[2] / perspectiveCoefficients.z;

    //interpoalte a/w
    result = attr0 * barycentricCoords.x + attr1 * barycentricCoords.y + attr2 * barycentricCoords.z;
    float wInterp = perspectiveCoefficients.x * barycentricCoords.x + perspectiveCoefficients.y * barycentricCoords.y + perspectiveCoefficients.z * barycentricCoords.z;
    
    //return result multiplied by interpolated w
    return result * wInterp;
}

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() 
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 textureCoords = pixelCoords / globalBuffer.ViewPortSize;

    vec4 visibilityData = texture(visibilityBuffer, textureCoords);

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
        vec3 barycentricCoords = GetBarycentricCoordinates(vClip, textureCoords * 2.0 - 1.0);

        vec3[3] normals = {
            vertices[0].Normal.xyz,
            vertices[1].Normal.xyz,
            vertices[2].Normal.xyz,
        };

        vec3 perspectiveCoefficients = vec3(vClip[0].w, vClip[1].w, vClip[2].w);
        vec3 normal = Interpolate3dAtribute(normals, barycentricCoords, perspectiveCoefficients);

        imageStore(gBufferAlbedo, pixelCoords, vec4(normal, 1.0));
    }
    else
    {
        imageStore(gBufferAlbedo, pixelCoords, vec4(0.35f, 0.55f, 0.7f, 1.0f));
    }
}