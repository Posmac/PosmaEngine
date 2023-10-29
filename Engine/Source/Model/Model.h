#pragma once

#include <vector>
#include <utility>
#include <algorithm>

#include "Mesh.h"
#include "Render/Vk.h"
#include "RenderBackend/Vulkan/Core.h"
#include "Include/vulkan/vulkan.h"
#include "RenderBackend/Vulkan/Buffer.h"

namespace psm
{
    class Model
    {
    public:
        Model();
        Model(std::vector<Mesh>& meshes);
        void BindBuffers(VkCommandBuffer commandBuffer) const;
        void Init(VkCommandPool commandPool);
        void Deinit();
    public:
        std::string Name;
        std::vector<Mesh> Meshes;
    private:

        VkBuffer m_VertexBuffer; //contains all geometry data of all meshes
        VkDeviceMemory m_VertexBufferMemory;

        VkBuffer m_IndexBuffer; //contains all indices of all meshes
        VkDeviceMemory m_IndexBufferMemory;
    };
}