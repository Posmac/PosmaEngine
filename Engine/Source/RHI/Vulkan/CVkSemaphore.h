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
        CVkSemaphore(DevicePtr device, const SSemaphoreConfig& config);
        virtual ~CVkSemaphore();
        virtual void Wait() override;
        virtual void Signal() override; //support by 1.3 only
        virtual void* Raw() override;
        virtual void* Raw() const override;
    private:
        VkSemaphore mSemaphore;
        VkDevice mDeviceInternal;
    };
}