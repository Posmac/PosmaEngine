#pragma once

#include <Windows.h>
#include <filesystem>

#include "../Formats/ImageFormats.h"

#include "../Configs/TextureConfig.h"
#include "../Configs/BufferConfig.h"
#include "../Configs/SamplerConfig.h"
#include "../Configs/SwapchainConfig.h"
#include "../Configs/PipelineLayoutConfig.h"
#include "../Configs/CommandQueueConfig.h"
#include "../Configs/PipelineConfig.h"
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

        virtual TexturePtr CreateImage(const ImageConfig& config) = 0;
        virtual TexturePtr CreateImageWithData(const ImageConfig& config, const UntypedBuffer& data) = 0;
        virtual BufferPtr CreateBuffer(const BufferConfig& config) = 0;
        virtual SamplerPtr CreateSampler(const SamplerConfig& config) = 0;
        virtual SwapchainPtr CreateSwapchain(const SwapchainConfig& config) = 0;
        virtual PipelineLayoutPtr CreatePipelineLayout(const PipelineLayoutConfig& config) = 0;
        virtual CommandQueuePtr CreateCommandQueue(const CommandQueueConfig& config) = 0;
        virtual PipelinePtr CreateRenderPipeline(const PipelineConfig& config) = 0;
        virtual PipelinePtr CreateComputePipeline(const PipelineConfig& config) = 0;
        virtual ShaderModulePtr CreateShaderFromFilename(const std::filesystem::path& path) = 0;

        virtual DeviceData GetDeviceData() = 0;
    };
}