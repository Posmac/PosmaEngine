#include "Framebuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateFramebuffers(VkDevice device, 
            const std::vector<VkImageView>& swapchainImageViews, 
            const VkExtent2D extent, 
            uint32_t size,
            VkRenderPass renderPas, 
            std::vector<VkFramebuffer>* framebuffers)
        {
            framebuffers->resize(size);

            for (int i = 0; i < swapchainImageViews.size(); i++)
            {
                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.flags = 0;
                framebufferInfo.width = extent.width;
                framebufferInfo.height = extent.height;
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments = &swapchainImageViews[i];
                framebufferInfo.renderPass = renderPas;
                framebufferInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                    &(*framebuffers)[i]);

                VK_CHECK_RESULT(result);
            }
        }

        void DestroyFramebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers)
        {
            for (auto& framebuffer : framebuffers)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
        }
    }
}