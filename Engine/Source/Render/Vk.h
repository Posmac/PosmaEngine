#pragma once

#include <vector>
#include <optional>
#include <Windows.h>

#include "Vulkan/Core.h"

#include "Vulkan/Instance.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/Queue.h"
#include "Vulkan/DebugUtilsMessenger.h"

#include "Vulkan/SwapChain.h"
#include "Vulkan/Win32Surface.h"
#include "Vulkan/Framebuffer.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Shader.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/Image.h"
#include "Vulkan/Synchronization.h"
#include "Vulkan/Descriptors.h"
#include "Vulkan/VulkanImGui.h"
#include "Vulkan/Sampler.h"
#include "PerFrameData.h"

namespace psm
{
    namespace vk
    {
        //base
        extern VkInstance Instance;
        extern VkDevice Device;

        //gpu
        extern VkPhysicalDevice PhysicalDevice;
        extern VkPhysicalDeviceProperties PhysicalDeviceProperties;
        extern VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
        extern QueueFamilyIndices Queues;

        //surface
        extern VkSurfaceKHR Surface;
        extern SurfaceData SurfData;

        //global samplers
        extern VkSampler Sampler;

        //global buffer
        extern VkBuffer PerFrameBuffer;

        //MSAA
        extern VkSampleCountFlagBits MaxMsaaSamples;

        class Vk
        {
            //singleton realization
        public:
            Vk(Vk&) = delete;
            void operator=(const Vk&) = delete;
            static Vk* GetInstance();
        private:
            Vk() = default;
            static Vk* s_Instance;
        public:
            void Init(HINSTANCE hInstance, HWND hWnd);
            void UpdatePerFrameBuffer(const PerFrameData& data);
            void Deinit();
        private:

            //base
            VkInstance m_Instance;
            VkDevice m_Device;

            //gpu
            VkPhysicalDevice m_PhysicalDevice;
            VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
            VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
            QueueFamilyIndices m_Queues;

            //surface
            VkSurfaceKHR m_Surface;
            SurfaceData m_SurfaceData;

            //debug
            VkDebugUtilsMessengerEXT m_DebugMessenger;

            //samplers
            VkSampler m_Sampler;

            //per frame buffer
            VkBuffer m_PerFrameBuffer;
            VkDeviceMemory m_PerFrameBufferMemory;
            void* m_PerFrameMapping;

            //msaa
            VkSampleCountFlagBits m_MaxMsaaSamples;

            //extensions and layers
            std::vector<const char*> m_ValidationLayers =
            {
                "VK_LAYER_KHRONOS_validation",
                "VK_LAYER_LUNARG_monitor",
                "VK_LAYER_LUNARG_api_dump",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_LUNARG_object_tracker",
            };

            std::vector<const char*> m_InstanceExtensions =
            {
                "VK_KHR_surface",
                "VK_KHR_get_physical_device_properties2",
                "VK_KHR_win32_surface",
                "VK_EXT_debug_utils",
                "VK_EXT_debug_report"
            };

            std::vector<const char*> m_DeviceExtensions =
            {
                "VK_KHR_swapchain",
            };

        private:
            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);
        };
    }
}