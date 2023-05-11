#include "CommandBuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateCommandPool(VkDevice logicalDevice, int graphicsFamilyIndex, VkCommandPool* commandPool)
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

        void CreateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool,
            uint32_t size, std::vector<VkCommandBuffer>* commandBuffers)
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
}
