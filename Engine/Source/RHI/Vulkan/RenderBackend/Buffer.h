#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Common.h"
#include "Memory.h"
#include "CommandBuffer.h"

namespace psm
{
    namespace vk
    {
        VkResult CreateBuffer(VkDevice device, 
            VkPhysicalDevice gpu, 
            VkDeviceSize size, 
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, 
            VkBuffer* buffer, 
            VkDeviceMemory* bufferMemory);

        VkResult CreateBufferAndMapMemory(VkDevice device, 
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
            VkCommandBuffer commandBuffer,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkExtent3D imageExtent);

        VkResult MapMemory(VkDevice device,
                       VkDeviceMemory bufferMemory,
                       VkDeviceSize offset,
                       VkDeviceSize size,
                       VkMemoryMapFlags flags,
                       void** mapping);

        void UnmapMemory(VkDevice device, VkDeviceMemory memory);
        void DestroyBuffer(VkDevice device, VkBuffer buffer);
        void FreeMemory(VkDevice device, VkDeviceMemory memory);

    }
}