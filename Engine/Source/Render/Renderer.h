#pragma once

#include <Windows.h>

#include "Vk.h"
#include "Include/vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "Utilities/TextureLoader.h"
#include "Vulkan/Descriptors.h"
#include "Vulkan/Sampler.h"

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
        void Render(float deltaTime);
        void LoadDataIntoBuffer();
    private:

        //swapchain (abstract info vulkan windows class maybe)
        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderFinishedSemaphore;

        //render pass (should be simply to create)
        VkRenderPass m_RenderPass;
        std::vector<VkFramebuffer> m_Framebuffers;

        //command buffers (should be simply to create)
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        struct PushConstant
        {
            float Time;
        } m_PushConstant;

        //pipeline (should be simply to create)
        VkPipelineLayout PipelineLayout;
        VkPipeline Pipeline;
        VkDescriptorSetLayout ShaderDescriptorSetLayout;

        //uniform buffers for shaders (should be simply to create)
        struct ShaderUBO
        {
            glm::vec4 Offset;
            glm::vec4 Color;
        } m_VertexUBO;

        VkBuffer shaderBuffer;
        VkDeviceMemory shaderBufferMemory;
        void* shaderBufferMapping;
        VkDescriptorPool shaderUniformPool;
        VkDescriptorSet shaderUniformDescriptorSet;

        //move to model class
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        std::vector<Vertex> m_Vertices;

        //sample for shaders
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;

        VkImage cobbleImage;
        VkDeviceMemory cobbleImageMemory;
        VkImageView cobbleImageView;

        VkSampler sampler;

        //Imgui
        //vk::VulkanImGui m_VkImgui;
        //VkDescriptorPool m_ImGuiDescriptorsPool;//used for imgui
    };
}