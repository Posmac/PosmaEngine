#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void SelectPhysicalDevice(VkInstance instance,
                                  VkPhysicalDeviceFeatures* features,
                                  VkPhysicalDeviceProperties* properties,
                                  VkSampleCountFlagBits* maxSamples,
                                  VkPhysicalDevice* gpu);

        VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice gpu,
                                                      VkPhysicalDeviceProperties physicalDeviceProperties);
    }
}