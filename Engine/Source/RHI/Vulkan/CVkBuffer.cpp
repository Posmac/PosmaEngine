#include "CVkBuffer.h"

#include "../RHICommon.h"
#include "TypeConvertor.h"
#include "CVkDevice.h"
#include "../Memory/UntypedBuffer.h"

namespace psm
{
    CVkBuffer::CVkBuffer(DevicePtr device, const SBufferConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkPhysicalDevice physicalDevice = reinterpret_cast<VkPhysicalDevice>(device->GetDeviceData().vkData.PhysicalDevice);

        mSize = config.Size;

        VkResult result = CreateBuffer(mDeviceInternal,
                         physicalDevice,
                         config.Size,
                         ToVulkan(config.Usage),
                         ToVulkan(config.MemoryProperties),
                         &mVkBuffer,
                         &mVkMemory);

        VK_CHECK_RESULT(result);
    }

    uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props)
    {
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

        for(uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if(typeFilter & (1 << i) &&
                (memProps.memoryTypes[i].propertyFlags & props) == props)
            {
                return i;
            }
        }
    }

    VkResult CVkBuffer::CreateBuffer(VkDevice device, VkPhysicalDevice gpu, VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
        if(result != VK_SUCCESS)
        {
            return result;
        }

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(device, *buffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(gpu, memReq.memoryTypeBits, properties);

        result = vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
        if(result != VK_SUCCESS)
        {
            DestroyBuffer(device, *buffer);
            return result;
        }

        result = vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
        if(result != VK_SUCCESS)
        {
            FreeMemory(device, *bufferMemory);
            DestroyBuffer(device, *buffer);
        }

        return result;
    }

    VkResult CVkBuffer::CreateBufferAndMapMemory(VkDevice device, VkPhysicalDevice gpu, 
        VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory,
        void** mapping)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
        if(result != VK_SUCCESS)
        {
            return result;
        }

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(device, *buffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(gpu, memReq.memoryTypeBits, properties);

        vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
        if(result != VK_SUCCESS)
        {
            DestroyBuffer(device, *buffer);
            return result;
        }

        vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
        if(result != VK_SUCCESS)
        {
            FreeMemory(device, *bufferMemory);
            DestroyBuffer(device, *buffer);
            return result;
        }

        vkMapMemory(device, *bufferMemory, 0, size, 0, mapping);
        if(result != VK_SUCCESS)
        {
            FreeMemory(device, *bufferMemory);
            DestroyBuffer(device, *buffer);
        }

        return result;
    }

    //TODO: correction needed
    void CVkBuffer::CopyBuffer(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue,
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

    //TODO: correction needed
    void CVkBuffer::CopyBufferToImage(VkDevice device,
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

    VkResult CVkBuffer::MapMemory(VkDevice device,
                   VkDeviceMemory bufferMemory,
                   VkDeviceSize offset,
                   VkDeviceSize size,
                   VkMemoryMapFlags flags,
                   void** mapping)
    {
        return vkMapMemory(device, bufferMemory, offset, size, flags, mapping);
    }

    void CVkBuffer::UnmapMemory(VkDevice device, VkDeviceMemory memory)
    {
        vkUnmapMemory(device, memory);
    }

    void CVkBuffer::FreeMemory(VkDevice device, VkDeviceMemory memory)
    {
        vkFreeMemory(device, memory, nullptr);
    }

    void CVkBuffer::DestroyBuffer(VkDevice device, VkBuffer buffer)
    {
        vkDestroyBuffer(device, buffer, nullptr);
    }

    CVkBuffer::~CVkBuffer()
    {
        DestroyBuffer(mDeviceInternal, mVkBuffer);
        FreeMemory(mDeviceInternal, mVkMemory);
    }

    void CVkBuffer::Map(SBufferMapConfig& config)
    {
        vkMapMemory(mDeviceInternal, mVkMemory, config.Offset, config.Size, 0, config.pData);
    }

    void CVkBuffer::Unmap()
    {
        vkUnmapMemory(mDeviceInternal, mVkMemory);
    }

    void CVkBuffer::Flush(SBufferFlushConfig & config)
    {
        //can be made once at the finish
        VkMappedMemoryRange range{};
        range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.pNext = nullptr;
        range.memory = mVkMemory;
        range.offset = config.Offset;
        range.size = config.Size;

        vkFlushMappedMemoryRanges(mDeviceInternal, 1, &range);
    }

    void CVkBuffer::UpdateBuffer(const SUntypedBuffer& data)
    {
        void* pData;

        SBufferMapConfig mapping =
        {
            .Size = data.size(),
            .Offset = 0,
            .pData = &pData,
            .Flags = 0
        };

        Map(mapping);

        memcpy(pData, data.data(), data.size());

        Unmap();
    }

    uint64_t CVkBuffer::Size()
    {
        return mSize;
    }

    void* CVkBuffer::Raw()
    {
        return mVkBuffer;
    }

    void* CVkBuffer::Raw() const
    {
        return mVkBuffer;
    }
}