#pragma once

#include <vector>
#include <optional>

#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/RenderPassFormats.h"
#include "RHI/Enums/PipelineFormats.h"

#include "RHI/RHICommon.h"

namespace psm
{
    struct SRenderPassBeginConfig;
    struct SAttachmentDescription;
    struct SAttachmentReference;
    struct SRenderPassConfig;
    struct SSubpassDependency;
    struct SSubpassDescription;
    struct DepthStencilClearValues;

    struct SAttachmentDescription
    {
        EAttachmentDescriptionFlags Flags;
        EFormat Format;
        ESamplesCount Samples;
        EAttachmentLoadOp LoadOperation;
        EAttachmentStoreOp StoreOperation;
        EAttachmentLoadOp StencilLoadOperation;
        EAttachmentStoreOp StencilStoreOperation;
        EImageLayout InitialLayout;
        EImageLayout FinalLayout;
    };

    struct SAttachmentReference
    {
        uint32_t Attachment;
        EImageLayout Layout;
    };

    struct SRenderPassConfig
    {
        std::vector<SAttachmentDescription> ColorAttachements;
        std::optional<SAttachmentDescription> DepthAttachment;
        std::optional<SAttachmentDescription> ResolveAttachment;

        std::optional<SAttachmentReference> ColorAttachmentReference;
        std::optional<SAttachmentReference> DepthStencilAttachmentReference;
        std::optional<SAttachmentReference> ResolveAttachemntReference;

        std::vector<SSubpassDescription> SubpassDescriptions;
        std::vector<SSubpassDependency> SubpassDependensies;
    };

    struct SSubpassDependency
    {
        uint32_t SrcSubpass;
        uint32_t DstSubpass;
        EPipelineStageFlags SrcStageMask;
        EPipelineStageFlags DstStageMask;
        EAccessFlags SrcAccessMask;
        EAccessFlags DstAccessMask;
        EDependencyFlags DependencyFlags;
    };
    
    struct SSubpassDescription
    {
        EPipelineBindPoint PipelineBindPoint;
        uint32_t InputAttachmentCount;
        const SAttachmentReference* pInputAttachments;
        uint32_t ColorAttachmentCount;
        const SAttachmentReference* pColorAttachments;
        const SAttachmentReference* pResolveAttachments;
        const SAttachmentReference* pDepthStencilAttachment;
        uint32_t PreserveAttachmentCount;
        const uint32_t* pPreserveAttachments;
    };

    union UClearColorValue
    {
        float       float32[4];
        int32_t     int32[4];
        uint32_t    uint32[4];
    };

    struct DepthStencilClearValues
    {
        float Depth;
        uint32_t Stencil;
    };

    union UClearValue
    {
        UClearColorValue Color;
        DepthStencilClearValues DepthStencil;
    };

    struct SRenderPassBeginConfig
    {
        RenderPassPtr RenderPass;
        FramebufferPtr Framebuffer;
        CommandBufferPtr CommandBuffer;
        SResourceOffset2D Offset;
        SResourceExtent2D Extent;
        uint32_t ClearValuesCount;
        UClearValue* pClearValues;
        ESubpassContents SubpassContents;
    };
}