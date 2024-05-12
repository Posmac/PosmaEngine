#pragma once

#include <vector>

#include "RHI/RHICommon.h"

#include "RHI/Interface/Types.h"

namespace psm
{
    struct SFramebufferConfig
    {
        std::vector<void*> Attachments;
        SResourceExtent2D Size;
        RenderPassPtr RenderPass;
    };
}