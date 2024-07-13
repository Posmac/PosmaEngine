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

        ModelLoader::Instance()->Init(mDevice, mCommandPool);
        TextureLoader::Instance()->Init(mDevice, mCommandPool);

        InitImGui(static_cast<HWND>(config.win32.hWnd));

        isInit = true;
    }

    void Renderer::RegisterRenderPasses()
    {
        auto preshadowsLambda = [this]()
        {
            //OpaqueInstances::GetInstance()->UpdateDepthDescriptors(mCurrentImageIndex);
        };

        auto predefaultLambda = [this]()
        {
            //OpaqueInstances::GetInstance()->UpdateDefaultDescriptors(mCurrentImageIndex);
        };

        //create render passes
        auto shadowsLambda = [this]()
        {
            OpaqueInstances::GetInstance()->UpdateDepthDescriptors(mCurrentImageIndex);
            OpaqueInstances::GetInstance()->RenderDepth(mCommandBuffers[mCurrentFrame], mShadowMapPipeline);
        };

        auto defaultLambda = [this]()
        {
            OpaqueInstances::GetInstance()->UpdateDefaultDescriptors(mCurrentImageIndex);
            OpaqueInstances::GetInstance()->Render(mCommandBuffers[mCurrentFrame], mDefaultRenderPipeline);

            mGui->PrepareNewFrame();
            {
                ShadowsGenerator::Instance()->DrawShadowParams();
            }
            mGui->Render(mCommandBuffers[mCurrentFrame]);
        };

        mDefaultBackbufferPass = std::make_shared<graph::DefaultBackbufferRenderPassNode>(graph::DEFAULT_RENDERPASS,
                                                                                          mDevice,
                                                                                          mResourceMediator,
                                                                                          mSwapchain,
                                                                                          mSwapchain->GetSwapchainImageFormat());

        mShadowMapRenderPass = std::make_shared<graph::ShadowMapRenderPassNode>(graph::SHADOWMAP_RENDERPASS, 
                                                                                mResourceMediator,
                                                                                mDevice,
                                                                                graph::SHADOW_MAP_SIZE,
                                                                                mSwapchain->GetImagesCount());

        mDefaultBackbufferPass->AddRenderCallback(defaultLambda);
        mShadowMapRenderPass->AddRenderCallback(shadowsLambda);
        mDefaultBackbufferPass->AddPreRenderCallback(predefaultLambda);
        mShadowMapRenderPass->AddPreRenderCallback(preshadowsLambda);

        //add everything into graph
        mRenderGraph = std::make_shared<graph::RenderGraph>();
        mRenderGraph->AddRenderPass(mShadowMapRenderPass); //add default backbuffer renderpass
        mRenderGraph->AddRenderPass(mDefaultBackbufferPass);   //shadow map generation renderpass

        //create all necessary resources and link them
        mRenderGraph->GenerateResourceDependencies(mSwapchain->GetImagesCount());

        //create render pipelines
        mDefaultRenderPipeline = std::make_shared<graph::DefaultRenderPipelineNode>(graph::DEFAULT_GRAPHICS_PIPELINE,
                                                                                    mDevice,
                                                                                    mDefaultBackbufferPass->GetRenderPass(),
                                                                                    mResourceMediator,
                                                                                    mSwapchain->GetSwapchainSize());

        mShadowMapPipeline = std::make_shared<graph::ShadowMapPipelineNode>(graph::SHADOWMAP_GRAPHICS_PIPELINE,
                                                                            mDevice,
                                                                            mShadowMapRenderPass->GetRenderPass(),
                                                                            mResourceMediator,
                                                                            graph::SHADOW_MAP_SIZE);
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
        mDefaultBackbufferPass = nullptr;
        mDefaultRenderPipeline = nullptr;
        mShadowMapPipeline = nullptr;
        mShadowMapRenderPass = nullptr;

        mImageAvailableSemaphores.clear();;
        mRenderFinishedSemaphores.clear();
        mFlightFences.clear();

        mGlobalBuffer = nullptr;
        mSwapchain = nullptr;

        mSampler = nullptr;
        mDescriptorPool = nullptr;

        mCommandPool->FreeCommandBuffers(mCommandBuffers);
        /*for(auto& cb : mCommandBuffers)
            cb = nullptr;*/
        mCommandBuffers.clear();

        mCommandPool = nullptr;

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
    
        mCurrentImageIndex = BeginRender();

        mDevice->UpdateBuffer(mGlobalBuffer, &buffer);
        ShadowsGenerator::Instance()->Update();

        for(auto& renderPass : *mRenderGraph)
        {
            renderPass->PreRender(mCommandBuffers[mCurrentFrame], mCurrentImageIndex);
            renderPass->Render();
            renderPass->PostRender(mCommandBuffers[mCurrentFrame]);
        }

        EndRender(mCurrentImageIndex);
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
        mDefaultBackbufferPass->RecreateFramebuffers(mSwapchain);
    }

    void Renderer::CreateRenderFrameCommandBuffers()
    {
        SCommandBufferConfig cmdBuffersConfig =
        {
            .Size = mSwapchain->GetImagesCount(),
            .IsBufferLevelPrimary = true,
        };

        mCommandBuffers = mDevice->CreateCommandBuffers(mCommandPool, cmdBuffersConfig);
    }

    void Renderer::CreateCommandPool()
    {
        SCommandPoolConfig cmdPoolConfig =
        {
            .QueueFamilyIndex = mDevice->GetDeviceData().vkData.GraphicsQueueIndex,
            .QueueType = EQueueType::GRAHPICS
        };

        mCommandPool = mDevice->CreateCommandPool(cmdPoolConfig);
    }

    void Renderer::CreateGlobalBuffer()
    {
        SBufferConfig bufferConfig =
        {
            .Size = sizeof(GlobalBuffer),
            .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mGlobalBuffer = mDevice->CreateBuffer(bufferConfig);

        mResourceMediator->RegisterBufferResource(graph::GLOBAL_CBUFFER, mGlobalBuffer);
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

    uint32_t Renderer::BeginRender()
    {
        uint32_t imageIndex;
        SSwapchainAquireNextImageConfig nextImageConfig
        {
            .Timeout = UINT64_MAX,
            .Semaphore = mImageAvailableSemaphores[mCurrentFrame]
        };

        mSwapchain->GetNextImage(nextImageConfig, &imageIndex);

        //begin command buffer
        SCommandBufferBeginConfig commandBufferBegin =
        {
            .BufferIndex = mCurrentFrame,
            .Usage = ECommandBufferUsage::NONE,
        };

        mCommandBuffers[mCurrentFrame]->Begin(commandBufferBegin);

        return imageIndex;
    }

    void Renderer::EndRender(uint32_t imageIndex)
    {
        mCommandBuffers[mCurrentFrame]->End();

        SSubmitConfig submitConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
            .SubmitCount = 1,
            .WaitStageFlags = EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT,
            .WaitSemaphoresCount = 1,
            .pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame],
            .CommandBuffersCount = 1,
            .pCommandBuffers = &mCommandBuffers[mCurrentFrame],
            .SignalSemaphoresCount = 1,
            .pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame],
            .Fence = mFlightFences[mCurrentFrame],
        };

        mDevice->Submit(submitConfig);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SPresentConfig presentConfig =
        {
            .Queue = mDevice->GetDeviceData().vkData.GraphicsQueue,
            .WaitSemaphoresCount = 1,
            .pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame],
            .ImageIndex = imageIndex
        };

        //mDevice->Present(presentConfig);
        mSwapchain->Present(presentConfig);

        SFenceWaitConfig fenceWait =
        {
            .WaitAll = true,
            .Timeout = FLT_MAX,
        };

        while(!mFlightFences[mCurrentFrame]->Wait(fenceWait))
        {
            LogMessage(MessageSeverity::Info, "Waiting for fence");
        };

        mFlightFences[mCurrentFrame]->Reset();

        mCommandBuffers[mCurrentFrame]->Reset();

        mCurrentFrame = (mCurrentFrame + 1) % mSwapchain->GetImagesCount();
        mTotalFrames++;
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
        mRenderFinishedSemaphores.resize(swapchainImages);
        mFlightFences.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            mFlightFences[i] = mDevice->CreateFence({ false });
            mImageAvailableSemaphores[i] = mDevice->CreateSemaphore({ false });
            mRenderFinishedSemaphores[i] = mDevice->CreateSemaphore({ false });
        }
    }

    void Renderer::InitImGui(HWND hWnd)
    {
        //create default imgui render pass with different render target!!
        mGui = mDevice->CreateGui(mDefaultBackbufferPass->GetRenderPass(), mCommandPool, mSwapchain->GetImagesCount(), ESamplesCount::COUNT_1);
    }

    void Renderer::CreateDefaultDescriptorPool()
    {
        constexpr uint32_t maxUniformBuffers = 500;
        constexpr uint32_t maxCombinedImageSamples = 500;
        constexpr uint32_t maxDescriptorSets = 500;

        //create descriptor pool for everything
        std::vector<SDescriptorPoolSize> shaderDescriptors =
        {
            {
                EDescriptorType::UNIFORM_BUFFER, //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                maxUniformBuffers
            },
            {
                EDescriptorType::COMBINED_IMAGE_SAMPLER, //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                maxCombinedImageSamples
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