#include "Sampler.h"

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
            VkSampler* sampler)
        {
            VkSamplerCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.magFilter = magFilter;
            info.minFilter = minFilter;
            info.addressModeU = uAdress;
            info.addressModeV = vAdress;
            info.addressModeW = wAdress;
            info.anisotropyEnable = enableAniso;
            info.borderColor = borderColor;
            info.compareEnable = enableComparision;
            info.compareOp = compareOp;
            info.flags = flags;
            info.maxAnisotropy = maxAniso;
            info.maxLod = maxLod;
            info.minLod = minLod;
            info.mipLodBias = mipLodBias;
            info.mipmapMode = mipmapMode;
            info.unnormalizedCoordinates = unnormalizedCoords;

            return vkCreateSampler(device, &info, nullptr, sampler);;
        }

        void DestroySampler(VkDevice device, VkSampler sampler)
        {
            vkDestroySampler(device, sampler, nullptr);
        }
    }
}