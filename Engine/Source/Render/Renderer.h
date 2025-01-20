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

#include "Passes/CompositeBackbufferRenderPass.h"
#include "Passes/ShadowMapRenderPass.h"
#include "Passes/GbuffferRenderPass.h"
#include "Passes/VisibilityBufferGeneratorRenderPass.h"
#include "Passes/VisibilityBufferShadeRenderPass.h"

#include "Pipelines/CompositeRenderPipelineNode.h"
#include "Pipelines/ShadowMapPipelineNode.h"
#include "Pipelines/GbufferPipelineNode.h"
#include "Pipelines/VisibilityBufferGeneratorPipeline.h"
#include "Pipelines/VisibilityBufferShadePipeline.h"

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
        void BeginComputeRender(CommandBufferPtr& commandBuffer);
        void BeginGraphicsRender(CommandBufferPtr& commandBuffer);

        void EndComputeRender(CommandBufferPtr& commandBuffer);
        void EndGraphicsRender(CommandBufferPtr& commandBuffer);

        //void SubmitGraphicsRender(CommandBufferPtr& commandBuffer, EPipelineStageFlags stageFlags, SemaphorePtr signalSemaphore, SemaphorePtr waitSemaphore);
        //void SubmitComputeRender(CommandBufferPtr& commandBuffer, EPipelineStageFlags stageFlags, SemaphorePtr signalSemaphore, SemaphorePtr waitSemaphore);

        //uint32_t GetSwapchainImage();
        //void Present(uint32_t imageIndex);

        void RegisterRenderPasses();
        void CreateSwapchain(HWND hWnd);
        void CreateSwapchainSyncObjects();
        void ResizeWindowInternal(HWND hWnd);
        void CreateRenderFrameCommandBuffers();
        void CreateCommandPool();
        void CreateGlobalBuffer();
        void CreateSampler();
        void CreateResourceManagers();
        

        void InitImGui(HWND hWnd);//needs to be initialized in another place
        void CreateDefaultDescriptorPool();

    private:

        //old
        HWND mWindow; //abstract it?
        HINSTANCE hInstance;

        //new RHI data
        DevicePtr mDevice;
        std::vector<BufferPtr> mGlobalBuffers;
        SwapchainPtr mSwapchain;

        //present submit
        std::vector<SemaphorePtr> mImageAvailableSemaphores;

        //graphics
        CommandPoolPtr mGraphicsCommandPool;
        std::vector<CommandBufferPtr> mGraphicsCommandBuffers;
        std::vector<SemaphorePtr> mGraphicsRenderFinishedSemaphores;
        std::vector<FencePtr> mGraphicsFlightFences;

        //present
        std::vector<CommandBufferPtr> mPresentCommandBuffers;

        //compute
        CommandPoolPtr mComputeCommandPool;
        std::vector<CommandBufferPtr> mComputeCommandBuffers;

        std::vector<SemaphorePtr> mComputeDispatchFinishedSemaphores;
        //std::vector<FencePtr> mComputeFlightFences;

        bool isInit;
        uint32_t mCurrentImageIndex;
        uint32_t mCurrentFrame;
        uint64_t mTotalFrames;

        //gui
        ImGuiPtr mGui;

        //resize handling (later add command queue)
        std::queue<HWND> mWindowResizeQueue;

        //graph
        graph::RenderGraphPtr mRenderGraph;
        graph::ResourceMediatorPtr mResourceMediator;
        graph::ResourceStateManagerPtr mResourceStateManager;
        //renderpasses
        graph::RenderPassNodePtr mCompositeBackbufferPass;
        graph::RenderPassNodePtr mShadowMapRenderPass;
        graph::RenderPassNodePtr mGbufferRenderPass;
        graph::RenderPassNodePtr mVisBufferGenRenderPass;
        graph::RenderPassNodePtr mVisBufferShadeRenderPass;
        //graphics pipelines
        graph::RenderPipelineNodePtr mCompositeRenderPipeline;
        graph::RenderPipelineNodePtr mShadowMapPipeline;
        graph::RenderPipelineNodePtr mGbufferPipelineNode;
        graph::RenderPipelineNodePtr mVisBufferGenPipelineNode;
        graph::RenderPipelineNodePtr mVisBufferShadePipelineNode;

        DescriptorPoolPtr mDescriptorPool;

        SamplerPtr mSampler;
    };
}