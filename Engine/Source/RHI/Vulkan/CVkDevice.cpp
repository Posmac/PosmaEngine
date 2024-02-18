#include "CVkDevice.h"

#include "CVkSurface.h"
#include "CVkSwapchain.h"

#include <Windows.h>

#include "../RHICommon.h"
#include "../VkCommon.h"

#include "RenderBackend/BackedInfo.h"
#include "RenderBackend/PhysicalDevice.h"
#include "RenderBackend/LogicalDevice.h"

extern VkInstance Instance;

namespace psm
{
    DevicePtr CreateDefaultDeviceVk(PlatformConfig& config)
    {
        HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(config.win32.hInstance);
        HWND hWnd = reinterpret_cast<HWND>(config.win32.hWnd);

        VkPhysicalDevice gpu;

        //select physical device
        uint32_t physicalDevicesCount;
        vkEnumeratePhysicalDevices(Instance, &physicalDevicesCount, nullptr);

        if(physicalDevicesCount == 0)
        {
            FatalError("Unable to enumerate physical devices");
        }

        std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
        vkEnumeratePhysicalDevices(Instance, &physicalDevicesCount, physicalDevicesAvailable.data());

        for(auto& device : physicalDevicesAvailable)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(device, &deviceProps);

            if(deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                LogMessage(psm::MessageSeverity::Info, "Found discrette device: ");
                LogMessage(psm::MessageSeverity::Info, "Production: " + std::to_string(deviceProps.vendorID) + ", " + deviceProps.deviceName);
                gpu = device;
            }
        }

        if(gpu != VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

            LogMessage(psm::MessageSeverity::Info, "Unable to get discrette gpu, selected first one");
            LogMessage(psm::MessageSeverity::Info, "Production: " + std::to_string(deviceProps.vendorID) + ", " + deviceProps.deviceName);
            gpu = physicalDevicesAvailable[0];
        }
        
        //create surface
        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = hInstance;
        surfaceInfo.hwnd = hWnd;

        VkSurfaceKHR surface;
        VkResult result = vkCreateWin32SurfaceKHR(Instance, &surfaceInfo, nullptr, &surface);
        VK_CHECK_RESULT(result);

        return std::make_shared<CVkDevice>(gpu, surface);
    }

    CVkDevice::CVkDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        mPhysicalDevice = physicalDevice;
        //mSurface = surface;

        //check for features to enable
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        VkPhysicalDeviceFeatures featuresToEnable{}; //for now we dont enable any features

        //get device props
        VkPhysicalDeviceProperties deviceProps{};
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);

        //create device
        VerifyDeviceExtensionsSupport(DeviceExtensions, physicalDevice);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.enabledExtensionCount = DeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
        createInfo.enabledLayerCount = ValidationLayers.size();
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
        createInfo.pEnabledFeatures = &featuresToEnable;

        uint32_t queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

        if(queueFamilyPropertyCount == 0)
        {
            std::cout << "Failed to get physical device queue family properties" << std::endl;
        }

        std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

        int i = 0;
        for(const auto& family : availableQueueFamilyProperties)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                mQueues.GraphicsFamily = i;
            }
            if(presentSupport)
            {
                mQueues.PresentFamily = i;
            }
            if(mQueues.IsComplete())
            {
                break;
            }
            i++;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { mQueues.GraphicsFamily.value(),
                                                  mQueues.PresentFamily.value() };
        float queuePriority = 1.0f;

        for(uint32_t family : uniqueQueueFamilies)
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

        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &mDevice);

        if(result != VK_SUCCESS)
        {
            FatalError("Failed to create logical device");
        }

        vkGetDeviceQueue(mDevice, mQueues.GraphicsFamily.value(), 0, &mQueues.GraphicsQueue);
        vkGetDeviceQueue(mDevice, mQueues.PresentFamily.value(), 0, &mQueues.PresentQueue);
    }

    CVkDevice::~CVkDevice()
    {
        //mSurface->DestroySurface(Instance, mSurface);
        vk::DestroyDevice(mDevice);
    }

    void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable, VkPhysicalDevice gpu)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, nullptr);

        if(availableExtensionsCount == 0)
        {
            FatalError("Failed to enumerate available device extensions");
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, availableExtensions.data());

        std::vector<const char*> actualExtensionsToEnable;

        for(auto& property : availableExtensions)
        {
            for(auto& extension : extensionsToEnable)
            {
                if(strcmp(property.extensionName, extension) == 0)
                {
                    actualExtensionsToEnable.push_back(extension);
                    LogMessage(psm::MessageSeverity::Info, "Supported device extension: " + std::string(extension));
                }
                else
                {
                    LogMessage(psm::MessageSeverity::Warning, "Extension isn`t supported: " + std::string(extension));
                }
            }
        }

        extensionsToEnable = std::move(actualExtensionsToEnable);
    }

    SwapchainPtr CVkDevice::CreateSwapchain(const SwapchainConfig& config)
    {
        return std::make_shared<CVkSwapchain>(this, config);
    }
}