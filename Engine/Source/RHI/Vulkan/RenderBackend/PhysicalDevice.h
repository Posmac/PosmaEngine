#pragma once

#include <vector>

#include "Core.h"

namespace psm
{
    namespace vk
    {
        void SelectPhysicalDevice(VkInstance instance,
                                  VkPhysicalDeviceFeatures* features,
                                  VkPhysicalDeviceProperties* properties,
                                  VkPhysicalDevice* gpu);

        VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice gpu,
                                                      VkPhysicalDeviceProperties physicalDeviceProperties);

        VkFormat FindSupportedFormat(VkPhysicalDevice gpu,
                                     const std::vector<VkFormat>& candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features);
    }
}