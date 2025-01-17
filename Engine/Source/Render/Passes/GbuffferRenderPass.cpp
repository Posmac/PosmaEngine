#include "GbuffferRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

namespace psm
{
    namespace graph
    {
        GbuffferRenderPassNode::GbuffferRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            SResourceExtent3D framebufferSize,
                                            uint32_t framesCount)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = framebufferSize;
            mTargets.reserve(framesCount);
            mTargetsCount = 7; //we have 7 render targets described in GbufferTargets
            CreateDepthStencilRenderTarget(framebufferSize);
            CreateRenderPass(framesCount); //mTarget`s formats filled there
            CreateFramebuffers(framebufferSize, framesCount); //mTarget`s images and framebuffers created there
        }

        GbuffferRenderPassNode::~GbuffferRenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "GbuffferRenderPassNode destructor");

            mDepthStencilRenderTarget.Image = nullptr;

            for(auto& target : mTargets)
            {
                target.Framebuffer = nullptr;
                target.AlbedoTarget.Image = nullptr;
                target.DepthTarget.Image = nullptr;
                target.EmissionTarget.Image = nullptr;
                target.NormalTarget.Image = nullptr;
                target.RoughnessMetalnessTarget.Image = nullptr;
                target.SpecularGlosinessTarget.Image = nullptr;
                target.WorldPositionsTarget.Image = nullptr;
            }

            mTargets.clear();
        }

        void GbuffferRenderPassNode::PreRender(CommandBufferPtr& commandBuffer, uint32_t index)
        {
            std::vector<UClearValue> clearValues;
            clearValues.resize(mTargetsCount + 1); //+1 because we have depth stencil target

            for(int i = 0; i < mTargetsCount; i++)
            {
                clearValues[i].Color = { 0,0,0, 1 };
            }

            clearValues[mTargetsCount].DepthStencil = {0.0f, 0};

            SRenderPassBeginConfig shadowMapRenderPassBeginConfig =
            {
                .RenderPass = mRenderPass,
                .Framebuffer = mTargets[index].Framebuffer,
                .CommandBuffer = commandBuffer,
                .Offset = {0, 0},
                .Extent = {mFramebuffersSize.width, mFramebuffersSize.height},
                .ClearValuesCount = static_cast<uint32_t>(clearValues.size()),
                .pClearValues = clearValues.data(),
                .SubpassContents = ESubpassContents::INLINE
            };

            mRenderPass->BeginRenderPass(shadowMapRenderPassBeginConfig);

            mDeviceInternal->SetViewport(commandBuffer, 0, 0, static_cast<float>(mFramebuffersSize.width), static_cast<float>(mFramebuffersSize.height), 0.0f, 1.0f);
            mDeviceInternal->SetScissors(commandBuffer, { 0,0 }, { mFramebuffersSize.width, mFramebuffersSize.height });
        }

        void GbuffferRenderPassNode::PostRender(CommandBufferPtr& commandBuffer)
        {
            mRenderPass->EndRenderPass(commandBuffer);
        }

        void GbuffferRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {

        }

        void GbuffferRenderPassNode::CollectReferences(uint32_t framesCount)
        {
            mResourceMediator->RegisterImageResource(GBUFFER_DEPTHSTECIL_RENDERTARGET_NAME, mDepthStencilRenderTarget.Image);

            for(int i = 0; i < mTargets.size(); i++)
            {
                foundation::Name albedoRefName = GetResourceIndexedName(GBUFFER_ALBEDO_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(albedoRefName, mTargets[i].AlbedoTarget.Image);

                foundation::Name normalRefName = GetResourceIndexedName(GBUFFER_NORMAL_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(normalRefName, mTargets[i].NormalTarget.Image);

                foundation::Name depthRefName = GetResourceIndexedName(GBUFFER_DEPTH_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(depthRefName, mTargets[i].DepthTarget.Image);
            
                foundation::Name worldPosRefName = GetResourceIndexedName(GBUFFER_WORLDPOS_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(worldPosRefName, mTargets[i].WorldPositionsTarget.Image);

                foundation::Name emissionRefName = GetResourceIndexedName(GBUFFER_EMISSION_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(emissionRefName, mTargets[i].EmissionTarget.Image);

                foundation::Name specularGlossRefName = GetResourceIndexedName(GBUFFER_SPECULAR_GLOSS_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(specularGlossRefName, mTargets[i].SpecularGlosinessTarget.Image);

                foundation::Name roughMetallRefName = GetResourceIndexedName(GBUFFER_ROUGH_METAL_RENDERTARGET_NAME, i);
                mResourceMediator->RegisterImageResource(roughMetallRefName, mTargets[i].RoughnessMetalnessTarget.Image);
            }
        }

        void GbuffferRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {
            uint32_t framebuffersCount = mTargets.size();

            mDepthStencilRenderTarget.Image = nullptr;

            for(auto& target : mTargets)
            {
                target.AlbedoTarget.Image = nullptr;
                target.NormalTarget.Image = nullptr;
                target.DepthTarget.Image = nullptr;
                target.WorldPositionsTarget.Image = nullptr;
                target.EmissionTarget.Image = nullptr;
                target.RoughnessMetalnessTarget.Image = nullptr;
                target.SpecularGlosinessTarget.Image = nullptr;

                target.Framebuffer = nullptr;
            }

            mTargets.clear();

            CreateDepthStencilRenderTarget(swapchain->GetSwapchainSize());
            CreateFramebuffers(swapchain->GetSwapchainSize(), framebuffersCount);

            mResourceMediator->UpdateImageReference(GBUFFER_DEPTHSTECIL_RENDERTARGET_NAME, mDepthStencilRenderTarget.Image);

            for(int i = 0; i < mTargets.size(); i++)
            {
                mResourceMediator->UpdateImageReference(GBUFFER_ALBEDO_RENDERTARGET_NAME, mTargets[i].AlbedoTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_NORMAL_RENDERTARGET_NAME, mTargets[i].NormalTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_DEPTH_RENDERTARGET_NAME, mTargets[i].DepthTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_WORLDPOS_RENDERTARGET_NAME, mTargets[i].WorldPositionsTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_EMISSION_RENDERTARGET_NAME, mTargets[i].EmissionTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_SPECULAR_GLOSS_RENDERTARGET_NAME, mTargets[i].SpecularGlosinessTarget.Image);
                mResourceMediator->UpdateImageReference(GBUFFER_ROUGH_METAL_RENDERTARGET_NAME, mTargets[i].RoughnessMetalnessTarget.Image);
            }
        }

        void GbuffferRenderPassNode::CreateRenderPass(uint32_t framesCount)
        {
            EFormat albedoFormat = EFormat::R8G8B8A8_SNORM;

            SAttachmentDescription albedoDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = albedoFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference albedoReference =
            {
                .Attachment = 0,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat normalFormat = EFormat::R16G16B16A16_SFLOAT;

            SAttachmentDescription normalDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = normalFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference normalReference =
            {
                .Attachment = 1,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat depthAttachmentFormat = EFormat::R32_SFLOAT;

            SAttachmentDescription depthAttachmentDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = depthAttachmentFormat,
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
                .Attachment = 2,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat worldPositionsFormat = EFormat::R32G32B32A32_SFLOAT;

            SAttachmentDescription worldPosDescr =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = worldPositionsFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference worldPosReference =
            {
                .Attachment = 3,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat emissionTargetFormat = EFormat::R8G8B8A8_SNORM;

            SAttachmentDescription emissionDesc =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = emissionTargetFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference emissionReference =
            {
                .Attachment = 4,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat specularGlosinessFormat = EFormat::R8G8B8A8_SNORM;

            SAttachmentDescription specularGlosinessDesc =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = specularGlosinessFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference specularGlossReference =
            {
                .Attachment = 5,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            EFormat roughnessMetallnessFormat = EFormat::R8G8B8A8_SNORM;

            SAttachmentDescription roughnessMetallnessDesc =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = roughnessMetallnessFormat,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference roughnessMetalnessReference =
            {
                .Attachment = 6,
                .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
            };

            SAttachmentDescription depthBackbufferDescription =
            {
                .Flags = EAttachmentDescriptionFlags::NONE,
                .Format = mDepthStencilRenderTarget.Format,
                .Samples = ESamplesCount::COUNT_1,
                .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
                .StoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
                .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
                .InitialLayout = EImageLayout::UNDEFINED,
                .FinalLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL
            };

            SAttachmentReference depthBackbufferReference =
            {
                .Attachment = 7,
                .Layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            std::vector<SAttachmentReference> colorDescriptionsRefs =
            {
                albedoReference,
                normalReference,
                depthAttachmentReference,
                worldPosReference,
                emissionReference,
                specularGlossReference,
                roughnessMetalnessReference
            };

            SSubpassDescription subpassDescr =
            {
                .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
                .InputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .ColorAttachmentCount = static_cast<uint32_t>(colorDescriptionsRefs.size()),
                .pColorAttachments = colorDescriptionsRefs.data(),
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = &depthBackbufferReference,
                .PreserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr
            };

            std::vector<SSubpassDependency> subpassDependency =
            {
                {
                    .SrcSubpass = VK_SUBPASS_EXTERNAL,
                    .DstSubpass = 0,
                    .SrcStageMask = EPipelineStageFlags::BOTTOM_OF_PIPE_BIT,
                    .DstStageMask = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT,
                    .SrcAccessMask = EAccessFlags::NONE,
                    .DstAccessMask = EAccessFlags::COLOR_ATTACHMENT_WRITE_BIT,
                    .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
                },
            };

            SRenderPassConfig renderPassConfig =
            {
                .ColorAttachements = {albedoDescription, normalDescription, depthAttachmentDescription, worldPosDescr, emissionDesc, specularGlosinessDesc, roughnessMetallnessDesc},
                .DepthAttachment = depthBackbufferDescription,
                .ResolveAttachment = {},

                //.ColorAttachmentReference = colorDescriptionsRefs,
                //.DepthStencilAttachmentReference = depthReference,
                //.ResolveAttachemntReference = {},

                .SubpassDescriptions = {subpassDescr},
                .SubpassDependensies = subpassDependency
            };

            mRenderPass = mDeviceInternal->CreateRenderPass(renderPassConfig);

            GbufferTargetInfo albedoInfo;
            albedoInfo.Format = albedoFormat;

            GbufferTargetInfo normalInfo;
            normalInfo.Format = normalFormat;

            GbufferTargetInfo depthInfo;
            depthInfo.Format = depthAttachmentFormat;

            GbufferTargetInfo worldPosInfo;
            worldPosInfo.Format = worldPositionsFormat;

            GbufferTargetInfo emissionInfo;
            emissionInfo.Format = emissionTargetFormat;

            GbufferTargetInfo specularGlosinessInfo;
            specularGlosinessInfo.Format = specularGlosinessFormat;

            GbufferTargetInfo roughnessMetallnessInfo;
            roughnessMetallnessInfo.Format = roughnessMetallnessFormat;

            GbufferTargets targets =
            {
                .AlbedoTarget = albedoInfo,
                .NormalTarget = normalInfo,
                .DepthTarget = depthInfo,
                .WorldPositionsTarget = worldPosInfo,
                .EmissionTarget = emissionInfo,
                .SpecularGlosinessTarget = specularGlosinessInfo,
                .RoughnessMetalnessTarget = roughnessMetallnessInfo,
            };

            for(int i = 0; i < framesCount; i++)
            {
                mTargets.push_back(targets);
            }
        }

        void GbuffferRenderPassNode::CreateDepthStencilRenderTarget(SResourceExtent3D size)
        {
            mDepthStencilRenderTarget.Format = EFormat::D32_SFLOAT;

            SImageConfig imageConfig =
            {
                .ImageSize = size,
                .MipLevels = 1,
                .ArrayLevels = 1,
                .Type = EImageType::TYPE_2D,
                .Format = mDepthStencilRenderTarget.Format,
                .Tiling = EImageTiling::OPTIMAL,
                .InitialLayout = EImageLayout::UNDEFINED,
                .Usage = EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
                .SharingMode = ESharingMode::EXCLUSIVE,
                .SamplesCount = ESamplesCount::COUNT_1,
                .Flags = EImageCreateFlags::NONE,
                .ViewFormat = mDepthStencilRenderTarget.Format,
                .ViewType = EImageViewType::TYPE_2D,
                .ViewAspect = EImageAspect::DEPTH_BIT
            };

            mDepthStencilRenderTarget.Image = mDeviceInternal->CreateImage(imageConfig);

            SCommandPoolConfig cmdPoolConfig =
            {
                .QueueFamilyIndex = mDeviceInternal->GetDeviceData().vkData.GraphicsQueueIndex,
                .QueueType = EQueueType::GRAHPICS
            };

            auto cmdPool = mDeviceInternal->CreateCommandPool(cmdPoolConfig);

            CommandBufferPtr commandBuffer = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(cmdPool);

            SImageLayoutTransition imageLayoutTransition =
            {
                .Format = mDepthStencilRenderTarget.Format,
                .OldLayout = EImageLayout::UNDEFINED,
                .NewLayout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,//EImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                .DestinationStage = EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
                .SourceMask = EAccessFlags::NONE,
                .DestinationMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_READ_BIT | EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .ImageAspectFlags = EImageAspect::DEPTH_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, mDepthStencilRenderTarget.Image, imageLayoutTransition);

            commandBuffer->End();

            mDeviceInternal->SubmitSingleTimeCommandBuffer(cmdPool, commandBuffer);
        }

        void GbuffferRenderPassNode::CreateFramebuffers(SResourceExtent3D framebufferSize, uint32_t framesCount)
        {
            for(int i = 0; i < mTargets.size(); i++)
            {
                SImageConfig imageConfig =
                {
                    .ImageSize = framebufferSize,
                    .MipLevels = 1,
                    .ArrayLevels = 1,
                    .Type = EImageType::TYPE_2D,
                    //.Format
                    .Tiling = EImageTiling::OPTIMAL,
                    .InitialLayout = EImageLayout::UNDEFINED,
                    //.Usage = 
                    .SharingMode = ESharingMode::EXCLUSIVE,
                    .SamplesCount = ESamplesCount::COUNT_1,
                    .Flags = EImageCreateFlags::NONE,
                    //.ViewFormat = 
                    .ViewType = EImageViewType::TYPE_2D,
                    //.ViewAspect 
                };

                GbufferTargets& targets = mTargets[i];

                SImageConfig albedoImageConfig = imageConfig;
                albedoImageConfig.Format = targets.AlbedoTarget.Format;
                albedoImageConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                albedoImageConfig.ViewFormat = targets.AlbedoTarget.Format;
                albedoImageConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.AlbedoTarget.Image = mDeviceInternal->CreateImage(albedoImageConfig);

                SImageConfig normalImageConfig = imageConfig;
                normalImageConfig.Format = targets.NormalTarget.Format;
                normalImageConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                normalImageConfig.ViewFormat = targets.NormalTarget.Format;
                normalImageConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.NormalTarget.Image = mDeviceInternal->CreateImage(normalImageConfig);

                SImageConfig depthImageConfig = imageConfig;
                depthImageConfig.Format = targets.DepthTarget.Format;
                depthImageConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                depthImageConfig.ViewFormat = targets.DepthTarget.Format;
                depthImageConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.DepthTarget.Image = mDeviceInternal->CreateImage(depthImageConfig);

                SImageConfig worldPosImageConfig = imageConfig;
                worldPosImageConfig.Format = targets.WorldPositionsTarget.Format;
                worldPosImageConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                worldPosImageConfig.ViewFormat = targets.WorldPositionsTarget.Format;
                worldPosImageConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.WorldPositionsTarget.Image = mDeviceInternal->CreateImage(worldPosImageConfig);

                SImageConfig emissionConfig = imageConfig;
                emissionConfig.Format = targets.EmissionTarget.Format;
                emissionConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                emissionConfig.ViewFormat = targets.EmissionTarget.Format;
                emissionConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.EmissionTarget.Image = mDeviceInternal->CreateImage(emissionConfig);

                SImageConfig specularGlosConfig = imageConfig;
                specularGlosConfig.Format = targets.SpecularGlosinessTarget.Format;
                specularGlosConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                specularGlosConfig.ViewFormat = targets.SpecularGlosinessTarget.Format;
                specularGlosConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.SpecularGlosinessTarget.Image = mDeviceInternal->CreateImage(specularGlosConfig);

                SImageConfig roughnessMetallicConfig = imageConfig;
                roughnessMetallicConfig.Format = targets.RoughnessMetalnessTarget.Format;
                roughnessMetallicConfig.Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT;
                roughnessMetallicConfig.ViewFormat = targets.RoughnessMetalnessTarget.Format;
                roughnessMetallicConfig.ViewAspect = EImageAspect::COLOR_BIT;

                targets.RoughnessMetalnessTarget.Image = mDeviceInternal->CreateImage(roughnessMetallicConfig);

                SFramebufferConfig framebufferConfig =
                {
                    .Attachments = {targets.AlbedoTarget.Image->RawImageView(),
                                    targets.NormalTarget.Image->RawImageView(),
                                    targets.DepthTarget.Image->RawImageView(),
                                    targets.WorldPositionsTarget.Image->RawImageView(),
                                    targets.EmissionTarget.Image->RawImageView(),
                                    targets.SpecularGlosinessTarget.Image->RawImageView(),
                                    targets.RoughnessMetalnessTarget.Image->RawImageView(),
                                    mDepthStencilRenderTarget.Image->RawImageView() },
                    .Size = {framebufferSize.width, framebufferSize.height},
                    .RenderPass = mRenderPass
                };

                targets.Framebuffer = mDeviceInternal->CreateFramebuffer(framebufferConfig);
            }
        }
    }
}