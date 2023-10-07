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

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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
        void PrepareDirDepth();
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features);
        void Deinit();
        void Render(PerFrameData& data);
        void LoadTextureIntoMemory(const RawTextureData& textureData, 
                                   uint32_t mipLevels, 
                                   Texture* texture);
        void ResizeWindow(HWND hWnd);
        void CreateSwapchain(HWND hWnd);
        void CreateFramebuffers();
        void InitImGui(HWND hWnd);
        void PrepareOffscreenRenderpass();
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

        //dir light depth image
        std::vector<VkImage> m_DirDepthImage;
        std::vector<VkDeviceMemory> m_DirDepthImageMemory;
        std::vector<VkImageView> m_DirDepthImageView;

        VkFormat m_DirDepthFormat;
        VkExtent3D m_DirDepthSize;

        glm::mat4 m_DirViewProjMatrix;

        //shadow buffer data
        VkBuffer m_DirShadowBuffer;
        VkDeviceMemory m_DirShadowBufferMemory;
        void* m_DirShadowBufferMapping;

        //renderer related thing
        VkRenderPass m_ShadowRenderPass;
        std::vector<VkFramebuffer> m_ShadowFramebuffers;

        //Imgui
        VkDescriptorPool m_ImGuiDescriptorsPool;

        //imgui data
        float range = 77;
        float nearPlane = -70;
        float farPlane = 70;
        glm::vec3 position = glm::vec3(-1.0f, 1.0f, 0.0f);
        glm::vec3 lookAt;
        glm::vec3 up = glm::vec3(0.0f, 1.0f ,0.0f);

        //shadow biasing
        float depthBias = -1;
        float depthSlope = -1;
    };
}