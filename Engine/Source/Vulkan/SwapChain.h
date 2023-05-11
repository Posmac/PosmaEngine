#pragma once

#include <vector>

#include "Include/vulkan/vulkan.h"
#include "Win32Surface.h"

namespace psm
{
    namespace vk
    {
        void CreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
            SurfaceData surfaceData, VkSwapchainKHR* swapchain,
            VkFormat* swapchainImageFormat, VkExtent2D* swapchainExtent);
        void CheckFormatSupport(VkFormat& format, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckPresentModeSupport(VkPresentModeKHR& presentMode, const std::vector<VkPresentModeKHR>& presentModes);
        void QuerrySwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
            VkFormat swapchainImagesFormat, std::vector<VkImage>* swapchainImages,
            std::vector<VkImageView>* swapchainImageViews);
        void DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain);
    }
}