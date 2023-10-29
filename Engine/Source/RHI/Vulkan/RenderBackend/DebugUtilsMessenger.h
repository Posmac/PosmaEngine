#pragma once

#include "Core.h"

namespace psm
{
    namespace vk
    {
        void PopulateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback,
            VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo);
        void CreateDebugUtilsMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback,
            VkDebugUtilsMessengerEXT* messenger);
        void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger);
    }
}