#include "Renderer.h"

#include <iostream>
#include <fstream>

namespace psm
{
    Renderer* Renderer::s_Instance = nullptr;

    Renderer* Renderer::Instance()
    {
        if (s_Instance == nullptr)
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

        //render pass creation
        constexpr uint32_t colorAttachmentDescriptionCount = 1;
        VkAttachmentDescription colorAttachmentDescriptions[] =
        {
            0,                                //flags
            m_SwapChainImageFormat,           //attachment format
            VK_SAMPLE_COUNT_1_BIT,            //samples number
            VK_ATTACHMENT_LOAD_OP_CLEAR,      //load op
            VK_ATTACHMENT_STORE_OP_STORE,     //store op
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  //stencil load op
            VK_ATTACHMENT_STORE_OP_DONT_CARE, //stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,        //initialLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   //final layout
        };

        constexpr uint32_t colorAttachmentRefCount = 1;
        VkAttachmentReference colorAttachmentReferences[colorAttachmentRefCount] =
        {
            0,                                        //attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, //layout
        };

        constexpr uint32_t subpassDescrCount = 1;
        VkSubpassDescription subpassDescr[subpassDescrCount] =
        {
             0,                               // flags
             VK_PIPELINE_BIND_POINT_GRAPHICS, //pipeline bind point
             0,                               // input attachment count
             nullptr,                         //pInput attachments
             colorAttachmentRefCount,         // ColorAttachments Count
             colorAttachmentReferences,       //pColorAttachments
             nullptr,                         //pResolveAttachments
             nullptr,                         //pDepthStencilAttachment
             0,                               //preserveAttachmentCount
             nullptr                          //pPreserveAttachments
        };

        vk::CreateRenderPass(vk::Device, colorAttachmentDescriptions, colorAttachmentDescriptionCount,
            subpassDescr, subpassDescrCount, &m_RenderPass);

        vk::CreateFramebuffers(vk::Device, m_SwapchainImageViews, m_SwapChainExtent, m_SwapchainImageViews.size(),
            m_RenderPass, &m_Framebuffers);

        //command buffers
        vk::CreateCommandPool(vk::Device, vk::Queues.GraphicsFamily.value(), &m_CommandPool);
        vk::CreateCommandBuffers(vk::Device, m_CommandPool, m_SwapChainImages.size(), &m_CommandBuffers);

        {
            //image creation
            putils::RawTextureData textureData{ putils::RGB_Type::Rgb_alpha };
            putils::LoadRawTextureData("Textures/texture.jpg", &textureData);

            vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, 1, 1,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                &image, &imageMemory, &imageView);

            vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                textureData.Width * textureData.Height * textureData.Type,
                textureData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &image);

            putils::CleanRawTextureData(textureData.Data);

            //load another texture
            putils::RawTextureData cobbleData{ putils::RGB_Type::Rgb_alpha };
            putils::LoadRawTextureData("Textures/Cobblestone/Cobblestone_albedo.png", &cobbleData);

            vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                { (uint32_t)cobbleData.Width, (uint32_t)cobbleData.Height, 1 }, 1, 1,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                &cobbleImage, &cobbleImageMemory, &cobbleImageView);

            vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                cobbleData.Width * cobbleData.Height * cobbleData.Type,
                cobbleData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                { (uint32_t)cobbleData.Width, (uint32_t)cobbleData.Height, 1 },
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &cobbleImage);

            putils::CleanRawTextureData(cobbleData.Data);
        }

        putils::LoadModel("../Engine/Models/Skull/Skull.obj", &model);
        model.Init(m_CommandPool);

        OpaqueInstances::Instance()->Init(&model, m_RenderPass, m_SwapChainExtent);
        OpaqueInstances::Instance()->AddInstance(glm::mat4(1.0));

        OpaqueInstances::Instance()->UpdateDescriptorSets(imageView, cobbleImageView);
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

    void Renderer::Render(float deltaTime)
    {
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
            {0, 0}, m_SwapChainExtent, clearColor.data(), clearColor.size(), 
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
}