#include "PhysicalDevice.h"

namespace psm
{
    namespace vk
    {
        void SelectPhysicalDevice(VkInstance instance, 
                                  VkPhysicalDeviceFeatures* features, 
                                  VkPhysicalDeviceProperties* properties,
                                  VkSampleCountFlagBits* maxSamples,
                                  VkPhysicalDevice* gpu)
        {
            uint32_t physicalDevicesCount;
            vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

            if(physicalDevicesCount == 0)
            {
                std::cout << "Unable to enumerate physical devices" << std::endl;
            }

            std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
            vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevicesAvailable.data());

            for(auto& device : physicalDevicesAvailable)
            {
                VkPhysicalDeviceProperties deviceProps{};
                vkGetPhysicalDeviceProperties(device, &deviceProps);

                VkPhysicalDeviceFeatures deviceFeatures{};
                vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

                if(deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    std::cout << "Found discrette device: " << std::endl;
                    std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                    *gpu = device;
                    *properties = deviceProps;
                    *features = deviceFeatures;
                    *maxSamples = GetMaxUsableSampleCount(device, deviceProps);
                }
            }

            if(gpu == VK_NULL_HANDLE)
            {
                VkPhysicalDeviceProperties deviceProps{};
                vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

                VkPhysicalDeviceFeatures deviceFeatures{};
                vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

                std::cout << "Unable to get discrette gpu, selected first one" << std::endl;
                std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                *gpu = physicalDevicesAvailable[0];
                *properties = deviceProps;
                *features = deviceFeatures;
                *maxSamples = GetMaxUsableSampleCount(physicalDevicesAvailable[0], deviceProps);
            }
        }

        VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice gpu,
                                                      VkPhysicalDeviceProperties physicalDeviceProperties)
        {
            VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                physicalDeviceProperties.limits.framebufferDepthSampleCounts;

            if(counts & VK_SAMPLE_COUNT_64_BIT)
            {
                return VK_SAMPLE_COUNT_64_BIT;
            }
            if(counts & VK_SAMPLE_COUNT_32_BIT)
            {
                return VK_SAMPLE_COUNT_32_BIT;
            }
            if(counts & VK_SAMPLE_COUNT_16_BIT)
            {
                return VK_SAMPLE_COUNT_16_BIT;
            }
            if(counts & VK_SAMPLE_COUNT_8_BIT)
            {
                return VK_SAMPLE_COUNT_8_BIT;
            }
            if(counts & VK_SAMPLE_COUNT_4_BIT)
            {
                return VK_SAMPLE_COUNT_4_BIT;
            }
            if(counts & VK_SAMPLE_COUNT_2_BIT)
            {
                return VK_SAMPLE_COUNT_2_BIT;
            }

            return VK_SAMPLE_COUNT_1_BIT;
        }
    }
}