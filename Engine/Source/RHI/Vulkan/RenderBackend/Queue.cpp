#include "Queue.h"

namespace psm
{
    namespace vk
    {
        void Submit(VkQueue queue, 
            uint32_t submitCount,
            VkPipelineStageFlags waitStageFlags, 
            const VkSemaphore* pWaitSemaphores,
            uint32_t waitSemaphoresCount,
            const VkCommandBuffer* pCommandBuffers,
            uint32_t commandBuffersCount,
            const VkSemaphore* pSignalSemaphores,
            uint32_t signalSemaphoresCount,
            VkFence fence)
        {
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = nullptr;
            submitInfo.waitSemaphoreCount = waitSemaphoresCount;
            submitInfo.pWaitSemaphores = pWaitSemaphores;
            submitInfo.pWaitDstStageMask = &waitStageFlags;
            submitInfo.commandBufferCount = commandBuffersCount;
            submitInfo.pCommandBuffers = pCommandBuffers;
            submitInfo.signalSemaphoreCount = signalSemaphoresCount;
            submitInfo.pSignalSemaphores = pSignalSemaphores;

            VkResult result = vkQueueSubmit(queue, submitCount, &submitInfo, fence);
        }

        VkResult Present(VkQueue queue,
            const VkSemaphore* pWaitSemaphores,
            uint32_t waitSemaphoresCount, 
            VkSwapchainKHR* pSwapchains,
            uint32_t swapchainsCount,
            uint32_t* imageIndices)
        {
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = waitSemaphoresCount;
            presentInfo.pWaitSemaphores = pWaitSemaphores;
            presentInfo.swapchainCount = swapchainsCount;
            presentInfo.pSwapchains = pSwapchains;
            presentInfo.pImageIndices = imageIndices;

            return vkQueuePresentKHR(queue, &presentInfo);
        }
    }
}