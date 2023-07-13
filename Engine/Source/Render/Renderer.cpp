#include "Renderer.h"

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

    void Renderer::Init(HINSTANCE hInstance, HWND hWnd)
    {
        //m_VkImgui.Init(hWnd, m_SwapChainImages.size(), *this, &m_ImGuiDescriptorsPool);

        //later move into windows class
        vk::CreateSwapchain(vk::Device, vk::PhysicalDevice, vk::Surface, vk::SurfData, &m_SwapChain, &m_SwapChainImageFormat,
                            &m_SwapChainExtent);

        vk::QuerrySwapchainImages(vk::Device, m_SwapChain, m_SwapChainImageFormat, &m_SwapChainImages, &m_SwapchainImageViews);
        vk::CreateVkSemaphore(vk::Device, 0, &m_ImageAvailableSemaphore);
        vk::CreateVkSemaphore(vk::Device, 0, &m_RenderFinishedSemaphore);
        vk::CreateImageViews(vk::Device, m_SwapChainImages, m_SwapChainImageFormat,
                             VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &m_SwapchainImageViews);

        //create it before init render pass
        CreateDepthImage();
        CreateMsaaImage();

        //render pass creation
        constexpr uint32_t attachmentsDescriptionCount = 3;
        VkAttachmentDescription attachmentsDescriptions[attachmentsDescriptionCount] =
        {
            {
                 0,                                         //flags
                 m_SwapChainImageFormat,                    //attachment format
                 vk::MaxMsaaSamples,                        //samples number
                 VK_ATTACHMENT_LOAD_OP_CLEAR,               //load op
                 VK_ATTACHMENT_STORE_OP_STORE,              //store op
                 VK_ATTACHMENT_LOAD_OP_DONT_CARE,           //stencil load op
                 VK_ATTACHMENT_STORE_OP_DONT_CARE,          //stencilStoreOp
                 VK_IMAGE_LAYOUT_UNDEFINED,                 //initialLayout
                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL   //final layout
            },
            {
                 0,                                               //flags
                 m_DepthFormat,                                   //attachment format
                 vk::MaxMsaaSamples,                              //samples number
                 VK_ATTACHMENT_LOAD_OP_CLEAR,                     //load op
                 VK_ATTACHMENT_STORE_OP_DONT_CARE,                //store op
                 VK_ATTACHMENT_LOAD_OP_DONT_CARE,                 //stencil load op
                 VK_ATTACHMENT_STORE_OP_DONT_CARE,                //stencilStoreOp
                 VK_IMAGE_LAYOUT_UNDEFINED,                       //initialLayout
                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL //final layout
            },
            {
                0,
                m_SwapChainImageFormat,
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            },
        };

        constexpr uint32_t colorAttachmentRefCount = 1;
        VkAttachmentReference colorAttachmentReferences[colorAttachmentRefCount] =
        {
            0,                                        //attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, //layout
        };

        VkAttachmentReference depthReference =
        {
            1,                                                //attachment
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  //layout
        };

        VkAttachmentReference resolveAttachments =
        {
            2,                                         //attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL   //layout
        };

        constexpr uint32_t subpassDescrCount = 1;
        VkSubpassDescription subpassDescr[subpassDescrCount] =
        {
             0,                               //flags
             VK_PIPELINE_BIND_POINT_GRAPHICS, //pipeline bind point
             0,                               //input attachment count
             nullptr,                         //pInput attachments
             colorAttachmentRefCount,         //ColorAttachments Count
             colorAttachmentReferences,       //pColorAttachments
             &resolveAttachments,          //pResolveAttachments
             &depthReference,                 //pDepthStencilAttachment
             0,                               //preserveAttachmentCount
             nullptr                          //pPreserveAttachments
        };

        constexpr uint32_t dependenciesCount = 1;
        VkSubpassDependency dependency[dependenciesCount] =
        {
            VK_SUBPASS_EXTERNAL,                                                                        //src subpass
            0,                                                                                          //dst subpass
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, //src stage mask
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, //dst stage mask
            0,                                                                                          //src access mask
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,        //dst access mask
            0,                                                                                          //dependency flags
        };

        vk::CreateRenderPass(vk::Device, attachmentsDescriptions, attachmentsDescriptionCount,
                             subpassDescr, subpassDescrCount, dependency, dependenciesCount, &m_RenderPass);

        uint32_t frameBufferAttachmentCount = m_SwapChainImages.size();
        std::vector<vk::FramebufferAttachment> frameBufferAttachment(frameBufferAttachmentCount);
        for(int i = 0; i < frameBufferAttachmentCount; i++)
        {
            frameBufferAttachment[i].Attachments = { m_MsaaImageView, m_DepthImageView,
                                                      m_SwapchainImageViews[i] };
        };

        vk::CreateFramebuffers(vk::Device, frameBufferAttachment, 3,
                               m_SwapChainExtent, m_SwapchainImageViews.size(), m_RenderPass, &m_Framebuffers);

        //command buffers
        vk::CreateCommandPool(vk::Device, vk::Queues.GraphicsFamily.value(), &m_CommandPool);
        vk::CreateCommandBuffers(vk::Device, m_CommandPool, m_SwapChainImages.size(), &m_CommandBuffers);

        //init mesh systems
        OpaqueInstances::Instance()->Init(m_RenderPass, m_SwapChainExtent);
        ModelLoader::Instance()->Init(m_CommandPool);
    }

    void Renderer::CreateDepthImage()
    {
        m_DepthFormat = FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        vk::CreateImageAndView(vk::Device, vk::PhysicalDevice, { m_SwapChainExtent.width, m_SwapChainExtent.height, 1 },
                               1, 1, VK_IMAGE_TYPE_2D, m_DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, vk::MaxMsaaSamples,
                               0, m_DepthFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT,
                               &m_DepthImage, &m_DepthImageMemory, &m_DepthImageView);
    }

    void Renderer::CreateMsaaImage()
    {
        vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                               { m_SwapChainExtent.width, m_SwapChainExtent.height, 1 },
                               1, 1, VK_IMAGE_TYPE_2D, m_SwapChainImageFormat, VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                               VK_SHARING_MODE_EXCLUSIVE, vk::MaxMsaaSamples, 0, m_SwapChainImageFormat,
                               VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                               &m_MsaaImage, &m_MsaaImageMemory, &m_MsaaImageView);
        VkImage m_MsaaImage;
        VkDeviceMemory m_MsaaImageMemory;
        VkImageView m_MsaaImageView;
    }

    VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                           VkImageTiling tiling,
                                           VkFormatFeatureFlags features)
    {
        for(auto& format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vk::PhysicalDevice, format, &props);

            if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        LOG_CRITICAL("Unsupported format");
        assert(true);
    }

    void Renderer::Deinit()
    {
        //m_VkImgui.Deinit();

        vk::DestroyFramebuffers(vk::Device, m_Framebuffers);
        vk::DestroyRenderPass(vk::Device, m_RenderPass);
        vk::DestroyImageViews(vk::Device, m_SwapchainImageViews);
        vk::DestroySwapchain(vk::Device, m_SwapChain);
        vk::DestroySemaphore(vk::Device, m_ImageAvailableSemaphore);
        vk::DestroySemaphore(vk::Device, m_RenderFinishedSemaphore);

        vk::Vk::GetInstance()->Deinit();
    }

    void Renderer::Render(const PerFrameData& data)
    {
        vk::Vk::GetInstance()->UpdatePerFrameBuffer(data);
        OpaqueInstances::Instance()->UpdateDescriptorSets();

        //basic
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vk::Device, m_SwapChain, UINT64_MAX,
                                                m_ImageAvailableSemaphore, nullptr, &imageIndex);
        VK_CHECK_RESULT(result);

        result = vkResetCommandBuffer(m_CommandBuffers[imageIndex], 0);
        VK_CHECK_RESULT(result);

        vk::BeginCommandBuffer(m_CommandBuffers[imageIndex], 0);

        std::array<VkClearValue, 2> clearColor{};
        clearColor[0].color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].depthStencil = { 1.0f, 0 };

        vk::BeginRenderPass(m_RenderPass, m_Framebuffers[imageIndex],
                            { 0, 0 }, m_SwapChainExtent, clearColor.data(), clearColor.size(),
                            m_CommandBuffers[imageIndex], VK_SUBPASS_CONTENTS_INLINE);

        vk::SetViewPortAndScissors(m_CommandBuffers[imageIndex],
                                   0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width),
                                   static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f,
                                   { 0, 0 }, m_SwapChainExtent);

        //related to specific pipeline
        OpaqueInstances::Instance()->Render(m_CommandBuffers[imageIndex]);

        //render IMGui
        //m_VkImgui.Render(m_CommandBuffers[imageIndex]);

        //continue
        vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

        vk::EndCommandBuffer(m_CommandBuffers[imageIndex]);

        vk::Submit(vk::Queues.GraphicsQueue, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   &m_ImageAvailableSemaphore, 1, &m_CommandBuffers[imageIndex], 1,
                   &m_RenderFinishedSemaphore, 1, VK_NULL_HANDLE);

        vk::Present(vk::Queues.PresentQueue, &m_RenderFinishedSemaphore, 1,
                    &m_SwapChain, 1, &imageIndex);

        vkDeviceWaitIdle(vk::Device);
    }

    void Renderer::LoadTextureIntoMemory(const RawTextureData& textureData, Texture* texture)
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
                               { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, 1, 1,
                               VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                               VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                               &texture->Image, &texture->ImageMemory, &texture->ImageView);

        vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                                         textureData.Width * textureData.Height * textureData.Type,
                                         textureData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                                         { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
                                         VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &texture->Image);
    }
}