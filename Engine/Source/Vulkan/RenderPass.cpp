#include "RenderPass.h"

namespace psm
{
    namespace vk
    {
        void CreateRenderPass(VkDevice logicalDevice, VkFormat imageFormat, 
            VkRenderPass* renderPass)
        {
            VkAttachmentDescription colorAttachmentDescription{};
            colorAttachmentDescription.flags = 0;
            colorAttachmentDescription.format = imageFormat;
            colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentReference{};
            colorAttachmentReference.attachment = 0;
            colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpassDescr{};
            subpassDescr.flags = 0;
            subpassDescr.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescr.inputAttachmentCount = 0;
            subpassDescr.pInputAttachments = nullptr;
            subpassDescr.colorAttachmentCount = 1;
            subpassDescr.pColorAttachments = &colorAttachmentReference;
            subpassDescr.pResolveAttachments = nullptr;
            subpassDescr.pDepthStencilAttachment = nullptr;
            subpassDescr.preserveAttachmentCount = 0;
            subpassDescr.pPreserveAttachments = nullptr;

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.pNext = nullptr;
            renderPassInfo.flags = 0;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &colorAttachmentDescription;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpassDescr;
            renderPassInfo.dependencyCount = 0;
            renderPassInfo.pDependencies = nullptr;

            VkResult result = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr,
                renderPass);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create render pass" << std::endl;
            }
        }

        void DestroyRenderPass(VkDevice device, VkRenderPass renderpass)
        {
            vkDestroyRenderPass(device, renderpass, nullptr);
        }
    }
}

