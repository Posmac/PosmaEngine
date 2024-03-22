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
#include "RHI/Vulkan/CVkRenderPass.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

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
        SSwapchainConfig swapchainConfig =
        {
            device->GetSurface()
        };

        mSwapchain = mDevice->CreateSwapchain(swapchainConfig);

        //swapchain creation
        //CreateSwapchain(hWnd);
        int swapchainImages = mSwapchain->GetImagesCount();
        mImageAvailableSemaphores.resize(swapchainImages);
        mRenderFinishedSemaphores.resize(swapchainImages);
        mFlightFences.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            mFlightFences[i] = mDevice->CreateFence({false});
            mImageAvailableSemaphores[i] = mDevice->CreateSemaphore({false});
            mRenderFinishedSemaphores[i] = mDevice->CreateSemaphore({false});
        }

        //create it before init render pass
        CreateDepthImage();
        CreateMsaaImage();

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
            .FinalLayout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
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

        SAttachmentDescription resolveDescription =
        {
            .Flags = EAttachmentDescriptionFlags::NONE,
            .Format = mSwapchain->GetSwapchainImageFormat(),
            .Samples = ESamplesCount::COUNT_1,
            .LoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
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

        SAttachmentReference depthAttachmentReference =
        {
            .Attachment = 1,
            .Layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        SAttachmentReference resolveAttachmentReference =
        {
            .Attachment = 2,
            .Layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL
        };

        SSubpassDescription subpassDescription =
        {
            .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
            .InputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .ColorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentReference,
            .pResolveAttachments = &resolveAttachmentReference,
            .pDepthStencilAttachment = &depthAttachmentReference,
            .PreserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr
        };

        SSubpassDependency subpassDependency =
        {
            .SrcSubpass = VK_SUBPASS_EXTERNAL,
            .DstSubpass = 0,
            .SrcStageMask = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT | EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
            .DstStageMask = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT | EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
            .SrcAccessMask = EAccessFlags::NONE,
            .DstAccessMask = EAccessFlags::COLOR_ATTACHMENT_WRITE_BIT | EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .DependencyFlags = EDependencyFlags::NONE
        };
        
        SRenderPassConfig renderPassConfig =
        {
            .ColorAttachements = {colorDescription},
            .DepthAttachment = depthStencilDescription,
            .ResolveAttachment = resolveDescription,
            
            .ColorAttachmentReference = colorAttachmentReference,
            .DepthStencilAttachmentReference = depthAttachmentReference,
            .ResolveAttachemntReference = resolveAttachmentReference,

            .SubpassDescriptions = {subpassDescription},
            .SubpassDependensies = {subpassDependency}
        };

        mRenderPass = mDevice->CreateRenderPass(renderPassConfig);

        CreateFramebuffers();

        //command buffers


        SCommandPoolConfig cmdPoolConfig =
        {
            .QueueFamilyIndex = 0, //get from somewhere
            .QueueType = EQueueType::GRAHPICS
        };

        mCommandPool = mDevice->CreateCommandPool(cmdPoolConfig);

        SCommandBufferConfig cmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mCommandBuffers = mDevice->CreateCommandBuffers(mCommandPool, cmdBuffersConfig);

        SBufferConfig bufferConfig =
        {
            .Size = sizeof(GlobalBuffer),
            .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mGlobalBuffer = mDevice->CreateBuffer(bufferConfig);

        PrepareDirDepth();
        PrepareOffscreenRenderpass();

        //init other things bellow
        //__debugbreak();

        //init shadow system
        Shadows::Instance()->Init(mDevice, mSwapchain->GetImagesCount());

        //init mesh systems
        SResourceExtent3D swapchainSize = mSwapchain->GetSwapchainSize();
        OpaqueInstances::GetInstance()->Init(mDevice, mRenderPass, mShadowRenderPass, { swapchainSize.width, swapchainSize.height });
        ModelLoader::Instance()->Init(mDevice, mCommandPool);

        //InitImGui(hWnd);

        isInit = true;
    }

    void Renderer::CreateDepthImage()
    {
        //mDepthStencilFormat = EFormat::D32_SFLOAT_S8_UINT;

        /*if(mDepthRenderTargetTexture == nullptr)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyImageViews(vk::Device, { m_DepthImageView });
            vk::FreeMemory(vk::Device, m_DepthImageMemory);
            vk::DestroyImage(vk::Device, m_DepthImage);
        }*/

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
            .Usage = EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .SharingMode = ESharingMode::EXCLUSIVE,
            .SamplesCount = ESamplesCount::COUNT_1, 
            .Flags = EImageCreateFlags::NONE,
            .ViewFormat = mDepthStencilFormat,
            .ViewType = EImageViewType::TYPE_2D,
            .ViewAspect = EImageAspect::DEPTH_BIT
        };

        mDepthRenderTargetTexture = mDevice->CreateImage(imageConfig);
    }

    void Renderer::CreateMsaaImage()
    {
        /*if(m_MsaaImage != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyImageViews(vk::Device, { m_MsaaImageView });
            vk::FreeMemory(vk::Device, m_MsaaImageMemory);
            vk::DestroyImage(vk::Device, m_MsaaImage);
        }*/

        SImageConfig imageConfig =
        {
            .ImageSize = mSwapchain->GetSwapchainSize(),
            .MipLevels = 1,
            .ArrayLevels = 1,
            .Type = EImageType::TYPE_2D,
            .Format = mSwapchain->GetSwapchainImageFormat(),
            .Tiling = EImageTiling::OPTIMAL,
            .InitialLayout = EImageLayout::UNDEFINED,
            .Usage = EImageUsageType::USAGE_TRANSIENT_ATTACHMENT_BIT | EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT,
            .SharingMode = ESharingMode::EXCLUSIVE,
            .SamplesCount = ESamplesCount::COUNT_1, //for now supporto only 1 msaa
            .Flags = EImageCreateFlags::NONE,
            .ViewFormat = mSwapchain->GetSwapchainImageFormat(),
            .ViewType = EImageViewType::TYPE_2D,
            .ViewAspect = EImageAspect::COLOR_BIT
        };

        mMSAARenderTarget = mDevice->CreateImage(imageConfig);
    }

    void Renderer::PrepareDirDepth()
    {
        mShadowMapSize = { 2048, 2048 };

        SImageConfig imageConfig =
        {
            .ImageSize = { (uint32_t)mShadowMapSize.width, (uint32_t)mShadowMapSize.height, 1 },
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

        mDirectionalDepthImages.resize(mSwapchain->GetImagesCount());

        for(auto& image : mDirectionalDepthImages)
        {
            image = mDevice->CreateImage(imageConfig);
        }
    }

    void Renderer::Deinit()
    {
        /*vkimgui::Deinit();

        vk::DestroyFramebuffers(vk::Device, m_Framebuffers);
        vk::DestroyRenderPass(vk::Device, m_RenderPass);
        vk::DestroyImageViews(vk::Device, m_SwapchainImageViews);
        vk::DestroySwapchain(vk::Device, m_SwapChain);

        for(int i = 0; i < m_SwapChainImages.size(); i++)
        {
            vk::DestroySemaphore(vk::Device, m_ImageAvailableSemaphores[i]);
            vk::DestroySemaphore(vk::Device, m_RenderFinishedSemaphores[i]);
            vk::DestroyFence(vk::Device, m_FlightFences[i]);
        }

        vk::Vk::GetInstance()->Deinit();*/
    }

    void Renderer::Render(GlobalBuffer& buffer)
    {
        if(!isInit)
        {
            return;
        }


        SFenceWaitConfig waitConfig =
        {
            .WaitAll = true,
            .Timeout = FLT_MAX,
        };

        mFlightFences[mCurrentFrame]->Wait(waitConfig);
        //vkWaitForFences(vk::Device, 1, &m_FlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        Shadows::Instance()->Update();

        //vk::Vk::GetInstance()->UpdatePerFrameBuffer(data);
        BufferPtr& shadowBuffer = Shadows::Instance()->GetGPUBuffer();
        OpaqueInstances::GetInstance()->UpdateShadowDescriptors(shadowBuffer);

        //basic
        uint32_t imageIndex;
        SSwapchainAquireNextImageConfig nextImageConfig
        {
            .Timeout = UINT64_MAX,
            .Semaphore = mImageAvailableSemaphores[mCurrentFrame]
        };

        mSwapchain->GetNextImage(nextImageConfig, &imageIndex);

        /*VkResult result = vkAcquireNextImageKHR(vk::Device, m_SwapChain, UINT64_MAX,
                                                m_ImageAvailableSemaphores[m_CurrentFrame],
                                                nullptr, &imageIndex);
        VK_CHECK_RESULT(result);*/

        mFlightFences[mCurrentFrame]->Reset();

        //vkResetFences(vk::Device, 1, &m_FlightFences[m_CurrentFrame]);

        mCommandBuffers[mCurrentFrame]->Reset();

       /* result = vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        VK_CHECK_RESULT(result);*/
        
        SCommandBufferBeginConfig commandBufferBegin =
        {
            .BufferIndex = mCurrentFrame,
            .Usage = ECommandBufferUsage::NONE,
        };

        mCommandBuffers[mCurrentFrame]->Begin(commandBufferBegin);
        //vk::BeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //offscreen rendering pass

        UClearValue depthClearValue =
        {
            .DepthStencil = {1.0f, 0}
        };

        SRenderPassBeginConfig renderPassBeginConfig =
        {
            .RenderPass = mShadowRenderPass,
            .Framebuffer = mDirectionalDepthFramebuffers[imageIndex],
            .CommandBuffer = mCommandBuffers[mCurrentFrame],
            .Offset = {0, 0},
            .Extent = mShadowMapSize,
            .ClearValuesCount = 1,
            .pClearValues = &depthClearValue,
            .SubpassContents = ESubpassContents::INLINE
        };

        mRenderPass->BeginRenderPass(renderPassBeginConfig);
        /*vk::BeginRenderPass(m_ShadowRenderPass, m_ShadowFramebuffers[imageIndex], { 0,0 },
                            { m_DirDepthSize.width, m_DirDepthSize.height }, &depthClearColor, 1, m_CommandBuffers[m_CurrentFrame],
                            VK_SUBPASS_CONTENTS_INLINE);*/

        mRenderPass->SetViewport(mCommandBuffers[mCurrentFrame], 0.0f, 0.0f, static_cast<float>(mShadowMapSize.width), static_cast<float>(mShadowMapSize.height), 0.0f, 1.0f);
        mRenderPass->SetScissors(mCommandBuffers[mCurrentFrame], { 0, 0 }, mShadowMapSize);

       /* vk::SetViewPortAndScissors(m_CommandBuffers[m_CurrentFrame],
                                 0.0f, 0.0f, static_cast<float>(m_DirDepthSize.width),
                                 static_cast<float>(m_DirDepthSize.height), 0.0f, 1.0f,
                                 );*/

        //related to specific pipeline
        OpaqueInstances::GetInstance()->RenderDepth2D(mCommandBuffers[mCurrentFrame], 0.0f, 0.0f);

        mShadowRenderPass->EndRenderPass(mCommandBuffers[mCurrentFrame]);
        //vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //default pass
        std::array<UClearValue, 2> clearColor{};
        clearColor[0].Color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].DepthStencil = { 1.0f, 0 };

        SImageBarrierConfig imageBarrierConfig =
        {
            .CommandBuffer = mCommandBuffers[mCurrentFrame],
            .Image = mDirectionalDepthImages[mCurrentFrame],
            .OldLayout = EImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
            .NewLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
            .SrcAccessMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .DstAccessMask = EAccessFlags::SHADER_READ_BIT,
            .SourceStage = EPipelineStageFlags:: LATE_FRAGMENT_TESTS_BIT,
            .DestinationStage = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
            .BaseMipLevel = 0,
            .LevelCount = 1,
            .BaseArrayLayer = 0,
            .LayerCount = 1,
            .AspectMask = EImageAspect::DEPTH_BIT
        };

        mDevice->InsertImageMemoryBarrier(imageBarrierConfig);
     /*   vk::ImageLayoutTransition(vk::Device,
                                  m_CommandBuffers[m_CurrentFrame],
                                  m_DirDepthImage[m_CurrentFrame],
                                  m_DirDepthFormat,
                                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                  VK_ACCESS_SHADER_READ_BIT,
                                  VK_IMAGE_ASPECT_DEPTH_BIT,
                                  1);*/

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

        /*vk::BeginRenderPass(m_RenderPass, m_Framebuffers[imageIndex],
                            { 0, 0 }, m_SwapChainExtent, clearColor.data(), clearColor.size(),
                            m_CommandBuffers[m_CurrentFrame], VK_SUBPASS_CONTENTS_INLINE);*/
        mRenderPass->SetViewport(mCommandBuffers[mCurrentFrame], 0.0f, 0.0f, static_cast<float>(mSwapchain->GetSwapchainSize().width),
                                   static_cast<float>(mSwapchain->GetSwapchainSize().width), 0.0f, 1.0f);
        mRenderPass->SetScissors(mCommandBuffers[mCurrentFrame], { 0, 0 }, { mSwapchain->GetSwapchainSize().width, mSwapchain->GetSwapchainSize().height } );

        /*vk::SetViewPortAndScissors(m_CommandBuffers[m_CurrentFrame],
                                   0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width),
                                   static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f,
                                   );*/

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

        //render default
        //auto& shadowMap = Shadows::Instance()->GetDirMap();
        auto& matrixBuffer = Shadows::Instance()->GetGPUBuffer();
        OpaqueInstances::GetInstance()->UpdateDescriptorSets(mDirectionalDepthImages[mCurrentFrame], matrixBuffer, mGlobalBuffer);
        OpaqueInstances::GetInstance()->Render(mCommandBuffers[mCurrentFrame]);
        
        //render IMGui
        //vkimgui::PrepareNewFrame();

        //{
        //    ImGui::Begin("Data");
        //    //ImGui::Text("This is some text.");

        //    ImGui::SliderFloat("range", &range, -1000.0f, 1000.0f);
        //    ImGui::SliderFloat("nearPlane", &nearPlane, -1000.0f, 1000.0f);
        //    ImGui::SliderFloat("farPlane", &farPlane, -1000.0f, 1000.0f);

        //    ImGui::SliderFloat3("position", &position[0], -10, 10);
        //    ImGui::SliderFloat3("lookAt", &lookAt[0], -10, 10);
        //    ImGui::SliderFloat3("up", &up[0], -10, 10);

        //    ImGui::SliderFloat("Bias", &depthBias, -10, 10);
        //    ImGui::SliderFloat("Slope", &depthSlope, -10, 10);

        //    ImGui::End();
        //}

        //vkimgui::Render(m_CommandBuffers[m_CurrentFrame]);

        //continue
        mRenderPass->EndRenderPass(mCommandBuffers[mCurrentFrame]);
        //vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

        /*vk::ImageLayoutTransition(vk::Device, m_CommandBuffers[m_CurrentFrame],
                                  m_DirDepthImage[m_CurrentFrame], 
                                  m_DirDepthFormat,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                  VK_ACCESS_SHADER_READ_BIT,
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                  VK_IMAGE_ASPECT_DEPTH_BIT,
                                  1);*/

        mCommandBuffers[mCurrentFrame]->End();

        //vk::EndCommandBuffer(m_CommandBuffers[m_CurrentFrame]);
        
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

       /* vk::Submit(vk::Queues.GraphicsQueue, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   &m_ImageAvailableSemaphores[m_CurrentFrame], 1, &m_CommandBuffers[m_CurrentFrame], 1,
                   &m_RenderFinishedSemaphores[m_CurrentFrame], 1, m_FlightFences[m_CurrentFrame]);*/

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SPresentConfig presentConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
            .WaitSemaphoresCount = 1,
            .pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame],
            .ImageIndex = imageIndex
        };

        mDevice->Present(presentConfig);

        mCurrentFrame = (mCurrentFrame + 1) % mSwapchain->GetImagesCount();

        mDevice->WaitIdle();
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

        /*vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                               { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, mipLevels, 1,
                               VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                               VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                               &texture->Image, &texture->ImageMemory, &texture->ImageView);*/


        //vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
        //                                 , m_CommandPool, vk::Queues.GraphicsQueue,
        //                                 { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
        //                                 mipLevels,
        //                                 VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
        //                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &texture->Image);
    }

    void Renderer::ResizeWindow(HWND hWnd)
    {
        return;

        if(mDevice == nullptr)
            return;

        mWindow = hWnd;
        CreateSwapchain(hWnd);
        CreateFramebuffers();
        CreateMsaaImage();
        CreateDepthImage();
        InitImGui(hWnd);
    }

    void Renderer::CreateSwapchain(HWND hWnd) //DELETE
    {
       /* if(m_SwapChain != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyImageViews(vk::Device, m_SwapchainImageViews);
            vk::DestroySwapchain(vk::Device, m_SwapChain);

            vkimgui::Deinit();
        }

        vk::CreateSwapchain(vk::Device, vk::PhysicalDevice, vk::Surface, vk::SurfData,
                            &m_SwapChain, &m_SwapChainImageFormat,
                            &m_SwapChainExtent);
        vk::QuerrySwapchainImages(vk::Device, m_SwapChain, m_SwapChainImageFormat,
                                  &m_SwapChainImages, &m_SwapchainImageViews);
        vk::CreateImageViews(vk::Device, m_SwapChainImages, m_SwapChainImageFormat,
                             VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &m_SwapchainImageViews);*/
    }

    void Renderer::CreateFramebuffers()
    {
        /*if(m_Framebuffers.size() != 0)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyFramebuffers(vk::Device, m_Framebuffers);
        }*/

        uint32_t frameBufferAttachmentCount = mSwapchain->GetImagesCount();
        auto swapchainSize = mSwapchain->GetSwapchainSize();
        mFramebuffers.resize(frameBufferAttachmentCount);

        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            SFramebufferConfig framebufferConfig =
            {
                .Attachments = { mMSAARenderTarget, mDepthRenderTargetTexture },
                .SwapchainImage = mSwapchain->ImageAtIndex(i),
                .Size = { swapchainSize.width, swapchainSize.height },
                .RenderPass = mRenderPass
            };

            mFramebuffers[i] = mDevice->CreateFramebuffer(framebufferConfig);
        };

       /* vk::CreateFramebuffers(vk::Device, frameBufferAttachment, 3,
                               m_SwapChainExtent, m_SwapchainImageViews.size(),
                               m_RenderPass, &m_Framebuffers);*/
    }

    void Renderer::InitImGui(HWND hWnd)
    {
        /*vkimgui::Init(hWnd, m_SwapChainImages.size(), m_RenderPass,
                      vk::Queues.GraphicsQueue, vk::Queues.GraphicsFamily.value(),
                      m_CommandPool, m_CommandBuffers[0], vk::MaxMsaaSamples, &m_ImGuiDescriptorsPool);*/
    }

    void Renderer::PrepareOffscreenRenderpass()
    {
        SAttachmentDescription attachmentDescription =
        {
            .Flags = EAttachmentDescriptionFlags::NONE,
            .Format = mDepthStencilFormat,
            .Samples = ESamplesCount::COUNT_1,
            .LoadOperation = EAttachmentLoadOp::LOAD_OP_CLEAR,
            .StoreOperation = EAttachmentStoreOp::STORE_OP_STORE,
            .StencilLoadOperation = EAttachmentLoadOp::LOAD_OP_DONT_CARE,
            .StencilStoreOperation = EAttachmentStoreOp::STORE_OP_DONT_CARE,
            .InitialLayout = EImageLayout::UNDEFINED,
            .FinalLayout = EImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL
        };

        SAttachmentReference depthReference =
        {
            .Attachment = 0,
            .Layout = EImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL
        };

        SSubpassDescription subpassDescription =
        {
            .PipelineBindPoint = EPipelineBindPoint::GRAPHICS,
            .InputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .ColorAttachmentCount = 0,
            .pColorAttachments = nullptr,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = &depthReference,
            .PreserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr
        };
        
        constexpr uint32_t dependenciesCount = 2;
        std::vector<SSubpassDependency> dependencies;
        dependencies.resize(dependenciesCount);

        dependencies[0] =
        {
            .SrcSubpass = VK_SUBPASS_EXTERNAL,
            .DstSubpass = 0,
            .SrcStageMask = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
            .DstStageMask = EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT,
            .SrcAccessMask = EAccessFlags::SHADER_READ_BIT,
            .DstAccessMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
        };

        dependencies[1] =
        {
            .SrcSubpass = 0,
            .DstSubpass = VK_SUBPASS_EXTERNAL,
            .SrcStageMask = EPipelineStageFlags::LATE_FRAGMENT_TESTS_BIT,
            .DstStageMask = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
            .SrcAccessMask = EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .DstAccessMask = EAccessFlags::SHADER_READ_BIT,
            .DependencyFlags = EDependencyFlags::BY_REGION_BIT,
        };

        SRenderPassConfig renderPassConfig =
        {
            .ColorAttachements = {},
            .DepthAttachment = attachmentDescription,
            .ResolveAttachment = {},

            .ColorAttachmentReference = {},
            .DepthStencilAttachmentReference = depthReference,
            .ResolveAttachemntReference = {},

            .SubpassDescriptions = {subpassDescription},
            .SubpassDependensies = dependencies
        };

        mShadowRenderPass = mDevice->CreateRenderPass(renderPassConfig);

        //vk::CreateRenderPass(vk::Device, attachmentsDescriptions, attachmentsDescriptionCount,
        //                     subpassDescr, subpassDescrCount, dependency, dependenciesCount, &m_ShadowRenderPass);

        //frame buffers
        uint32_t frameBufferAttachmentCount = mSwapchain->GetImagesCount();
        mDirectionalDepthFramebuffers.resize(frameBufferAttachmentCount);

        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            SFramebufferConfig framebufferConfig =
            {
                .Attachments = { mDirectionalDepthImages[i] },
                .Size = mShadowMapSize,
                .RenderPass = mShadowRenderPass,
            };

            mDirectionalDepthFramebuffers[i] = mDevice->CreateFramebuffer(framebufferConfig);
        };

       /* vk::CreateFramebuffers(vk::Device, frameBufferAttachment, 1,
                               { m_DirDepthSize.width, m_DirDepthSize.height }, 
                               m_SwapchainImageViews.size(),
                               m_ShadowRenderPass, 
                               &m_ShadowFramebuffers);*/
    }
}