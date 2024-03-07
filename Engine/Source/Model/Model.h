#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <string>

#include "Mesh.h"
#include "RHI/Interface/Types.h"

//#include "Render/Vk.h"
//#include "RenderBackend/Vulkan/Core.h"
//#include "Include/vulkan/vulkan.h"
//#include "RenderBackend/Vulkan/Buffer.h"

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
    private:
        BufferPtr mVertexBuffer;
        //VkBuffer m_VertexBuffer; //contains all geometry data of all meshes
        //VkDeviceMemory m_VertexBufferMemory;

        BufferPtr mIndexBuffer;
        //VkBuffer m_IndexBuffer; //contains all indices of all meshes
        //VkDeviceMemory m_IndexBufferMemory;
    };
}