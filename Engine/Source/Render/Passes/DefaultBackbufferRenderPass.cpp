#include "DefaultBackbufferRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

namespace psm
{
    namespace graph
    {
        DefaultBackbufferRenderPassNode::DefaultBackbufferRenderPassNode(const foundation::Name& name,
                                                                         const DevicePtr& device,
                                                                         const ResourceMediatorPtr& resourceMediator,
                                                                         const SwapchainPtr swapchain,
                                                                         EFormat swapchainImagesFormat)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = swapchain->GetSwapchainSize();
            CreateDepthStencilRenderTarget(swapchain);
            CreateRenderPass(swapchainImagesFormat);
            CreateFramebuffers(swapchain);
        }

        DefaultBackbufferRenderPassNode::~DefaultBackbufferRenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "DefaultBackbufferRenderPassNode destructor");

            mDepthStencilRenderTarget = nullptr;
            for(auto& img : mShadowMapRefs)
                img = nullptr;
            mShadowMapRefs.clear();
        }

        void DefaultBackbufferRenderPassNode::PreRender(CommandBufferPtr& commandBuffer, uint32_t index)
        {
            RenderPassNode::PreRender(commandBuffer, index);

            std::array<UClearValue, 2> clearColor{};
            clearColor[0].Color = { {0.2f, 0.2f, 0.2f, 1.0f} };
            clearColor[1].DepthStencil = { 1.0f, 0 };

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

        void DefaultBackbufferRenderPassNode::CreateRenderPass(EFormat swapchainImagesFormat)
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

            SAttachmentDescription depthStencilDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = mDepthStencilFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            SAttachmentReference colorAttachmentReference =
            {
                .Attachment = 0,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            SAttachmentReference depthAttachmentReference =
            {
                .Attachment = 1,
                .Layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            SSubpassDescription subpassDescription =
            {
                .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
                .InputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .ColorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentReference,
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = &depthAttachmentReference,
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
                .DepthAttachment = depthStencilDescription,
                .ResolveAttachment = {},

                .ColorAttachmentReference = colorAttachmentReference,
                .DepthStencilAttachmentReference = depthAttachmentReference,
                .ResolveAttachemntReference = {},

                .SubpassDescriptions = {subpassDescription},
                .SubpassDependensies = {dependency}
            };

            mRenderPass = mDeviceInternal->CreateRenderPass(renderPassConfig);
        }

        void DefaultBackbufferRenderPassNode::CreateDepthStencilRenderTarget(const SwapchainPtr& swapchain)
        {
            auto desiredFormats = { EFormat::D32_SFLOAT, EFormat::D32_SFLOAT_S8_UINT, EFormat::D24_UNORM_S8_UINT };

            mDepthStencilFormat = mDeviceInternal->FindSupportedFormat(desiredFormats, EImageTiling::OPTIMAL, EFeatureFormat::DEPTH_STENCIL_ATTACHMENT_BIT);

            SResourceExtent3D imageSize = swapchain->GetSwapchainSize();

            SImageConfig imageConfig =
            {
                .ImageSize = imageSize,
                .MipLevels = 1,
                .ArrayLevels = 1,
                .Type = EImageType::TYPE_2D,
                .Format = mDepthStencilFormat,
                .Tiling = EImageTiling::OPTIMAL,
                .InitialLayout = EImageLayout::UNDEFINED,
                .Usage = EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
                .SharingMode = ESharingMode::EXCLUSIVE,
                .SamplesCount = ESamplesCount::COUNT_1,
                .Flags = EImageCreateFlags::NONE,
                .ViewFormat = mDepthStencilFormat,
                .ViewType = EImageViewType::TYPE_2D,
                .ViewAspect = EImageAspect::DEPTH_BIT
            };

            mDepthStencilRenderTarget = mDeviceInternal->CreateImage(imageConfig);

            SCommandPoolConfig cmdPoolConfig =
            {
                .QueueFamilyIndex = mDeviceInternal->GetDeviceData().vkData.GraphicsQueueIndex,
                .QueueType = EQueueType::GRAHPICS
            };

            auto cmdPool = mDeviceInternal->CreateCommandPool(cmdPoolConfig);

            CommandBufferPtr commandBuffer = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(cmdPool);

            SImageLayoutTransition imageLayoutTransition =
            {
                .Format = mDepthStencilFormat,
                .OldLayout = EImageLayout::UNDEFINED,
                .NewLayout = EImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                .DestinationStage = EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
                .SourceMask = EAccessFlags::NONE,
                .DestinationMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_READ_BIT | EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .ImageAspectFlags = EImageAspect::DEPTH_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, mDepthStencilRenderTarget, imageLayoutTransition);

            commandBuffer->End();

            mDeviceInternal->SubmitSingleTimeCommandBuffer(cmdPool, commandBuffer);
        }

        void DefaultBackbufferRenderPassNode::CreateFramebuffers(const SwapchainPtr swapchain)
        {
            mFramebuffersSize = swapchain->GetSwapchainSize();

            uint32_t frameBufferAttachmentCount = swapchain->GetImagesCount();
            mFramebuffers.resize(frameBufferAttachmentCount);

            for(int i = 0; i < frameBufferAttachmentCount; i++)
            {
                SFramebufferConfig framebufferConfig =
                {
                    .Attachments = { swapchain->ImageAtIndex(i), mDepthStencilRenderTarget->RawImageView() },
                    .Size = { mFramebuffersSize.width, mFramebuffersSize.height },
                    .RenderPass = mRenderPass
                };

                mFramebuffers[i] = mDeviceInternal->CreateFramebuffer(framebufferConfig);
            };
        }

        void DefaultBackbufferRenderPassNode::PostRender(CommandBufferPtr& commandBuffer)
        {
            mRenderPass->EndRenderPass(commandBuffer);
        }

        void DefaultBackbufferRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {
            for(int i = 0; i < framesCount; i++)
            {
                foundation::Name refName = GetResourceIndexedName(SHADOWMAP_RENDERTARGET_NAME, i);
                auto shadowMapRef = mResourceMediator->GetImageByName(refName);
                mShadowMapRefs.push_back(shadowMapRef);
            }
        }

        void DefaultBackbufferRenderPassNode::CollectReferences()
        {
            mResourceMediator->RegisterImageResource(DEPTHSTECIL_RENDERTARGET_NAME, mDepthStencilRenderTarget);
        }

        void DefaultBackbufferRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {
            uint32_t mFramebuffersCount = mFramebuffers.size();

            mDepthStencilRenderTarget = nullptr;

            for(auto& fb : mFramebuffers)
                fb = nullptr;
            mFramebuffers.clear();

            CreateDepthStencilRenderTarget(swapchain);
            CreateFramebuffers(swapchain);

            ImagePtr& depthStencilImage = mResourceMediator->GetImageByName(DEPTHSTECIL_RENDERTARGET_NAME);
            depthStencilImage = mDepthStencilRenderTarget;
        }
    }
}