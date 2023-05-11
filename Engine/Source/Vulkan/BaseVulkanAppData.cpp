#include "BaseVulkanAppData.h"

namespace psm
{
    namespace vk
    {
        bool BaseVulkanAppData::Init(HINSTANCE hInstance, HWND hWnd)
        {
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
            vk::PopulateDebugUtilsMessenger(DebugMessengerCallback, &debugMessengerInfo);

            vk::CreateInstance(ValidationLayers, InstanceExtensions, debugMessengerInfo, &Instance);
            vk::CreateDebugUtilsMessenger(Instance, DebugMessengerCallback, &DebugMessenger);
            vk::CreateSurface(hInstance, hWnd, Instance, &Surface);
            vk::SelectPhysicalDevice(Instance, &PhysicalDeviceFeatures, &PhysicalDeviceProperties, &PhysicalDevice);
            vk::PopulateSurfaceData(Instance, PhysicalDevice, Surface, &SurfaceData);
            vk::CreateLogicalDevice(DeviceExtensions, ValidationLayers, PhysicalDeviceFeatures, PhysicalDevice,
                Surface, &Queues, &Device);
            vk::CreateSwapchain(Device, PhysicalDevice, Surface, SurfaceData, &SwapChain, &SwapChainImageFormat,
                &SwapChainExtent, &ImageAvailableSemaphore, &RenderFinishedSemaphore);
            vk::QuerrySwapchainImages(Device, SwapChain, SwapChainImageFormat, &SwapChainImages, &SwapchainImageViews);
            vk::CreateRenderPass(Device, SwapChainImageFormat, &RenderPass);

            VkShaderModule vertexShader = vk::CreateShaderModule(Device, "../Engine/Shaders/triangle.vert.txt");
            VkShaderModule fragmentShader = vk::CreateShaderModule(Device, "../Engine/Shaders/triangle.frag.txt");

            vk::CreatePipeline(Device, vertexShader, fragmentShader, sizeof(Vertex), SwapChainExtent, RenderPass,
                &DescriptorSetLayout, &PipelineLayout, &Pipeline);
            vk::CreateFramebuffers(Device, SwapchainImageViews, SwapChainExtent, SwapchainImageViews.size(), RenderPass, &Framebuffers);
            vk::CreateCommandPool(Device, Queues.GraphicsFamily.value(), &CommandPool);
            vk::CreateCommandBuffers(Device, CommandPool, SwapChainImages.size(), &CommandBuffers);

            return true;
        }

        void BaseVulkanAppData::Deinit()
        {
            vk::DestroyFramebuffers(Device, Framebuffers);
            vk::DestroyPipeline(Device, Pipeline);
            vk::DestroyPipelineLayout(Device, PipelineLayout);
            vk::DestroyRenderPass(Device, RenderPass);
            vk::DestroyImageViews(Device, SwapchainImageViews);
            vk::DestroySwapchain(Device, SwapChain);
            vk::DestroyDebugUtilsMessenger(Instance, DebugMessenger);
            vk::DestroySurface(Instance, Surface);
            vk::DestroyDevice(Device);
            vk::DestroyInstance(Instance);
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL BaseVulkanAppData::DebugMessengerCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            std::cout << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }
    }
}