#pragma once

#include <string>
#include <vector>
#include <iostream>

//#include "Core.h"

namespace psm
{
    namespace vk
    {
        void CreateCommandPool(VkDevice logicalDevice, 
            int graphicsFamilyIndex, 
            VkCommandPool* commandPool);

        void CreateCommandBuffers(VkDevice logicalDevice, 
            VkCommandPool commandPool,
            uint32_t size, 
            std::vector<VkCommandBuffer>* commandBuffers);

        void BeginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags);
        void EndCommandBuffer(VkCommandBuffer commandBuffer);
    }

    namespace putils
    {
        VkCommandBuffer BeginSingleTimeCommandBuffer(VkDevice device, VkCommandPool pool);

        void EndSingleTimeCommandBuffer(VkDevice device,
            VkCommandPool commandPool,
            VkCommandBuffer commandBuffer,
            VkQueue graphicsQueue);
    }
}