#pragma once

#include "Common.h"

namespace psm
{
    namespace vk
    {
        VkResult CreateTextureSampler(VkDevice device,
            VkFilter magFilter,
            VkFilter minFilter,
            VkSamplerAddressMode uAdress,
            VkSamplerAddressMode vAdress,
            VkSamplerAddressMode wAdress,
            bool enableAniso,
            float maxAniso,
            VkBorderColor borderColor,
            bool enableComparision,
            VkCompareOp compareOp,
            VkSamplerCreateFlags flags,
            float maxLod,
            float minLod,
            float mipLodBias,
            VkSamplerMipmapMode mipmapMode,
            bool unnormalizedCoords,
            VkSampler* sampler);

        void DestroySampler(VkDevice device, VkSampler sampler);
    }
}