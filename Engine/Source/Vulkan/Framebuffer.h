#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateFramebuffers(VkDevice device, const std::vector<VkImageView>& swapchainImageViews,
            const VkExtent2D extent, uint32_t size,
            VkRenderPass renderPas, std::vector<VkFramebuffer>* framebuffers);
        void DestroyFramebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers);
    }
}