#pragma once

#include <Windows.h>

#include "Core.h"
#include "Render/Vk.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

#include "glm/glm.hpp"

namespace psm
{
    namespace vkimgui
    {
        void Init(HWND hWnd,
                  uint32_t swapchainImagesCount,
                  VkRenderPass renderPass,
                  VkQueue graphicsQueue,
                  uint32_t queueFamily,
                  VkCommandPool cmdPool,
                  VkCommandBuffer cmdBuf,
                  VkSampleCountFlagBits samplesCount,
                  VkDescriptorPool* descriptorPool);
        void Deinit();
        void Render(VkCommandBuffer commandBuffer);
        void PrepareNewFrame();
    }
}