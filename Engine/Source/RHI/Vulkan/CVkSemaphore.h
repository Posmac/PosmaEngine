#pragma once

#include "../Interface/Types.h"
#include "../Interface/Semaphore.h"

#include "../Configs/SyncronizationConfigs.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkSemaphore : public ISemaphore, public std::enable_shared_from_this<CVkSemaphore>
    {
    public:
        CVkSemaphore(DevicePtr device, const SemaphoreConfig& config);
        virtual ~CVkSemaphore();
        virtual void Wait() override;
        virtual void Signal() override; //support by 1.3 only
    private:
        VkSemaphore mSemaphore;
        VkDevice mDeviceInternal;
    };
}