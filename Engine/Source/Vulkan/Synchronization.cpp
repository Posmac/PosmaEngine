#include "Synchronization.h"

namespace psm
{
    namespace vk
    {
        void CreateVkSemaphore(VkDevice device, VkSemaphoreCreateFlags flags, VkSemaphore* semaphore)
        {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext = nullptr;
            semaphoreInfo.flags = flags;

            VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, semaphore);
            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create semaphore" << std::endl;
            }
        }

        void CreateVkFence(VkDevice device, VkFenceCreateFlags flags, VkFence* fence)
        {
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.pNext = nullptr;
            fenceInfo.flags = flags;

            VkResult result = vkCreateFence(device, &fenceInfo, nullptr, fence);
            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create semaphore" << std::endl;
            }
        }

        void DestroySemaphore(VkDevice device, VkSemaphore semaphore)
        {
            vkDestroySemaphore(device, semaphore, nullptr);
        }

        void DestroyFence(VkDevice device, VkFence fence)
        {
            vkDestroyFence(device, fence, nullptr);
        }
    }
}