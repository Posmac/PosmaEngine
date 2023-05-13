#include "Buffer.h"

namespace psm
{
    namespace vk
    {
        void CreateBuffer(VkDevice device, VkPhysicalDevice gpu, VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
        {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(device, &bufferInfo, nullptr, buffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create buffer");
            }

            VkMemoryRequirements memReq{};
            vkGetBufferMemoryRequirements(device, *buffer, &memReq);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex = FindMemoryType(gpu, memReq.memoryTypeBits, properties);

            if (vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS) 
            {
                throw std::runtime_error("Failed to allocate memory");
            }

            vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
        }

        void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue,
            VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        {
            VkCommandBufferAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandPool = commandPool;
            allocateInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkBufferCopy copyRegion{};
            copyRegion.size = size;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};

            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }

        uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props)
        {
            VkPhysicalDeviceMemoryProperties memProps;
            vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

            for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) 
            {
                if (typeFilter & (1 << i) &&
                    (memProps.memoryTypes[i].propertyFlags & props) == props) 
                {
                    return i;
                }
            }
        }

        void UnmapMemory(VkDevice device, VkDeviceMemory memory)
        {
            vkUnmapMemory(device, memory);
        }

        void DestroyBuffer(VkDevice device, VkBuffer buffer)
        {
            vkDestroyBuffer(device, buffer, nullptr);
        }

        void FreeMemory(VkDevice device, VkDeviceMemory memory)
        {
            vkFreeMemory(device, memory, nullptr);
        }
    }
}