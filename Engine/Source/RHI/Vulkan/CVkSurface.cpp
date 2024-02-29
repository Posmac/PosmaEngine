#include "CVkSurface.h"

#include "../VkCommon.h"
#include "CVkDevice.h"

namespace psm
{
    CVkSurface::CVkSurface(const PlatformConfig& config)
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = (HINSTANCE)(config.win32.hInstance);
        surfaceInfo.hwnd = (HWND)(config.win32.hWnd);

        VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mInstanceInternal, &surfaceInfo, nullptr, &mSurface));//nullptr
    }

    void CVkSurface::DestroySurface()
    {
        vkDestroySurfaceKHR(mInstanceInternal, mSurface, nullptr);
    }

    void CVkSurface::PopulateSurfaceData(const DeviceData& data, SurfacePtr surface)
    {
        mInstanceInternal = reinterpret_cast<VkInstance>(data.vkData.Instance);
        VkPhysicalDevice physicalDevice = reinterpret_cast<VkPhysicalDevice>(data.vkData.PhysicalDevice);

        std::shared_ptr<CVkSurface> vkSurface = std::static_pointer_cast<CVkSurface>(surface);

        //get surface capabilities, formats and present modes
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkSurface->GetSurface(), &mSurfaceData.Capabilities);

        uint32_t surfaceSuportedFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface->GetSurface(), &surfaceSuportedFormatsCount, nullptr);

        Assert(surfaceSuportedFormatsCount == 0, "Failed to enumerate physical device surface formats");

        mSurfaceData.Formats.resize(surfaceSuportedFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface->GetSurface(), &surfaceSuportedFormatsCount, mSurfaceData.Formats.data());

        uint32_t surfaceSupportedPresentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface->GetSurface(), &surfaceSupportedPresentModesCount, nullptr);

        Assert(surfaceSupportedPresentModesCount == 0, "Failed to get supported surface present modes");

        mSurfaceData.PresentModes.resize(surfaceSupportedPresentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface->GetSurface(), &surfaceSupportedPresentModesCount, mSurfaceData.PresentModes.data());
    }

    VkSurfaceKHR CVkSurface::GetSurface()
    {
        return mSurface;
    }

    CVkSurface::SurfaceData& CVkSurface::GetSurfaceData()
    {
        return mSurfaceData;
    }
}