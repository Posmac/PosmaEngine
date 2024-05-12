#include "CVkSwapchain.h"

#include "CVkDevice.h"
#include "CVkSemaphore.h"
#include "RHI/RHICommon.h"
#include "CVkSurface.h"
#include "RHI/Enums/ImageFormats.h"

namespace psm
{
    CVkSwapchain::CVkSwapchain(const DevicePtr& device, const SSwapchainConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        if(mDeviceInternal == nullptr)
        {
            LogMessage(psm::MessageSeverity::Fatal, "Device is nullptr");
        }

        std::shared_ptr<CVkSurface> vkSurface = std::static_pointer_cast<CVkSurface>(config.Surface);
        vkSurface->PopulateSurfaceData(device->GetDeviceData(), config.Surface);

        CVkSurface::SurfaceData data = vkSurface->GetSurfaceData();

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.flags = 0;
        swapchainCreateInfo.surface = reinterpret_cast<VkSurfaceKHR>(vkSurface->Raw());
        swapchainCreateInfo.minImageCount = 
            (data.Capabilities.minImageCount + 1 <= data.Capabilities.maxImageCount) ?
            data.Capabilities.minImageCount + 1 : data.Capabilities.maxImageCount;

        VkFormat imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkPresentModeKHR presetMode = VK_PRESENT_MODE_MAILBOX_KHR;

        CheckFormatSupport(imageFormat, data.Formats);
        CheckColorSpaceSupport(colorSpace, data.Formats);
        CheckPresentModeSupport(presetMode, data.PresentModes);

        swapchainCreateInfo.imageFormat = imageFormat;
        swapchainCreateInfo.imageColorSpace = colorSpace;
        swapchainCreateInfo.imageExtent = data.Capabilities.currentExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presetMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = nullptr;

        VkResult result = vkCreateSwapchainKHR(mDeviceInternal, &swapchainCreateInfo, nullptr, &mSwapChain);

        if(result != VK_SUCCESS)
        {
            LogMessage(psm::MessageSeverity::Error, "Failed to create swapchain");
        }

        mSwapChainImageFormat = imageFormat;
        mSwapChainExtent = data.Capabilities.currentExtent;

        LogMessage(psm::MessageSeverity::Info, "Created swapchain: width = " + std::to_string(mSwapChainExtent.width) 
                   + ", height: " + std::to_string(mSwapChainExtent.height));

        QuerrySwapchainImages();
    }

    CVkSwapchain::~CVkSwapchain()
    {
        DestroySwapchain(mDeviceInternal, mSwapChain);
    }

    void CVkSwapchain::Resize(uint32_t width, uint32_t height)
    {

    }

    void CVkSwapchain::GetNextImage(const SSwapchainAquireNextImageConfig& config, uint32_t* index)
    {
        VkResult result = vkAcquireNextImageKHR(mDeviceInternal, mSwapChain, config.Timeout, 
                                                reinterpret_cast<VkSemaphore>(config.Semaphore->Raw()),
                                                nullptr, index);

        VK_CHECK_RESULT(result);
    }

    void* CVkSwapchain::ImageAtIndex(uint32_t index)
    {
        return mSwapchainImageViews[index];
    }

    void CVkSwapchain::Present(const SPresentConfig& config)
    {
        std::vector<VkSemaphore> waitSemaphores(config.WaitSemaphoresCount);
        for(int i = 0; i < config.WaitSemaphoresCount; i++)
        {
            waitSemaphores[i] = reinterpret_cast<VkSemaphore>(config.pWaitSemaphores[i]->Raw());
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = waitSemaphores.size();
        presentInfo.pWaitSemaphores = waitSemaphores.data();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mSwapChain;
        presentInfo.pImageIndices = &config.ImageIndex;

        VkResult result = vkQueuePresentKHR(reinterpret_cast<VkQueue>(config.Queue), &presentInfo);
        VK_CHECK_RESULT(result);
    }

    void CVkSwapchain::SetVsyncMode(bool enabled)
    {

    }

    uint32_t CVkSwapchain::GetImagesCount()
    {
        return mSwapChainImages.size();
    }

    EFormat CVkSwapchain::GetSwapchainImageFormat()
    {
        return FromVulkan(mSwapChainImageFormat);
    }

    SResourceExtent3D CVkSwapchain::GetSwapchainSize()
    {
        return { mSwapChainExtent.width, mSwapChainExtent.height, 1 };
    }

    void* CVkSwapchain::Raw()
    {
        return mSwapChain;
    }

    void* CVkSwapchain::Raw() const
    {
        return mSwapChain;
    }

    void CVkSwapchain::CheckFormatSupport(VkFormat& format, const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for(auto& formatProperty : formats)
        {
            if(formatProperty.format == format)
            {
                break;
            }
        }

        LogMessage(psm::MessageSeverity::Warning, "Desired image format isn`t supported. Assigned VK_FORMAT_R8G8B8A8_UNORM format for image");
        format = VK_FORMAT_R8G8B8A8_UNORM;
    }

    void CVkSwapchain::CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace, const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for(auto& formatProperty : formats)
        {
            if(formatProperty.colorSpace == colorSpace)
            {
                break;
            }
        }

        LogMessage(psm::MessageSeverity::Warning, "Desired color mode isn`t supported. Assigned VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color mode");
        colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }

    void CVkSwapchain::CheckPresentModeSupport(VkPresentModeKHR& presentMode, const std::vector<VkPresentModeKHR>& modes)
    {
        for(auto& availablePresentMode : modes)
        {
            if(availablePresentMode == presentMode)
            {
                break;
            }
        }

        LogMessage(psm::MessageSeverity::Warning, "Desired present mode isn`t supported. Assigned VK_PRESENT_MODE_IMMEDIATE_KHR present mode");
        presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    void CVkSwapchain::QuerrySwapchainImages()
    {
        uint32_t imagesCount;
        vkGetSwapchainImagesKHR(mDeviceInternal, mSwapChain, &imagesCount, nullptr);
        mSwapChainImages.resize(imagesCount);
        mSwapchainImageViews.resize(imagesCount);
        vkGetSwapchainImagesKHR(mDeviceInternal, mSwapChain, &imagesCount, mSwapChainImages.data());

        for(int i = 0; i < mSwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.image = mSwapChainImages[i];
            createInfo.format = mSwapChainImageFormat;
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(mDeviceInternal, &createInfo, nullptr, &mSwapchainImageViews[i]);
            VK_CHECK_RESULT(result);
        }

        LogMessage(psm::MessageSeverity::Warning, "Total swapchain images retrived is: " +  std::to_string(mSwapChainImages.size()));
    }

    void CVkSwapchain::DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain)
    {
        for(auto& imageView : mSwapchainImageViews)
            vkDestroyImageView(mDeviceInternal, imageView, nullptr);

        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
}