#pragma once

#include <vector>
#include <optional>

#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/RenderPassFormats.h"

namespace psm
{
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

    struct SAttachmentDescription
    {
        EAttachmentDescriptionFlags Flags;
        EImageFormat Format;
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
}