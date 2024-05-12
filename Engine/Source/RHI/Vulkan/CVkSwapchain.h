#pragma once

#include "Include/vulkan/vulkan.h"
#include "RHI/Interface/Swapchain.h"
#include "RHI/RHICommon.h"
#include "RHI/Interface/Types.h"

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
        virtual void* ImageAtIndex(uint32_t index) override;
        virtual void Present(const SPresentConfig& config) override;
        virtual void SetVsyncMode(bool enabled) override;
        virtual uint32_t GetImagesCount() override;
        virtual EFormat GetSwapchainImageFormat() override;
        virtual SResourceExtent3D GetSwapchainSize() override;

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        void CheckFormatSupport(VkFormat& format, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace, const std::vector<VkSurfaceFormatKHR>& formats);
        void CheckPresentModeSupport(VkPresentModeKHR& presentMode, const std::vector<VkPresentModeKHR>& presentModes);
        void QuerrySwapchainImages();
        void DestroySwapchain(VkDevice device, VkSwapchainKHR swapchain);
    private:
        VkDevice mDeviceInternal;

        VkSwapchainKHR mSwapChain;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;

        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapchainImageViews;
    };
}