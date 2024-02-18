#pragma once

#include "../Interface/Sampler.h"
#include "../Configs/SamplerConfig.h"

namespace psm
{
    class CVkSampler final: ISampler
    {
    public:
        CVkSampler(DevicePtr& device, const SamplerConfig& config);
        virtual ~CVkSampler();
    private:
        VkDevice mVkDevice;
        VkSampler mVkSampler;
    };
}