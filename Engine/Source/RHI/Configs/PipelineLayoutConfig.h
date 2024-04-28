#pragma once

#include <cstdint>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/DescriptorPool.h"
#include "RHI/Enums/PipelineFormats.h"

namespace psm
{
    struct SPushConstant
    {
        EShaderStageFlag StageFlags;
        uint32_t Offset;
        uint32_t Size;
    };

    struct SPipelineLayoutConfig
    {
        DescriptorSetLayoutPtr* pLayouts;
        uint32_t LayoutsSize;
        SPushConstant* pPushConstants;
        uint32_t PushConstantsSize;
    };
}