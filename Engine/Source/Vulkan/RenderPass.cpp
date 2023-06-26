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
            renderPassInfo.dependencyCount = 0;
            renderPassInfo.pDependencies = nullptr;

            VkResult result = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr,
                renderPass);

            VK_CHECK_RESULT(result);
        }

        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass)
        {
            vkDestroyRenderPass(device, renderpass, nullptr);
        }
    }
}

