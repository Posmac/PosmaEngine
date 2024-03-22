#pragma once

#include <cstdint>

namespace psm
{
    enum class EBufferUsage : uint32_t
    {
        USAGE_TRANSFER_SRC_BIT = 0x00000001,
        USAGE_TRANSFER_DST_BIT = 0x00000002,
        USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
        USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
        USAGE_UNIFORM_BUFFER_BIT = 0x00000010,
        USAGE_STORAGE_BUFFER_BIT = 0x00000020,
        USAGE_INDEX_BUFFER_BIT = 0x00000040,
        USAGE_VERTEX_BUFFER_BIT = 0x00000080,
        USAGE_INDIRECT_BUFFER_BIT = 0x00000100,
        MAX = 0x0FFFFFFF
        //not complete
    };

    EBufferUsage operator | (const EBufferUsage rhs, const EBufferUsage lhs);
    uint32_t operator & (const EBufferUsage rhs, const EBufferUsage lhs);

    enum class EMemoryProperties : uint32_t
    {
        MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
        MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
        MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
        MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
        MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
        MEMORY_PROPERTY_PROTECTED_BIT = 0x00000020,
        MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD = 0x00000040,
        MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD = 0x00000080,
        MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV = 0x00000100,
        MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF,
        MAX = 0x0FFFFFFF
    };

    EMemoryProperties operator | (const EMemoryProperties rhs, const EMemoryProperties lhs);
    uint32_t operator & (const EMemoryProperties rhs, const EMemoryProperties lhs);
}