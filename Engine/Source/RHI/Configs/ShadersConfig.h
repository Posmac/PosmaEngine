#pragma once

#include <cstdint>
#include <vector>

#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/PipelineFormats.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    enum class EFrontFace : uint8_t
    {
        COUNTER_CLOCKWISE = 0,
        CLOCKWISE = 1,
    };

    enum class ECullMode : uint32_t
    {
        NONE = 0,
        FRONT_BIT = 0x00000001,
        BACK_BIT = 0x00000002,
        FRONT_AND_BACK = 0x00000003,
    };

    enum class EPolygonMode : uint8_t
    {
        FILL = 0,
        LINE = 1,
        POINT = 2,
    };

    struct SDescriptorSetAllocateConfig
    {
        DescriptorPoolPtr DescriptorPool;
        std::vector<DescriptorSetLayoutPtr> DescriptorSetLayouts;
        uint32_t MaxSets;
    };

    struct SRasterizationConfig
    {
        bool DepthClampEnable;
        bool RasterizerDiscardEnable;
        ECullMode CullMode;
        EPolygonMode PolygonMode;
        EFrontFace FrontFace;
        bool DepthBiasEnable;
        float DepthBiasConstantFactor;
        float DepthBiasClamp;
        float DepthBiasSlopeFactor;
        float LineWidth;
    };

    struct SInputAssemblyConfig
    {
        EPrimitiveTopology Topology;
        bool RestartPrimitives;
    };

    struct SVertexInputBindingDescription
    {
        uint32_t Binding;
        uint32_t Stride;
        EVertexInputRate InputRate;
    };

    struct SVertexInputAttributeDescription
    {
        uint32_t Location;
        uint32_t Binding;
        EFormat Format;
        uint32_t Offset;
    };

    struct SShaderModuleConfig
    {
        ShaderPtr Shader;
        EShaderStageFlag Type;
        const char* EntryPoint;
    };

    struct SDescriptorPoolSize
    {
        EDescriptorType DescriptorType;
        uint32_t MaxSize;
    };

    struct SDescriptorPoolConfig
    {
        std::vector<SDescriptorPoolSize> DesciptorPoolSizes;
        uint32_t MaxDesciptorPools;
    };

    struct SDescriptorLayoutInfo
    {
        uint32_t Binding;
        EDescriptorType DescriptorType;
        uint32_t DescriptorCount;
        EShaderStageFlag ShaderStage;
    };

    struct SDescriptorSetLayoutConfig
    {
        SDescriptorLayoutInfo* pLayoutsInfo;
        uint32_t LayoutsCount;
    };

    struct SUpdateTextureConfig
    {
        SamplerPtr Sampler;
        ImagePtr Image;
        EImageLayout ImageLayout;
        uint32_t DstBinding;
        EDescriptorType  DescriptorType;
    };

    struct SUpdateBuffersConfig
    {
        BufferPtr Buffer;
        uint64_t Offset;
        uint64_t Range;
        uint32_t DstBinding;
        EDescriptorType DescriptorType;
    };
}
