#pragma once

#include "../Interface/Types.h"
#include "../Interface/Fence.h"

#include "../Configs/SyncronizationConfigs.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkFence : public IFence, public std::enable_shared_from_this<CVkFence>
    {
    public:
        CVkFence(DevicePtr device, const FenceConfig& config);
        ~CVkFence();
        virtual void Wait() override;
        virtual void Reset() override;
        virtual void Signal() override;
    private:
        VkFence mFence;
        VkDevice mDeviceInternal;
    };
}