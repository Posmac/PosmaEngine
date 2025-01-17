#include "CompositeBackbufferRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

namespace psm
{
    namespace graph
    {
        CompositeBackbufferRenderPassNode::CompositeBackbufferRenderPassNode(const foundation::Name& name,
                                                                         const DevicePtr& device,
                                                                         const ResourceMediatorPtr& resourceMediator,
                                                                         const SwapchainPtr swapchain,
                                                                         EFormat swapchainImagesFormat)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = swapchain->GetSwapchainSize();
            CreateRenderPass(swapchainImagesFormat);
            CreateFramebuffers(swapchain);
        }

        CompositeBackbufferRenderPassNode::~CompositeBackbufferRenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "CompositeBackbufferRenderPassNode destructor");

            for(auto& fb : mFramebuffers)
                fb = nullptr;
            mFramebuffers.clear();

            /*for(auto& img : mShadowMapRefs)
                img = nullptr;
            mShadowMapRefs.clear();*/
        }

        void CompositeBackbufferRenderPassNode::PreRender(CommandBufferPtr& commandBuffer, uint32_t index)
        {
            std::array<UClearValue, 2> clearColor{};
            clearColor[0].Color = { {0.2f, 0.2f, 0.2f, 1.0f} };
            clearColor[1].DepthStencil = { 0.0f, 0 };

            SRenderPassBeginConfig defaultRenderPassBeginConfig =
            {
                .RenderPass = mRenderPass,
                .Framebuffer = mFramebuffers[index],
                .CommandBuffer = commandBuffer,
                .Offset = {0, 0},
                .Extent = {mFramebuffersSize.width, mFramebuffersSize.height},
                .ClearValuesCount = clearColor.size(),
                .pClearValues = clearColor.data(),
                .SubpassContents = ESubpassContents::INLINE,
            };

            mRenderPass->BeginRenderPass(defaultRenderPassBeginConfig);

            mDeviceInternal->SetViewport(commandBuffer, 0.0f, 0.0f, static_cast<float>(mFramebuffersSize.width), static_cast<float>(mFramebuffersSize.width), 0.0f, 1.0f);
            mDeviceInternal->SetScissors(commandBuffer, { 0, 0 }, { mFramebuffersSize.width, mFramebuffersSize.height });
        }

        void CompositeBackbufferRenderPassNode::CreateRenderPass(EFormat swapchainImagesFormat)
        {
            SAttachmentDescription colorDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = swapchainImagesFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::PRESENT_SRC_KHR
            };

            SAttachmentReference colorAttachmentReference =
            {
                .Attachment = 0,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            SSubpassDescription subpassDescription =
            {
                .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
                .InputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .ColorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentReference,
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = nullptr,
                .PreserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr
            };

            SSubpassDependency dependency =
            {
                .SrcSubpass = VK_SUBPASS_EXTERNAL,
                .DstSubpass = 0,
                .SrcStageMask = EPipelineStageFlags::BOTTOM_OF_PIPE_BIT,
                .DstStageMask = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT | EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
                .SrcAccessMask = EAccessFlags::NONE,
                .DstAccessMask = EAccessFlags::COLOR_ATTACHMENT_WRITE_BIT | EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            };

            SRenderPassConfig renderPassConfig =
            {
                .ColorAttachements = {colorDescription},
                .DepthAttachment = {},
                .ResolveAttachment = {},

                //.ColorAttachmentReference = colorAttachmentReference,
                //.DepthStencilAttachmentReference = depthAttachmentReference,
                //.ResolveAttachemntReference = {},

                .SubpassDescriptions = {subpassDescription},
                .SubpassDependensies = {dependency}
            };

            mRenderPass = mDeviceInternal->CreateRenderPass(renderPassConfig);
        }

        void CompositeBackbufferRenderPassNode::CreateFramebuffers(const SwapchainPtr swapchain)
        {
            mFramebuffersSize = swapchain->GetSwapchainSize();

            uint32_t frameBufferAttachmentCount = swapchain->GetImagesCount();
            mFramebuffers.resize(frameBufferAttachmentCount);

            for(int i = 0; i < frameBufferAttachmentCount; i++)
            {
                SFramebufferConfig framebufferConfig =
                {
                    .Attachments = { swapchain->ImageAtIndex(i)},
                    .Size = { mFramebuffersSize.width, mFramebuffersSize.height },
                    .RenderPass = mRenderPass
                };

                mFramebuffers[i] = mDeviceInternal->CreateFramebuffer(framebufferConfig);
            };
        }

        void CompositeBackbufferRenderPassNode::PostRender(CommandBufferPtr& commandBuffer)
        {
            mRenderPass->EndRenderPass(commandBuffer);
        }

        void CompositeBackbufferRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {
           /* for(int i = 0; i < framesCount; i++)
            {
                foundation::Name refName = GetResourceIndexedName(SHADOWMAP_RENDERTARGET_NAME, i);
                auto shadowMapRef = mResourceMediator->GetImageByName(refName);
                mShadowMapRefs.push_back(shadowMapRef);
            }*/
        }

        void CompositeBackbufferRenderPassNode::CollectReferences(uint32_t framesCount)
        {
            /*mGbufferTargetsRefs.resize(framesCount);

            for(int i = 0; i < framesCount; i++)
            {
                internalGbufferRefImages& images = mGbufferTargetsRefs[i];

                foundation::Name albedoRefName = GetResourceIndexedName(GBUFFER_ALBEDO_RENDERTARGET_NAME, i);
                images.AlbedoTargetImage = mResourceMediator->GetImageByName(albedoRefName);

                foundation::Name normalRefName = GetResourceIndexedName(GBUFFER_NORMAL_RENDERTARGET_NAME, i);
                images.NormalTargetImage = mResourceMediator->GetImageByName(normalRefName);

                foundation::Name depthRefName = GetResourceIndexedName(GBUFFER_DEPTH_RENDERTARGET_NAME, i);
                images.DepthTargetImage = mResourceMediator->GetImageByName(depthRefName);

                foundation::Name worldPosRefName = GetResourceIndexedName(GBUFFER_WORLDPOS_RENDERTARGET_NAME, i);
                images.WorldPositionsTargetImage = mResourceMediator->GetImageByName(worldPosRefName);

                foundation::Name emissionRefName = GetResourceIndexedName(GBUFFER_EMISSION_RENDERTARGET_NAME, i);
                images.EmissionTargetImage = mResourceMediator->GetImageByName(emissionRefName);

                foundation::Name specularGlossRefName = GetResourceIndexedName(GBUFFER_SPECULAR_GLOSS_RENDERTARGET_NAME, i);
                images.SpecularGlosinessTargetImage = mResourceMediator->GetImageByName(specularGlossRefName);

                foundation::Name roughMetallRefName = GetResourceIndexedName(GBUFFER_ROUGH_METAL_RENDERTARGET_NAME, i);
                images.RoughnessMetalnessTargetImage = mResourceMediator->GetImageByName(roughMetallRefName);
            }*/
        }

        void CompositeBackbufferRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {
            for(auto& fb : mFramebuffers)
                fb = nullptr;
            mFramebuffers.clear();

            CreateFramebuffers(swapchain);
        }
    }
}