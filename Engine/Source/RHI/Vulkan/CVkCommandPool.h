#pragma once

#include "RHI/RHICommon.h"
#include <memory>

#include "RHI/Interface/CommandPool.h"
#include "RHI/Interface/Types.h"
#include "RHI/Configs/CommandBuffers.h"

namespace psm
{
    class CVkCommandPool : public ICommandPool, std::enable_shared_from_this<CVkCommandPool>
    {
    public:
        CVkCommandPool(const DevicePtr& device, const SCommandPoolConfig& config);
        virtual ~CVkCommandPool();

        virtual void FreeCommandBuffers(const std::vector<CommandBufferPtr>& commandBuffers) override;
        
        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mDeviceInternal;
        VkCommandPool mCommandPool;
    };
}