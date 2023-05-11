#include "DebugUtilsMessenger.h"

namespace psm
{
    namespace vk
    {
        void PopulateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback,
            VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo)
        {
            debugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo->flags = 0;
            debugMessengerCreateInfo->pNext = nullptr;
            debugMessengerCreateInfo->pfnUserCallback = callback;
            debugMessengerCreateInfo->messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessengerCreateInfo->messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        }

        void CreateDebugUtilsMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback , 
            VkDebugUtilsMessengerEXT* messenger)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            PopulateDebugUtilsMessenger(callback, &createInfo);

            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                VkResult result = func(instance, &createInfo, nullptr, messenger);
                if (result != VK_SUCCESS)
                {
                    std::cout << "Failed to create Debug utils messenger EXT" << std::endl;
                }
            }
        }

        void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                "vkDestroyDebugUtilsMessengerEXT");

            if (func != nullptr)
            {
                func(instance, messenger, nullptr);
            }
        }
    }
}