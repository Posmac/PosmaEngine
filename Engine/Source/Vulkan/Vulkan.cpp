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

        std::vector<const char*> layers =
        {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_monitor",
            "VK_LAYER_LUNARG_api_dump",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_object_tracker",
        };
        VerifyLayersSupport(layers);

        std::vector<const char*> extensions =
        {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
            "VK_KHR_swapchain"
            "VK_KHR_get_physical_device_properties2",
            "VK_EXT_debug_utils",
            "VK_EXT_debug_report",
        };
        VerifyInstanceExtensionsSupport(extensions);

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = layers.size();
        instanceCreateInfo.ppEnabledLayerNames = layers.data();
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames= extensions.data();

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
                }
                else
                {
                    std::cout << "Layer: " << layerName << " isn`t supported" << std::endl;
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
            std::cout << "Failed to enumerate available extensions" << std::endl;
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
                }
                else
                {
                    std::cout << "Extension: " << extension << " isn`t supported" << std::endl;
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
            
            if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                std::cout << "Found discrette device: " << std::endl;
                std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                m_PhysicalDevice = device;
                m_PhysicalDeviceProperties = deviceProps;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);
            std::cout << "Unable to get discrette gpu, selected first one" << std::endl;
            std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
            m_PhysicalDevice = physicalDevicesAvailable[0];
            m_PhysicalDeviceProperties = deviceProps;
        }
    }
}