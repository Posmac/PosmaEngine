#pragma once

#include <vector>
#include <Windows.h>
#include <filesystem>

#include "RHI/Enums/ImageFormats.h"

#include "RHI/Configs/TextureConfig.h"
#include "RHI/Configs/BufferConfig.h"
#include "RHI/Configs/SamplerConfig.h"
#include "RHI/Configs/SwapchainConfig.h"
#include "RHI/Configs/PipelineLayoutConfig.h"
#include "RHI/Configs/PipelineConfig.h"
#include "RHI/Configs/SyncronizationConfigs.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "RHI/Configs/CommandBuffers.h"
#include "RHI/Configs/FramebuffersConfig.h"
#include "RHI/Configs/Barriers.h"
#include "RHI/Configs/ShadersConfig.h"

#include "RHI/Memory/UntypedBuffer.h"
#include "Types.h"

#include "RHI/RHICommon.h"

#include "RHI/DeviceData.h"
#include "IObject.h"

namespace psm
{
    struct MeshRange;

    class IDevice : public IObject
    {
    public:
        IDevice() = default;
        virtual ~IDevice() = default;
    public:
        static DevicePtr CreateSystemDefaultDevice(const PlatformConfig& config);

        virtual SurfacePtr CreateSurface(const PlatformConfig& config) = 0;
        virtual ImagePtr CreateImage(const SImageConfig& config) = 0;
        virtual ImagePtr CreateImageWithData(const CommandPoolPtr& commandPool, const SImageConfig& config, const SUntypedBuffer& data, const SImageToBufferCopyConfig& copyConfig) = 0;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) = 0;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) = 0;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) = 0;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) = 0;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) = 0;
        virtual ShaderPtr CreateShaderFromFilename(const std::string& path, EShaderStageFlag shaderType) = 0;
        virtual FencePtr CreateFence(const SFenceConfig& config) = 0;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) = 0;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) = 0;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) = 0;
        virtual std::vector<CommandBufferPtr> CreateCommandBuffers(const CommandPoolPtr& commandPool, const SCommandBufferConfig& config) = 0;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) = 0;
        virtual DescriptorPoolPtr CreateDescriptorPool(const SDescriptorPoolConfig& config) = 0;
        virtual DescriptorSetLayoutPtr CreateDescriptorSetLayout(const SDescriptorSetLayoutConfig& config) = 0;
        virtual ImGuiPtr CreateGui(const RenderPassPtr& renderPass, const CommandPoolPtr& commandPool, uint8_t swapchainImagesCount, ESamplesCount samplesCount) = 0;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) = 0;
        virtual void Submit(const SSubmitConfig& config) = 0;
        virtual void Present(const SPresentConfig& config) = 0;
        virtual bool WaitIdle() = 0;
        virtual void BindVertexBuffers(const CommandBufferPtr& commandBuffer, const SVertexBufferBindConfig& config) = 0;
        virtual void BindIndexBuffer(const CommandBufferPtr& commandBuffer, const SIndexBufferBindConfig& config) = 0;
        virtual void CopyBuffer(const CommandBufferPtr& commandBuffer, uint64_t size, const BufferPtr& sourceBuffer, const BufferPtr& destinationBuffer) = 0;//copy buffer fully
        virtual void CopyBufferToImage(const CommandBufferPtr& commandBuffer, const BufferPtr& sourceBuffer, const ImagePtr& destrinationImage, SResourceExtent3D copySize, EImageAspect imageAspect, EImageLayout imageLayoutAfterCopy, uint64_t bufferOffset = 0, uint64_t rowPitch = 0, uint64_t slicePitch = 0) = 0;
        virtual void BindDescriptorSets(const CommandBufferPtr& commandBuffer, EPipelineBindPoint bindPoint, const PipelineLayoutPtr& pipelineLayout, const std::vector<DescriptorSetPtr>& descriptors) = 0;
        virtual void DrawIndexed(const CommandBufferPtr& commandBuffer, const MeshRange& range, uint32_t totalInstances, uint32_t firstInstance) = 0;
        virtual void SetDepthBias(const CommandBufferPtr& commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
        virtual void UpdateDescriptorSets(const DescriptorSetPtr& descriptorSet, const std::vector<SUpdateTextureConfig>& updateTextures, const std::vector<SUpdateBuffersConfig>& updateBuffers) = 0;
        virtual DescriptorSetPtr AllocateDescriptorSets(SDescriptorSetAllocateConfig& config) = 0;
        virtual void ImageLayoutTransition(const CommandBufferPtr& commandBuffer, const ImagePtr& image, const SImageLayoutTransition& config) = 0;
        virtual void SetViewport(const CommandBufferPtr& commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth) = 0;
        virtual void SetScissors(const CommandBufferPtr& commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent) = 0;
        virtual void UpdateBuffer(const BufferPtr& buffer, void* data) = 0;

        //specific
        virtual CommandBufferPtr BeginSingleTimeSubmitCommandBuffer(CommandPoolPtr& cmdPool) = 0;
        virtual void SubmitSingleTimeCommandBuffer(CommandPoolPtr& cmdPool, CommandBufferPtr commandBuffer) = 0;

        virtual EFormat FindSupportedFormat(const std::vector<EFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) = 0;

        virtual DeviceData GetDeviceData() = 0;
        virtual SurfacePtr GetSurface() = 0;
    };
}