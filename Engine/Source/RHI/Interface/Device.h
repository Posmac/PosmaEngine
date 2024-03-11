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
#include "../Configs/ShadersConfig.h"

#include "../Memory/UntypedBuffer.h"
#include "Types.h"
#include "../VkCommon.h"
#include "../DeviceData.h"

namespace psm
{
    struct MeshRange;

    class IDevice
    {
    public:
        IDevice() = default;
        virtual ~IDevice() = default;
    public:
        static void CreateSystemDefaultDevice(DevicePtr& device, PlatformConfig& config);

        virtual ImagePtr CreateImage(const SImageConfig& config) = 0;
        virtual ImagePtr CreateImageWithData(const SImageConfig& config, const SUntypedBuffer& data, const SImageLayoutTransition& transition) = 0;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) = 0;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) = 0;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) = 0;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) = 0;
        virtual CommandQueuePtr CreateCommandQueue(const SCommandQueueConfig& config) = 0;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) = 0;
        virtual ShaderPtr CreateShaderFromFilename(const std::filesystem::path& path, EShaderStageFlag shaderType) = 0;
        virtual FencePtr CreateFence(const SFenceConfig& config) = 0;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) = 0;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) = 0;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) = 0;
        virtual CommandBufferPtr CreateCommandBuffers(CommandPoolPtr commandPool, const SCommandBufferConfig& config) = 0;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) = 0;
        virtual DescriptorPoolPtr CreateDescriptorPool(const SDescriptorPoolConfig& config) = 0;
        virtual DescriptorSetLayoutPtr CreateDescriptorSetLayout(const SDescriptorSetLayoutConfig& config) = 0;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) = 0;
        virtual void Submit(const SSubmitConfig& config) = 0;
        virtual void Present(const SPresentConfig& config) = 0;
        virtual void WaitIndle() = 0;
        virtual void BindVertexBuffers(const SVertexBufferBindConfig& config) = 0;
        virtual void BindIndexBuffer(const SIndexBufferBindConfig& config) = 0;
        virtual void CopyBuffer(BufferPtr sourceBuffer, BufferPtr destinationBuffer) = 0;//copy buffer fully
        virtual void BindDescriptorSets(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint, PipelinePtr pipeline, const std::vector<DescriptorSetPtr>& descriptors) = 0;
        virtual void DrawIndexed(CommandBufferPtr commandBuffer, const MeshRange& range, uint32_t totalInstances, uint32_t firstInstance) = 0;
        virtual void SetDepthBias(CommandBufferPtr commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
        virtual void UpdateDescriptorSets(DescriptorSetPtr* descriptorSets, uint32_t setsCount, const std::vector<SUpdateTextureConfig>& updateTextures, const std::vector<SUpdateBuffersConfig>& updateBuffers) = 0;
        virtual void AllocateDescriptorSets(const SDescriptorSetAllocateConfig& config) = 0;

        virtual EFormat FindSupportedFormat(const std::vector<EFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) = 0;

        virtual DeviceData GetDeviceData() = 0;
        virtual SurfacePtr GetSurface() = 0;
    };
}