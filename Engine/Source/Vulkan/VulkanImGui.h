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
        struct BaseVulkanAppData;

        class VulkanImGui
        {
        public:
            void Init(HWND hWnd, uint32_t swapchainImagesCount,
                const BaseVulkanAppData& appData, VkDescriptorPool* descriptorPool);
            void Deinit();
            void Render(VkCommandBuffer commandBuffer);
        };
    }
}