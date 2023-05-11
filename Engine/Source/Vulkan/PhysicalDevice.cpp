#include "PhysicalDevice.h"

namespace psm
{
    namespace vk
    {
        void SelectPhysicalDevice(VkInstance instance,  VkPhysicalDeviceFeatures* features, 
            VkPhysicalDeviceProperties* properties, VkPhysicalDevice* gpu)
        {
            uint32_t physicalDevicesCount;
            vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

            if (physicalDevicesCount == 0)
            {
                std::cout << "Unable to enumerate physical devices" << std::endl;
            }

            std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
            vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevicesAvailable.data());

            for (auto& device : physicalDevicesAvailable)
            {
                VkPhysicalDeviceProperties deviceProps{};
                vkGetPhysicalDeviceProperties(device, &deviceProps);

                VkPhysicalDeviceFeatures deviceFeatures{};
                vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

                if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    std::cout << "Found discrette device: " << std::endl;
                    std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                    *gpu = device;
                    *properties = deviceProps;
                    *features= deviceFeatures;
                }
            }

            if (gpu == VK_NULL_HANDLE)
            {
                VkPhysicalDeviceProperties deviceProps{};
                vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

                VkPhysicalDeviceFeatures deviceFeatures{};
                vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

                std::cout << "Unable to get discrette gpu, selected first one" << std::endl;
                std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                *gpu = physicalDevicesAvailable[0];
                *properties = deviceProps;
                *features= deviceFeatures;
            }
        }
    }
}