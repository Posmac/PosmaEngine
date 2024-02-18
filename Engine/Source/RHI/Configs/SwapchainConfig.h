#pragma once

namespace psm
{
    struct SwapchainPresentConfig
    {
        uint32_t ImageIndex;
    };

    struct SwapchainConfig
    {
        VkSurfaceKHR Surface;
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };
}