#pragma once

#include <vector>

//extensions and layers
inline std::vector<const char*> ValidationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_LUNARG_monitor",
    "VK_LAYER_LUNARG_api_dump",
    "VK_LAYER_LUNARG_core_validation",
    "VK_LAYER_LUNARG_object_tracker",
};

inline std::vector<const char*> InstanceExtensions =
{
    "VK_KHR_surface",
    "VK_KHR_get_physical_device_properties2",
    "VK_KHR_win32_surface",
    "VK_EXT_debug_utils",
    "VK_EXT_debug_report"
};

inline std::vector<const char*> DeviceExtensions =
{
    "VK_KHR_swapchain",
};

namespace psm
{
    
}