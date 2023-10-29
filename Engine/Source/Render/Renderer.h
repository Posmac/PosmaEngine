#pragma once

#include <Windows.h>
#include <cassert>
#include <array>

#include "Utilities/TextureLoader.h"
#include "Utilities/ModelLoader.h"

#include "Instances/OpaqueInstances.h"

#include "PerFrameData.h"
#include "Shadows.h"

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
        Renderer();
        static Renderer* s_Instance;
        //class specific
    public:
        void Init(HINSTANCE hInstance, HWND hWnd);
        void CreateDepthImage();
        void CreateMsaaImage();
        void PrepareDirDepth();
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
        bool isInit;

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