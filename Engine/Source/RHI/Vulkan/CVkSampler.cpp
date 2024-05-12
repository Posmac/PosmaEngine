#include "CVkSampler.h"

#include "../RHICommon.h"

#include "TypeConvertor.h"

#include "CVkDevice.h"

namespace psm
{
    CVkSampler::CVkSampler(DevicePtr device, const SSamplerConfig& config)
    {
        mVkDevice = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkSamplerCreateInfo info = 
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = ToVulkan(config.MagFilter),
            .minFilter = ToVulkan(config.MinFilter),
            .mipmapMode = ToVulkan(config.SamplerMode),
            .addressModeU = ToVulkan(config.UAddress),
            .addressModeV = ToVulkan(config.VAddress),
            .addressModeW = ToVulkan(config.WAddress),
            .mipLodBias = config.MipLodBias,
            .anisotropyEnable = config.EnableAniso,
            .maxAnisotropy = config.MaxAniso,
            .compareEnable = config.EnableComparision,
            .compareOp = ToVulkan(config.CompareOp),
            .minLod = config.MinLod,
            .maxLod = config.MaxLod,
            .borderColor = ToVulkan(config.BorderColor),
            .unnormalizedCoordinates = config.UnnormalizedCoords,
        };

        VkResult result =  vkCreateSampler(mVkDevice, &info, nullptr, &mVkSampler);;
        VK_CHECK_RESULT(result);
    }

    CVkSampler::~CVkSampler()
    {
        vkDestroySampler(mVkDevice, mVkSampler, nullptr);
    }

    void* CVkSampler::Raw()
    {
        return mVkSampler;
    }

    void* CVkSampler::Raw() const
    {
        return mVkSampler;
    }
}

