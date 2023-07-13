#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        struct FramebufferAttachment
        {
            std::vector<VkImageView> Attachments;
        };

        void CreateFramebuffers(VkDevice device,
                                const std::vector<FramebufferAttachment>& attachments,
                                uint32_t oneAttachmentSize,
                                const VkExtent2D extent,
                                uint32_t frameBuffersSize,
                                VkRenderPass renderPas,
                                std::vector<VkFramebuffer>* framebuffers);
        void DestroyFramebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers);
    }
}