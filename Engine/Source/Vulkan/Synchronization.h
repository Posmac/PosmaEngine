#pragma once

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateVkSemaphore(VkDevice device, VkSemaphoreCreateFlags flags, VkSemaphore* semaphore);
        void CreateVkFence(VkDevice device, VkFenceCreateFlags flags, VkFence* fence);

        void DestroySemaphore(VkDevice device, VkSemaphore semaphore);
        void DestroyFence(VkDevice device, VkFence fence);
    }
}