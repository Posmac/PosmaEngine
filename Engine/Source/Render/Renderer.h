#pragma once

#include <Windows.h>
#include <cassert>
#include <array>

//#include "../RHI/Interface/Device.h"
//#include "../RHI/Interface/Swapchain.h"
//#include "../RHI/Interface/Buffer.h"
//#include "../RHI/Interface/ImGui.h"
//#include "../RHI/Interface/Image.h"
//
//#include "Utilities/TextureLoader.h"
//#include "Utilities/ModelLoader.h"
//
//#include "Actors/OpaqueInstances.h"
//
//#include "PerFrameData.h"
#include "Shadows.h"

#include "RHI/Interface/Types.h"
#include "RHI/Interface/Device.h"

#include "RHI/Enums/ImageFormats.h"

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
        void Init(DevicePtr device, const PlatformConfig& config);
        void CreateDepthImage();
        void CreateMsaaImage();
        void PrepareDirDepth();
        void Deinit();
        void Render();

       /* void Render(PerFrameData& data);
        void LoadTextureIntoMemory(const RawTextureData& textureData, 
                                   uint32_t mipLevels, 
                                   Texture* texture);*/
        void ResizeWindow(HWND hWnd);
        void CreateSwapchain(HWND hWnd);
        void CreateFramebuffers();
        void InitImGui(HWND hWnd);
        void PrepareOffscreenRenderpass();
    private:

        //new RHI data
        DevicePtr mDevice;
        SwapchainPtr mSwapchain;
        RenderPassPtr mRenderPass;

        CommandPoolPtr mCommandPool;
        CommandBufferPtr mCommandBuffers;

        std::vector<SemaphorePtr> mImageAvailableSemaphores;
        std::vector<SemaphorePtr> mRenderFinishedSemaphores;
        std::vector<FencePtr> mFlightFences;

        bool isInit;
        uint32_t mCurrentFrame;

        ImagePtr mDepthRenderTargetTexture;
        EImageFormat mDepthStencilFormat;

        ImagePtr mRenderTarget;

        //CDevicePtr m_Device;
        //CSwapchainPtr m_Swapchain;

        //CBufferPtr m_PerFrameBuffer;
        //CImagePtr m_DepthImage;
        //CImagePtr m_MsaaImage;

        ////renderer related thing
        //VkRenderPass m_RenderPass;
        //std::vector<VkFramebuffer> m_Framebuffers;

        ////renderer related thing (maybe)
        //VkCommandPool m_CommandPool;
        //std::vector<VkCommandBuffer> m_CommandBuffers;

        //VkImage m_DepthImage;
        //VkDeviceMemory m_DepthImageMemory;
        //VkImageView m_DepthImageView;
        //VkFormat m_DepthFormat;

        //msaa image

        /*VkImage m_MsaaImage;
        VkDeviceMemory m_MsaaImageMemory;
        VkImageView m_MsaaImageView;*/
    };
}