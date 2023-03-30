#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <Windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vulkan_core.h"
#include "Include/vulkan/vulkan_win32.h"

namespace psm
{
    class Vulkan
    {
    public:
        Vulkan();
        void Init(HINSTANCE hInstance, HWND hWnd);
        void Deinit();
        void InitVulkanInstace();
    private:
        void VerifyLayersSupport(std::vector< const char*>& layersToEnable);
        void VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        void CreateSurface(HINSTANCE hInstance, HWND hWnd);
        void SelectPhysicalDevice();
    private:
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkDevice m_LogicalDevice;
        VkSurfaceKHR m_Surface;
    };
}