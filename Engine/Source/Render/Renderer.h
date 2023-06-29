#pragma once

#include <Windows.h>

#include "Vk.h"
#include "Include/vulkan/vulkan.hpp"
#include "Utilities/TextureLoader.h"
#include "Vulkan/Descriptors.h"
#include "Vulkan/Sampler.h"

#include "Utilities/ModelLoader.h"
#include "Instances/OpaqueInstances.h"
#include "PerFrameData.h"

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
        void Deinit();
        void Render(const PerFrameData& data);

    private:
        //swapchain (abstract info vulkan windows class maybe)
        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderFinishedSemaphore;

        //renderer related thing
        VkRenderPass m_RenderPass;
        std::vector<VkFramebuffer> m_Framebuffers;

        //renderer related thing (maybe)
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        //sample for shaders (move to textures loader)
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;

        VkImage cobbleImage;
        VkDeviceMemory cobbleImageMemory;
        VkImageView cobbleImageView;

        Model model;

        //Imgui
        //vk::VulkanImGui m_VkImgui;
        //VkDescriptorPool m_ImGuiDescriptorsPool;//used for imgui
    };
}