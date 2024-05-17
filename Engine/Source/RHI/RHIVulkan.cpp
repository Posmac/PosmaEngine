#include "RHIVulkan.h"

#include "RHICommon.h"
#include "Common.h"
#include "RHI/Vulkan/VkUtils.h"

VkInstance Instance;
VkDebugUtilsMessengerEXT DebugMessenger;
VkDebugReportCallbackEXT DebugCallback;

namespace psm
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugInfoCallback(VkDebugReportFlagsEXT flags,
                                                            VkDebugReportObjectTypeEXT objectType,
                                                            uint64_t object,
                                                            size_t location,
                                                            int32_t messageCode,
                                                            const char* pLayerPrefix,
                                                            const char* pMessage,
                                                            void* pUserData)
    {
        LogMessage(MessageSeverity::Warning, pMessage);
        return false;
    }

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

    void VerifyLayersSupport(std::vector<const char*>& layersToEnable)
    {
        uint32_t availableLayersCount;
        vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);

        if(availableLayersCount == 0)
        {
            LogMessage(MessageSeverity::Warning, "No available layers");
        }

        std::vector<VkLayerProperties> availableLayers(availableLayersCount);
        vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data());

        std::vector<const char*> actualLayersToEnable;
        for(auto& property : availableLayers)
        {
            for(auto& layerName : layersToEnable)
            {
                if(strcmp(layerName, property.layerName) == 0)
                {
                    actualLayersToEnable.push_back(layerName);
                    //LogMessage(MessageSeverity::Warning, "Layer ");
                }
                else
                {
                    //std::cout << " isn`t supported" << std::endl;
                }
            }
        }

        layersToEnable = std::move(actualLayersToEnable);
    }

    void VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

        if(availableExtensionsCount == 0)
        {
            LogMessage(MessageSeverity::Warning, "No available extensions");
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

        std::vector<const char*> actualExtensionsToEnable;

        for(auto& property : availableExtensions)
        {
            for(auto& extension : extensionsToEnable)
            {
                if(strcmp(property.extensionName, extension) == 0)
                {
                    actualExtensionsToEnable.push_back(extension);
                    //LogMessage(MessageSeverity::Warning, "");
                }
                else
                {
                    //std::cout << " isn`t supported" << std::endl;
                }
            }
        }

        extensionsToEnable = actualExtensionsToEnable;
    }

    void InitVk(const InitOptions& options)
    {
        VkDebugReportCallbackCreateInfoEXT reportInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
                            | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT,
            .pfnCallback = DebugInfoCallback,
            .pUserData = nullptr,
        };

        //create instance and debug utils messenger
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = &reportInfo,
            .flags = 0,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugMessengerCallback,
        };


        //create instance
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pEngineName = "Vulkan Engine";
        applicationInfo.pApplicationName = "Vulkan Engine Application";
        applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        auto validationLayers = ValidationLayers;
        auto instanceExtenstions = InstanceExtensions;

        VerifyLayersSupport(validationLayers);
        VerifyInstanceExtensionsSupport(instanceExtenstions);

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerInfo;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledExtensionCount = instanceExtenstions.size();
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtenstions.data();

        VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);
        VK_CHECK_RESULT(res);

        auto dbgMessengerFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
        if(dbgMessengerFunc != nullptr)
        {
            VkResult result = dbgMessengerFunc(Instance, &debugMessengerInfo, nullptr, &DebugMessenger);
            VK_CHECK_RESULT(result);
        }

        auto dbgCallbackFunc = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
        if(dbgCallbackFunc != nullptr)
        {
            VkResult result = dbgCallbackFunc(Instance, &reportInfo, nullptr, &DebugCallback);
            VK_CHECK_RESULT(result);
        }
    }

    void DeinitVk()
    {
        auto dbgMessengerFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");

        if(dbgMessengerFunc != nullptr)
        {
            dbgMessengerFunc(Instance, DebugMessenger, nullptr);
        }

        auto dbgCallbackFunc = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");
        if(dbgCallbackFunc != nullptr)
        {
            dbgCallbackFunc(Instance, DebugCallback, nullptr);
        }

        vkDestroyInstance(Instance, nullptr);
    }
}