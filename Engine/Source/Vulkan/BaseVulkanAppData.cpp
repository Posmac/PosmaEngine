#include "BaseVulkanAppData.h"

namespace psm
{
    namespace vk
    {
        void BaseVulkanAppData::Init(HINSTANCE hInstance, HWND hWnd)
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
                &SwapChainExtent);
            vk::QuerrySwapchainImages(Device, SwapChain, SwapChainImageFormat, &SwapChainImages, &SwapchainImageViews);
            vk::CreateVkSemaphore(Device, 0, &ImageAvailableSemaphore);
            vk::CreateVkSemaphore(Device, 0, &RenderFinishedSemaphore);
            vk::CreateImageViews(Device, SwapChainImages, SwapChainImageFormat,
                VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &SwapchainImageViews);
            vk::CreateRenderPass(Device, SwapChainImageFormat, &RenderPass);

            VkShaderModule vertexShader; 
            VkShaderModule fragmentShader;
            vk::CreateShaderModule(Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
            vk::CreateShaderModule(Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

            vk::CreatePipeline(Device, vertexShader, fragmentShader, sizeof(Vertex), SwapChainExtent, RenderPass,
                &DescriptorSetLayout, &PipelineLayout, &Pipeline);
            vk::CreateFramebuffers(Device, SwapchainImageViews, SwapChainExtent, SwapchainImageViews.size(), 
                RenderPass, &Framebuffers);
            vk::CreateCommandPool(Device, Queues.GraphicsFamily.value(), &CommandPool);
            vk::CreateCommandBuffers(Device, CommandPool, SwapChainImages.size(), &CommandBuffers);

            vk::DestroyShaderModule(Device, vertexShader);
            vk::DestroyShaderModule(Device, fragmentShader);
        }

        void BaseVulkanAppData::Deinit()
        {
            vk::DestroyFramebuffers(Device, Framebuffers);
            vk::DestroyPipeline(Device, Pipeline);
            vk::DestroyPipelineLayout(Device, PipelineLayout);
            vk::DestroyRenderPass(Device, RenderPass);
            vk::DestroyImageViews(Device, SwapchainImageViews);
            vk::DestroySwapchain(Device, SwapChain);vk::DestroyDebugUtilsMessenger(Instance, DebugMessenger);
            vk::DestroySurface(Instance, Surface);
            vk::DestroySemaphore(Device, ImageAvailableSemaphore);
            vk::DestroySemaphore(Device, RenderFinishedSemaphore);
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