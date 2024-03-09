#pragma once

#include <memory>

#include "RHI/Interface/Sampler.h"
#include "RHI/Configs/SamplerConfig.h"

namespace psm
{
    class CVkSampler : public ISampler, std::enable_shared_from_this<CVkSampler>
    {
    public:
        CVkSampler(DevicePtr device, const SSamplerConfig& config);
        virtual ~CVkSampler();
    private:
        VkDevice mVkDevice;
        VkSampler mVkSampler;
    };
}