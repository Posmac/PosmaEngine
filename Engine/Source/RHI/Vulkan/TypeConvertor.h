#pragma once

#include <cassert>

#include "Include/vulkan/vulkan.h"
#include "../Context/SamplerContext.h"
#include "../Context/BufferContext.h"

namespace psm
{
    VkFilter ToVulkan(EFilterMode mode)
    {
        switch(mode)
        {
            case FILTER_NEAREST:
                return VK_FILTER_NEAREST;
            case FILTER_LINEAR:
                return VK_FILTER_LINEAR;
            default:
                assert(false);
        }
    }

    VkSamplerMipmapMode ToVulkan(ESamplerMipmapMode mode)
    {
        switch(mode)
        {
            case SAMPLER_MIPMAP_MODE_NEAREST:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case SAMPLER_MIPMAP_MODE_LINEAR:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            default:
                assert(false);
        }
    }

    VkSamplerAddressMode ToVulkan(ESamplerAddressMode mode)
    {
        switch(mode)
        {
            case SAMPLER_MODE_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SAMPLER_MODE_MIRRORED_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SAMPLER_MODE_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case SAMPLER_MODE_CLAMP_TO_BORDER:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case SAMPLER_MODE_MIRROR_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            default:
                assert(false);
        }
    }

    VkBorderColor ToVulkan(EBorderColor color)
    {
        switch(color)
        {
            case BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
                return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case BORDER_COLOR_INT_TRANSPARENT_BLACK:
                return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            case BORDER_COLOR_FLOAT_OPAQUE_BLACK:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            case BORDER_COLOR_INT_OPAQUE_BLACK:
                return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            case BORDER_COLOR_FLOAT_OPAQUE_WHITE:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case BORDER_COLOR_INT_OPAQUE_WHITE:
                return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
            default:
                assert(false);
        }
    }

    VkCompareOp ToVulkan(ECompareOp operation)
    {
        switch(operation)
        {
            case COMPARE_OP_NEVER:
                return VK_COMPARE_OP_NEVER;
            case COMPARE_OP_LESS:
                return VK_COMPARE_OP_LESS;
            case COMPARE_OP_EQUAL:
                return VK_COMPARE_OP_EQUAL;
            case COMPARE_OP_LESS_OR_EQUAL:
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            case COMPARE_OP_GREATER:
                return VK_COMPARE_OP_GREATER;
            case COMPARE_OP_NOT_EQUAL:
                return VK_COMPARE_OP_NOT_EQUAL;
            case COMPARE_OP_GREATER_OR_EQUAL:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case COMPARE_OP_ALWAYS:
                return VK_COMPARE_OP_ALWAYS;
            default:
                assert(false);
        }
    }

    VkBufferUsageFlags ToVulkan(EBufferUsage usage)
    {
        switch(usage)
        {
            case USAGE_TRANSFER_SRC_BIT:
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            case USAGE_TRANSFER_DST_BIT:
                return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            case USAGE_UNIFORM_TEXEL_BUFFER_BIT:
                return VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            case USAGE_STORAGE_TEXEL_BUFFER_BIT:
                return VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            case USAGE_UNIFORM_BUFFER_BIT:
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            case USAGE_STORAGE_BUFFER_BIT:
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            case USAGE_INDEX_BUFFER_BIT:
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            case USAGE_VERTEX_BUFFER_BIT:
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case USAGE_INDIRECT_BUFFER_BIT:
                return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            default:
                assert(false);
        }
    }

    VkMemoryPropertyFlags ToVulkan(EMemoryProperties properties)
    {
        switch(properties)
        {
            case MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
                return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            case MEMORY_PROPERTY_HOST_VISIBLE_BIT:
                return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            case MEMORY_PROPERTY_HOST_COHERENT_BIT:
                return VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            case MEMORY_PROPERTY_HOST_CACHED_BIT:
                return VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            case MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
                return VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
            case MEMORY_PROPERTY_PROTECTED_BIT:
                return VK_MEMORY_PROPERTY_PROTECTED_BIT;
            case MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
                return VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
            case MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
                return VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
            case MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV:
                return VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
            default:
                assert(false);
        }
    }
}