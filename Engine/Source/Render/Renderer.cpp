#include "Renderer.h"

#include "RHI/Enums/RenderPassFormats.h"

#include "RHI/Configs/SwapchainConfig.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "RHI/Configs/FramebuffersConfig.h"
#include "RHI/Configs/Barriers.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkSwapchain.h"
#include "RHI/Vulkan/CVkImage.h"
#include "RHI/Vulkan/CVkFence.h"
#include "RHI/Vulkan/CVkCommandBuffer.h"
#include "RHI/Vulkan/CVkCommandPool.h"
#include "RHI/Vulkan/CVkRenderPass.h"
#include "RHI/Vulkan/CVkBuffer.h"
#include "RHI/Vulkan/CVkImGui.h"
#endif

#include "imgui/imgui.h"

#include "Graph/GraphResourceNames.h"
#include "Graph/ResourceGeneralInfo.h"
#include "Passes/VisibilityBufferShadeRenderPass.h"

namespace psm
{
    Renderer* Renderer::s_Instance = nullptr;

    Renderer* Renderer::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new Renderer();
        }

        return s_Instance;
    }

    Renderer::Renderer()
    {
        isInit = false;
    }

    void Renderer::Init(DevicePtr device, const PlatformConfig& config)
    {
        mCurrentFrame = 0;

        mDevice = device;

        hInstance = static_cast<HINSTANCE>(config.win32.hInstance);

        //init general things for rendering
        CreateSwapchain(static_cast<HWND>(config.win32.hWnd));
        CreateSwapchainSyncObjects();
        CreateCommandPool();
        CreateRenderFrameCommandBuffers();
        CreateDefaultDescriptorPool();

        CreateResourceManagers();
        CreateGlobalBuffer();
        OpaqueInstances::GetInstance()->Init(mDevice, mResourceMediator, mDescriptorPool);
        ShadowsGenerator::Instance()->Init(mDevice, mResourceMediator);

        RegisterRenderPasses();
        CreateSampler();

        ModelLoader::Instance()->Init(mDevice, mGraphicsCommandPool);
        TextureLoader::Instance()->Init(mDevice, mGraphicsCommandPool);

        InitImGui(static_cast<HWND>(config.win32.hWnd));

        isInit = true;
    }

    void Renderer::RegisterRenderPasses()
    {
        // c - compute, g - graphics
        // (SMP) SHADOW MAP PASS(g) -> (GBP) GBUFFER PASS(g) -> (VGP) VISBUFFER GEN PASS(g) -> (VSP) VISBUFFER SHADE PASS(c) -> (CMP) COMPOSITE PASS(g)

        // CMDBUF - graphics command buffer
        // COMPBUF - compute command buffer
        // CMDBUF START -> SMP -> GBP -> VGP -> CMDBUF END -> COMPBUF START -> VSP -> COMPBUF END -> CMDBUF START -> CMP -> CMDBUF END -> PRESENT

        //SMP
        auto preShadowsLambda = [this]()
        {
            BeginGraphicsRender(mGraphicsCommandBuffers[mCurrentFrame]);
        };

        auto renderShadowsLambda = [this]()
        {
            OpaqueInstances::GetInstance()->UpdateDepthDescriptors(mCurrentFrame);
            OpaqueInstances::GetInstance()->RenderDepth(mGraphicsCommandBuffers[mCurrentFrame], mShadowMapPipeline);
        };

        auto postShadowsLambda = [this]()
        {
        };

        //GBP
        auto preGbufferLambda = [this]()
        {
        };

        auto renderGbufferLambda = [this]()
        {
            OpaqueInstances::GetInstance()->UpdateDefaultDescriptors(mCurrentFrame);
            OpaqueInstances::GetInstance()->RenderGbuffer(mGraphicsCommandBuffers[mCurrentFrame], mGbufferPipelineNode);
        };

        auto postGbufferLambda = [this]()
        {
        };

        //VGP
        auto preVisBufferLambda = [this]()
        {
        };

        auto renderVisBufferLambda = [this]()
        {
            OpaqueInstances::GetInstance()->RenderVisBuffer(mGraphicsCommandBuffers[mCurrentFrame], mVisBufferGenPipelineNode);
        };

        auto postVisBufferLambda = [this]()
        {
            EndGraphicsRender(mGraphicsCommandBuffers[mCurrentFrame]);

            SSubmitConfig submitConfig =
            {
                .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
                .SubmitCount = 1,
                .WaitStageFlags = {},
                .WaitSemaphores = {},
                .CommandBuffers = {mGraphicsCommandBuffers[mCurrentFrame]},
                .SignalSemaphores = {mGraphicsRenderFinishedSemaphores[mCurrentFrame]},
                .Fence = mGraphicsFlightFences[mCurrentFrame],
            };

            mDevice->SubmitQueue(submitConfig);

            SFenceWaitConfig fenceWait =
            {
                .WaitAll = true,
                .Timeout = FLT_MAX,
            };

            while(!mGraphicsFlightFences[mCurrentFrame]->Wait(fenceWait))
            {
                LogMessage(MessageSeverity::Info, "Waiting for graphics fence");
            };

            mGraphicsFlightFences[mCurrentFrame]->Reset();
        };

        //VSP
        auto preVisBufShadeLambda = [this]()
        {
            BeginComputeRender(mComputeCommandBuffers[mCurrentFrame]);
        };

        auto dispathVisBufShadeLambda = [this]()
        {
            mVisBufferShadePipelineNode->Bind(mComputeCommandBuffers[mCurrentFrame], EPipelineBindPoint::COMPUTE);

            DescriptorSetPtr set = mResourceMediator->GetDescriptorSetByName(graph::VISBUF_SHADE_SET);
            mDevice->BindDescriptorSets(mComputeCommandBuffers[mCurrentFrame], EPipelineBindPoint::COMPUTE, mVisBufferShadePipelineNode->GetPipelineLayout(), { set });

            mDevice->Dispatch(mComputeCommandBuffers[mCurrentFrame], graph::PARTICLE_COUNT / 256, 1, 1);
        };

        auto postVisBufShadeLambda = [this]()
        {
            EndComputeRender(mComputeCommandBuffers[mCurrentFrame]);

            SSubmitConfig submitComputeConfig =
            {
                .Queue = mDevice->GetDeviceData().vkData.ComputeQueue,
                .SubmitCount = 1,
                .WaitStageFlags = {EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT},
                .WaitSemaphores = {mGraphicsRenderFinishedSemaphores[mCurrentFrame]},
                .CommandBuffers = {mComputeCommandBuffers[mCurrentFrame]},
                .SignalSemaphores = {mComputeDispatchFinishedSemaphores[mCurrentFrame]},
                .Fence = nullptr
            };

            mDevice->SubmitQueue(submitComputeConfig);
        };

        //CMP
        auto preCompositeLambda = [this]()
        {
            BeginGraphicsRender(mGraphicsCommandBuffers[mCurrentFrame]);
        };

        auto renderCompositeLambda = [this]()
        {
            //update descriptors from Gbuffer and shadow map
            OpaqueInstances::GetInstance()->UpdateGBufferDescriptors(mCurrentFrame);

            //draw full quad
            CommandBufferPtr& commandBuffer = mGraphicsCommandBuffers[mCurrentFrame];
            mCompositeRenderPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

            OpaqueInstances::GetInstance()->BindCompositeDescriptors(commandBuffer, mCompositeRenderPipeline);

            mDevice->Draw(commandBuffer, 3, 1, 0, 0); //draw full screen triangle

            //render gui
            mGui->PrepareNewFrame();
            {
                ShadowsGenerator::Instance()->DrawShadowParams();
            }
            mGui->Render(mGraphicsCommandBuffers[mCurrentFrame]);
        };

        auto postCompositeLambda = [this]()
        {
            EndGraphicsRender(mGraphicsCommandBuffers[mCurrentFrame]);

            //get next image
            uint32_t imageIndex;
            SSwapchainAquireNextImageConfig nextImageConfig
            {
                .Timeout = UINT64_MAX,
                .Semaphore = mImageAvailableSemaphores[mCurrentImageIndex]
            };

            mSwapchain->GetNextImage(nextImageConfig, &imageIndex);

            //submit graphics renderer
            SSubmitConfig submitConfig =
            {
                .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
                .SubmitCount = 1,
                .WaitStageFlags = {EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT, EPipelineStageFlags::COMPUTE_SHADER_BIT},
                .WaitSemaphores = {mImageAvailableSemaphores[mCurrentImageIndex], mComputeDispatchFinishedSemaphores[mCurrentFrame]},
                .CommandBuffers = {mGraphicsCommandBuffers[mCurrentFrame]},
                .SignalSemaphores = {mGraphicsRenderFinishedSemaphores[mCurrentFrame]},
                .Fence = mGraphicsFlightFences[mCurrentFrame],
            };

            mDevice->SubmitQueue(submitConfig);

            //present
            SFenceWaitConfig fenceWait =
            {
                .WaitAll = true,
                .Timeout = FLT_MAX,
            };

            SPresentConfig presentConfig =
            {
                .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
                .WaitSemaphoresCount = 1,
                .pWaitSemaphores = &mGraphicsRenderFinishedSemaphores[mCurrentFrame],
                .ImageIndex = imageIndex
            };

            mSwapchain->Present(presentConfig);

            while(!mGraphicsFlightFences[mCurrentFrame]->Wait(fenceWait))
            {
                LogMessage(MessageSeverity::Info, "Waiting for graphics fence");
            };

            mGraphicsFlightFences[mCurrentFrame]->Reset();

            mCurrentImageIndex = imageIndex;
        };

        // create render passes
        mCompositeBackbufferPass = std::make_shared<graph::CompositeBackbufferRenderPassNode>(graph::COMPOSITE_RENDERPASS,
                                                                                          mDevice,
                                                                                          mResourceMediator,
                                                                                          mSwapchain,
                                                                                          mSwapchain->GetSwapchainImageFormat());

        mShadowMapRenderPass = std::make_shared<graph::ShadowMapRenderPassNode>(graph::SHADOWMAP_RENDERPASS,
                                                                                mResourceMediator,
                                                                                mDevice,
                                                                                graph::SHADOW_MAP_SIZE,
                                                                                mSwapchain->GetImagesCount());

        mGbufferRenderPass = std::make_shared<graph::GbuffferRenderPassNode>(graph::GBUFFER_RENDERPASS,
                                                                             mDevice,
                                                                             mResourceMediator,
                                                                             mSwapchain->GetSwapchainSize(),
                                                                             mSwapchain->GetImagesCount());

        mVisBufferGenRenderPass = std::make_shared<graph::VisibilityBufferGeneratorRenderPassNode>(graph::VISBUF_GENERATION_RENDERPASS,
                                                                             mDevice,
                                                                             mResourceMediator,
                                                                             mSwapchain->GetSwapchainSize(),
                                                                             mSwapchain->GetImagesCount());

        mVisBufferShadeRenderPass = std::make_shared<graph::VisibilityBufferShadeRenderPassNode>(graph::VISBUF_SHADE_RENDERPASS,
                                                                                                 mDevice,
                                                                                                 mResourceMediator,
                                                                                                 mComputeCommandPool,
                                                                                                 mGraphicsCommandPool,
                                                                                                 mDescriptorPool,
                                                                                                 mSwapchain->GetSwapchainSize(),
                                                                                                 mSwapchain->GetImagesCount());

        mCompositeBackbufferPass->AddRenderCallback(renderCompositeLambda);
        mCompositeBackbufferPass->AddPreRenderCallback(preCompositeLambda);
        mCompositeBackbufferPass->AddPostRenderCallback(postCompositeLambda);

        mShadowMapRenderPass->AddRenderCallback(renderShadowsLambda);
        mShadowMapRenderPass->AddPreRenderCallback(preShadowsLambda);
        mShadowMapRenderPass->AddPostRenderCallback(postShadowsLambda);

        mGbufferRenderPass->AddRenderCallback(renderGbufferLambda);
        mGbufferRenderPass->AddPreRenderCallback(preGbufferLambda);
        mGbufferRenderPass->AddPostRenderCallback(postGbufferLambda);

        mVisBufferGenRenderPass->AddRenderCallback(renderVisBufferLambda);
        mVisBufferGenRenderPass->AddPreRenderCallback(preVisBufferLambda);
        mVisBufferGenRenderPass->AddPostRenderCallback(postVisBufferLambda);

        mVisBufferShadeRenderPass->AddRenderCallback(dispathVisBufShadeLambda);
        mVisBufferShadeRenderPass->AddPreRenderCallback(preVisBufShadeLambda);
        mVisBufferShadeRenderPass->AddPostRenderCallback(postVisBufShadeLambda);

        //add everything into graph
        mRenderGraph = std::make_shared<graph::RenderGraph>();

        mRenderGraph->AddGraphicsRenderPass(mShadowMapRenderPass);      //shadow map generation renderpass
        mRenderGraph->AddGraphicsRenderPass(mGbufferRenderPass);        //add gbuffer pass
        mRenderGraph->AddGraphicsRenderPass(mVisBufferGenRenderPass);   //visibility buffer generation render pass
        mRenderGraph->AddComputeRenderPass(mVisBufferShadeRenderPass); //visibility buffer shade render pass
        mRenderGraph->AddGraphicsRenderPass(mCompositeBackbufferPass);  //add default backbuffer renderpass

        //create all necessary resources and link them
        mRenderGraph->GenerateResourceDependencies(mSwapchain->GetImagesCount());

        //create render pipelines
        mCompositeRenderPipeline = std::make_shared<graph::CompositeRenderPipelineNode>(graph::COMPOSITE_GRAPHICS_PIPELINE,
                                                                                    mDevice,
                                                                                    mCompositeBackbufferPass->GetRenderPass(),
                                                                                    mResourceMediator,
                                                                                    mSwapchain->GetSwapchainSize());

        mShadowMapPipeline = std::make_shared<graph::ShadowMapPipelineNode>(graph::SHADOWMAP_GRAPHICS_PIPELINE,
                                                                            mDevice,
                                                                            mShadowMapRenderPass->GetRenderPass(),
                                                                            mResourceMediator,
                                                                            graph::SHADOW_MAP_SIZE);

        mGbufferPipelineNode = std::make_shared<graph::GbufferPipelineNode>(graph::GBUFFER_GRAPHICS_PIPELINE,
                                                                            mDevice,
                                                                            mGbufferRenderPass->GetRenderPass(),
                                                                            mResourceMediator,
                                                                            mSwapchain->GetSwapchainSize());

        mVisBufferGenPipelineNode = std::make_shared<graph::VisibilityBufferGeneratorPipelineNode>(graph::VISBUF_GENERATION_PIPELINE,
                                                                            mDevice,
                                                                            mVisBufferGenRenderPass->GetRenderPass(),
                                                                            mResourceMediator,
                                                                            mSwapchain->GetSwapchainSize());

        mVisBufferShadePipelineNode = std::make_shared<graph::VisibilityBufferShadePipelineNode>(graph::VISBUF_SHADE_PIPELINE,
                                                                            mDevice,
                                                                            mResourceMediator);
    }

    void Renderer::Deinit()
    {
        isInit = false;
        mWindowResizeQueue = {};

        while(!mDevice->WaitIdle())
        {
            LogMessage(MessageSeverity::Warning, "Wait idle");
        }

        ShadowsGenerator::Instance()->Deinit();
        OpaqueInstances::GetInstance()->Deinit();
        ModelLoader::Instance()->Deinit();
        TextureLoader::Instance()->Deinit();

        mRenderGraph = nullptr;
        mResourceMediator = nullptr;
        mResourceStateManager = nullptr;
        mCompositeBackbufferPass = nullptr;
        mCompositeRenderPipeline = nullptr;
        mShadowMapPipeline = nullptr;
        mShadowMapRenderPass = nullptr;

        mImageAvailableSemaphores.clear();;
        mGraphicsRenderFinishedSemaphores.clear();
        mGraphicsFlightFences.clear();

        mGlobalBuffers.clear();
        mSwapchain = nullptr;

        mSampler = nullptr;
        mDescriptorPool = nullptr;

        mGraphicsCommandPool->FreeCommandBuffers(mGraphicsCommandBuffers);
        /*for(auto& cb : mGraphicsCommandBuffers)
            cb = nullptr;*/
        mGraphicsCommandBuffers.clear();

        mGraphicsCommandPool = nullptr;

        mGui = nullptr;
        mDevice = nullptr;
    }

    void Renderer::Render(GlobalBuffer& buffer)
    {
        if(mWindowResizeQueue.size() > 0)
        {
            ResizeWindowInternal(mWindowResizeQueue.front());
            mWindowResizeQueue.pop();
        }

        if(!isInit)
        {
            return;
        }

        mDevice->UpdateBuffer(mGlobalBuffers[mCurrentFrame], &buffer);
        ShadowsGenerator::Instance()->Update();

        mCompositeBackbufferPass->SetState(mGraphicsCommandBuffers[mCurrentFrame], mCurrentFrame);
        mShadowMapRenderPass->SetState(mGraphicsCommandBuffers[mCurrentFrame], mCurrentFrame);
        mGbufferRenderPass->SetState(mGraphicsCommandBuffers[mCurrentFrame], mCurrentFrame);
        mVisBufferGenRenderPass->SetState(mGraphicsCommandBuffers[mCurrentFrame], mCurrentFrame);
        mVisBufferShadeRenderPass->SetState(mComputeCommandBuffers[mCurrentFrame], mCurrentFrame);

        mRenderGraph->Render();

        mCompositeBackbufferPass->ResetState();
        mShadowMapRenderPass->ResetState();
        mGbufferRenderPass->ResetState();
        mVisBufferGenRenderPass->ResetState();
        mVisBufferShadeRenderPass->ResetState();

        mCurrentFrame = (mCurrentFrame + 1) % mSwapchain->GetImagesCount();
        mTotalFrames++;
    }

    void Renderer::ResizeWindowInternal(HWND hWnd)
    {
        if(mDevice == nullptr)
            return;

        while(!mDevice->WaitIdle())
        {
            LogMessage(MessageSeverity::Warning, "Wait idle");
        }

        mSwapchain = nullptr;

        mWindow = hWnd;
        psm::PlatformConfig platformConfig =
        {
            .win32 =
            {
                .hInstance = hInstance,
                .hWnd = mWindow
            }
        };

        mDevice->CreateSurface(platformConfig);
        CreateSwapchain(mWindow);
        mCompositeBackbufferPass->RecreateFramebuffers(mSwapchain);
    }

    void Renderer::CreateRenderFrameCommandBuffers()
    {
        SCommandBufferConfig graphicsCmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mGraphicsCommandBuffers = mDevice->CreateCommandBuffers(mGraphicsCommandPool, graphicsCmdBuffersConfig);

        SCommandBufferConfig computeCmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mComputeCommandBuffers = mDevice->CreateCommandBuffers(mComputeCommandPool, computeCmdBuffersConfig);
    }

    void Renderer::CreateCommandPool()
    {
        SCommandPoolConfig graphicsCmdPoolConfig =
        {
            .QueueFamilyIndex = mDevice->GetDeviceData().vkData.GraphicsQueueIndex,
            .QueueType = EQueueType::GRAHPICS
        };

        mGraphicsCommandPool = mDevice->CreateCommandPool(graphicsCmdPoolConfig);

        SCommandPoolConfig computeCmdPoolConfig =
        {
            .QueueFamilyIndex = mDevice->GetDeviceData().vkData.ComputeQueueIndex,
            .QueueType = EQueueType::COMPUTE
        };

        mComputeCommandPool = mDevice->CreateCommandPool(computeCmdPoolConfig);
    }

    void Renderer::CreateGlobalBuffer()
    {
        for(int i = 0; i < mSwapchain->GetImagesCount(); i++)
        {
            SBufferConfig bufferConfig =
            {
                .Size = sizeof(GlobalBuffer),
                .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
            };


            BufferPtr buffer = mDevice->CreateBuffer(bufferConfig);
            mGlobalBuffers.push_back(buffer);

            foundation::Name name = graph::GetResourceIndexedName(graph::GLOBAL_CBUFFER, i);
            mResourceMediator->RegisterBufferResource(name, buffer);
        }
    }

    void Renderer::CreateSampler()
    {
        SSamplerConfig samplerConfig =
        {
            .MagFilter = EFilterMode::FILTER_LINEAR,
            .MinFilter = EFilterMode::FILTER_LINEAR,
            .UAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .VAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .WAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .BorderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .EnableComparision = false,
            .CompareOp = ECompareOp::COMPARE_OP_ALWAYS,
            .SamplerMode = ESamplerMipmapMode::SAMPLER_MIPMAP_MODE_LINEAR,
            .EnableAniso = false,
            .MaxAniso = 0.0f,
            .MaxLod = 0.0,
            .MinLod = 0.0,
            .MipLodBias = 0.0,
            .UnnormalizedCoords = false,
        };

        mSampler = mDevice->CreateSampler(samplerConfig);

        mResourceMediator->RegisterSampler(graph::DEFAULT_SAMPLER, mSampler);
    }

    void Renderer::CreateResourceManagers()
    {
        //create resource mediator 
        mResourceMediator = std::make_shared<graph::ResourceMediator>(mDevice);
        mResourceStateManager = std::make_shared<graph::ResourceStateManager>(mDevice, mResourceMediator);
    }

    void Renderer::ResizeWindow(HWND hWnd)
    {
        mWindowResizeQueue.push(hWnd);
    }

    void Renderer::BeginComputeRender(CommandBufferPtr& commandBuffer)
    {
        SCommandBufferBeginConfig commandBufferBegin =
        {
            .Usage = ECommandBufferUsage::NONE,
        };

        commandBuffer->Reset();
        commandBuffer->Begin(commandBufferBegin);
    }

    void Renderer::EndComputeRender(CommandBufferPtr& commandBuffer)
    {
        commandBuffer->End();
    }

    void Renderer::BeginGraphicsRender(CommandBufferPtr& commandBuffer)
    {
        //begin command buffer
        SCommandBufferBeginConfig commandBufferBegin =
        {
            .Usage = ECommandBufferUsage::NONE,
        };

        commandBuffer->Reset();
        commandBuffer->Begin(commandBufferBegin);
    }

    void Renderer::EndGraphicsRender(CommandBufferPtr& commandBuffer)
    {
        commandBuffer->End();
    }

    void Renderer::CreateSwapchain(HWND hWnd)
    {
        SSwapchainConfig swapchainConfig =
        {
            mDevice->GetSurface()
        };

        mSwapchain = mDevice->CreateSwapchain(swapchainConfig);
    }

    void Renderer::CreateSwapchainSyncObjects()
    {
        int swapchainImages = mSwapchain->GetImagesCount();
        mImageAvailableSemaphores.resize(swapchainImages);
        mGraphicsRenderFinishedSemaphores.resize(swapchainImages);
        mGraphicsFlightFences.resize(swapchainImages);

        //mComputeFlightFences.resize(swapchainImages);
        mComputeDispatchFinishedSemaphores.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            mGraphicsFlightFences[i] = mDevice->CreateFence({ false });
            mImageAvailableSemaphores[i] = mDevice->CreateSemaphore({ false });
            mGraphicsRenderFinishedSemaphores[i] = mDevice->CreateSemaphore({ false });
        }

        for(int i = 0; i < swapchainImages; i++)
        {
            //mComputeFlightFences[i] = mDevice->CreateFence({ false });
            mComputeDispatchFinishedSemaphores[i] = mDevice->CreateSemaphore({ false });
        }
    }

    void Renderer::InitImGui(HWND hWnd)
    {
        //create default imgui render pass with different render target!!
        mGui = mDevice->CreateGui(mCompositeBackbufferPass->GetRenderPass(), mGraphicsCommandPool, mSwapchain->GetImagesCount(), ESamplesCount::COUNT_1);
    }

    void Renderer::CreateDefaultDescriptorPool()
    {
        constexpr uint32_t maxUniformBuffers = 500;
        constexpr uint32_t maxCombinedImageSamples = 500;
        constexpr uint32_t maxDescriptorSets = 500;
        constexpr uint32_t maxStorageBuffer = 500;
        constexpr uint32_t maxStorageImage = 500;

        //create descriptor pool for everything
        std::vector<SDescriptorPoolSize> shaderDescriptors =
        {
            {
                EDescriptorType::UNIFORM_BUFFER,
                maxUniformBuffers
            },
            {
                EDescriptorType::COMBINED_IMAGE_SAMPLER,
                maxCombinedImageSamples
            },
            {
                EDescriptorType::STORAGE_BUFFER,
                maxStorageBuffer
            },
            {
                EDescriptorType::STORAGE_IMAGE,
                maxStorageImage
            }
        };

        SDescriptorPoolConfig descriptorPoolConfig =
        {
            .DesciptorPoolSizes = shaderDescriptors,
            .MaxDesciptorPools = maxDescriptorSets
        };

        mDescriptorPool = mDevice->CreateDescriptorPool(descriptorPoolConfig);
    }
}