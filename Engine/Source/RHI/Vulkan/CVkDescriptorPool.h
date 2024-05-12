#pragma once

#include <memory>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/DescriptorPool.h"

#include "RHI/Configs/ShadersConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkDescriptorPool : public IDescriptorPool, std::enable_shared_from_this<CVkDescriptorPool>
    {
    public:
        CVkDescriptorPool(DevicePtr device, const SDescriptorPoolConfig& config);
        virtual ~CVkDescriptorPool();

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mDeviceInternal;
        VkDescriptorPool mDescriptorPool;
    };
}