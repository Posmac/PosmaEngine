#include "Renderer.h"

#include "RHI/Enums/RenderPassFormats.h"

#include "RHI/Configs/SwapchainConfig.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "RHI/Configs/FramebuffersConfig.h"
#include "RHI/Configs/Barriers.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkSwapchain.h"
#include "RHI/Vulkan/CVkImage.h"
#include "RHI/Vulkan/CVkFence.h"
#include "RHI/Vulkan/CVkCommandBuffer.h"
#include "RHI/Vulkan/CVkCommandPool.h"
#include "RHI/Vulkan/CVkRenderPass.h"
#include "RHI/Vulkan/CVkBuffer.h"
#include "RHI/Vulkan/CVkImGui.h"
#endif

#include "imgui/imgui.h"

namespace psm
{
    Renderer* Renderer::s_Instance = nullptr;

    Renderer* Renderer::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new Renderer();
        }

        return s_Instance;
    }

    Renderer::Renderer()
    {
        isInit = false;
    }

    void Renderer::Init(DevicePtr device, const PlatformConfig& config)
    {
        mCurrentFrame = 0;

        mDevice = device;

        hInstance = static_cast<HINSTANCE>(config.win32.hInstance);

        //init general things for rendering
        CreateSwapchain(static_cast<HWND>(config.win32.hWnd));
        CreateSwapchainSyncObjects();
        CreateCommandPool();
        CreateRenderFrameCommandBuffers();
        CreateDepthImage();
        CreateDefaultRenderPass();
        CreateFramebuffers();
        CreateGlobalBuffer();

        //init shadow system
        Shadows::Instance()->Init(mDevice, mSwapchain->GetImagesCount());
        PrepareShadowMapRenderPass();

        //init mesh systems
        auto shadowParams = Shadows::Instance()->GetShadowParams();
        auto shadowMapSize = shadowParams.DirectionalShadowTextureSize;
        SResourceExtent3D swapchainSize = mSwapchain->GetSwapchainSize();

        OpaqueInstances::GetInstance()->Init(mDevice, mRenderPass, mShadowMapRenderPass, { swapchainSize.width, swapchainSize.height }, { shadowMapSize.width, shadowMapSize.height });
        ModelLoader::Instance()->Init(mDevice, mCommandPool);

        InitImGui(static_cast<HWND>(config.win32.hWnd));

        isInit = true;
    }

    void Renderer::CreateDepthImage()
    {
        auto desiredFormats = { EFormat::D32_SFLOAT, EFormat::D32_SFLOAT_S8_UINT, EFormat::D24_UNORM_S8_UINT };

        mDepthStencilFormat = mDevice->FindSupportedFormat(desiredFormats, EImageTiling::OPTIMAL, EFeatureFormat::DEPTH_STENCIL_ATTACHMENT_BIT);

        SResourceExtent3D imageSize = mSwapchain->GetSwapchainSize();

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

        mDepthRenderTargetTexture = mDevice->CreateImage(imageConfig);

        CommandBufferPtr commandBuffer = BeginSingleTimeSubmitCommandBuffer();

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

        mDevice->ImageLayoutTransition(commandBuffer, mDepthRenderTargetTexture, imageLayoutTransition);

        commandBuffer->End();
        SubmitSingleTimeCommandBuffer(commandBuffer);
    }

    void Renderer::Deinit()
    {

    }

    void Renderer::Render(GlobalBuffer& buffer)
    {
        if(mWindowResizeQueue.size() > 0)
        {
            ResizeWindowInternal(mWindowResizeQueue.front());
            mWindowResizeQueue.pop();
        }

        if(!isInit)
        {
            return;
        }

        //basic
        uint32_t imageIndex;
        SSwapchainAquireNextImageConfig nextImageConfig
        {
            .Timeout = UINT64_MAX,
            .Semaphore = mImageAvailableSemaphores[mCurrentFrame]
        };

        mSwapchain->GetNextImage(nextImageConfig, &imageIndex);

        SCommandBufferBeginConfig commandBufferBegin =
        {
            .BufferIndex = mCurrentFrame,
            .Usage = ECommandBufferUsage::NONE,
        };

        mCommandBuffers[mCurrentFrame]->Begin(commandBufferBegin);

        UClearValue shadowMapClearColor;
        shadowMapClearColor.DepthStencil = { 1.0f, 0 };

        auto shadowParams = Shadows::Instance()->GetShadowParams();
        auto shadowMapSize = shadowParams.DirectionalShadowTextureSize;

        SRenderPassBeginConfig shadowMapRenderPassBeginConfig =
        {
            .RenderPass = mShadowMapRenderPass,
            .Framebuffer = mShadowMapFramebuffers[imageIndex],
            .CommandBuffer = mCommandBuffers[mCurrentFrame],
            .Offset = {0, 0},
            .Extent = {shadowMapSize.width, shadowMapSize.height},
            .ClearValuesCount = 1,
            .pClearValues = &shadowMapClearColor,
            .SubpassContents = ESubpassContents::INLINE
        };

        mShadowMapRenderPass->BeginRenderPass(shadowMapRenderPassBeginConfig);
        mDevice->SetViewport(mCommandBuffers[mCurrentFrame], 0, 0, static_cast<float>(shadowMapSize.width), static_cast<float>(shadowMapSize.height), 0.0f, 1.0f);
        mDevice->SetScissors(mCommandBuffers[mCurrentFrame], { 0,0 }, { shadowMapSize.width, shadowMapSize.height });
        mDevice->SetDepthBias(mCommandBuffers[mCurrentFrame], shadowParams.DepthBias, 0.0f, shadowParams.DepthSlope);

        //update some matrix buffers

        Shadows::Instance()->Update();
        auto& shadowMapBuffer = Shadows::Instance()->GetGPUBuffer();
        ////
        OpaqueInstances::GetInstance()->UpdateDepthDescriptors(shadowMapBuffer);
        OpaqueInstances::GetInstance()->RenderDepth(mCommandBuffers[mCurrentFrame]);

        mShadowMapRenderPass->EndRenderPass(mCommandBuffers[mCurrentFrame]);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //default pass
        std::array<UClearValue, 2> clearColor{};
        clearColor[0].Color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].DepthStencil = { 1.0f, 0 };

        SRenderPassBeginConfig defaultRenderPassBeginConfig =
        {
            .RenderPass = mRenderPass,
            .Framebuffer = mFramebuffers[imageIndex],
            .CommandBuffer = mCommandBuffers[mCurrentFrame],
            .Offset = {0, 0},
            .Extent = {mSwapchain->GetSwapchainSize().width, mSwapchain->GetSwapchainSize().height},
            .ClearValuesCount = clearColor.size(),
            .pClearValues = clearColor.data(),
            .SubpassContents = ESubpassContents::INLINE,
        };

        mRenderPass->BeginRenderPass(defaultRenderPassBeginConfig);
        mDevice->SetViewport(mCommandBuffers[mCurrentFrame], 0.0f, 0.0f, static_cast<float>(mSwapchain->GetSwapchainSize().width), static_cast<float>(mSwapchain->GetSwapchainSize().width), 0.0f, 1.0f);
        mDevice->SetScissors(mCommandBuffers[mCurrentFrame], { 0, 0 }, { mSwapchain->GetSwapchainSize().width, mSwapchain->GetSwapchainSize().height });

        void* pData = nullptr;

        SBufferMapConfig map =
        {
            .Size = sizeof(GlobalBuffer),
            .Offset = 0,
            .pData = &pData,
            .Flags = 0
        };

        mGlobalBuffer->Map(map);
        assert(pData != nullptr);
        memcpy(pData, &buffer, sizeof(GlobalBuffer));
        mGlobalBuffer->Unmap();

        OpaqueInstances::GetInstance()->UpdateDefaultDescriptors(mGlobalBuffer, shadowMapBuffer, mDirDepthShadowMaps[mCurrentFrame]);
        OpaqueInstances::GetInstance()->Render(mCommandBuffers[mCurrentFrame]);

        mGui->PrepareNewFrame();
        {
            Shadows::Instance()->DrawShadowParams();
        }
        mGui->Render(mCommandBuffers[mCurrentFrame]);

        //continue
        mRenderPass->EndRenderPass(mCommandBuffers[mCurrentFrame]);

        mCommandBuffers[mCurrentFrame]->End();

        SSubmitConfig submitConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
            .SubmitCount = 1,
            .WaitStageFlags = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT,
            .WaitSemaphoresCount = 1,
            .pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame],
            .CommandBuffersCount = 1,
            .pCommandBuffers = &mCommandBuffers[mCurrentFrame],
            .SignalSemaphoresCount = 1,
            .pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame],
            .Fence = mFlightFences[mCurrentFrame],
        };

        mDevice->Submit(submitConfig);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SPresentConfig presentConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
            .WaitSemaphoresCount = 1,
            .pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame],
            .ImageIndex = imageIndex
        };

        mDevice->Present(presentConfig);

        SFenceWaitConfig fenceWait =
        {
            .WaitAll = true,
            .Timeout = FLT_MAX,
        };

        while(!mFlightFences[mCurrentFrame]->Wait(fenceWait))
        {
            LogMessage(MessageSeverity::Info, "Waiting for fence");
        };

        mFlightFences[mCurrentFrame]->Reset();

        mCommandBuffers[mCurrentFrame]->Reset();

        mCurrentFrame = (mCurrentFrame + 1) % mSwapchain->GetImagesCount();
        mTotalFrames++;
    }

    ImagePtr Renderer::LoadTextureIntoMemory(const RawTextureData& textureData, uint32_t mipLevels)
    {
        if(textureData.Data == nullptr)
        {
            LogMessage(MessageSeverity::Error, "Raw texture data pointer is null");
        }

        SImageConfig imageConfig =
        {
            .ImageSize = { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
            .MipLevels = static_cast<int>(mipLevels),
            .ArrayLevels = 1,
            .Type = EImageType::TYPE_2D,
            .Format = EFormat::R8G8B8A8_SRGB,
            .Tiling = EImageTiling::OPTIMAL,
            .InitialLayout = EImageLayout::UNDEFINED,
            .Usage = EImageUsageType::USAGE_TRANSFER_SRC_BIT | EImageUsageType::USAGE_TRANSFER_DST_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
            .SharingMode = ESharingMode::EXCLUSIVE,
            .SamplesCount = ESamplesCount::COUNT_1,
            .Flags = EImageCreateFlags::NONE,
            .ViewFormat = EFormat::R8G8B8A8_SRGB,
            .ViewType = EImageViewType::TYPE_2D,
            .ViewAspect = EImageAspect::COLOR_BIT
        };

        SUntypedBuffer textureBuffer(textureData.Width * textureData.Height * textureData.Type, textureData.Data);

        SImageToBufferCopyConfig layoutTransition =
        {
            .FormatBeforeTransition = EFormat::R8G8B8A8_SRGB,
            .LayoutBeforeTransition = EImageLayout::UNDEFINED,
            .FormatAfterTransition = EFormat::R8G8B8A8_SRGB,
            .LayoutAfterTransition = EImageLayout::SHADER_READ_ONLY_OPTIMAL
        };

        return mDevice->CreateImageWithData(mCommandPool, imageConfig, textureBuffer, layoutTransition);
    }

    void Renderer::PrepareShadowMapRenderPass()
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

        mShadowMapRenderPass = mDevice->CreateRenderPass(renderPassConfig);

        auto shadowParams = Shadows::Instance()->GetShadowParams();
        auto shadowMapSize = shadowParams.DirectionalShadowTextureSize;

        mShadowMapFramebuffers.resize(mSwapchain->GetImagesCount());
        mDirDepthShadowMaps.resize(mSwapchain->GetImagesCount());
        for(int i = 0; i < mShadowMapFramebuffers.size(); i++)
        {
            SImageConfig imageConfig =
            {
                .ImageSize = {shadowMapSize.width,shadowMapSize.height, 1},
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

            mDirDepthShadowMaps[i] = mDevice->CreateImage(imageConfig);

            SFramebufferConfig framebufferConfig =
            {
                .Attachments = {mDirDepthShadowMaps[i]->RawImageView()},
                .Size = {shadowMapSize.width, shadowMapSize.height},
                .RenderPass = mShadowMapRenderPass,
            };

            mShadowMapFramebuffers[i] = mDevice->CreateFramebuffer(framebufferConfig);
        }
    }

    void Renderer::ResizeWindowInternal(HWND hWnd)
    {
        if(mDevice == nullptr)
            return;

        while(!mDevice->WaitIdle())
        {
            LogMessage(MessageSeverity::Warning, "Wait idle");
        }

        mSwapchain = nullptr;
        //mImageAvailableSemaphores.clear();
        //mRenderFinishedSemaphores.clear();
        //mFlightFences.clear();

        mFramebuffers.clear();
        mDepthRenderTargetTexture = nullptr;

        mWindow = hWnd;
        psm::PlatformConfig platformConfig =
        {
            .win32 =
            {
                .hInstance = hInstance,
                .hWnd = mWindow
            }
        };

        mDevice->GetSurface() = nullptr;
        mDevice->CreateSurface(platformConfig);
        CreateSwapchain(mWindow);
        CreateDepthImage();
        CreateFramebuffers();
    }

    void Renderer::CreateRenderFrameCommandBuffers()
    {
        SCommandBufferConfig cmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mCommandBuffers = mDevice->CreateCommandBuffers(mCommandPool, cmdBuffersConfig);
    }

    void Renderer::CreateCommandPool()
    {
        SCommandPoolConfig cmdPoolConfig =
        {
            .QueueFamilyIndex = mDevice->GetDeviceData().vkData.GraphicsQueueIndex,
            .QueueType = EQueueType::GRAHPICS
        };

        mCommandPool = mDevice->CreateCommandPool(cmdPoolConfig);
    }

    void Renderer::CreateDefaultRenderPass()
    {
        SAttachmentDescription colorDescription =
        {
            .Flags = EAttachmentDescriptionFlags::NONE,
            .Format = mSwapchain->GetSwapchainImageFormat(),
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

        mRenderPass = mDevice->CreateRenderPass(renderPassConfig);
    }

    void Renderer::CreateGlobalBuffer()
    {
        SBufferConfig bufferConfig =
        {
            .Size = sizeof(GlobalBuffer),
            .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mGlobalBuffer = mDevice->CreateBuffer(bufferConfig);
    }

    CommandBufferPtr Renderer::BeginSingleTimeSubmitCommandBuffer()
    {
        SCommandBufferConfig commandBufferConfig =
        {
            .Size = 1,
            .IsBufferLevelPrimary = true
        };

        std::vector<CommandBufferPtr> commandBuffers = mDevice->CreateCommandBuffers(mCommandPool, commandBufferConfig);
        CommandBufferPtr commandBuffer = commandBuffers[0];

        SCommandBufferBeginConfig beginConfig =
        {
            .BufferIndex = 0,
            .Usage = ECommandBufferUsage::ONE_TIME_SUBMIT_BIT,
        };

        commandBuffer->Begin(beginConfig);

        return commandBuffer;
    }

    void Renderer::SubmitSingleTimeCommandBuffer(CommandBufferPtr commandBuffer)
    {
        //submits single time command buffer, waits fences to complete and resets it
        SFenceConfig fenceConfig =
        {
            .Signaled = false
        };

        FencePtr submitFence = mDevice->CreateFence(fenceConfig);

        SSubmitConfig submitConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue, //not sure if Queue should be abstracted to CVk(IQueue)
            .SubmitCount = 1,
            .WaitStageFlags = EPipelineStageFlags::NONE,
            .WaitSemaphoresCount = 0,
            .pWaitSemaphores = nullptr,
            .CommandBuffersCount = 1,
            .pCommandBuffers = &commandBuffer,
            .SignalSemaphoresCount = 0,
            .pSignalSemaphores = nullptr,
            .Fence = submitFence,
        };

        mDevice->Submit(submitConfig);

        SFenceWaitConfig waitConfig =
        {
            .WaitAll = true,
            .Timeout = static_cast<float>(100000000000)
        };

        while(!submitFence->Wait(waitConfig))
        {
            LogMessage(MessageSeverity::Warning, "Wait for fence");
        }

        mCommandPool->FreeCommandBuffers({ commandBuffer });
    }

    void Renderer::ResizeWindow(HWND hWnd)
    {
        mWindowResizeQueue.push(hWnd);
    }

    void Renderer::CreateSwapchain(HWND hWnd)
    {
        SSwapchainConfig swapchainConfig =
        {
            mDevice->GetSurface()
        };

        mSwapchain = mDevice->CreateSwapchain(swapchainConfig);
    }

    void Renderer::CreateSwapchainSyncObjects()
    {
        int swapchainImages = mSwapchain->GetImagesCount();
        mImageAvailableSemaphores.resize(swapchainImages);
        mRenderFinishedSemaphores.resize(swapchainImages);
        mFlightFences.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            mFlightFences[i] = mDevice->CreateFence({ false });
            mImageAvailableSemaphores[i] = mDevice->CreateSemaphore({ false });
            mRenderFinishedSemaphores[i] = mDevice->CreateSemaphore({ false });
        }
    }

    void Renderer::CreateFramebuffers()
    {
        uint32_t frameBufferAttachmentCount = mSwapchain->GetImagesCount();
        auto swapchainSize = mSwapchain->GetSwapchainSize();
        mFramebuffers.resize(frameBufferAttachmentCount);

        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            SFramebufferConfig framebufferConfig =
            {
                .Attachments = { mSwapchain->ImageAtIndex(i), mDepthRenderTargetTexture->RawImageView() },
                .Size = { swapchainSize.width, swapchainSize.height },
                .RenderPass = mRenderPass
            };

            mFramebuffers[i] = mDevice->CreateFramebuffer(framebufferConfig);
        };
    }

    void Renderer::InitImGui(HWND hWnd)
    {
        mGui = mDevice->CreateGui(mRenderPass, mCommandPool, mSwapchain->GetImagesCount(), ESamplesCount::COUNT_1);
    }
}