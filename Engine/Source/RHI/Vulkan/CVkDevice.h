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
        virtual ImagePtr CreateImageWithData(const SImageConfig& config, const UntypedBuffer& data) override;
        virtual BufferPtr CreateBuffer(const SBufferConfig& config) override;
        virtual SamplerPtr CreateSampler(const SSamplerConfig& config) override;
        virtual SwapchainPtr CreateSwapchain(const SSwapchainConfig& config) override;
        virtual PipelineLayoutPtr CreatePipelineLayout(const SPipelineLayoutConfig& config) override;
        virtual CommandQueuePtr CreateCommandQueue(const SCommandQueueConfig& config) override;
        virtual PipelinePtr CreateRenderPipeline(const SPipelineConfig& config) override;
        virtual PipelinePtr CreateComputePipeline(const SPipelineConfig& config) override;
        virtual ShaderModulePtr CreateShaderFromFilename(const std::filesystem::path& path) override;
        virtual FencePtr CreateFence(const SFenceConfig& config) override;
        virtual SemaphorePtr CreateSemaphore(const SSemaphoreConfig& config) override;
        virtual RenderPassPtr CreateRenderPass(const SRenderPassConfig& config) override;
        virtual CommandPoolPtr CreateCommandPool(const SCommandPoolConfig& config) override;
        virtual CommandBufferPtr CreateCommandBuffers(CommandPoolPtr commandPool, const SCommandBufferConfig& config) override;
        virtual FramebufferPtr CreateFramebuffer(const SFramebufferConfig& config) override;

        virtual void InsertImageMemoryBarrier(const SImageBarrierConfig& config) override;
        virtual void Submit(const SSubmitConfig& config) override;

        virtual EImageFormat FindSupportedFormat(const std::vector<EImageFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature) override;

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