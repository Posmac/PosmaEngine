#include "Vulkan.h"

namespace psm
{
    Vulkan::Vulkan()
    {

    }

    void Vulkan::Init(HINSTANCE hInstance, HWND hWnd)
    {
        InitVulkanInstace();
        CreateSurface(hInstance, hWnd);
        SelectPhysicalDevice();
        CreateLogicalDevice();
    }

    void Vulkan::Deinit()
    {
        vkDestroyInstance(m_Instance, nullptr);
    }

    void Vulkan::InitVulkanInstace()
    {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pEngineName = "Vulkan Engine";
        applicationInfo.pApplicationName = "Vulkan Engine Application";
        applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        VerifyLayersSupport(m_ValidationLayers);
        VerifyInstanceExtensionsSupport(m_InstanceExtensions);

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = m_ValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        instanceCreateInfo.enabledExtensionCount = m_InstanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames= m_InstanceExtensions.data();

        VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to create VkInstance" << std::endl;
        }
    }

    void Vulkan::VerifyLayersSupport(std::vector<const char*>& layersToEnable)
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

    void Vulkan::VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable)
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

    void Vulkan::VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionsCount, nullptr);

        if (availableExtensionsCount == 0)
        {
            std::cout << "Failed to enumerate available device extensions" << std::endl;
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionsCount, availableExtensions.data());

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

    void Vulkan::CreateSurface(HINSTANCE hInstance, HWND hWnd)
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = hInstance;
        surfaceInfo.hwnd = hWnd;

        VkResult result = vkCreateWin32SurfaceKHR(m_Instance, &surfaceInfo, nullptr, &m_Surface);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create surface" << std::endl;
        }
    }

    void Vulkan::SelectPhysicalDevice()
    {
        uint32_t physicalDevicesCount;
        vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, nullptr);

        if (physicalDevicesCount == 0)
        {
            std::cout << "Unable to enumerate physical devices" << std::endl;
        }

        std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
        vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, physicalDevicesAvailable.data());

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
                m_PhysicalDevice = device;
                m_PhysicalDeviceProperties = deviceProps;
                m_PhysicalDeviceFeatures = deviceFeatures;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

            std::cout << "Unable to get discrette gpu, selected first one" << std::endl;
            std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
            m_PhysicalDevice = physicalDevicesAvailable[0];
            m_PhysicalDeviceProperties = deviceProps;
            m_PhysicalDeviceFeatures = deviceFeatures;
        }
    }

    void Vulkan::CreateLogicalDevice()
    {
        VerifyDeviceExtensionsSupport(m_DeviceExtensions);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.enabledExtensionCount = m_DeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
        createInfo.enabledLayerCount = m_ValidationLayers.size();
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        createInfo.pEnabledFeatures = &m_PhysicalDeviceFeatures;

        uint32_t queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertyCount, nullptr);

        if (queueFamilyPropertyCount == 0)
        {
            std::cout << "Failed to get physical device queue family properties" << std::endl;
        }

        std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

        int i = 0;
        for (const auto& family : availableQueueFamilyProperties) 
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                m_QueueIndices.GraphicsFamily = i;
            }
            if (presentSupport) 
            {
                m_QueueIndices.PresentFamily = i;
            }
            if (m_QueueIndices.IsComplete())
            {
                break;
            }
            i++;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { m_QueueIndices.GraphicsFamily.value(),
                                                  m_QueueIndices.PresentFamily.value() };
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

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create logical device" << std::endl;
        }

        vkGetDeviceQueue(m_LogicalDevice, m_QueueIndices.GraphicsFamily.value(), 0, &m_QueueIndices.GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, m_QueueIndices.PresentFamily.value(), 0, &m_QueueIndices.PresentQueue);
    }
}