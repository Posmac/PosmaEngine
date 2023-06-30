#include "RenderPass.h"

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
            VkRenderPass* renderPass)
        {
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.pNext = nullptr;
            renderPassInfo.flags = 0;
            renderPassInfo.attachmentCount = attachmentsCount;
            renderPassInfo.pAttachments = pAttachments;
            renderPassInfo.subpassCount = subpassDescCount;
            renderPassInfo.pSubpasses = pSubpassDescriptions;
            renderPassInfo.dependencyCount = dependenciesCount;
            renderPassInfo.pDependencies = dependencies;

            VkResult result = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr,
                renderPass);

            VK_CHECK_RESULT(result);
        }

        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass)
        {
            vkDestroyRenderPass(device, renderpass, nullptr);
        }

        void BeginRenderPass(VkRenderPass renderPass, 
            VkFramebuffer framebuffer, 
            VkOffset2D offset,
            VkExtent2D extent, 
            VkClearValue* clearValues, 
            uint32_t clearValuesCount, 
            VkCommandBuffer commandBuffer, 
            VkSubpassContents subpassContents)
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffer;
            renderPassInfo.renderArea.offset = offset;
            renderPassInfo.renderArea.extent = extent;
            renderPassInfo.clearValueCount = clearValuesCount;
            renderPassInfo.pClearValues = clearValues;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, subpassContents);
        }

        void SetViewPortAndScissors(VkCommandBuffer commandBuffer, 
            float viewPortX, float viewPortY,
            float viewPortWidth, float viewPortHeight, 
            float viewPortMinDepth, float viewPortMaxDepth, 
            VkOffset2D scissorsOffet,
            VkExtent2D scissorsExtent)
        {
            VkViewport viewport{};
            viewport.x = viewPortX;
            viewport.y = viewPortY;
            viewport.width = viewPortWidth;
            viewport.height = viewPortHeight;
            viewport.minDepth = viewPortMinDepth;
            viewport.maxDepth = viewPortMaxDepth;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = scissorsOffet;
            scissor.extent = scissorsExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        }
    }
}

