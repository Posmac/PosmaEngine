#pragma once

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateRenderPass(VkDevice logicalDevice, VkFormat imageFormat,
            VkRenderPass* renderPass);
        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass);
    }
}