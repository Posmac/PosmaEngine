#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        VkShaderModule CreateShaderModule(VkDevice logicalDevice, const std::string& path);
    }
}