#include "CVkFramebuffer.h"
#include "CVkDevice.h"
#include "CVkRenderPass.h"
#include "CVkImage.h"

namespace psm
{
    CVkFramebuffer::CVkFramebuffer(const DevicePtr& device, const SFramebufferConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);
        VkRenderPass vkRenderPass = reinterpret_cast<VkRenderPass>(config.RenderPass->Raw());
        assert(vkRenderPass != nullptr);

        std::vector<VkImageView> attachments;
        attachments.resize(config.Attachments.size());

        for(int i = 0; i < attachments.size(); i++)
        {
            attachments[i] = reinterpret_cast<VkImageView>(config.Attachments[i]);
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.flags = 0;
        framebufferInfo.width = config.Size.width;
        framebufferInfo.height = config.Size.height;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.renderPass = vkRenderPass;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mDeviceInternal, &framebufferInfo, nullptr, &mFramebuffer);

        VK_CHECK_RESULT(result);
    }

    CVkFramebuffer::~CVkFramebuffer()
    {
        vkDestroyFramebuffer(mDeviceInternal, mFramebuffer, nullptr);
    }

    void* CVkFramebuffer::Raw()
    {
        return mFramebuffer;
    }

    void* CVkFramebuffer::Raw() const
    {
        return mFramebuffer;
    }
}


