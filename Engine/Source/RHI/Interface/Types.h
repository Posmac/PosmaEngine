#pragma once

#include <memory>

namespace psm
{
    class IDevice;
    class ISwapchain;
    class ISurface;
    class IBuffer;
    class ITexture;
    class ISampler;
    class IPipelineLayout;
    class IPipeline;
    class ICommandQueue;
    class IShaderModule;

    using DevicePtr = std::shared_ptr<IDevice>;
    using SwapchainPtr = std::shared_ptr<ISwapchain>;
    using SurfacePtr = std::shared_ptr<ISurface>;
    using TexturePtr = std::shared_ptr<ITexture>;
    using BufferPtr = std::shared_ptr<IBuffer>;
    using SamplerPtr = std::shared_ptr<ISampler>;
    using PipelineLayoutPtr = std::shared_ptr<IPipelineLayout>;
    using PipelinePtr = std::shared_ptr<IPipeline>;
    using CommandQueuePtr = std::shared_ptr<ICommandQueue>;
    using ShaderModulePtr = std::shared_ptr<IShaderModule>;
}