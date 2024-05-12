#pragma once

#include <memory>
#include <cstdint>

#include "Include/vulkan/vulkan.h"
#include "RHI/Interface/Buffer.h"
#include "RHI/Memory/UntypedBuffer.h"

namespace psm
{
    class CVkBuffer : public IBuffer, std::enable_shared_from_this<CVkBuffer>
    {
    public:
        CVkBuffer(const DevicePtr& device, const SBufferConfig& config);
        virtual ~CVkBuffer();
        virtual void Map(const SBufferMapConfig& config) override;
        virtual void Unmap() override;
        virtual void Flush(const SBufferFlushConfig& config) override;
        virtual void UpdateBuffer(const SUntypedBuffer& data) override;
        virtual uint64_t Size() override;

        virtual void* Raw() override;
        virtual void* Raw() const override;

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
        uint64_t mSize;
        VkDevice mDeviceInternal;
        VkBuffer mVkBuffer;
        VkDeviceMemory mVkMemory;
    };
}