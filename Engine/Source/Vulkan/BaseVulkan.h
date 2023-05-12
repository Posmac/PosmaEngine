#pragma once

#include <array>

#include "BaseVulkanAppData.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Win32Surface.h"
#include "Buffer.h"

#include "Mesh/Mesh.h"

#include "VulkanImGui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace psm
{
    namespace vk
    {
        class BaseVulkan
        {
        public:
            BaseVulkan(BaseVulkan&) = delete;
            void operator=(const BaseVulkan&) = delete;
            static BaseVulkan* Instance();
        private:
            BaseVulkan() = default;
            static BaseVulkan* s_Instance;
        public:
            void Init(HINSTANCE hInstance, HWND hWnd);
            void Deinit();
            void Render();
        private:
            void LoadModelData();
        private:
            //general
            BaseVulkanAppData m_VulkanData;

            //for mesh
            VkBuffer m_VertexBuffer;
            VkDeviceMemory m_VertexBufferMemory;
            std::vector<Vertex> m_Vertices;

            //uniform buffers for shaders
            struct VertexShaderUBO
            {
                glm::vec4 Offset;
            } m_VertexUBO;

            struct FragmentShaderUBO
            {
                glm::vec4 color;
            } m_FragmentUBO;

            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;

            VkBuffer fragmentBuffer;
            VkDeviceMemory fragmentBufferMemory;

            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;

            std::vector<VkBuffer> uniformBuffers;
            std::vector<VkDeviceMemory> uniformBufferMemory;
            std::vector<void*> uniformBufferMapping;

            VkDescriptorPool uniformPool;
            std::vector<VkDescriptorSet> uniformDescriptorSets;

            //Imgui
            VulkanImGui m_VkImgui;
            VkDescriptorPool m_ImGuiDescriptorsPool;//used for imgui
        };
    }
}