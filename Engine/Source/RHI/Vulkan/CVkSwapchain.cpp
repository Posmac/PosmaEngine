#include "CVkSwapchain.h"

//XXX RM
//#include "RenderBackend/SwapChain.h"

#include "CVkDevice.h"
#include "RHI/RHICommon.h"
#include "CVkSurface.h"
#include "RHI/Enums/ImageFormats.h"

namespace psm
{
    CVkSwapchain::CVkSwapchain(DevicePtr device, const SSwapchainConfig& config)
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
        swapchainCreateInfo.surface = vkSurface->GetSurface();
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
    }

    CVkSwapchain::~CVkSwapchain()
    {
        DestroySwapchain(mDeviceInternal, mSwapChain);
    }

    void CVkSwapchain::Resize(uint32_t width, uint32_t height)
    {

    }

    void CVkSwapchain::GetNextImage(uint32_t* index)
    {}

    TexturePtr& CVkSwapchain::ImageAtIndex(uint32_t index)
    {
        // TODO: insert return statement here
    }

    void CVkSwapchain::Present(const SSwapchainPresentConfig& config)
    {}

    void CVkSwapchain::SetVsyncMode(bool enabled)
    {}

    uint32_t CVkSwapchain::GetImagesCount()
    {
        return mSwapChainImages.size();
    }

    EImageFormat CVkSwapchain::GetSwapchainImageFormat()
    {
        return FromVulkan(mSwapChainImageFormat);
    }

    SResourceExtent3D CVkSwapchain::GetSwapchainSize()
    {
        return { mSwapChainExtent.width, mSwapChainExtent.height, 1 };
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

    void CVkSwapchain::QuerrySwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
        VkFormat swapchainImagesFormat, std::vector<VkImage>* swapchainImages,
        std::vector<VkImageView>* swapchainImageViews)
    {
        uint32_t imagesCount;
        vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, nullptr);
        swapchainImages->resize(imagesCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, swapchainImages->data());

        LogMessage(psm::MessageSeverity::Warning, "Total swapchain images retrived is: " +  std::to_string(swapchainImages->size()));
    }

    void CVkSwapchain::DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain)
    {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
}