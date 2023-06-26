#pragma once

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateRenderPass(VkDevice logicalDevice,
            const VkAttachmentDescription* pAttachments,
            uint32_t attachmentsCount,
            const VkSubpassDescription* pSubpassDescriptions,
            uint32_t subpassDescCount,
            VkRenderPass* renderPass);

        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass);
    }
}