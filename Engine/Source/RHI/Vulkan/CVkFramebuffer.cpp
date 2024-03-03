#include "CVkFramebuffer.h"
#include "CVkDevice.h"
#include "CVkRenderPass.h"
#include "CVkImage.h"

namespace psm
{
    CVkFramebuffer::CVkFramebuffer(DevicePtr device, const SFramebufferConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        VkRenderPass vkRenderPass = reinterpret_cast<VkRenderPass>(config.RenderPass->GetNativeRawPtr());

        std::vector<VkImageView> attachments;
        attachments.resize(config.Attachments.size());

        for(int i = 0; i < attachments.size(); i++)
        {
            attachments[i] = reinterpret_cast<VkImageView>(config.Attachments[i]->GetImageView());
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.flags = 0;
        framebufferInfo.width = config.Size.width;
        framebufferInfo.height = config.Size.height;
        framebufferInfo.attachmentCount = config.Attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.renderPass = vkRenderPass;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mDeviceInternal, &framebufferInfo, nullptr, &mFramebuffer);

        VK_CHECK_RESULT(result);
    }

    CVkFramebuffer::~CVkFramebuffer()
    {

    }

    void* CVkFramebuffer::GetRawPointer()
    {
        return mFramebuffer;
    }
}


