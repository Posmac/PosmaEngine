#pragma once

#include <vector>
#include <memory>

#include "RHI/Interface/CommandBuffer.h"
#include "RHI/Configs/CommandBuffers.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkCommandBuffer : public ICommandBuffer, std::enable_shared_from_this<CVkCommandBuffer>
    {
    public:
        CVkCommandBuffer(DevicePtr device, CommandPoolPtr commandPool, const CommandBufferConfig& config);
        virtual ~CVkCommandBuffer();
    private:
        VkDevice mDeviceInternal;
        std::vector<VkCommandBuffer> mCommandBuffers;
    };
}