#include "ShadowMapRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

#include "Render/ShadowsGenerator.h"

namespace psm
{
    namespace graph
    {
        ShadowMapRenderPassNode::ShadowMapRenderPassNode(const foundation::Name& name,
                                                         const ResourceMediatorPtr& resourceMediator,
                                                         const DevicePtr& device,
                                                         SResourceExtent3D framebufferSize,
                                                         uint32_t framebuffersCount)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = framebufferSize;
            CreateRenderPass(device);
            CreateFramebuffers(device, framebufferSize, framebuffersCount);
            CreateBuffer();
        }

        void ShadowMapRenderPassNode::PreRender(CommandBufferPtr& commandBuffer, uint32_t index)
        {
            RenderPassNode::PreRender(commandBuffer, index);

            UClearValue shadowMapClearColor;
            shadowMapClearColor.DepthStencil = { 1.0f, 0 };

            SRenderPassBeginConfig shadowMapRenderPassBeginConfig =
            {
                .RenderPass = mRenderPass,
                .Framebuffer = mFramebuffers[index],
                .CommandBuffer = commandBuffer,
                .Offset = {0, 0},
                .Extent = {mFramebuffersSize.width, mFramebuffersSize.height},
                .ClearValuesCount = 1,
                .pClearValues = &shadowMapClearColor,
                .SubpassContents = ESubpassContents::INLINE
            };

            mRenderPass->BeginRenderPass(shadowMapRenderPassBeginConfig);

            mDeviceInternal->SetViewport(commandBuffer, 0, 0, static_cast<float>(mFramebuffersSize.width), static_cast<float>(mFramebuffersSize.height), 0.0f, 1.0f);
            mDeviceInternal->SetScissors(commandBuffer, { 0,0 }, { mFramebuffersSize.width, mFramebuffersSize.height });
            mDeviceInternal->SetDepthBias(commandBuffer, ShadowsGenerator::Instance()->DepthBias, 0.0f, ShadowsGenerator::Instance()->DepthSlope);
        }

        void ShadowMapRenderPassNode::CreateRenderPass(const DevicePtr& device)
        {
            SAttachmentDescription depthAttachmentDescr =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = EFormat::D32_SFLOAT,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            };

            SAttachmentReference depthReference =
            {
                .Attachment = 0,
                .Layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            SSubpassDescription subpassDescr =
            {
                .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
                .InputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .ColorAttachmentCount = 0,
                .pColorAttachments = nullptr,
                .pResolveAttachments = 0,
                .pDepthStencilAttachment = &depthReference,
                .PreserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr,
            };

            std::vector<SSubpassDependency> subpassDependency =
            {
                {
                    .SrcSubpass = VK_SUBPASS_EXTERNAL,
                    .DstSubpass = 0,
                    .SrcStageMask = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
                    .DstStageMask = EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
                    .SrcAccessMask = EAccessFlags::SHADER_READ_BIT,
                    .DstAccessMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
                },
                {
                    .SrcSubpass = 0,
                    .DstSubpass = VK_SUBPASS_EXTERNAL,
                    .SrcStageMask = EPipelineStageFlags::LATE_FRAGMENT_TESTS_BIT,
                    .DstStageMask = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
                    .SrcAccessMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .DstAccessMask = EAccessFlags::SHADER_READ_BIT,
                    .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
                }
            };

            SRenderPassConfig renderPassConfig =
            {
                .ColorAttachements = {},
                .DepthAttachment = depthAttachmentDescr,
                .ResolveAttachment = {},
                .ColorAttachmentReference = {},
                .DepthStencilAttachmentReference = depthReference,
                .ResolveAttachemntReference = {},
                .SubpassDescriptions = {subpassDescr},
                .SubpassDependensies = subpassDependency,
            };

            mRenderPass = device->CreateRenderPass(renderPassConfig);
        }

        void ShadowMapRenderPassNode::CreateFramebuffers(const DevicePtr& device,
                                    SResourceExtent3D framebufferSize,
                                    uint32_t framebuffersCount)
        {
            mFramebuffers.resize(framebuffersCount);
            mRenderTargets.resize(framebuffersCount);

            for(int i = 0; i < framebuffersCount; i++)
            {
                SImageConfig imageConfig =
                {
                    .ImageSize = framebufferSize,
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

                mRenderTargets[i] = device->CreateImage(imageConfig);

                SFramebufferConfig framebufferConfig =
                {
                    .Attachments = {mRenderTargets[i]->RawImageView()},
                    .Size = {framebufferSize.width, framebufferSize.height},
                    .RenderPass = mRenderPass,
                };

                mFramebuffers[i] = device->CreateFramebuffer(framebufferConfig);
            }
        }

        void ShadowMapRenderPassNode::CreateBuffer()
        {
            uint64_t bufferSize = sizeof(mShadowsBuffer);

            SBufferConfig bufferConfig =
            {
                .Size = bufferSize,
                .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
            };

            mGPUShadowBuffer = mDeviceInternal->CreateBuffer(bufferConfig);
        }

        void ShadowMapRenderPassNode::PostRender(CommandBufferPtr& commandBuffer)
        {
            mRenderPass->EndRenderPass(commandBuffer);
        }

        void ShadowMapRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {
            //nothing to add
        }

        void ShadowMapRenderPassNode::CollectReferences()
        {
            for(size_t i = 0; i < mRenderTargets.size(); i++)
            {
                foundation::Name refName = GetResourceIndexedName(SHADOWMAP_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(refName, mRenderTargets[i]);
            }
            mResourceMediator->RegisterBufferResource(SHADOW_CBUFFER, mGPUShadowBuffer);
        }
    }
}