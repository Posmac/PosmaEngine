#pragma once

#include <optional>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> GraphicsFamily;
            std::optional<uint32_t> PresentFamily;
            VkQueue GraphicsQueue;
            VkQueue PresentQueue;

            bool IsComplete()
            {
                return GraphicsFamily.has_value() && PresentFamily.has_value();
            }

            bool IdenticalQueues()
            {
                return GraphicsFamily.value() == PresentFamily.value();
            }
        };

        void Submit(VkQueue queue,
            uint32_t submitCount,
            VkPipelineStageFlags waitStageFlags,
            const VkSemaphore* pWaitSemaphores,
            uint32_t waitSemaphoresCount,
            const VkCommandBuffer* pCommandBuffers,
            uint32_t commandBuffersCount,
            const VkSemaphore* pSignalSemaphores,
            uint32_t signalSemaphoresCount,
            VkFence fence);

        void Present(VkQueue queue,
            const VkSemaphore* pWaitSemaphores,
            uint32_t waitSemaphoresCount,
            VkSwapchainKHR* pSwapchains,
            uint32_t swapchainsCount,
            uint32_t* imageIndices);
    }
}