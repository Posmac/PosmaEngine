#include "Memory.h"

namespace psm
{
    namespace vk
    {
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

        void InsertBufferMemoryBarrier(VkCommandBuffer commandBuffer,
            VkBuffer buffer, 
            VkDeviceSize size,
            VkAccessFlags srcAccessMask,
            VkAccessFlags dstAccessMask,
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask)
        {
            VkBufferMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.srcAccessMask = srcAccessMask;
            barrier.dstAccessMask = dstAccessMask;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer = buffer;
            barrier.offset = 0;
            barrier.size = size;

            vkCmdPipelineBarrier(commandBuffer, //command buffer
                srcStageMask,     //src pipeline stage flags
                dstStageMask, //dst pipeline stage flags
                0,                              //dependency flags
                0,                              //memory barrier count
                nullptr,                        //pMemory barriers
                1,                              //buffer memory barriers count
                &barrier,                       //pBuffer memory barriers
                0,                              //image memory barriers
                nullptr);                       //pImage memory barriers
        }
    }
}