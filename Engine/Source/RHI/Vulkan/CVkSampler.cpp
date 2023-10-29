#include "CVkSampler.h"

#include "RenderBackend/Sampler.h"
#include "TypeConvertor.h"

#include "CVkDevice.h"

namespace psm
{
    RHI_RESULT CVkSampler::Init(CDevice* pDevice, 
                                EFilterMode magFilter, 
                                EFilterMode minFilter,
                                ESamplerAddressMode uAddress,
                                ESamplerAddressMode vAddress,
                                ESamplerAddressMode wAddress,
                                EBorderColor borderColor,
                                bool enableComparision,
                                ECompareOp compareOp,
                                ESamplerMipmapMode samplerMode,
                                bool enableAniso,
                                float maxAniso, 
                                float maxLod,
                                float minLod,
                                float mipLodBias, 
                                bool unnormalizedCoords)
    {
        CVkDevice* device = (CVkDevice*)pDevice;

        vk::CreateTextureSampler(device->GetDevice(),
                                 ToVulkan(magFilter),
                                 ToVulkan(minFilter), 
                                 ToVulkan(uAddress), 
                                 ToVulkan(vAddress),
                                 ToVulkan(wAddress),
                                 enableAniso, 
                                 maxAniso, 
                                 ToVulkan(borderColor),
                                 enableComparision, 
                                 ToVulkan(compareOp), 
                                 0, //sampler flags 
                                 maxLod, 
                                 minLod,
                                 mipLodBias, 
                                 ToVulkan(samplerMode),
                                 unnormalizedCoords, 
                                 &m_Sampler);

        return VK_SUCCESS;
    }
}

