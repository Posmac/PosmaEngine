#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateBuffer(VkDevice device, VkPhysicalDevice gpu, VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

        void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue,
            VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props);

        void DestroyBuffer(VkDevice device, VkBuffer buffer);
        void FreeMemory(VkDevice device, VkDeviceMemory memory);
    }
}