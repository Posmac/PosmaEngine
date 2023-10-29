#include "CVkDevice.h"
#include "CVkSwapchain.h"

#include "RenderBackend/BackedInfo.h"
#include "RenderBackend/DebugUtilsMessenger.h"
#include "RenderBackend/Instance.h"
#include "RenderBackend/PhysicalDevice.h"
#include "RenderBackend/LogicalDevice.h"

namespace psm
{
    RHI_RESULT CVkDevice::InitInternals()
    {
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        vk::PopulateDebugUtilsMessenger(DebugMessengerCallback, &debugMessengerInfo);

        vk::CreateInstance(m_ValidationLayers, m_InstanceExtensions, debugMessengerInfo, &m_Instance);
        vk::CreateDebugUtilsMessenger(m_Instance, DebugMessengerCallback, &m_DebugMessenger);
        vk::SelectPhysicalDevice(m_Instance, &m_PhysicalDeviceFeatures, &m_PhysicalDeviceProperties, &m_PhysicalDevice);

        return VK_SUCCESS;
    }

    RHI_RESULT CVkDevice::InitDevice(CSwapchain* pSwapchain)
    {
        CVkSwapchain* swapchain = (CVkSwapchain*)pSwapchain;

        vk::CreateLogicalDevice(m_DeviceExtensions, m_ValidationLayers, m_PhysicalDeviceFeatures, m_PhysicalDevice,
            swapchain->GetSurface(), &m_Queues, &m_Device);

        return VK_SUCCESS;
    }

    VkInstance CVkDevice::GetInstance()
    {
        return m_Instance;
    }

    VkDevice CVkDevice::GetDevice()
    {
        return m_Device;
    }

    VkPhysicalDevice CVkDevice::GetPhysicalDevice()
    {
        return m_PhysicalDevice;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL CVkDevice::DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                     VkDebugUtilsMessageTypeFlagsEXT messageTypes, 
                                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
                                                                     void* pUserData)
    {
        std::cout << pCallbackData->pMessage << std::endl << std::endl;
        return VK_FALSE;
    }
}