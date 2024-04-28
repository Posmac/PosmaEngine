#pragma once

#include "RHI/Interface/Types.h"
#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/ResourceFormats.h"
#include "RHI/Enums/RenderPassFormats.h"

namespace psm
{
    struct SImageBarrierConfig
    {
        CommandBufferPtr CommandBuffer;
        ImagePtr Image;
        EImageLayout OldLayout;
        EImageLayout NewLayout;
        EAccessFlags SrcAccessMask;
        EAccessFlags DstAccessMask;
        EPipelineStageFlags SourceStage;
        EPipelineStageFlags DestinationStage;
        uint32_t BaseMipLevel;
        uint32_t LevelCount;
        uint32_t BaseArrayLayer;
        uint32_t LayerCount;
        EImageAspect AspectMask;
        uint32_t MipLevels;
    };

    struct SImageToBufferCopyConfig
    {
        EFormat FormatBeforeTransition;
        EImageLayout LayoutBeforeTransition;
        EFormat FormatAfterTransition;
        EImageLayout LayoutAfterTransition;
    };

    struct SImageLayoutTransition
    {
        EFormat Format;
        EImageLayout OldLayout;
        EImageLayout NewLayout;
        EPipelineStageFlags SourceStage;
        EPipelineStageFlags DestinationStage;
        EAccessFlags SourceMask;
        EAccessFlags DestinationMask;
        EImageAspect ImageAspectFlags;
        uint32_t MipLevel;
    };
}