#pragma once

#include "RenderBackend/Core.h"
#include "../Interface/Sampler.h"

namespace psm
{
    class CVkSampler : CSampler
    {
    public:
        CVkSampler() = default;
        virtual ~CVkSampler() = default;
        virtual RHI_RESULT Init(CDevice* pDevice,
                                EFilterMode magFilter,
                                EFilterMode minFilter,
                                ESamplerAddressMode uAddress,
                                ESamplerAddressMode vAddress,
                                ESamplerAddressMode wAddress,
                                EBorderColor borderColor,
                                bool enableComparision,
                                ECompareOp compareOp,
                                ESamplerMipmapMode samplerMode,
                                bool enableAniso = false,
                                float maxAniso = 0.0f,
                                float maxLod = 1.0f,
                                float minLod = 0.0f,
                                float mipLodBias = 0.0f,
                                bool unnormalizedCoords = false) override;
    private:
        VkSampler m_Sampler;
    };
}