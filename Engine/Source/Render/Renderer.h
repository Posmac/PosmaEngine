#pragma once

#include <Windows.h>

#include "Vk.h"
#include "Include/vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "Vulkan/VulkanImGui.h"
#include "Vulkan/Descriptors.h"

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

        //swapchain
        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderFinishedSemaphore;

        //render pass
        VkRenderPass m_RenderPass;
        std::vector<VkFramebuffer> m_Framebuffers;

        //command buffers
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        struct PushConstant
        {
            float Time;
        } m_PushConstant;

        //pipeline
        VkPipelineLayout PipelineLayout;
        VkPipeline Pipeline;
        VkDescriptorSetLayout FragmentDescriptorSetLayout;
        VkDescriptorSetLayout VertexDescriptorSetLayout;

        //uniform buffers for shaders
        struct VertexShaderUBO
        {
            glm::vec4 Offset;
        } m_VertexUBO;

        struct FragmentShaderUBO
        {
            glm::vec4 color;
        } m_FragmentUBO;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        void* vertexBufferMapping;
        VkDescriptorPool vertexUniformPool;
        VkDescriptorSet vertexUniformDescriptorSet;

        VkBuffer fragmentBuffer;
        VkDeviceMemory fragmentBufferMemory;
        void* fragmentBufferMapping;
        VkDescriptorPool fragmentUniformPool;
        VkDescriptorSet fragmentUniformDescriptorSet;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        std::vector<Vertex> m_Vertices;

        //Imgui
        //VulkanImGui m_VkImgui;
        //VkDescriptorPool m_ImGuiDescriptorsPool;//used for imgui
    };
}