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
        struct ShaderModuleInfo
        {
            VkShaderModule Module;
            VkShaderStageFlagBits Stage;
            std::string Name;
        };

        void CreateShaderModule(VkDevice logicalDevice, const std::string& path, VkShaderModule* module);
        void DestroyShaderModule(VkDevice device, VkShaderModule module);
    }
}