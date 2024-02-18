#pragma once

#include "../VkCommon.h"
#include "../Interface/Buffer.h"

namespace psm
{
    class CVkBuffer final: IBuffer
    {
    public:
        CVkBuffer(DevicePtr& device, const BufferConfig& config);
        virtual ~CVkBuffer();
        virtual RESULT Map() override;
        virtual RESULT Unmap() override;
        virtual RESULT UpdateBuffer(const UntypedBuffer& data) override;
        virtual void* GetMappedDataPtr() override;
    private:
        VkResult CreateBuffer(VkDevice device,
            VkPhysicalDevice gpu,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer* buffer,
            VkDeviceMemory* bufferMemory);

        VkResult CreateBufferAndMapMemory(VkDevice device,
            VkPhysicalDevice gpu,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer* buffer,
            VkDeviceMemory* bufferMemory,
            void** mapping);

        void CopyBuffer(VkDevice device,
            VkCommandBuffer commandBuffer,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            VkDeviceSize size);

        void CopyBufferToImage(VkDevice device,
            VkCommandBuffer commandBuffer,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkExtent3D imageExtent);

        VkResult MapMemory(VkDevice device,
                       VkDeviceMemory bufferMemory,
                       VkDeviceSize offset,
                       VkDeviceSize size,
                       VkMemoryMapFlags flags,
                       void** mapping);

        void UnmapMemory(VkDevice device, VkDeviceMemory memory);
        void DestroyBuffer(VkDevice device, VkBuffer buffer);
        void FreeMemory(VkDevice device, VkDeviceMemory memory);
    private:
        VkDevice mVkDevice;
        void* mpMappedData;
        VkBuffer mVkBuffer;
        VkDeviceMemory mVkMemory;
    };
}