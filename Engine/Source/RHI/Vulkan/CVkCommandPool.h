#pragma once

#include "../VkCommon.h"
#include <memory>

#include "RHI/Interface/CommandPool.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    class CVkCommandPool : public ICommandPool, std::enable_shared_from_this<CVkCommandPool>
    {
    public:
        CVkCommandPool(DevicePtr device, const SCommandPoolConfig& config);
        virtual ~CVkCommandPool();

        virtual void* GetCommandPool() override;

    private:
        VkDevice mDeviceInternal;
        VkCommandPool mCommandPool;
    };
}