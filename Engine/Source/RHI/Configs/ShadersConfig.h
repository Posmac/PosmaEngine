#pragma once

#include <cstdint>
#include <vector>

#include "RHI/Enums/ImageFormats.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    struct SVertexInputAttributeDescription
    {
        uint32_t Location;
        uint32_t Binding;
        EFormat Format;
        uint32_t Offset;
    };

    enum class EShaderStageFlag : uint32_t
    {
        VERTEX_BIT = 0x00000001,
        TESSELLATION_CONTROL_BIT = 0x00000002,
        TESSELLATION_EVALUATION_BIT = 0x00000004,
        GEOMETRY_BIT = 0x00000008,
        FRAGMENT_BIT = 0x00000010,
        COMPUTE_BIT = 0x00000020,
        ALL_GRAPHICS = 0x0000001F,
        ALL = 0x7FFFFFFF,
    };

    struct SShaderModuleConfig
    {
        ShaderPtr Shader;
        EShaderStageFlag Type;
        const char* EntryPoint;
    };

    enum class EDescriptorType : uint8_t
    {
        SAMPLER = 0,
        COMBINED_IMAGE_SAMPLER = 1,
        SAMPLED_IMAGE = 2,
        STORAGE_IMAGE = 3,
        UNIFORM_TEXEL_BUFFER = 4,
        STORAGE_TEXEL_BUFFER = 5,
        UNIFORM_BUFFER = 6,
        STORAGE_BUFFER = 7,
        UNIFORM_BUFFER_DYNAMIC = 8,
        STORAGE_BUFFER_DYNAMIC = 9,
        INPUT_ATTACHMENT = 10,
    };

    struct SDescriptorPoolSize
    {
        EDescriptorType DescriptorType;
        uint32_t MaxSize;
    };

    struct SDescriptorPoolConfig
    {
        std::vector< SDescriptorPoolSize> DesciptorPoolSizes;
        uint32_t MaxDesciptorPools;
    };
}
