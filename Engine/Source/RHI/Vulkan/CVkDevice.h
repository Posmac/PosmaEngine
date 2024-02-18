#pragma once

#include "RenderBackend/Common.h"
#include "RenderBackend/Queue.h"
#include "../Interface/Device.h"
#include "../Interface/Types.h"

namespace psm
{
    class CVkDevice final : public IDevice, public std::enable_shared_from_this<CVkDevice>
    {
    public:
        CVkDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        virtual ~CVkDevice();
    public:
        virtual TexturePtr CreateImage(const ImageConfig& config) override;
        virtual TexturePtr CreateImageWithData(const ImageConfig& config, const UntypedBuffer& data) override;
        virtual BufferPtr CreateBuffer(const BufferConfig& config) override;
        virtual SamplerPtr CreateSampler(const SamplerConfig& config) override;
        virtual SwapchainPtr CreateSwapchain(const SwapchainConfig& config) override;
        virtual PipelineLayoutPtr CreatePipelineLayout(const PipelineLayoutConfig& config) override;
        virtual CommandQueuePtr CreateCommandQueue(const CommandQueueConfig& config) override;
        virtual PipelinePtr CreateRenderPipeline(const PipelineConfig& config) override;
        virtual PipelinePtr CreateComputePipeline(const PipelineConfig& config) override;
        virtual ShaderModulePtr CreateShaderFromFilename(const std::filesystem::path& path) override;

        virtual DeviceData GetDeviceData() override;

        void SetDebugNameForResource(void* resource, VkDebugReportObjectTypeEXT type, const char* debugName);
    private:
        VkDevice mDevice;
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceProperties mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

        vk::QueueFamilyIndices mQueues;

        //VkSurfaceKHR mSurface;//temporary
    };

    DevicePtr CreateDefaultDeviceVk(PlatformConfig& config);
}