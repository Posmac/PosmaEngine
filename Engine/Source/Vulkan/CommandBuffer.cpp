#include "CommandBuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateCommandPool(VkDevice logicalDevice,
            int graphicsFamilyIndex,
            VkCommandPool* commandPool)
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo{};
            commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.pNext = nullptr;
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;

            VkResult result = vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, commandPool);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create command pool" << std::endl;
            }
        }

        void CreateCommandBuffers(VkDevice logicalDevice,
            VkCommandPool commandPool,
            uint32_t size,
            std::vector<VkCommandBuffer>* commandBuffers)
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.pNext = nullptr;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = size;//because we have 2 framebufers

            commandBuffers->resize(size);

            VkResult result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers->data());
            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to allocate command buffers" << std::endl;
            }
        }
    }

    namespace putils
    {
        VkCommandBuffer BeginSingleTimeCommandBuffer(VkDevice device, VkCommandPool pool)
        {
            VkCommandBufferAllocateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.pNext = nullptr;
            info.commandPool = pool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            info.commandBufferCount = 1;

            VkCommandBuffer buffer;
            vkAllocateCommandBuffers(device, &info, &buffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pNext = nullptr;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo = nullptr;

            vkBeginCommandBuffer(buffer, &beginInfo);

            return buffer;
        }

        void EndSingleTimeCommandBuffer(VkDevice device, 
            VkCommandPool commandPool, 
            VkCommandBuffer commandBuffer, 
            VkQueue graphicsQueue)
        {
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }
    }
}
