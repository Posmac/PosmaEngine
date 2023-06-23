#pragma once

#include <cstdint>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        uint32_t FindMemoryType(VkPhysicalDevice gpu, 
            uint32_t typeFilter, 
            VkMemoryPropertyFlags props);
    }
}