#include "Instance.h"

namespace psm
{
    namespace vk
    {
        VkResult CreateInstance(std::vector<const char*>& validationLayers,
            std::vector<const char*>& instanceExtenstions,
            VkDebugUtilsMessengerCreateInfoEXT debugMessegerCreateInfoExt,
            VkInstance* instance)
        {
            VkApplicationInfo applicationInfo{};
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pNext = nullptr;
            applicationInfo.pEngineName = "Vulkan Engine";
            applicationInfo.pApplicationName = "Vulkan Engine Application";
            applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
            applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

            VerifyLayersSupport(validationLayers);
            VerifyInstanceExtensionsSupport(instanceExtenstions);

            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessegerCreateInfoExt;
            instanceCreateInfo.flags = 0;
            instanceCreateInfo.pApplicationInfo = &applicationInfo;
            instanceCreateInfo.enabledLayerCount = validationLayers.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.enabledExtensionCount = instanceExtenstions.size();
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtenstions.data();

            return vkCreateInstance(&instanceCreateInfo, nullptr, instance);
        }

        void DestroyInstance(VkInstance instance)
        {
            vkDestroyInstance(instance, nullptr);
        }

        void VerifyLayersSupport(std::vector<const char*>& layersToEnable)
        {
            uint32_t availableLayersCount;
            vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);

            if (availableLayersCount == 0)
            {
                std::cout << "Failed to enumerate available layers count\n";
            }

            std::vector<VkLayerProperties> availableLayers(availableLayersCount);
            vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data());

            std::vector<const char*> actualLayersToEnable;
            for (auto& property : availableLayers)
            {
                for (auto& layerName : layersToEnable)
                {
                    if (strcmp(layerName, property.layerName) == 0)
                    {
                        actualLayersToEnable.push_back(layerName);
                        std::cout << "Layer: " << layerName;
                        std::cout << " is supported" << std::endl;
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

            if (availableExtensionsCount == 0)
            {
                std::cout << "Failed to enumerate available instance extensions" << std::endl;
            }

            std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

            std::vector<const char*> actualExtensionsToEnable;

            for (auto& property : availableExtensions)
            {
                for (auto& extension : extensionsToEnable)
                {
                    if (strcmp(property.extensionName, extension) == 0)
                    {
                        actualExtensionsToEnable.push_back(extension);
                        std::cout << "Instance extension: " << extension;
                        std::cout << " is supported" << std::endl;
                    }
                    else
                    {
                        //std::cout << " isn`t supported" << std::endl;
                    }
                }
            }

            extensionsToEnable = actualExtensionsToEnable;
        }
    }
}