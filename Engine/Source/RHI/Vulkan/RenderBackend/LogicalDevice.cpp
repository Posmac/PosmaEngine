#include "LogicalDevice.h"

namespace psm
{
    namespace vk
    {
        void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable, 
            VkPhysicalDevice gpu)
        {
            uint32_t availableExtensionsCount;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, nullptr);

            if (availableExtensionsCount == 0)
            {
                std::cout << "Failed to enumerate available device extensions" << std::endl;
            }

            std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, availableExtensions.data());

            std::vector<const char*> actualExtensionsToEnable;

            for (auto& property : availableExtensions)
            {
                for (auto& extension : extensionsToEnable)
                {
                    if (strcmp(property.extensionName, extension) == 0)
                    {
                        actualExtensionsToEnable.push_back(extension);
                        std::cout << "Device extension: " << extension;
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

        void DestroyDevice(VkDevice device)
        {
            vkDestroyDevice(device, nullptr);
        }

        void CreateLogicalDevice(std::vector<const char*>& deviceExtensions, 
                                 std::vector<const char*>& validationLayers,
                                 VkPhysicalDeviceFeatures featuresToEnable, 
                                 VkPhysicalDevice gpu, 
                                 VkSurfaceKHR surface, 
                                 QueueFamilyIndices* queueData, 
                                 VkDevice* device)
        {
            VerifyDeviceExtensionsSupport(deviceExtensions, gpu);

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.enabledExtensionCount = deviceExtensions.size();
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();
            createInfo.enabledLayerCount = validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
            createInfo.pEnabledFeatures = &featuresToEnable;

            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyPropertyCount, nullptr);

            if (queueFamilyPropertyCount == 0)
            {
                std::cout << "Failed to get physical device queue family properties" << std::endl;
            }

            std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, 
                &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

            int i = 0;
            for (const auto& family : availableQueueFamilyProperties)
            {
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
                if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    queueData->GraphicsFamily = i;
                }
                if (presentSupport)
                {
                    queueData->PresentFamily = i;
                }
                if (queueData->IsComplete())
                {
                    break;
                }
                i++;
            }

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = { queueData->GraphicsFamily.value(),
                                                      queueData->PresentFamily.value() };
            float queuePriority = 1.0f;

            for (uint32_t family : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = family;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            createInfo.queueCreateInfoCount = queueCreateInfos.size();
            createInfo.pQueueCreateInfos = queueCreateInfos.data();

            VkResult result = vkCreateDevice(gpu, &createInfo, nullptr, device);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create logical device" << std::endl;
            }

            vkGetDeviceQueue(*device, queueData->GraphicsFamily.value(), 0, &queueData->GraphicsQueue);
            vkGetDeviceQueue(*device, queueData->PresentFamily.value(), 0, &queueData->PresentQueue);
        }
    }
}