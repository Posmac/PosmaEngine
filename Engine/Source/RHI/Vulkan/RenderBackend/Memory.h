#pragma once

#include "Include/vulkan/vulkan.h"

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