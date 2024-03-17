#pragma once

#include <vector>

#include "RHI/VkCommon.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    struct SFramebufferConfig
    {
        std::vector<ImagePtr> Attachments;
        void* SwapchainImage;
        SResourceExtent2D Size;
        RenderPassPtr RenderPass;
    };

   
}