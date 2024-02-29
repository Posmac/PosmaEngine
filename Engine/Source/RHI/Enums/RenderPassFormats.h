#pragma once

#include <cstdint>

namespace psm
{
    enum class EAttachmentDescriptionFlags : uint8_t
    {
        NONE = 0, //for now
    };

    enum class EAttachmentLoadOp : uint8_t
    {
        LOAD_OP_LOAD = 0,
        LOAD_OP_CLEAR = 1,
        LOAD_OP_DONT_CARE = 2,
    };

    enum class EAttachmentStoreOp : uint8_t
    {
        STORE_OP_STORE = 0,
        STORE_OP_DONT_CARE = 1,
    };

    enum class EPipelineBindPoint : uint8_t
    {
        GRAPHICS = 0,
        COMPUTE = 1,
    };

    enum class EPipelineStageFlags : uint32_t
    {
        TOP_OF_PIPE_BIT = 0x00000001,
        DRAW_INDIRECT_BIT = 0x00000002,
        VERTEX_INPUT_BIT = 0x00000004,
        VERTEX_SHADER_BIT = 0x00000008,
        TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
        TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
        GEOMETRY_SHADER_BIT = 0x00000040,
        FRAGMENT_SHADER_BIT = 0x00000080,
        EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
        LATE_FRAGMENT_TESTS_BIT = 0x00000200,
        COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
        COMPUTE_SHADER_BIT = 0x00000800,
        TRANSFER_BIT = 0x00001000,
        BOTTOM_OF_PIPE_BIT = 0x00002000,
        HOST_BIT = 0x00004000,
        ALL_GRAPHICS_BIT = 0x00008000,
        ALL_COMMANDS_BIT = 0x00010000,
        NONE = 0,
    };

    enum class EAccessFlags : uint32_t
    {
        INDIRECT_COMMAND_READ_BIT = 0x00000001,
        INDEX_READ_BIT = 0x00000002,
        VERTEX_ATTRIBUTE_READ_BIT = 0x00000004,
        UNIFORM_READ_BIT = 0x00000008,
        INPUT_ATTACHMENT_READ_BIT = 0x00000010,
        SHADER_READ_BIT = 0x00000020,
        SHADER_WRITE_BIT = 0x00000040,
        COLOR_ATTACHMENT_READ_BIT = 0x00000080,
        COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
        DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200,
        DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400,
        TRANSFER_READ_BIT = 0x00000800,
        TRANSFER_WRITE_BIT = 0x00001000,
        HOST_READ_BIT = 0x00002000,
        HOST_WRITE_BIT = 0x00004000,
        MEMORY_READ_BIT = 0x00008000,
        MEMORY_WRITE_BIT = 0x00010000,
        NONE = 0,
    };

    enum class EDependencyFlags : uint8_t
    {
        NONE = 0, //for now
    };

    EPipelineStageFlags operator | (const EPipelineStageFlags& rhs, const EPipelineStageFlags& lhs)
    {
        return static_cast<EPipelineStageFlags>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    EPipelineStageFlags operator & (const EPipelineStageFlags& rhs, const EPipelineStageFlags& lhs)
    {
        return static_cast<EPipelineStageFlags>(static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs));
    }

    bool operator != (const EPipelineStageFlags rhs, uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    EAccessFlags operator | (const EAccessFlags& rhs, const EAccessFlags& lhs)
    {
        return static_cast<EAccessFlags>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    EAccessFlags operator & (const EAccessFlags& rhs, const EAccessFlags& lhs)
    {
        return static_cast<EAccessFlags>(static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs));
    }

    bool operator != (const EAccessFlags rhs, uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }
}