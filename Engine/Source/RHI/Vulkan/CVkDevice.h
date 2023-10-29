#pragma once

#include "RenderBackend/Core.h"
#include "RenderBackend/Queue.h"
#include "../Interface/Device.h"

namespace psm
{
    class CVkDevice : CDevice
    {
    public:
        CVkDevice() = default;
        virtual ~CVkDevice() = default;
    public:
        virtual RHI_RESULT InitInternals() override;
        virtual RHI_RESULT InitDevice(CSwapchain* pSwapchain) override;
        virtual RHI_RESULT CreateImage() override;
    public:
        VkInstance GetInstance();
        VkDevice GetDevice(); 
        VkPhysicalDevice GetPhysicalDevice();
    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    private:
        VkInstance m_Instance;
        VkDevice m_Device;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
        vk::QueueFamilyIndices m_Queues;
    };
}