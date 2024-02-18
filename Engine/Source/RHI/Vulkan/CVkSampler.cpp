#include "CVkSampler.h"

#include "../RHICommon.h"

#include "RenderBackend/Sampler.h"
#include "TypeConvertor.h"

#include "CVkDevice.h"

namespace psm
{
    CVkSampler::CVkSampler(DevicePtr& device, const SamplerConfig& config)
    {
        mVkDevice = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        VkResult result = vk::CreateTextureSampler(mVkDevice, ToVulkan(config.MagFilter), ToVulkan(config.MinFilter), 
                                                   ToVulkan(config.UAddress), ToVulkan(config.VAddress), 
                                                   ToVulkan(config.WAddress), config.EnableAniso, config.MaxAniso,
                                                   ToVulkan(config.BorderColor), config.EnableComparision, 
                                                   ToVulkan(config.CompareOp), 0, //sampler flags 
                                                   config.MaxLod, config.MinLod, config.MipLodBias, ToVulkan(config.SamplerMode),
                                                   config.UnnormalizedCoords, &mVkSampler);
        VK_CHECK_RESULT(result);
    }

    CVkSampler::~CVkSampler()
    {
        vk::DestroySampler(mVkDevice, mVkSampler);
    }
}

