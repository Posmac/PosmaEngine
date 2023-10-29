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
            VK_CHECK_RESULT(result);
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
            VK_CHECK_RESULT(result);
        }

        void BeginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags)
        {
            VkCommandBufferBeginInfo begin{};
            begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin.flags = flags;
            begin.pNext = nullptr;
            begin.pInheritanceInfo = nullptr;

            VkResult result = vkBeginCommandBuffer(commandBuffer, &begin);
            VK_CHECK_RESULT(result);
        }

        void EndCommandBuffer(VkCommandBuffer commandBuffer)
        {
            VkResult result = vkEndCommandBuffer(commandBuffer);
            VK_CHECK_RESULT(result);
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
            VkResult result = vkAllocateCommandBuffers(device, &info, &buffer);
            VK_CHECK_RESULT(result);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pNext = nullptr;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo = nullptr;

            result = vkBeginCommandBuffer(buffer, &beginInfo);
            VK_CHECK_RESULT(result);

            return buffer;
        }

        void EndSingleTimeCommandBuffer(VkDevice device, 
            VkCommandPool commandPool, 
            VkCommandBuffer commandBuffer, 
            VkQueue graphicsQueue)
        {
            VkResult result = vkEndCommandBuffer(commandBuffer);
            VK_CHECK_RESULT(result);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            // Create fence to ensure that the command buffer has finished executing
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = 0;
            VkFence fence;
            result = vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
            VK_CHECK_RESULT(result);

            // Submit to the queue
            result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
            VK_CHECK_RESULT(result);
            // Wait for the fence to signal that command buffer has finished executing
            result = vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000);
            VK_CHECK_RESULT(result);

            vkDestroyFence(device, fence, nullptr);
            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }
    }
}
