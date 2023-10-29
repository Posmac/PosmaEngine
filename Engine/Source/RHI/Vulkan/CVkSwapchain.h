#pragma once

#include "RenderBackend/Core.h"
#include "RenderBackend/Win32Surface.h"

#include "../Interface/Swapchain.h"

namespace psm
{
    class CVkSwapchain : CSwapchain
    {
    public:
        CVkSwapchain() = default;
        virtual ~CVkSwapchain() = default;
    public:
        virtual RHI_RESULT Init(CDevice* device, HINSTANCE hInstance, HWND hWnd) override;
    public:
        VkSurfaceKHR GetSurface();
    private:
        CVkDevice* m_ParentDevice;
        HWND m_Hwnd;

        VkSwapchainKHR m_SwapChain;
        VkSurfaceKHR m_Surface;

        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        //should be moved to device
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_FlightFences;

        vk::SurfaceData m_SurfaceData;
        VkSampleCountFlagBits m_MaxMsaaSamples; //temporary
    };
}