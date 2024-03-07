#pragma once

#include <vector>

#include "RenderBackend/Common.h"
#include "RenderBackend/Queue.h"
#include "../Interface/Device.h"
#include "../Interface/Types.h"

class CVkSurface;

//TODO: Implement functions that can create more than 1 resource at a time (framebuffers, command buffers etc)

namespace psm
{
    class CVkDevice final : public IDevice, public std::enable_shared_from_this<CVkDevice>
    {
    public:
        CVkDevice(VkPhysicalDevice physicalDevice, std::shared_ptr<CVkSurface> surface);
        virtual ~CVkDevice();
    public:
        virtual ImagePtr CreateImage(const SImageConfig& config) override;
        virtual ImagePtr CreateImageWithData(const SImageConfig& config, const SUntypedBuffer& data, const SImageLayoutTransition& transition) override;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) override;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) override;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) override;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) override;
        virtual CommandQueuePtr CreateCommandQueue(const SCommandQueueConfig& config) override;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) override;
        virtual PipelinePtr CreateComputePipeline(const SPipelineConfig& config) override;
        virtual ShaderPtr CreateShaderFromFilename(const std::filesystem::path& path, EShaderStageFlag shaderType) override;
        virtual FencePtr CreateFence(const SFenceConfig& config) override;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) override;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) override;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) override;
        virtual CommandBufferPtr CreateCommandBuffers(CommandPoolPtr commandPool, const SCommandBufferConfig& config) override;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) override;
        virtual DescriptorPoolPtr CreateDescriptorPool(const SDescriptorPoolConfig& config) override;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) override;
        virtual void Submit(const SSubmitConfig& config) override;
        virtual void Present(const SPresentConfig& config) override;
        virtual void WaitIndle() override;
        virtual void BindVertexBuffers(const SVertexBufferBindConfig& config) override;
        virtual void BindIndexBuffer(const SIndexBufferBindConfig& config) override;
        virtual void CopyBuffer(BufferPtr sourceBuffer, BufferPtr destinationBuffer) override;//copy buffer fully
        virtual void BindDescriptorSets(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint, PipelinePtr pipeline, const std::vector<DescriptorSetPtr>& descriptors) override;
        virtual void DrawIndexed(CommandBufferPtr commandBuffer, const MeshRange& range, uint32_t totalInstances, uint32_t firstInstance) override;
        virtual void SetDepthBias(CommandBufferPtr commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;

        virtual EFormat FindSupportedFormat(const std::vector<EFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) override;

        virtual DeviceData GetDeviceData() override;
        virtual SurfacePtr GetSurface() override;

        void SetDebugNameForResource(void* resource, VkDebugReportObjectTypeEXT type, const char* debugName);
    private:
        VkDevice mDevice;
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceProperties mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

        vk::QueueFamilyIndices mQueues;

        std::shared_ptr<CVkSurface> mVkSurface; //not sure if it should stay there
    };

    DevicePtr CreateDefaultDeviceVk(PlatformConfig& config);
}