#pragma once

#include <Windows.h>
#include <cassert>
#include <array>
#include <vector>

#include <queue>

#include "Utilities/ModelLoader.h"
#include "Actors/OpaqueInstances.h"
#include "GlobalBuffer.h"
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
        void Deinit();

        void Render(GlobalBuffer& buffer);
        void ResizeWindow(HWND hWnd);

        ImagePtr LoadTextureIntoMemory(const RawTextureData& textureData, uint32_t mipLevels);//should be moved inside DevicePtr
    private:
        void CreateDepthImage();
        void CreateSwapchain(HWND hWnd);
        void CreateSwapchainSyncObjects();
        void CreateFramebuffers();
        void PrepareShadowMapRenderPass();
        void ResizeWindowInternal(HWND hWnd);
        void CreateRenderFrameCommandBuffers();
        void CreateCommandPool();
        void CreateDefaultRenderPass();
        void CreateGlobalBuffer();
        //specific
        CommandBufferPtr BeginSingleTimeSubmitCommandBuffer();
        void SubmitSingleTimeCommandBuffer(CommandBufferPtr commandBuffer);

        void InitImGui(HWND hWnd);//needs to be initialized in another place

    private:

        //old
        HWND mWindow; //abstract it?
        HINSTANCE hInstance;

        //new RHI data
        DevicePtr mDevice;
        BufferPtr mGlobalBuffer;
        SwapchainPtr mSwapchain;

        //specific things
        RenderPassPtr mRenderPass;
        std::vector<FramebufferPtr> mFramebuffers;

        CommandPoolPtr mCommandPool;
        std::vector<CommandBufferPtr> mCommandBuffers;

        std::vector<SemaphorePtr> mImageAvailableSemaphores;
        std::vector<SemaphorePtr> mRenderFinishedSemaphores;
        std::vector<FencePtr> mFlightFences;

        bool isInit;
        uint32_t mCurrentFrame;
        uint64_t mTotalFrames;

        ImagePtr mDepthRenderTargetTexture;
        EFormat mDepthStencilFormat;

        RenderPassPtr mShadowMapRenderPass;
        std::vector<FramebufferPtr> mShadowMapFramebuffers;
        std::vector<ImagePtr> mDirDepthShadowMaps;

        //gui
        ImGuiPtr mGui;

        //resize handling (later add command queue)
        std::queue<HWND> mWindowResizeQueue;
    };
}