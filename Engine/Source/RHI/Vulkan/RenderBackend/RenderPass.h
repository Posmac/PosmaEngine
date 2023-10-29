#pragma once

#include "Core.h"

namespace psm
{
    namespace vk
    {
        void CreateRenderPass(VkDevice logicalDevice,
            const VkAttachmentDescription* pAttachments,
            uint32_t attachmentsCount,
            const VkSubpassDescription* pSubpassDescriptions,
            uint32_t subpassDescCount,
            const VkSubpassDependency* dependencies,
            uint32_t dependenciesCount,
            VkRenderPass* renderPass);

        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass);

        void BeginRenderPass(VkRenderPass renderPass,
            VkFramebuffer framebuffer,
            VkOffset2D offset,
            VkExtent2D extent,
            VkClearValue* clearValues,
            uint32_t clearValuesCount,
            VkCommandBuffer commandBuffer,
            VkSubpassContents subpassContents);

        void SetViewPortAndScissors(VkCommandBuffer commandBuffer,
            float viewPortX, float viewPortY,
            float viewPortWidth, float viewPortHeight,
            float viewPortMinDepth, float viewPortMaxDepth,
            VkOffset2D scissorsOffet,
            VkExtent2D scissorsExtent);
    }
}