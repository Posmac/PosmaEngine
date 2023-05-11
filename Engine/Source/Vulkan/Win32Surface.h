#pragma once

#include <vector>
#include <Windows.h>

#include "Core.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vulkan_core.h"
#include "Include/vulkan/vulkan_win32.h"

namespace psm
{
    namespace vk
    {
        struct SurfaceData
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Formats;
            std::vector<VkPresentModeKHR> PresentModes;
        };

        void CreateSurface(HINSTANCE hInstance, HWND hWnd, VkInstance instance, VkSurfaceKHR* surface);
        void PopulateSurfaceData(VkInstance instance, VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface, SurfaceData* surfaceData);
        void DestroySurface(VkInstance instance, VkSurfaceKHR surface);
    }
}