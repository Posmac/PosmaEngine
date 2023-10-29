#pragma once

#include <cstdint>

#include "Core.h"

namespace psm
{
    namespace vk
    {
        uint32_t FindMemoryType(VkPhysicalDevice gpu, 
            uint32_t typeFilter, 
            VkMemoryPropertyFlags props);

        void InsertBufferMemoryBarrier(VkCommandBuffer commandBuffer,
            VkBuffer buffer,
            VkDeviceSize size,
            VkAccessFlags srcAccessMask,
            VkAccessFlags dstAccessMask,
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask);
    }
}