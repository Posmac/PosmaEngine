#pragma once

#include <cstdint>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/DescriptorPool.h"

namespace psm
{
    struct SPipelineLayoutConfig
    {
        DescriptorSetLayoutPtr* pLayouts;
        uint32_t LayoutsSize;
        PushConstantPtr* pPushConstants;
        uint32_t PushConstantsSize;
    };
}