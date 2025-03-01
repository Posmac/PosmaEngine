#pragma once

#include <memory>

namespace psm
{
    class IDevice;
    class ISwapchain;
    class ISurface;
    class IBuffer;
    class IImage;
    class ISampler;
    class IPipelineLayout;
    class IPipeline;
    class ICommandQueue;
    class IShaderModule;
    class ISemaphore;
    class IFence;
    class IRenderPass;
    class ICommandBuffer;
    class ICommandPool;
    class IFramebuffer;
    class IDescriptorPool;
    class IDescriptorSet;
    class IDescriptorSetLayout;
    class IPipeline;
    class IShader;
    class IPushConstant;
    class IImGui;

    using DevicePtr = std::shared_ptr<IDevice>;
    using SwapchainPtr = std::shared_ptr<ISwapchain>;
    using SurfacePtr = std::shared_ptr<ISurface>;
    using ImagePtr = std::shared_ptr<IImage>;
    using BufferPtr = std::shared_ptr<IBuffer>;
    using SamplerPtr = std::shared_ptr<ISampler>;
    using PipelineLayoutPtr = std::shared_ptr<IPipelineLayout>;
    using PipelinePtr = std::shared_ptr<IPipeline>;
    using CommandQueuePtr = std::shared_ptr<ICommandQueue>;
    using ShaderModulePtr = std::shared_ptr<IShaderModule>;
    using SemaphorePtr = std::shared_ptr<ISemaphore>;
    using FencePtr = std::shared_ptr<IFence>;
    using RenderPassPtr = std::shared_ptr<IRenderPass>;
    using CommandBufferPtr = std::shared_ptr<ICommandBuffer>;
    using CommandPoolPtr = std::shared_ptr<ICommandPool>;
    using FramebufferPtr = std::shared_ptr<IFramebuffer>;
    using DescriptorPoolPtr = std::shared_ptr<IDescriptorPool>;
    using DescriptorSetPtr = std::shared_ptr<IDescriptorSet>;
    using DescriptorSetLayoutPtr = std::shared_ptr<IDescriptorSetLayout>;
    using ShaderPtr = std::shared_ptr<IShader>;
    using PushConstantPtr = std::shared_ptr<IPushConstant>;
    using ImGuiPtr = std::shared_ptr<IImGui>;
}