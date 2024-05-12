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
        CVkFence(const DevicePtr& device, const SFenceConfig& config);
        ~CVkFence();
        virtual void Wait(const SFenceWaitConfig& config) override;
        virtual void Reset() override;
        virtual void Signal() override;

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkFence mFence;
        VkDevice mDeviceInternal;
    };
}