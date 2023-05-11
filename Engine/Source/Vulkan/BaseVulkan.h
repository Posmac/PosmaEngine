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

            //Imgui
            VulkanImGui m_VkImgui;
            VkDescriptorPool m_ImGuiDescriptorsPool;//uses for imgui
        };
    }
}