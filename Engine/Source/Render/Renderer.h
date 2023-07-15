#pragma once

#include <Windows.h>
#include <cassert>

#include "Vk.h"
#include "Include/vulkan/vulkan.hpp"
#include "Utilities/TextureLoader.h"
#include "Vulkan/Descriptors.h"
#include "Vulkan/Sampler.h"

#include "Utilities/ModelLoader.h"
#include "Instances/OpaqueInstances.h"
#include "PerFrameData.h"
#include "Vulkan/VulkanImGui.h"

#include "glm/glm.hpp"

namespace psm
{
    class Renderer
    {
        //singleton realization
    public:
        Renderer(Renderer&) = delete;
        void operator=(const Renderer&) = delete;
        static Renderer* Instance();
    private:
        Renderer() = default;
        static Renderer* s_Instance;
        //class specific
    public:
        void Init(HINSTANCE hInstance, HWND hWnd);
        void CreateDepthImage();
        void CreateMsaaImage();
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features);
        void Deinit();
        void Render(const PerFrameData& data);
        void LoadTextureIntoMemory(const RawTextureData& textureData, 
                                   uint32_t mipLevels, 
                                   Texture* texture);
        void ResizeWindow(HWND hWnd);
        void CreateSwapchain(HWND hWnd);
        void CreateFramebuffers();
        void InitImGui(HWND hWnd);
    private:
        //swapchain (abstract info vulkan windows class maybe)
        HWND m_Hwnd;
        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_FlightFences;
        uint32_t m_CurrentFrame;

        //renderer related thing
        VkRenderPass m_RenderPass;
        std::vector<VkFramebuffer> m_Framebuffers;

        //renderer related thing (maybe)
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        //depth image
        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;
        VkFormat m_DepthFormat;

        //msaa image
        VkImage m_MsaaImage;
        VkDeviceMemory m_MsaaImageMemory;
        VkImageView m_MsaaImageView;

        //Imgui
        VkDescriptorPool m_ImGuiDescriptorsPool;
    };
}