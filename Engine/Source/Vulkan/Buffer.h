#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Core.h"
#include "Include/vulkan/vulkan.h"
#include "Memory.h"
#include "CommandBuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateBuffer(VkDevice device, 
            VkPhysicalDevice gpu, 
            VkDeviceSize size, 
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, 
            VkBuffer* buffer, 
            VkDeviceMemory* bufferMemory);

        void CreateBufferAndMapMemory(VkDevice device, 
            VkPhysicalDevice gpu,
            VkDeviceSize size, 
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, 
            VkBuffer* buffer, 
            VkDeviceMemory* bufferMemory,
            void** mapping);

        void CopyBuffer(VkDevice device, 
            VkCommandBuffer commandBuffer, 
            VkQueue graphicsQueue,
            VkBuffer srcBuffer, 
            VkBuffer dstBuffer, 
            VkDeviceSize size);

        void CopyBufferToImage(VkDevice device,
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkExtent3D imageExtent);

        void UnmapMemory(VkDevice device, VkDeviceMemory memory);
        void DestroyBuffer(VkDevice device, VkBuffer buffer);
        void FreeMemory(VkDevice device, VkDeviceMemory memory);

    }
}