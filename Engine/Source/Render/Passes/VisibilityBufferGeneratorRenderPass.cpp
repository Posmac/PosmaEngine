#include "VisibilityBufferGeneratorRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

namespace psm
{
    namespace graph
    {
        VisibilityBufferGeneratorRenderPassNode::VisibilityBufferGeneratorRenderPassNode(const foundation::Name& name, const DevicePtr& device, const ResourceMediatorPtr& resourceMediator, SResourceExtent3D framebufferSize, uint32_t framebuffersCount)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = framebufferSize;
            CreateDepthStencilRenderTarget(framebufferSize);
            CreateRenderPass(framebuffersCount);
            CreateFramebuffers(framebufferSize, framebuffersCount);
        }

        VisibilityBufferGeneratorRenderPassNode::~VisibilityBufferGeneratorRenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "VisibilityBufferGeneratorRenderPassNode");

            mDepthStencilRenderTarget = nullptr;

            for(auto& fb : mFramebuffers)
            {
                fb = nullptr;
            }

            for(auto& image : mVisBufferImages)
            {
                image = nullptr;
            }

            mFramebuffers.clear();
            mVisBufferImages.clear();
        }

        void VisibilityBufferGeneratorRenderPassNode::PreRender()
        {
            RenderPassNode::PreRender();

            std::array<UClearValue, 2> clearColor{};
            clearColor[0].Color = { {0.0f, 0.0f, 0.0f, 1.0f} };
            clearColor[1].DepthStencil = { 0.0f, 0 };

            SRenderPassBeginConfig defaultRenderPassBeginConfig =
            {
                .RenderPass = mRenderPass,
                .Framebuffer = mFramebuffers[mCurrentFramebufferIndex],
                .CommandBuffer = mCurrentCommandBuffer,
                .Offset = {0, 0},
                .Extent = {mFramebuffersSize.width, mFramebuffersSize.height},
                .ClearValuesCount = clearColor.size(),
                .pClearValues = clearColor.data(),
                .SubpassContents = ESubpassContents::INLINE,
            };

            mRenderPass->BeginRenderPass(defaultRenderPassBeginConfig);

            mDeviceInternal->SetViewport(mCurrentCommandBuffer, 0.0f, 0.0f, static_cast<float>(mFramebuffersSize.width),
                                         static_cast<float>(mFramebuffersSize.height), 0.0f, 1.0f);
            mDeviceInternal->SetScissors(mCurrentCommandBuffer, { 0, 0 }, { mFramebuffersSize.width, mFramebuffersSize.height });
        }

        void VisibilityBufferGeneratorRenderPassNode::PostRender()
        {
            mRenderPass->EndRenderPass(mCurrentCommandBuffer);

            RenderPassNode::PostRender();
        }

        void VisibilityBufferGeneratorRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {

        }

        void VisibilityBufferGeneratorRenderPassNode::CollectReferences(uint32_t framesCount)
        {
            mResourceMediator->RegisterImageResource(VISBUF_DEPTHSTECIL_RENDERTARGET_NAME, mDepthStencilRenderTarget);

            for(int i = 0; i < mVisBufferImages.size(); i++)
            {
                foundation::Name refName = GetResourceIndexedName(VISBUF_GENERATION_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(refName, mVisBufferImages[i]);
            }
        }

        void VisibilityBufferGeneratorRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {
            uint32_t framebuffersCount = mFramebuffers.size();

            mDepthStencilRenderTarget = nullptr;

            for(auto& fb : mFramebuffers)
            {
                fb = nullptr;
            }

            for(auto& image : mVisBufferImages)
            {
                image = nullptr;
            }

            mFramebuffers.clear();
            mVisBufferImages.clear();

            CreateDepthStencilRenderTarget(swapchain->GetSwapchainSize());
            CreateFramebuffers(swapchain->GetSwapchainSize(), framebuffersCount);

            mResourceMediator->UpdateImageReference(VISBUF_DEPTHSTECIL_RENDERTARGET_NAME, mDepthStencilRenderTarget);

            for(int i = 0; i < mVisBufferImages.size(); i++)
            {
                foundation::Name refName = GetResourceIndexedName(VISBUF_GENERATION_RENDERTARGET_NAME, i);
                mResourceMediator->UpdateImageReference(refName, mVisBufferImages[i]);
            }
        }

        void VisibilityBufferGeneratorRenderPassNode::CreateRenderPass(uint32_t framesCount)
        {
            EFormat visBufferFormat = EFormat::R8G8B8A8_UNORM;

            SAttachmentDescription visbufferDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = visBufferFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
            };

            SAttachmentReference visBufferReference =
            {
                .Attachment = 0,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            };

            SAttachmentDescription depthAttachmentDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = EFormat::D32_SFLOAT,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference depthAttachmentReference =
            {
                .Attachment = 1,
                .Layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            SSubpassDescription subpassDescr =
            {
                .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
                .InputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .ColorAttachmentCount = 1,
                .pColorAttachments = &visBufferReference,
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = &depthAttachmentReference,
                .PreserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr,
            };

            SSubpassDependency subpassDependency =
            {
                .SrcSubpass = VK_SUBPASS_EXTERNAL,
                .DstSubpass = 0,
                .SrcStageMask = EPipelineStageFlags::BOTTOM_OF_PIPE_BIT,
                .DstStageMask = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT,
                .SrcAccessMask = EAccessFlags::NONE,
                .DstAccessMask = EAccessFlags::COLOR_ATTACHMENT_WRITE_BIT,
                .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
            };

            SRenderPassConfig renderPassConfig =
            {
                .ColorAttachements = {visbufferDescription},
                .DepthAttachment = depthAttachmentDescription,
                .ResolveAttachment = {},

                .SubpassDescriptions = {subpassDescr},
                .SubpassDependensies = {subpassDependency}
            };

            mRenderPass = mDeviceInternal->CreateRenderPass(renderPassConfig);
        }

        void VisibilityBufferGeneratorRenderPassNode::CreateDepthStencilRenderTarget(SResourceExtent3D size)
        {
            SImageConfig imageConfig =
            {
                .ImageSize = size,
                .MipLevels = 1,
                .ArrayLevels = 1,
                .Type = EImageType::TYPE_2D,
                .Format = EFormat::D32_SFLOAT,
                .Tiling = EImageTiling::OPTIMAL,
                .InitialLayout = EImageLayout::UNDEFINED,
                .Usage = EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
                .SharingMode = ESharingMode::EXCLUSIVE,
                .SamplesCount = ESamplesCount::COUNT_1,
                .Flags = EImageCreateFlags::NONE,
                .ViewFormat = EFormat::D32_SFLOAT,
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
                .Format = EFormat::D32_SFLOAT,
                .OldLayout = EImageLayout::UNDEFINED,
                .NewLayout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,//EImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                .DestinationStage = EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
                .SourceMask = EAccessFlags::NONE,
                .DestinationMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_READ_BIT | EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .ImageAspectFlags = EImageAspect::DEPTH_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, mDepthStencilRenderTarget, imageLayoutTransition);

            mDeviceInternal->SubmitSingleTimeCommandBuffer(cmdPool, commandBuffer);
        }

        void VisibilityBufferGeneratorRenderPassNode::CreateFramebuffers(SResourceExtent3D framebufferSize, uint32_t framesCount)
        {
            for(int i = 0; i < framesCount; i++)
            {
                SImageConfig imageConfig =
                {
                    .ImageSize = framebufferSize,
                    .MipLevels = 1,
                    .ArrayLevels = 1,
                    .Type = EImageType::TYPE_2D,
                    .Format = EFormat::R8G8B8A8_UNORM,
                    .Tiling = EImageTiling::OPTIMAL,
                    .InitialLayout = EImageLayout::UNDEFINED,
                    .Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT
                            | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT
                            | EImageUsageType::USAGE_SAMPLED_BIT,
                    .SharingMode = ESharingMode::EXCLUSIVE,
                    .SamplesCount = ESamplesCount::COUNT_1,
                    .Flags = EImageCreateFlags::NONE,
                    .ViewFormat = EFormat::R8G8B8A8_UNORM,
                    .ViewType = EImageViewType::TYPE_2D,
                    .ViewAspect = EImageAspect::COLOR_BIT,
                };

                ImagePtr visRt = mDeviceInternal->CreateImage(imageConfig);

                mVisBufferImages.push_back(visRt);

                SFramebufferConfig framebufferConfig =
                {
                    .Attachments = { visRt->RawImageView(), mDepthStencilRenderTarget->RawImageView()},
                    .Size = {framebufferSize.width, framebufferSize.height},
                    .RenderPass = mRenderPass
                };

                FramebufferPtr framebuffer = mDeviceInternal->CreateFramebuffer(framebufferConfig);
                mFramebuffers.push_back(framebuffer);
            }
        }
    }
}