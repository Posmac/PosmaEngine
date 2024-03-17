#include "CVkRenderPass.h"

#include "RHI/VkCommon.h"
#include "TypeConvertor.h"
#include "CVkDevice.h"
#include "CVkCommandBuffer.h"
#include "CVkFramebuffer.h"

#include <vector>
#include <optional>

//#include "RenderBackend/RenderPass.h"

namespace psm
{
    CVkRenderPass::CVkRenderPass(DevicePtr device, const SRenderPassConfig& config)
    {
        //Get all VkAttachmentDescription for current subpass
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        attachmentDescriptions.resize(config.ColorAttachements.size());

        //add color subpass descriptions to current subpass
        auto srcDescrIter = config.ColorAttachements.begin();
        for(auto& description : attachmentDescriptions)
        {
            description.flags = ToVulkan(srcDescrIter->Flags);
            description.format = ToVulkan(srcDescrIter->Format);
            description.samples = ToVulkan(srcDescrIter->Samples);
            description.loadOp = ToVulkan(srcDescrIter->LoadOperation);
            description.storeOp = ToVulkan(srcDescrIter->StoreOperation);
            description.stencilLoadOp = ToVulkan(srcDescrIter->StencilLoadOperation);
            description.stencilStoreOp = ToVulkan(srcDescrIter->StencilStoreOperation);
            description.initialLayout = ToVulkan(srcDescrIter->InitialLayout);
            description.finalLayout = ToVulkan(srcDescrIter->FinalLayout);

            srcDescrIter++;
        }

        //add depth subpass description to current subpass
        if(config.DepthAttachment.has_value())
        {
            VkAttachmentDescription depthAttachment =
            {
                .flags = ToVulkan(config.DepthAttachment.value().Flags),
                .format = ToVulkan(config.DepthAttachment.value().Format),
                .samples = ToVulkan(config.DepthAttachment.value().Samples),
                .loadOp = ToVulkan(config.DepthAttachment.value().LoadOperation),
                .storeOp = ToVulkan(config.DepthAttachment.value().StoreOperation),
                .stencilLoadOp = ToVulkan(config.DepthAttachment.value().StencilLoadOperation),
                .stencilStoreOp = ToVulkan(config.DepthAttachment.value().StencilStoreOperation),
                .initialLayout = ToVulkan(config.DepthAttachment.value().InitialLayout),
                .finalLayout = ToVulkan(config.DepthAttachment.value().FinalLayout)
            };

            attachmentDescriptions.emplace_back(depthAttachment);
        }

        //add final(resolve or present) subpass description to current subpass
        if(config.ResolveAttachment.has_value())
        {
            VkAttachmentDescription resolveAttachment =
            {
                .flags = ToVulkan(config.ResolveAttachment.value().Flags),
                .format = ToVulkan(config.ResolveAttachment.value().Format),
                .samples = ToVulkan(config.ResolveAttachment.value().Samples),
                .loadOp = ToVulkan(config.ResolveAttachment.value().LoadOperation),
                .storeOp = ToVulkan(config.ResolveAttachment.value().StoreOperation),
                .stencilLoadOp = ToVulkan(config.ResolveAttachment.value().StencilLoadOperation),
                .stencilStoreOp = ToVulkan(config.ResolveAttachment.value().StencilStoreOperation),
                .initialLayout = ToVulkan(config.ResolveAttachment.value().InitialLayout),
                .finalLayout = ToVulkan(config.ResolveAttachment.value().FinalLayout)
            };

            attachmentDescriptions.emplace_back(resolveAttachment);
        }

        //add all subpasses for current render pass
        struct internalSubpassDescriptionData //used to hold all data before render pass is created
        {
            std::vector<VkAttachmentReference> InputReferences;
            std::vector<VkAttachmentReference> ColorReferences;
            std::vector<VkAttachmentReference> PreserveReferences;
            std::optional<VkAttachmentReference> DepthStencilReference;
            std::optional<VkAttachmentReference> ResolveReference;
        };

        std::vector<VkSubpassDescription> subpasses;
        std::vector<internalSubpassDescriptionData> subpassDesctiptionDatas;

        subpasses.resize(config.SubpassDescriptions.size());
        subpassDesctiptionDatas.resize(config.SubpassDescriptions.size());

        auto srcSubpassIter = config.SubpassDescriptions.begin();
        auto dstSubpassDataIter = subpassDesctiptionDatas.begin();

        for(auto& subpass : subpasses)
        {
            dstSubpassDataIter->InputReferences.resize(srcSubpassIter->InputAttachmentCount);
            dstSubpassDataIter->ColorReferences.resize(srcSubpassIter->ColorAttachmentCount);

            int inputRefCounter = 0;
            for(auto& inputRef : dstSubpassDataIter->InputReferences)
            {
                inputRef.attachment = srcSubpassIter->pInputAttachments[inputRefCounter].Attachment;
                inputRef.layout = ToVulkan(srcSubpassIter->pInputAttachments[inputRefCounter].Layout);

                inputRefCounter++;
            }

            int colorRefCounter = 0;
            for(auto& colorRef : dstSubpassDataIter->ColorReferences)
            {
                colorRef.attachment = srcSubpassIter->pColorAttachments[colorRefCounter].Attachment;
                colorRef.layout = ToVulkan(srcSubpassIter->pColorAttachments[colorRefCounter].Layout);

                colorRefCounter++;
            }

            subpass.flags = 0;
            subpass.pipelineBindPoint = ToVulkan(srcSubpassIter->PipelineBindPoint);
            subpass.inputAttachmentCount = srcSubpassIter->InputAttachmentCount;
            subpass.pInputAttachments = dstSubpassDataIter->InputReferences.data();
            subpass.colorAttachmentCount = srcSubpassIter->ColorAttachmentCount;
            subpass.pColorAttachments = dstSubpassDataIter->ColorReferences.data();
            subpass.preserveAttachmentCount = srcSubpassIter->PreserveAttachmentCount;
            subpass.pPreserveAttachments = srcSubpassIter->pPreserveAttachments;
            subpass.pDepthStencilAttachment = dstSubpassDataIter->DepthStencilReference.has_value() ? &dstSubpassDataIter->DepthStencilReference.value() : nullptr;
            subpass.pResolveAttachments = dstSubpassDataIter->ResolveReference.has_value() ? &dstSubpassDataIter->ResolveReference.value() : nullptr;

            srcSubpassIter++;
        }

        std::vector<VkSubpassDependency> dependencies;
        dependencies.resize(config.SubpassDependensies.size());

        auto srcSubpassDependencyIter = config.SubpassDependensies.begin();

        int srcDependenciesCounter = 0;

        for(auto& dependency : dependencies)
        {
            dependency.srcSubpass = srcSubpassDependencyIter->SrcSubpass;
            dependency.dstSubpass = srcSubpassDependencyIter->DstSubpass;
            dependency.srcAccessMask = ToVulkan(srcSubpassDependencyIter->SrcAccessMask);
            dependency.dstAccessMask = ToVulkan(srcSubpassDependencyIter->DstAccessMask);
            dependency.srcStageMask = ToVulkan(srcSubpassDependencyIter->SrcStageMask);
            dependency.dstStageMask = ToVulkan(srcSubpassDependencyIter->DstStageMask);
            dependency.dependencyFlags = ToVulkan(srcSubpassDependencyIter->DependencyFlags);

            srcDependenciesCounter++;
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.flags = 0;
        renderPassInfo.attachmentCount = attachmentDescriptions.size();
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = subpasses.size();
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkResult result = vkCreateRenderPass(mDeviceInternal, &renderPassInfo, nullptr, &mRenderPass);

        VK_CHECK_RESULT(result);
    }

    CVkRenderPass::~CVkRenderPass()
    {
        vkDestroyRenderPass(mDeviceInternal, mRenderPass, nullptr);
    }

    void CVkRenderPass::BeginRenderPass(const SRenderPassBeginConfig& config)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(config.CommandBuffer->GetRawPointer());
        VkRenderPass vkRenderPass = reinterpret_cast<VkRenderPass>(config.RenderPass->GetNativeRawPtr());
        VkFramebuffer vkFramebuffer = reinterpret_cast<VkFramebuffer>(config.Framebuffer->GetRawPointer());

        std::vector<VkClearValue> clearValues;
        clearValues.resize(config.ClearValuesCount);

        clearValues[0] =
        {
            .color = *config.pClearValues[0].Color.float32
        };

        clearValues[1] =
        {
            .depthStencil = 
            {
                .depth = config.pClearValues[1].DepthStencil.Depth,
                .stencil = config.pClearValues[1].DepthStencil.Stencil,
            }
        };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkRenderPass;
        renderPassInfo.framebuffer = vkFramebuffer;
        renderPassInfo.renderArea.offset = config.Offset;
        renderPassInfo.renderArea.extent = config.Extent;
        renderPassInfo.clearValueCount = config.ClearValuesCount;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, ToVulkan(config.SubpassContents));
    }

    void CVkRenderPass::EndRenderPass(CommandBufferPtr commandBuffer)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->GetRawPointer());
        vkCmdEndRenderPass(vkCommandBuffer);
    }

    void CVkRenderPass::SetViewport(CommandBufferPtr commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->GetRawPointer());
        VkViewport viewport =
        {
            .x = viewPortX,
            .y = viewPortY,
            .width = viewPortWidth,
            .height = viewPortHeight,
            .minDepth = viewPortMinDepth,
            .maxDepth = viewPortMaxDepth
        };

        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    }

    void CVkRenderPass::SetScissors(CommandBufferPtr commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->GetRawPointer());

        VkRect2D scissor =
        {
            .offset = scissorsOffet,
            .extent = scissorsExtent
        };
        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    }

    void* CVkRenderPass::GetNativeRawPtr()
    {
        return mRenderPass;
    }
}