#pragma once

#include <Windows.h>

#include "Include/vulkan/vulkan.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

namespace psm
{
    namespace vk
    {
        class VulkanImGui
        {
        public:
            void Init(HWND hWnd, uint32_t swapchainImagesCount,
                VkDescriptorPool* descriptorPool, VkRenderPass renderPass,
                VkQueue graphicsQueue, uint32_t queueFamily,
                VkCommandPool cmdPool, VkCommandBuffer cmdBuf);
            void Deinit();
            void Render(VkCommandBuffer commandBuffer);
        };
    }
}