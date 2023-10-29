#pragma once

#include "../RHISelector.h"

#include "../Context/SamplerContext.h"

namespace psm
{
    class CDevice;

    class CSampler
    {
    public:
        CSampler() = default;
        virtual ~CSampler() = default;
    public:
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
                                bool unnormalizedCoords = false) = 0;
    };
}
