#include "CVkSurface.h"

#include "../VkCommon.h"
#include "CVkDevice.h"

extern VkInstance Instance;

namespace psm
{
    CVkSurface::CVkSurface(const PlatformConfig& config)
    {
        assert(Instance != nullptr);

        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = (HINSTANCE)(config.win32.hInstance);
        surfaceInfo.hwnd = (HWND)(config.win32.hWnd);

        VkResult result = vkCreateWin32SurfaceKHR(Instance, &surfaceInfo, nullptr, &mSurface);
        VK_CHECK_RESULT(result);
    }

    void CVkSurface::DestroySurface()
    {
        vkDestroySurfaceKHR(Instance, mSurface, nullptr);
    }

    void CVkSurface::PopulateSurfaceData(const DeviceData& data, SurfacePtr surface)
    {
        VkPhysicalDevice physicalDevice = reinterpret_cast<VkPhysicalDevice>(data.vkData.PhysicalDevice);
        assert(physicalDevice != nullptr);

        VkSurfaceKHR vkSurface = reinterpret_cast<VkSurfaceKHR>(surface->GetSurface());
        assert(vkSurface != nullptr);

        //get surface capabilities, formats and present modes
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkSurface, &mSurfaceData.Capabilities);

        uint32_t surfaceSuportedFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface, &surfaceSuportedFormatsCount, nullptr);

        Assert(surfaceSuportedFormatsCount != 0, "Failed to enumerate physical device surface formats");

        mSurfaceData.Formats.resize(surfaceSuportedFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface, &surfaceSuportedFormatsCount, mSurfaceData.Formats.data());

        uint32_t surfaceSupportedPresentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface, &surfaceSupportedPresentModesCount, nullptr);

        Assert(surfaceSupportedPresentModesCount != 0, "Failed to get supported surface present modes");

        mSurfaceData.PresentModes.resize(surfaceSupportedPresentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface, &surfaceSupportedPresentModesCount, mSurfaceData.PresentModes.data());
    }

    void* CVkSurface::GetSurface()
    {
        return mSurface;
    }

    CVkSurface::SurfaceData& CVkSurface::GetSurfaceData()
    {
        return mSurfaceData;
    }
}