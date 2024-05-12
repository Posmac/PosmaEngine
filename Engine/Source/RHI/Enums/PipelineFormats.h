#pragma once

#include <cstdint>

namespace psm
{
    enum class EIndexType : uint32_t
    {
        UINT16 = 0,
        UINT32 = 1,
    };

    enum class EDynamicState
    {
        VIEWPORT = 0,
        SCISSOR = 1,
        LINE_WIDTH = 2,
        DEPTH_BIAS = 3,
    };

    enum class EPrimitiveTopology : uint32_t
    {
        POINT_LIST = 0,
        LINE_LIST = 1,
        LINE_STRIP = 2,
        TRIANGLE_LIST = 3,
        TRIANGLE_STRIP = 4,
        TRIANGLE_FAN = 5,
    };

    enum class EVertexInputRate : uint32_t
    {
        VERTEX = 0,
        INSTANCE = 1,
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

    enum class EDescriptorType : uint32_t
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

    enum class ESubpassContents : uint32_t
    {
        INLINE = 0,
        SECONDARY_COMMAND_BUFFERS = 1,
    };

    enum class EFrontFace : uint32_t
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

    enum class EPolygonMode : uint32_t
    {
        FILL = 0,
        LINE = 1,
        POINT = 2,
    };
}
