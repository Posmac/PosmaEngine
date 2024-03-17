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
    class CVkSurface : public ISurface
    {
    public:
        struct SurfaceData
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Formats;
            std::vector<VkPresentModeKHR> PresentModes;
        };

    public:
        CVkSurface(const PlatformConfig& config);
        virtual ~CVkSurface() = default;

        virtual void* GetSurface() override;

    public:
        void DestroySurface();
        void PopulateSurfaceData(const DeviceData& data, SurfacePtr surface);
        SurfaceData& GetSurfaceData();
    private:
        VkSurfaceKHR mSurface;

        SurfaceData mSurfaceData;
    };
}