#pragma once

#include <vector>
#include <optional>
#include <Windows.h>

#include "Core.h"

#include "Instance.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include "DebugUtilsMessenger.h"

#include "SwapChain.h"
#include "Win32Surface.h"
#include "Framebuffer.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Image.h"
#include "Synchronization.h"
#include "Descriptors.h"

#include "Mesh/Mesh.h"

namespace psm
{
    namespace vk
    {
        struct BaseVulkanAppData
        {
        public:
            void Init(HINSTANCE hInstance, HWND hWnd);
            void Deinit();

        public:
            //base
            VkInstance Instance;
            VkDevice Device;

            //gpu
            VkPhysicalDevice PhysicalDevice;
            VkPhysicalDeviceProperties PhysicalDeviceProperties;
            VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
            QueueFamilyIndices Queues;

            //debug
            VkDebugUtilsMessengerEXT DebugMessenger;

            //surface
            VkSurfaceKHR Surface;
            SurfaceData SurfaceData;

            //swapchain
            VkSwapchainKHR SwapChain;
            std::vector<VkImage> SwapChainImages;
            std::vector<VkImageView> SwapchainImageViews;
            VkFormat SwapChainImageFormat;
            VkExtent2D SwapChainExtent;
            VkSemaphore ImageAvailableSemaphore;
            VkSemaphore RenderFinishedSemaphore;
            std::vector<VkFramebuffer> Framebuffers; //foreach swapchain image

            //render pass
            VkRenderPass RenderPass;

            //pipeline
            VkPipelineLayout PipelineLayout;
            VkPipeline Pipeline;
            VkDescriptorSetLayout FragmentDescriptorSetLayout;
            VkDescriptorSetLayout VertexDescriptorSetLayout;

            //command buffers
            VkCommandPool CommandPool;
            std::vector<VkCommandBuffer> CommandBuffers;

            //extensions and layers
            std::vector<const char*> ValidationLayers =
            {
                "VK_LAYER_KHRONOS_validation",
                "VK_LAYER_LUNARG_monitor",
                "VK_LAYER_LUNARG_api_dump",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_LUNARG_object_tracker",
            };

            std::vector<const char*> InstanceExtensions =
            {
                "VK_KHR_surface",
                "VK_KHR_get_physical_device_properties2",
                "VK_KHR_win32_surface",
                "VK_EXT_debug_utils",
                "VK_EXT_debug_report"
            };

            std::vector<const char*> DeviceExtensions =
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