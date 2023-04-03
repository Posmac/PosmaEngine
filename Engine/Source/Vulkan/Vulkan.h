#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <optional>
#include <Windows.h>
#include <set>
#include <stdexcept>
#include <cstdlib>
#include <fstream>

#define VK_USE_PLATFORM_WIN32_KHR
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vulkan_core.h"
#include "Include/vulkan/vulkan_win32.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

#include "glm/glm.hpp"

namespace psm
{
    class Vulkan
    {
    public:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> GraphicsFamily;
            std::optional<uint32_t> PresentFamily;
            VkQueue GraphicsQueue;
            VkQueue PresentQueue;

            bool IsComplete()
            {
                return GraphicsFamily.has_value() && PresentFamily.has_value();
            }

            bool IdenticalQueues()
            {
                return GraphicsFamily.value() == PresentFamily.value();
            }
        };

        struct SurfaceData
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Formats;
            std::vector<VkPresentModeKHR> PresentModes;
        };

        struct Vertex
        {
            float x, y, z;
        };

        //singleton realization
    public:
        Vulkan(Vulkan&) = delete;
        void operator=(const Vulkan&) = delete;
        static Vulkan* Instance();
    private:
        Vulkan();
        static Vulkan* s_Instance;
        //class specific
    public:
        void Init(HINSTANCE hInstance, HWND hWnd);
        void Deinit();
        void InitVulkanInstace();
        void Render();
    private:
        void LoadModelData();
        void VerifyLayersSupport(std::vector< const char*>& layersToEnable);
        void VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        void CreateSurface(HINSTANCE hInstance, HWND hWnd);
        void PopulateSurfaceData();
        void SelectPhysicalDevice();
        void CreateLogicalDevice();
        void PopulateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo);
        void CreateDebugUtilsMessenger();
        void CreateSwapchain();
        void CheckFormatSupport(VkFormat& format);
        void CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace);
        void CheckPresentModeSupport(VkPresentModeKHR& presentMode);
        void QuerrySwapchainImages();
        void CreateRenderPass();
        void CreateDescriptorPool();
        void CreatePipeline();
        VkShaderModule CreateShaderModule(const std::string& path);
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:

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

        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
        VkDevice m_LogicalDevice;
        VkSurfaceKHR m_Surface;
        QueueFamilyIndices m_QueueIndices;
        VkDebugUtilsMessengerEXT m_DebugUtilsMessenger;
        SurfaceData m_SurfaceData;

        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        VkRenderPass m_RenderPass;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::vector<VkFramebuffer> m_Framebuffers;

        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        VkDescriptorPool m_DescriptorsPool;//for later use
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderFinishedSemaphore;

        std::vector<Vertex> m_Vertices;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    };
}