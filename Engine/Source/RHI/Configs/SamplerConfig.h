#pragma once

#include "../Formats/SamplerFormats.h"

namespace psm
{
    struct SamplerConfig
    {
        EFilterMode MagFilter;
        EFilterMode MinFilter;
        ESamplerAddressMode UAddress;
        ESamplerAddressMode VAddress;
        ESamplerAddressMode WAddress;
        EBorderColor BorderColor;
        bool EnableComparision;
        ECompareOp CompareOp;
        ESamplerMipmapMode SamplerMode;
        bool EnableAniso = false;
        float MaxAniso = 0.0f;
        float MaxLod = 1.0f;
        float MinLod = 0.0f;
        float MipLodBias = 0.0f;
        bool UnnormalizedCoords = false;
    };
}