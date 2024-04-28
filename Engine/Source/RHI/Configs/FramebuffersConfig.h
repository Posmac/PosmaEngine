#pragma once

#include <vector>

#include "RHI/VkCommon.h"
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