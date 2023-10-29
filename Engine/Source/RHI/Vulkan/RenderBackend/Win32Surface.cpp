#include "Win32Surface.h"

namespace psm
{
    namespace vk
    {
        void CreateSurface(HINSTANCE hInstance, HWND hWnd, VkInstance instance, VkSurfaceKHR* surface)
        {
            VkWin32SurfaceCreateInfoKHR surfaceInfo{};
            surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceInfo.pNext = nullptr;
            surfaceInfo.flags = 0;
            surfaceInfo.hinstance = hInstance;
            surfaceInfo.hwnd = hWnd;

            VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, surface));
        }

        void PopulateSurfaceData(VkInstance instance, VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface, SurfaceData* surfaceData)
        {
            //get surface capabilities, formats and present modes
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceData->Capabilities);

            uint32_t surfaceSuportedFormatsCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceSuportedFormatsCount, nullptr);

            if (surfaceSuportedFormatsCount == 0)
            {
                std::cout << "Failed to get supported surface formats" << std::endl;
            }

            surfaceData->Formats.resize(surfaceSuportedFormatsCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceSuportedFormatsCount,
                surfaceData->Formats.data());

            uint32_t surfaceSupportedPresentModesCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfaceSupportedPresentModesCount, nullptr);

            if (surfaceSupportedPresentModesCount == 0)
            {
                std::cout << "Failed to get supported surface present modes" << std::endl;
            }

            surfaceData->PresentModes.resize(surfaceSupportedPresentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfaceSupportedPresentModesCount,
                surfaceData->PresentModes.data());
        }

        void DestroySurface(VkInstance instance, VkSurfaceKHR surface)
        {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
    }
}