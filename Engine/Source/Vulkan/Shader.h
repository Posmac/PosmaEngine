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
        void CreateShaderModule(VkDevice logicalDevice, const std::string& path, VkShaderModule* module);
        void DestroyShaderModule(VkDevice device, VkShaderModule module);
    }
}