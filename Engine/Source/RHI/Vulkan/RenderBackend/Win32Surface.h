#pragma once

#include <vector>

#include "Common.h"



namespace psm
{
    namespace vk
    {
        //void CreateSurface(HINSTANCE hInstance, HWND hWnd, VkInstance instance, VkSurfaceKHR* surface);
        void PopulateSurfaceData(VkInstance instance, VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface, SurfaceData* surfaceData);
        //void DestroySurface(VkInstance instance, VkSurfaceKHR surface);
    }
}