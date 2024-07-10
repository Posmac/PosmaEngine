#pragma once

#include <Windows.h>
#include <cassert>
#include <array>
#include <vector>

#include <queue>

#include "Utilities/ModelLoader.h"
#include "Actors/OpaqueInstances.h"
#include "GlobalBuffer.h"
#include "ShadowsGenerator.h"

#include "RHI/Interface/Types.h"
#include "RHI/Interface/Device.h"

#include "RHI/Enums/ImageFormats.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include "Graph/RenderGraph.h"

#include "Passes/DefaultBackbufferRenderPass.h"
#include "Passes/ShadowMapRenderPass.h"

#include "Pipelines/DefaultRenderPipelineNode.h"
#include "Pipelines/ShadowMapPipelineNode.h"

#include "Graph/ResourceMediator.h"
#include "Graph/ResourceStateManager.h"

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

    private:
        uint32_t BeginRender();
        void EndRender(uint32_t imageIndex);

        void RegisterRenderPasses();
        void CreateSwapchain(HWND hWnd);
        void CreateSwapchainSyncObjects();
        void ResizeWindowInternal(HWND hWnd);
        void CreateRenderFrameCommandBuffers();
        void CreateCommandPool();
        void CreateGlobalBuffer();
        void CreateSampler();
        //specific
        CommandBufferPtr BeginSingleTimeSubmitCommandBuffer();
        void SubmitSingleTimeCommandBuffer(CommandBufferPtr commandBuffer);

        void InitImGui(HWND hWnd);//needs to be initialized in another place
        void CreateDefaultDescriptorPool();

    private:

        //old
        HWND mWindow; //abstract it?
        HINSTANCE hInstance;

        //new RHI data
        DevicePtr mDevice;
        BufferPtr mGlobalBuffer;
        SwapchainPtr mSwapchain;

        //specific things
        //RenderPassPtr mRenderPass;
        //std::vector<FramebufferPtr> mFramebuffers;

        CommandPoolPtr mCommandPool;
        std::vector<CommandBufferPtr> mCommandBuffers;

        std::vector<SemaphorePtr> mImageAvailableSemaphores;
        std::vector<SemaphorePtr> mRenderFinishedSemaphores;
        std::vector<FencePtr> mFlightFences;

        bool isInit;
        uint32_t mCurrentFrame;
        uint64_t mTotalFrames;

        //gui
        ImGuiPtr mGui;

        //resize handling (later add command queue)
        std::queue<HWND> mWindowResizeQueue;

        //graph
        graph::RenderGraph mRenderGraph;
        graph::ResourceMediatorPtr mResourceMediator;
        graph::ResourceStateManagerPtr mResourceStateManager;
        //renderpasses
        graph::RenderPassNodePtr mDefaultBackbufferPass;
        graph::RenderPassNodePtr mShadowMapRenderPass;
        //graphics pipelines
        graph::RenderPipelineNodePtr mDefaultRenderPipeline;
        graph::RenderPipelineNodePtr mShadowMapPipeline;

        DescriptorPoolPtr mDescriptorPool;

        SamplerPtr mSampler;
    };
}