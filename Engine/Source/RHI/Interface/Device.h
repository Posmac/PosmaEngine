#pragma once

#include <vector>
#include <Windows.h>
#include <filesystem>

#include "../Enums/ImageFormats.h"

#include "../Configs/TextureConfig.h"
#include "../Configs/BufferConfig.h"
#include "../Configs/SamplerConfig.h"
#include "../Configs/SwapchainConfig.h"
#include "../Configs/PipelineLayoutConfig.h"
#include "../Configs/CommandQueueConfig.h"
#include "../Configs/PipelineConfig.h"
#include "../Configs/SyncronizationConfigs.h"
#include "../Configs/RenderPassConfig.h"
#include "../Configs/CommandBuffers.h"
#include "../Configs/FramebuffersConfig.h"
#include "../Configs/Barriers.h"
#include "../Memory/UntypedBuffer.h"
#include "Types.h"
#include "../VkCommon.h"
#include "../DeviceData.h"

namespace psm
{
    class IDevice
    {
    public:
        IDevice() = default;
        virtual ~IDevice() = default;
    public:
        static void CreateSystemDefaultDevice(DevicePtr& device, PlatformConfig& config);

        virtual ImagePtr CreateImage(const SImageConfig& config) = 0;
        virtual ImagePtr CreateImageWithData(const SImageConfig& config, const UntypedBuffer& data) = 0;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) = 0;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) = 0;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) = 0;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) = 0;
        virtual CommandQueuePtr CreateCommandQueue(const SCommandQueueConfig& config) = 0;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) = 0;
        virtual PipelinePtr CreateComputePipeline(const SPipelineConfig& config) = 0;
        virtual ShaderModulePtr CreateShaderFromFilename(const std::filesystem::path& path) = 0;
        virtual FencePtr CreateFence(const SFenceConfig& config) = 0;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) = 0;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) = 0;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) = 0;
        virtual CommandBufferPtr CreateCommandBuffers(CommandPoolPtr commandPool, const SCommandBufferConfig& config) = 0;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) = 0;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) = 0;
        virtual void Submit(const SSubmitConfig& config) = 0;

        virtual EImageFormat FindSupportedFormat(const std::vector<EImageFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) = 0;

        virtual DeviceData GetDeviceData() = 0;
        virtual SurfacePtr GetSurface() = 0;
    };
}