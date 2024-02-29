#include "Renderer.h"

#include "RHI/Enums/RenderPassFormats.h"

#include "RHI/Configs/SwapchainConfig.h"
#include "RHI/Configs/RenderPassConfig.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkSwapchain.h"
#include "RHI/Vulkan/CVkImage.h"
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


        CommandPoolConfig cmdPoolConfig =
        {
            .QueueFamilyIndex = 1, //get from somewhere
            .QueueType = QueueType::GRAHPICS
        };

        mCommandPool = mDevice->CreateCommandPool(cmdPoolConfig);

        CommandBufferConfig cmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mCommandBuffers = mDevice->CreateCommandBuffers(mCommandPool, cmdBuffersConfig);

        PrepareDirDepth();
        PrepareOffscreenRenderpass();


        //init other things bellow
        __debugbreak();

        //init shadow system
        //Shadows::Instance()->Init();

        //init mesh systems
        //OpaqueInstances::GetInstance()->Init(m_RenderPass, m_ShadowRenderPass, m_SwapChainExtent);
        //ModelLoader::Instance()->Init(m_CommandPool);

        //InitImGui(hWnd);

        isInit = true;
    }

    void Renderer::CreateDepthImage()
    {
        mDepthStencilFormat = EImageFormat::D32_SFLOAT_S8_UINT;

        /*if(mDepthRenderTargetTexture == nullptr)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyImageViews(vk::Device, { m_DepthImageView });
            vk::FreeMemory(vk::Device, m_DepthImageMemory);
            vk::DestroyImage(vk::Device, m_DepthImage);
        }*/

        auto desiredFormats = { EImageFormat::D32_SFLOAT, EImageFormat::D32_SFLOAT_S8_UINT, EImageFormat::D24_UNORM_S8_UINT };

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

        mRenderTarget = mDevice->CreateImage(imageConfig);
    }

    void Renderer::PrepareDirDepth()
    {
      
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

    void Renderer::Render()
    {
        if(!isInit)
        {
            return;
        }

        vkWaitForFences(vk::Device, 1, &m_FlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        Shadows::Instance()->Update();

        //data.ViewProjectionMatrix = m_DirViewProjMatrix;

        vk::Vk::GetInstance()->UpdatePerFrameBuffer(data);
        OpaqueInstances::GetInstance()->UpdateShadowDescriptors(m_DirShadowBuffer);

        //basic
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vk::Device, m_SwapChain, UINT64_MAX,
                                                m_ImageAvailableSemaphores[m_CurrentFrame],
                                                nullptr, &imageIndex);
        VK_CHECK_RESULT(result);

        vkResetFences(vk::Device, 1, &m_FlightFences[m_CurrentFrame]);

        result = vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        VK_CHECK_RESULT(result);

        vk::BeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //offscreen rendering pass
        VkClearValue depthClearColor{};
        depthClearColor.depthStencil = { 1.0f, 0 };
        vk::BeginRenderPass(m_ShadowRenderPass, m_ShadowFramebuffers[imageIndex], { 0,0 },
                            { m_DirDepthSize.width, m_DirDepthSize.height }, &depthClearColor, 1, m_CommandBuffers[m_CurrentFrame],
                            VK_SUBPASS_CONTENTS_INLINE);

        vk::SetViewPortAndScissors(m_CommandBuffers[m_CurrentFrame],
                                 0.0f, 0.0f, static_cast<float>(m_DirDepthSize.width),
                                 static_cast<float>(m_DirDepthSize.height), 0.0f, 1.0f,
                                 { 0, 0 }, { m_DirDepthSize.width, m_DirDepthSize.height });

        //related to specific pipeline
        OpaqueInstances::GetInstance()->RenderDepth2D(m_CommandBuffers[m_CurrentFrame], depthBias, depthSlope);

        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //default pass
        std::array<VkClearValue, 2> clearColor{};
        clearColor[0].color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].depthStencil = { 1.0f, 0 };
        
        vk::ImageLayoutTransition(vk::Device,
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
                                  1);

        vk::BeginRenderPass(m_RenderPass, m_Framebuffers[imageIndex],
                            { 0, 0 }, m_SwapChainExtent, clearColor.data(), clearColor.size(),
                            m_CommandBuffers[m_CurrentFrame], VK_SUBPASS_CONTENTS_INLINE);

        vk::SetViewPortAndScissors(m_CommandBuffers[m_CurrentFrame],
                                   0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width),
                                   static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f,
                                   { 0, 0 }, m_SwapChainExtent);

        //render default
        OpaqueInstances::GetInstance()->UpdateDescriptorSets(m_DirDepthImageView[0], m_DirShadowBuffer);
        OpaqueInstances::GetInstance()->Render(m_CommandBuffers[m_CurrentFrame]);

        //render IMGui
        vkimgui::PrepareNewFrame();

        {
            ImGui::Begin("Data");
            //ImGui::Text("This is some text.");

            ImGui::SliderFloat("range", &range, -1000.0f, 1000.0f);
            ImGui::SliderFloat("nearPlane", &nearPlane, -1000.0f, 1000.0f);
            ImGui::SliderFloat("farPlane", &farPlane, -1000.0f, 1000.0f);

            ImGui::SliderFloat3("position", &position[0], -10, 10);
            ImGui::SliderFloat3("lookAt", &lookAt[0], -10, 10);
            ImGui::SliderFloat3("up", &up[0], -10, 10);

            ImGui::SliderFloat("Bias", &depthBias, -10, 10);
            ImGui::SliderFloat("Slope", &depthSlope, -10, 10);

            ImGui::End();
        }

        vkimgui::Render(m_CommandBuffers[m_CurrentFrame]);

        //continue
        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

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

        vk::EndCommandBuffer(m_CommandBuffers[m_CurrentFrame]);

        vk::Submit(vk::Queues.GraphicsQueue, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   &m_ImageAvailableSemaphores[m_CurrentFrame], 1, &m_CommandBuffers[m_CurrentFrame], 1,
                   &m_RenderFinishedSemaphores[m_CurrentFrame], 1, m_FlightFences[m_CurrentFrame]);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        result = vk::Present(vk::Queues.PresentQueue, &m_RenderFinishedSemaphores[m_CurrentFrame], 1,
                    &m_SwapChain, 1, &imageIndex);
        VK_CHECK_RESULT(result);

        m_CurrentFrame = (m_CurrentFrame + 1) % m_SwapChainImages.size();
        vkDeviceWaitIdle(vk::Device);
    }

    void Renderer::LoadTextureIntoMemory(const RawTextureData& textureData,
                                         uint32_t mipLevels,
                                         Texture* texture)
    {
        if(texture == nullptr)
        {
            LOG_ERROR("Texture pointer is null");
        }

        if(textureData.Data == nullptr)
        {
            LOG_ERROR("Raw texture data pointer is null");
        }

        vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                               { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, mipLevels, 1,
                               VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                               VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                               &texture->Image, &texture->ImageMemory, &texture->ImageView);

        vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                                         textureData.Width * textureData.Height * textureData.Type,
                                         textureData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                                         { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
                                         mipLevels,
                                         VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &texture->Image);
    }

    void Renderer::ResizeWindow(HWND hWnd)
    {
        return;

        if(vk::Device == VK_NULL_HANDLE)
            return;

        m_Hwnd = hWnd;
        CreateSwapchain(hWnd);
        CreateFramebuffers();
        CreateMsaaImage();
        CreateDepthImage();
        InitImGui(hWnd);
    }

    void Renderer::CreateSwapchain(HWND hWnd)
    {
        if(m_SwapChain != VK_NULL_HANDLE)
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
                             VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &m_SwapchainImageViews);
    }

    void Renderer::CreateFramebuffers()
    {
        if(m_Framebuffers.size() != 0)
        {
            vkDeviceWaitIdle(vk::Device);
            vk::DestroyFramebuffers(vk::Device, m_Framebuffers);
        }

        uint32_t frameBufferAttachmentCount = m_SwapChainImages.size();
        std::vector<vk::FramebufferAttachment> frameBufferAttachment(frameBufferAttachmentCount);
        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            frameBufferAttachment[i].Attachments = { m_MsaaImageView, m_DepthImageView,
                                                      m_SwapchainImageViews[i] };
        };

        vk::CreateFramebuffers(vk::Device, frameBufferAttachment, 3,
                               m_SwapChainExtent, m_SwapchainImageViews.size(),
                               m_RenderPass, &m_Framebuffers);
    }

    void Renderer::InitImGui(HWND hWnd)
    {
        vkimgui::Init(hWnd, m_SwapChainImages.size(), m_RenderPass,
                      vk::Queues.GraphicsQueue, vk::Queues.GraphicsFamily.value(),
                      m_CommandPool, m_CommandBuffers[0], vk::MaxMsaaSamples, &m_ImGuiDescriptorsPool);
    }

    void Renderer::PrepareOffscreenRenderpass()
    {
        //render pass creation
        constexpr uint32_t attachmentsDescriptionCount = 1;
        VkAttachmentDescription attachmentsDescriptions[attachmentsDescriptionCount] =
        {
            {
                 0,                                         //flags
                 m_DepthFormat,                    //attachment format
                 VK_SAMPLE_COUNT_1_BIT,                        //samples number
                 VK_ATTACHMENT_LOAD_OP_CLEAR,               //load op
                 VK_ATTACHMENT_STORE_OP_STORE,              //store op
                 VK_ATTACHMENT_LOAD_OP_DONT_CARE,           //stencil load op
                 VK_ATTACHMENT_STORE_OP_DONT_CARE,          //stencilStoreOp
                 VK_IMAGE_LAYOUT_UNDEFINED,                 //initialLayout
                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL   //final layout
            },
        };

        VkAttachmentReference depthReference =
        {
            0,                                                //attachment
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  //layout
        };

        constexpr uint32_t subpassDescrCount = 1;
        VkSubpassDescription subpassDescr[subpassDescrCount] =
        {
             0,                               //flags
             VK_PIPELINE_BIND_POINT_GRAPHICS, //pipeline bind point
             0,                               //input attachment count
             nullptr,                         //pInput attachments
             0,         //ColorAttachments Count
             nullptr,       //pColorAttachments
             nullptr,             //pResolveAttachments
             &depthReference,                 //pDepthStencilAttachment
             0,                               //preserveAttachmentCount
             nullptr                          //pPreserveAttachments
        };

        constexpr uint32_t dependenciesCount = 2;
        VkSubpassDependency dependency[dependenciesCount] =
        {
            {
                VK_SUBPASS_EXTERNAL,                                                                        //src subpass
                0,                                                                                          //dst subpass
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, //src stage mask
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, //dst stage mask
                VK_ACCESS_SHADER_READ_BIT,                                                                                          //src access mask
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,        //dst access mask
                VK_DEPENDENCY_BY_REGION_BIT, //dependency flags
            },
            {
                0,                                                                        //src subpass
                VK_SUBPASS_EXTERNAL,                                                                                          //dst subpass
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, //src stage mask
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, //dst stage mask
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,                                                                                          //src access mask
                VK_ACCESS_SHADER_READ_BIT,        //dst access mask
                VK_DEPENDENCY_BY_REGION_BIT,   //dependency flags
            },
        };

        vk::CreateRenderPass(vk::Device, attachmentsDescriptions, attachmentsDescriptionCount,
                             subpassDescr, subpassDescrCount, dependency, dependenciesCount, &m_ShadowRenderPass);

        //frame buffers
        uint32_t frameBufferAttachmentCount = m_SwapChainImages.size();
        std::vector<vk::FramebufferAttachment> frameBufferAttachment(frameBufferAttachmentCount);
        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            frameBufferAttachment[i].Attachments = { m_DirDepthImageView[i] };
        };

        vk::CreateFramebuffers(vk::Device, frameBufferAttachment, 1,
                               { m_DirDepthSize.width, m_DirDepthSize.height }, 
                               m_SwapchainImageViews.size(),
                               m_ShadowRenderPass, 
                               &m_ShadowFramebuffers);
    }
}