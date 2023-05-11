#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateCommandPool(VkDevice logicalDevice, int graphicsFamilyIndex, VkCommandPool* commandPool);
        void CreateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool,
            uint32_t size, std::vector<VkCommandBuffer>* commandBuffers);
    }
}