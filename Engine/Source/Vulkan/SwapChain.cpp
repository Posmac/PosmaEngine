#include "SwapChain.h"

namespace psm
{
    namespace vk
    {
        void CreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
            SurfaceData surfaceData, VkSwapchainKHR* swapchain,
            VkFormat* swapchainImageFormat, VkExtent2D* swapchainExtent)
        {
            VkSwapchainCreateInfoKHR swapchainCreateInfo{};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.pNext = nullptr;
            swapchainCreateInfo.flags = 0;
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.minImageCount =
                (surfaceData.Capabilities.minImageCount + 1 <= surfaceData.Capabilities.maxImageCount) ?
                surfaceData.Capabilities.minImageCount + 1 : surfaceData.Capabilities.maxImageCount;

            VkFormat imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            VkPresentModeKHR presetMode = VK_PRESENT_MODE_MAILBOX_KHR;

            CheckFormatSupport(imageFormat, surfaceData.Formats);
            CheckColorSpaceSupport(colorSpace, surfaceData.Formats);
            CheckPresentModeSupport(presetMode, surfaceData.PresentModes);

            swapchainCreateInfo.imageFormat = imageFormat;
            swapchainCreateInfo.imageColorSpace = colorSpace;
            swapchainCreateInfo.imageExtent = surfaceData.Capabilities.currentExtent;
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

            VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, swapchain);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create swapchain" << std::endl;
            }

            *swapchainImageFormat = imageFormat;
            *swapchainExtent = surfaceData.Capabilities.currentExtent;

            std::cout << "Current extent is: " << swapchainExtent->width <<
                " " << swapchainExtent->height << std::endl;
        }

        void CheckFormatSupport(VkFormat& format, const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (auto& formatProperty : formats)
            {
                if (formatProperty.format == format)
                {
                    break;
                }
            }

            std::cout << "Desired image format isn`t supported. Assigned VK_FORMAT_R8G8B8A8_UNORM format for image" << std::endl;
            format = VK_FORMAT_R8G8B8A8_UNORM;
        }

        void CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace, const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (auto& formatProperty : formats)
            {
                if (formatProperty.colorSpace == colorSpace)
                {
                    break;
                }
            }

            std::cout << "Desired color mode isn`t supported. Assigned VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color mode" << std::endl;
            colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }

        void CheckPresentModeSupport(VkPresentModeKHR& presentMode, const std::vector<VkPresentModeKHR>& modes)
        {
            for (auto& availablePresentMode : modes)
            {
                if (availablePresentMode == presentMode)
                {
                    break;
                }
            }

            std::cout << "Desired present mode isn`t supported. Assigned VK_PRESENT_MODE_IMMEDIATE_KHR present mode" << std::endl;
            presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }

        void QuerrySwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
            VkFormat swapchainImagesFormat, std::vector<VkImage>* swapchainImages,
            std::vector<VkImageView>* swapchainImageViews)
        {
            uint32_t imagesCount;
            vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, nullptr);
            swapchainImages->resize(imagesCount);
            vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, swapchainImages->data());

            std::cout << "Total swapchain images retrived is: " << swapchainImages->size() << std::endl;
        }

        void DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain)
        {
            vkDestroySwapchainKHR(device, swapchain, nullptr);
        }
    }
}