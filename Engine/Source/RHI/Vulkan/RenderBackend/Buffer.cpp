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

            VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
            VK_CHECK_RESULT(result);

            VkMemoryRequirements memReq{};
            vkGetBufferMemoryRequirements(device, *buffer, &memReq);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex = FindMemoryType(gpu, memReq.memoryTypeBits, properties);

            result = vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
            VK_CHECK_RESULT(result);

            result = vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
            VK_CHECK_RESULT(result);
        }

        void CreateBufferAndMapMemory(VkDevice device, VkPhysicalDevice gpu,
            VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory,
            void** mapping)
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

            vkMapMemory(device, *bufferMemory, 0,
                size, 0, mapping);
        }

        void CopyBuffer(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue,
            VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        {
            VkBufferCopy copyRegion{};
            copyRegion.size = size;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;

           /* vk::InsertBufferMemoryBarrier(commandBuffer, srcBuffer, size,
                VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);*/

            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            //vk::InsertBufferMemoryBarrier(commandBuffer, dstBuffer, size,
            //    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            //    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
        }

        void CopyBufferToImage(VkDevice device,
            VkCommandBuffer commandBuffer,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkExtent3D imageExtent)
        {
            VkBufferImageCopy copy{};
            copy.bufferOffset = 0;
            copy.bufferImageHeight = 0;
            copy.bufferRowLength = 0;

            copy.imageOffset = { 0, 0, 0 };
            copy.imageExtent = imageExtent;
            copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy.imageSubresource.baseArrayLayer = 0;
            copy.imageSubresource.layerCount = 1;
            copy.imageSubresource.mipLevel = 0;

            vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
        }

        void MapMemory(VkDevice device, 
                       VkDeviceMemory bufferMemory, 
                       VkDeviceSize offset, 
                       VkDeviceSize size, 
                       VkMemoryMapFlags flags, 
                       void** mapping)
        {
            vkMapMemory(device, bufferMemory, offset, size, size, mapping);
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