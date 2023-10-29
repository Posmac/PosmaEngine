#include "Framebuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateFramebuffers(VkDevice device, 
            const std::vector<FramebufferAttachment>& attachments,
            uint32_t oneAttachmentSize, 
            const VkExtent2D extent, 
            uint32_t frameBuffersSize,
            VkRenderPass renderPas, 
            std::vector<VkFramebuffer>* framebuffers)
        {
            framebuffers->resize(frameBuffersSize);

            for (int i = 0; i < attachments.size(); i++)
            {
                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.flags = 0;
                framebufferInfo.width = extent.width;
                framebufferInfo.height = extent.height;
                framebufferInfo.attachmentCount = oneAttachmentSize;
                framebufferInfo.pAttachments = attachments[i].Attachments.data();
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