#pragma once

#include <vector>
#include <cassert>

#include "../Interface/Device.h"
#include "../Interface/Types.h"
#include "QueueFamilyIndices.h"

class CVkSurface;

//TODO: Implement functions that can create more than 1 resource at a time (framebuffers, command buffers etc)

namespace psm
{
    class CVkDevice : public IDevice, public std::enable_shared_from_this<CVkDevice>
    {
    public:
        CVkDevice(VkPhysicalDevice physicalDevice, SurfacePtr surface);
        virtual ~CVkDevice();

        virtual void* Raw() override;
        virtual void* Raw() const override;

    public:
        virtual ImagePtr CreateImage(const SImageConfig& config) override;
        virtual ImagePtr CreateImageWithData(const CommandPoolPtr& commandPool, const SImageConfig& config, const SUntypedBuffer& data, const SImageToBufferCopyConfig& copyConfig) override;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) override;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) override;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) override;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) override;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) override;
        virtual ShaderPtr CreateShaderFromFilename(const std::string& path, EShaderStageFlag shaderType) override;
        virtual FencePtr CreateFence(const SFenceConfig& config) override;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) override;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) override;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) override;
        virtual std::vector<CommandBufferPtr> CreateCommandBuffers(const CommandPoolPtr& commandPool, const SCommandBufferConfig& config) override;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) override;
        virtual DescriptorPoolPtr CreateDescriptorPool(const SDescriptorPoolConfig& config) override;
        virtual DescriptorSetLayoutPtr CreateDescriptorSetLayout(const SDescriptorSetLayoutConfig& config) override;
        virtual ImGuiPtr CreateGui(const RenderPassPtr& renderPass, const CommandPoolPtr& commandPool, uint8_t swapchainImagesCount, ESamplesCount samplesCount) override;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) override;
        virtual void Submit(const SSubmitConfig& config) override;
        virtual void Present(const SPresentConfig& config) override;
        virtual void WaitIdle() override;
        virtual void BindVertexBuffers(const CommandBufferPtr& commandBuffer, const SVertexBufferBindConfig& config) override;
        virtual void BindIndexBuffer(const CommandBufferPtr& commandBuffer, const SIndexBufferBindConfig& config) override;
        virtual void CopyBuffer(const CommandBufferPtr& commandBuffer, uint64_t size, const BufferPtr& sourceBuffer, const BufferPtr& destinationBuffer) override;//copy buffer fully
        virtual void CopyBufferToImage(const CommandBufferPtr& commandBuffer, const BufferPtr& sourceBuffer, const ImagePtr& destrinationImage, SResourceExtent3D copySize, EImageAspect imageAspect, EImageLayout imageLayoutAfterCopy) override;
        virtual void BindDescriptorSets(const CommandBufferPtr& commandBuffer, EPipelineBindPoint bindPoint, const PipelineLayoutPtr& pipelineLayout, const std::vector<DescriptorSetPtr>& descriptors) override;
        virtual void DrawIndexed(const CommandBufferPtr& commandBuffer, const MeshRange& range, uint32_t totalInstances, uint32_t firstInstance) override;
        virtual void SetDepthBias(const CommandBufferPtr& commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;
        virtual void UpdateDescriptorSets(const DescriptorSetPtr& descriptorSet, const std::vector<SUpdateTextureConfig>& updateTextures, const std::vector<SUpdateBuffersConfig>& updateBuffers) override;
        virtual DescriptorSetPtr AllocateDescriptorSets(SDescriptorSetAllocateConfig& config) override;
        virtual void ImageLayoutTransition(const CommandBufferPtr& commandBuffer, const ImagePtr& image, const SImageLayoutTransition& config) override;//TODO: Remove it, you have InsertImageMemoryBarrier
        virtual bool CheckFenceStatus(const FencePtr& fence) override;
        virtual void SetViewport(const CommandBufferPtr& commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth) override;
        virtual void SetScissors(const CommandBufferPtr& commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent) override;

        virtual EFormat FindSupportedFormat(const std::vector<EFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) override;

        virtual DeviceData GetDeviceData() override;
        virtual SurfacePtr GetSurface() override;

        void SetDebugNameForResource(void* resource, VkDebugReportObjectTypeEXT type, const char* debugName);
    private:
        VkDevice mDevice;
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceProperties mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

        DeviceData mDeviceData;

        QueueFamilyIndices mQueues;
        
        SwapchainPtr mSwapchain;
        SurfacePtr mVkSurface;
    };

    DevicePtr CreateDefaultDeviceVk(const PlatformConfig& config);
}