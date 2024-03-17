#include "RHIVulkan.h"

#include "RHICommon.h"
#include "Common.h"
#include "VkCommon.h"

#include "Vulkan/RenderBackend/BackedInfo.h"
#include "Vulkan/RenderBackend/Instance.h"
#include "Vulkan/RenderBackend/DebugUtilsMessenger.h"

VkInstance Instance;
VkDebugUtilsMessengerEXT DebugMessenger;

namespace psm
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        constexpr auto vkToDebugSeverity = [](VkDebugUtilsMessageSeverityFlagBitsEXT severity)
        {
            if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                return MessageSeverity::Fatal;
            }
            else if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
                return MessageSeverity::Warning;
            }
            else
            {
                return MessageSeverity::Info;
            }
        };

        auto severity = vkToDebugSeverity(messageSeverity);
        
        if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LogMessage(severity, pCallbackData->pMessage);
        }

        return false;
    }

    void InitVk(const InitOptions& options)
    {
        //create instance and debug utils messenger
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        vk::PopulateDebugUtilsMessenger(DebugMessengerCallback, &debugMessengerInfo);
        VkResult result =  vk::CreateInstance(ValidationLayers, InstanceExtensions, debugMessengerInfo, &Instance);
        VK_CHECK_RESULT(result);
        vk::CreateDebugUtilsMessenger(Instance, DebugMessengerCallback, &DebugMessenger);
    }

    void DeinitVk()
    {
        vk::DestroyDebugUtilsMessenger(Instance, DebugMessenger);
        vk::DestroyInstance(Instance);
    }
}