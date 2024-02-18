#pragma once

#include <vector>
#include <Windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vulkan_core.h"
#include "Include/vulkan/vulkan_win32.h"

#include "../Interface/Surface.h"
#include "../interface/Types.h"
//#include "RenderBackend/Win32Surface.h"
#include "../DeviceData.h"

namespace psm
{
    class CVkSurface final : ISurface
    {
    public:
        CVkSurface(DevicePtr& device, const PlatformConfig& config);
        virtual ~CVkSurface() = default;
    public:
        void DestroySurface();
        void PopulateSurfaceData(VkInstance instance, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    private:
        VkInstance mInstanceInternal;
        VkSurfaceKHR mSurface;

        VkSurfaceCapabilitiesKHR mCapabilities;
        std::vector<VkSurfaceFormatKHR> mFormats;
        std::vector<VkPresentModeKHR> mPresentModes;
    };
}