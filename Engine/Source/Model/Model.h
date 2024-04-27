#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <string>

#include "Mesh.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    class Model
    {
    public:
        Model();
        Model(std::vector<Mesh>& meshes);
        void BindBuffers(DevicePtr device, CommandBufferPtr commandBuffer);
        void Init(DevicePtr device, CommandPoolPtr commandPool);
        void Deinit();
    public:
        std::string Name;
        std::vector<Mesh> Meshes;
        std::vector<Vertex> MeshVertices; //all model meshes verticces
        std::vector<uint32_t> MeshIndices; //all model meshes indices
    private:
        BufferPtr mVertexBuffer;
        BufferPtr mIndexBuffer;
    };
}