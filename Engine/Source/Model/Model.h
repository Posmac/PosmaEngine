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
        virtual ~Model();
        void Init(const DevicePtr& device, const CommandPoolPtr& commandPool);
        void BindBuffers(const DevicePtr& device, const CommandBufferPtr& commandBuffer);
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