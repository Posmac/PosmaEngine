#include "CVkSurface.h"

#include "../VkCommon.h"
#include "CVkDevice.h"

namespace psm
{
    CVkSurface::CVkSurface(DevicePtr& device, const PlatformConfig& config)
    {
        mInstanceInternal = reinterpret_cast<VkInstance>(device->GetDeviceData().vkData.Device);

        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = (HINSTANCE)(config.win32.hInstance);
        surfaceInfo.hwnd = (HWND)(config.win32.hWnd);

        VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mInstanceInternal, &surfaceInfo, nullptr, &mSurface));
    }

    void CVkSurface::DestroySurface()
    {
        vkDestroySurfaceKHR(mInstanceInternal, mSurface, nullptr);
    }

    void CVkSurface::PopulateSurfaceData(VkInstance instance, VkPhysicalDevice physicalDevice,
          VkSurfaceKHR surface)
    {
        //get surface capabilities, formats and present modes
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &mCapabilities);

        uint32_t surfaceSuportedFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceSuportedFormatsCount, nullptr);

        Assert(surfaceSuportedFormatsCount == 0, "Failed to enumerate physical device surface formats");

        mFormats.resize(surfaceSuportedFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceSuportedFormatsCount, mFormats.data());

        uint32_t surfaceSupportedPresentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfaceSupportedPresentModesCount, nullptr);

        Assert(surfaceSupportedPresentModesCount == 0, "Failed to get supported surface present modes");

        mPresentModes.resize(surfaceSupportedPresentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfaceSupportedPresentModesCount, mPresentModes.data());
    }
}