#pragma once

#include <vector>
#include <set>

#include "Common.h"
#include "Queue.h"

namespace psm
{
    namespace vk
    {
        void CreateLogicalDevice(std::vector<const char*>& deviceExtensions,
            std::vector<const char*>& validationLayers,
            VkPhysicalDeviceFeatures featuresToEnable, VkPhysicalDevice gpu,
            VkSurfaceKHR surface, QueueFamilyIndices* queueData, VkDevice* device);

        void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable,
            VkPhysicalDevice gpu);

        void DestroyDevice(VkDevice device);
    }
}