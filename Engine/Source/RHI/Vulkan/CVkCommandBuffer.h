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
        CVkCommandBuffer(DevicePtr device, VkCommandBuffer commandBuffer);
        virtual ~CVkCommandBuffer();

        virtual void Reset() override;
        virtual void Begin(const SCommandBufferBeginConfig& config) override;
        virtual void End() override;
        virtual void* GetRawPointer() override;

    private:
        VkDevice mDeviceInternal;
        VkCommandBuffer mCommandBuffer;
    };
}