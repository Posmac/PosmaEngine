#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <optional>
#include <Windows.h>
#include <set>

#define VK_USE_PLATFORM_WIN32_KHR
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vulkan_core.h"
#include "Include/vulkan/vulkan_win32.h"

namespace psm
{
    class Vulkan
    {
    public:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> GraphicsFamily;
            std::optional<uint32_t> PresentFamily;
            VkQueue GraphicsQueue;
            VkQueue PresentQueue;

            bool IsComplete()
            {
                return GraphicsFamily.has_value() && PresentFamily.has_value();
            }

            bool IdenticalQueues()
            {
                return GraphicsFamily.value() == PresentFamily.value();
            }
        };

    public:
        Vulkan();
        void Init(HINSTANCE hInstance, HWND hWnd);
        void Deinit();
        void InitVulkanInstace();
    private:
        void VerifyLayersSupport(std::vector< const char*>& layersToEnable);
        void VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        void CreateSurface(HINSTANCE hInstance, HWND hWnd);
        void SelectPhysicalDevice();
        void CreateLogicalDevice();
    private:

        std::vector<const char*> m_ValidationLayers =
        {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_monitor",
            "VK_LAYER_LUNARG_api_dump",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_object_tracker",
        };

        std::vector<const char*> m_InstanceExtensions =
        {
            "VK_KHR_surface",
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_win32_surface",
            "VK_EXT_debug_utils",
            "VK_EXT_debug_report"
        };

        std::vector<const char*> m_DeviceExtensions =
        {
            "VK_KHR_swapchain",
        };

        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
        VkDevice m_LogicalDevice;
        VkSurfaceKHR m_Surface;
        QueueFamilyIndices m_QueueIndices;
    };
}