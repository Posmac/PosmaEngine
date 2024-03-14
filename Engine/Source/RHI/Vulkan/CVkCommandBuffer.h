#pragma once

#include <vector>
#include <memory>

#include "RHI/Interface/CommandBuffer.h"
#include "RHI/Configs/CommandBuffers.h"
#include "RHI/Interface/Types.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    //TODO: Rewrite this class to hold only 1 VkCommandBuffer!!
    class CVkCommandBuffer : public ICommandBuffer, std::enable_shared_from_this<CVkCommandBuffer>
    {
    public:
        CVkCommandBuffer(DevicePtr device, CommandPoolPtr commandPool, const SCommandBufferConfig& config);
        virtual ~CVkCommandBuffer();

        virtual void ResetAtIndex(uint32_t index) override;
        virtual void BeginAtIndex(const SCommandBufferBeginConfig& config) override;
        virtual void EndCommandBuffer(uint32_t index) override;
        virtual void* GetRawPointer() override;

    private:
        VkDevice mDeviceInternal;
        uint32_t mCurrentCommandBuffer;
        std::vector<VkCommandBuffer> mCommandBuffers;
    };
}