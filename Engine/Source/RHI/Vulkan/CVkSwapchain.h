#pragma once

//XXX RM
//#include "RenderBackend/Win32Surface.h"

#include "Include/vulkan/vulkan.h"
#include "../Interface/Swapchain.h"
#include "../VkCommon.h"
#include "../Interface/Types.h"

namespace psm
{
    class CVkSwapchain final : public ISwapchain, public std::enable_shared_from_this<CVkSwapchain>
    {
    public:
        CVkSwapchain(DevicePtr device, const SwapchainConfig& config);
        virtual ~CVkSwapchain();

    private:
        void CheckFormatSupport(VkFormat& format, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckPresentModeSupport(VkPresentModeKHR& presentMode, const std::vector<VkPresentModeKHR>& presentModes);
        void QuerrySwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
            VkFormat swapchainImagesFormat, std::vector<VkImage>* swapchainImages,
            std::vector<VkImageView>* swapchainImageViews);
        void DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain);
    private:
        VkDevice mDeviceInternal;

        VkSwapchainKHR mSwapChain;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;

        //should be moved to device
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_FlightFences;

        VkSampleCountFlagBits m_MaxMsaaSamples; //temporary
    };
}