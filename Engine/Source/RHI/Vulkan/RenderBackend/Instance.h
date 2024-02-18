#pragma once

#include <vector>

#include "Common.h"

namespace psm
{
    namespace vk
    {
        VkResult CreateInstance(std::vector<const char*>& validationLayers,
            std::vector<const char*>& instanceExtenstions,
            VkDebugUtilsMessengerCreateInfoEXT debugMessegerCreateInfoExt, 
            VkInstance* instance);
        void DestroyInstance(VkInstance instance);
        void VerifyLayersSupport(std::vector< const char*>& layersToEnable);
        void VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
    }
}