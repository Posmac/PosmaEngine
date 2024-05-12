#pragma once

#include <memory>

#include "Include/vulkan/vulkan.h"

#include "RHI/Interface/Types.h"

#include "RHI/Interface/Sampler.h"
#include "RHI/Configs/SamplerConfig.h"

namespace psm
{
    class CVkSampler : public ISampler, std::enable_shared_from_this<CVkSampler>
    {
    public:
        CVkSampler(const DevicePtr& device, const SSamplerConfig& config);
        virtual ~CVkSampler();

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mVkDevice;
        VkSampler mVkSampler;
    };
}