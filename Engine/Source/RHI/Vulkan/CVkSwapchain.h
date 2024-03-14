#pragma once

//XXX RM
//#include "RenderBackend/Win32Surface.h"

#include "Include/vulkan/vulkan.h"
#include "../Interface/Swapchain.h"
#include "../VkCommon.h"
#include "../Interface/Types.h"

#include "TypeConvertor.h"

namespace psm
{
    class CVkSwapchain : public ISwapchain, public std::enable_shared_from_this<CVkSwapchain>
    {
    public:
        CVkSwapchain(DevicePtr device, const SSwapchainConfig& config);
        virtual ~CVkSwapchain();

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void GetNextImage(const SSwapchainAquireNextImageConfig& config, uint32_t* index) override;
        virtual ImagePtr ImageAtIndex(uint32_t index) override;
        virtual void Present(const SPresentConfig& config) override;
        virtual void SetVsyncMode(bool enabled) override;
        virtual uint32_t GetImagesCount() override;
        virtual EFormat GetSwapchainImageFormat() override;
        virtual SResourceExtent3D GetSwapchainSize() override;

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

        //should be moved to renderer
        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapchainImageViews;

        VkSampleCountFlagBits mMaxMsaaSamples; //temporary
    };
}