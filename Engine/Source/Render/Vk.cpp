#include "Vk.h"

namespace psm
{
    namespace vk
    {
        //base
        VkInstance Instance = VK_NULL_HANDLE;
        VkDevice Device = VK_NULL_HANDLE;
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties PhysicalDeviceProperties = {};
        VkPhysicalDeviceFeatures PhysicalDeviceFeatures = {};
        QueueFamilyIndices Queues = {};
        VkSurfaceKHR Surface = VK_NULL_HANDLE;
        SurfaceData SurfData = {};
        VkBuffer PerFrameBuffer = VK_NULL_HANDLE;

        //samplers
        VkSampler Sampler = VK_NULL_HANDLE;

        Vk* Vk::s_Instance = nullptr;;

        Vk* Vk::GetInstance()
        {
            if (s_Instance == nullptr)
            {
                s_Instance = new Vk();
            }

            return s_Instance;
        }

        void Vk::Init(HINSTANCE hInstance, HWND hWnd)
        {
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
            vk::PopulateDebugUtilsMessenger(DebugMessengerCallback, &debugMessengerInfo);

            vk::CreateInstance(m_ValidationLayers, m_InstanceExtensions, debugMessengerInfo, &m_Instance);
            vk::CreateDebugUtilsMessenger(m_Instance, DebugMessengerCallback, &m_DebugMessenger);
             vk::SelectPhysicalDevice(m_Instance, &m_PhysicalDeviceFeatures, &m_PhysicalDeviceProperties, &m_PhysicalDevice);

            vk::CreateSurface(hInstance, hWnd, m_Instance, &m_Surface);
            vk::PopulateSurfaceData(m_Instance, m_PhysicalDevice, m_Surface, &m_SurfaceData);

            vk::CreateLogicalDevice(m_DeviceExtensions, m_ValidationLayers, m_PhysicalDeviceFeatures, m_PhysicalDevice,
                m_Surface, &m_Queues, &m_Device);

            //base
            Instance = m_Instance;
            Device = m_Device;

            //gpu
            PhysicalDevice = m_PhysicalDevice;
            PhysicalDeviceProperties = m_PhysicalDeviceProperties;
            PhysicalDeviceFeatures = m_PhysicalDeviceFeatures;
            Queues = m_Queues;

            //surface
            Surface = m_Surface;
            SurfData = m_SurfaceData;

            //create image sampler
            vk::CreateTextureSampler(m_Device, VK_FILTER_LINEAR,
                VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_REPEAT, false, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                false, VK_COMPARE_OP_ALWAYS, 0, 0.0, 0.0, 0.0, VK_SAMPLER_MIPMAP_MODE_LINEAR, false, &m_Sampler);

            Sampler = m_Sampler;

            vk::CreateBufferAndMapMemory(m_Device, m_PhysicalDevice,
                sizeof(PerFrameData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &m_PerFrameBuffer, &m_PerFrameBufferMemory, &m_PerFrameMapping);

            PerFrameBuffer = m_PerFrameBuffer;
        }

        void Vk::UpdatePerFrameBuffer(const PerFrameData& data)
        {
            PerFrameData* pData = reinterpret_cast<PerFrameData*>(m_PerFrameMapping);
            *pData = data;
        }

        void Vk::Deinit()
        {
            vk::DestroySurface(m_Instance, m_Surface);
            vk::DestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger);
            vk::DestroySampler(m_Device, m_Sampler);
            vk::DestroyDevice(m_Device);
            vk::DestroyInstance(m_Instance);
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Vk::DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            std::cout << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }
    }
}