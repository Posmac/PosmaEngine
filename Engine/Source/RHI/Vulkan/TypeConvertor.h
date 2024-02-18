#pragma once

#include <cassert>

#include "Include/vulkan/vulkan.h"
#include "../Formats/SamplerFormats.h"
#include "../Formats/BufferFormats.h"
#include "../Formats/ImageFormats.h"

namespace psm
{
    VkFilter ToVulkan(EFilterMode mode)
    {
        switch(mode)
        {
            case EFilterMode::FILTER_NEAREST:
                return VK_FILTER_NEAREST;
            case EFilterMode::FILTER_LINEAR:
                return VK_FILTER_LINEAR;
            default:
                assert(false);
        }
    }

    VkSamplerMipmapMode ToVulkan(ESamplerMipmapMode mode)
    {
        switch(mode)
        {
            case ESamplerMipmapMode::SAMPLER_MIPMAP_MODE_NEAREST:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case ESamplerMipmapMode::SAMPLER_MIPMAP_MODE_LINEAR:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            default:
                assert(false);
        }
    }

    VkSamplerAddressMode ToVulkan(ESamplerAddressMode mode)
    {
        switch(mode)
        {
            case ESamplerAddressMode::SAMPLER_MODE_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case ESamplerAddressMode::SAMPLER_MODE_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case ESamplerAddressMode::SAMPLER_MODE_CLAMP_TO_BORDER:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case ESamplerAddressMode::SAMPLER_MODE_MIRROR_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            default:
                assert(false);
        }
    }

    VkBorderColor ToVulkan(EBorderColor color)
    {
        switch(color)
        {
            case EBorderColor::BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
                return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case EBorderColor::BORDER_COLOR_INT_TRANSPARENT_BLACK:
                return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            case EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            case EBorderColor::BORDER_COLOR_INT_OPAQUE_BLACK:
                return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            case EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_WHITE:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case EBorderColor::BORDER_COLOR_INT_OPAQUE_WHITE:
                return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
            default:
                assert(false);
        }
    }

    VkCompareOp ToVulkan(ECompareOp operation)
    {
        switch(operation)
        {
            case ECompareOp::COMPARE_OP_NEVER:
                return VK_COMPARE_OP_NEVER;
            case ECompareOp::COMPARE_OP_LESS:
                return VK_COMPARE_OP_LESS;
            case ECompareOp::COMPARE_OP_EQUAL:
                return VK_COMPARE_OP_EQUAL;
            case ECompareOp::COMPARE_OP_LESS_OR_EQUAL:
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            case ECompareOp::COMPARE_OP_GREATER:
                return VK_COMPARE_OP_GREATER;
            case ECompareOp::COMPARE_OP_NOT_EQUAL:
                return VK_COMPARE_OP_NOT_EQUAL;
            case ECompareOp::COMPARE_OP_GREATER_OR_EQUAL:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case ECompareOp::COMPARE_OP_ALWAYS:
                return VK_COMPARE_OP_ALWAYS;
            default:
                assert(false);
        }
    }

    VkBufferUsageFlags ToVulkan(EBufferUsage usage)
    {
        switch(usage)
        {
            case EBufferUsage::USAGE_TRANSFER_SRC_BIT:
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            case EBufferUsage::USAGE_TRANSFER_DST_BIT:
                return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            case EBufferUsage::USAGE_UNIFORM_TEXEL_BUFFER_BIT:
                return VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            case EBufferUsage::USAGE_STORAGE_TEXEL_BUFFER_BIT:
                return VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            case EBufferUsage::USAGE_UNIFORM_BUFFER_BIT:
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            case EBufferUsage::USAGE_STORAGE_BUFFER_BIT:
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            case EBufferUsage::USAGE_INDEX_BUFFER_BIT:
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            case EBufferUsage::USAGE_VERTEX_BUFFER_BIT:
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case EBufferUsage::USAGE_INDIRECT_BUFFER_BIT:
                return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            default:
                assert(false);
        }
    }

    VkMemoryPropertyFlags ToVulkan(EMemoryProperties properties)
    {
        switch(properties)
        {
            case EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
                return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT:
                return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT:
                return VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_HOST_CACHED_BIT:
                return VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
                return VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_PROTECTED_BIT:
                return VK_MEMORY_PROPERTY_PROTECTED_BIT;
            case EMemoryProperties::MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
                return VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
            case EMemoryProperties::MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
                return VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
            case EMemoryProperties::MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV:
                return VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
            default:
                assert(false);
        }
    }

    EImageType FromVulkan(VkImageType type)
    {
        switch(type)
        {
            case VK_IMAGE_TYPE_1D:
                return EImageType::TYPE_1D;
            case VK_IMAGE_TYPE_2D:
                return EImageType::TYPE_2D;
            case VK_IMAGE_TYPE_3D:
                return EImageType::TYPE_3D;
            default:
                break;
        }
    }

    EImageFormat FromVulkan(VkFormat format)
    {
        switch(format)
        {
            case VK_FORMAT_UNDEFINED:
                return EImageFormat::UNDEFINED;
            case VK_FORMAT_R4G4_UNORM_PACK8:
                return EImageFormat::R4G4_UNORM_PACK8;
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
                return EImageFormat::R4G4B4A4_UNORM_PACK16;
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
                return EImageFormat::B4G4R4A4_UNORM_PACK16;
            case VK_FORMAT_R5G6B5_UNORM_PACK16:
                return EImageFormat::R5G6B5_UNORM_PACK16;
            case VK_FORMAT_B5G6R5_UNORM_PACK16:
                return EImageFormat::B5G6R5_UNORM_PACK16;
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
                return EImageFormat::R5G5B5A1_UNORM_PACK16;
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
                return EImageFormat::B5G5R5A1_UNORM_PACK16;
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
                return EImageFormat::A1R5G5B5_UNORM_PACK16;
            case VK_FORMAT_R8_UNORM:
                return EImageFormat::R8_UNORM;
            case VK_FORMAT_R8_SNORM:
                return EImageFormat::R8_SNORM;
            case VK_FORMAT_R8_USCALED:
                return EImageFormat::R8_USCALED;
            case VK_FORMAT_R8_SSCALED:
                return EImageFormat::R8_SSCALED;
            case VK_FORMAT_R8_UINT:
                return EImageFormat::R8_UINT;
            case VK_FORMAT_R8_SINT:
                return EImageFormat::R8_SINT;
            case VK_FORMAT_R8_SRGB:
                return EImageFormat::R8_SRGB;
            case VK_FORMAT_R8G8_UNORM:
                return EImageFormat::R8G8_UNORM;
            case VK_FORMAT_R8G8_SNORM:
                return EImageFormat::R8G8_SNORM;
            case VK_FORMAT_R8G8_USCALED:
                return EImageFormat::R8G8_USCALED;
            case VK_FORMAT_R8G8_SSCALED:
                return EImageFormat::R8G8_SSCALED;
            case VK_FORMAT_R8G8_UINT:
                return EImageFormat::R8G8_UINT;
            case VK_FORMAT_R8G8_SINT:
                return EImageFormat::R8G8_SINT;
            case VK_FORMAT_R8G8_SRGB:
                return EImageFormat::R8G8_SRGB;
            case VK_FORMAT_R8G8B8_UNORM:
                return EImageFormat::R8G8B8_UNORM;
            case VK_FORMAT_R8G8B8_SNORM:
                return EImageFormat::R8G8B8_SNORM;
            case VK_FORMAT_R8G8B8_USCALED:
                return EImageFormat::R8G8B8_USCALED;
            case VK_FORMAT_R8G8B8_SSCALED:
                return EImageFormat::R8G8B8_SSCALED;
            case VK_FORMAT_R8G8B8_UINT:
                return EImageFormat::R8G8B8_UINT;
            case VK_FORMAT_R8G8B8_SINT:
                return EImageFormat::R8G8B8_SINT;
            case VK_FORMAT_R8G8B8_SRGB:
                return EImageFormat::R8G8B8_SRGB;
            case VK_FORMAT_B8G8R8_UNORM:
                return EImageFormat::B8G8R8_UNORM;
            case VK_FORMAT_B8G8R8_SNORM:
                return EImageFormat::B8G8R8_SNORM;
            case VK_FORMAT_B8G8R8_USCALED:
                return EImageFormat::B8G8R8_USCALED;
            case VK_FORMAT_B8G8R8_SSCALED:
                return EImageFormat::B8G8R8_SSCALED;
            case VK_FORMAT_B8G8R8_UINT:
                return EImageFormat::B8G8R8_UINT;
            case VK_FORMAT_B8G8R8_SINT:
                return EImageFormat::B8G8R8_SINT;
            case VK_FORMAT_B8G8R8_SRGB:
                return EImageFormat::B8G8R8_SRGB;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return EImageFormat::R8G8B8A8_UNORM;
            case VK_FORMAT_R8G8B8A8_SNORM:
                return EImageFormat::R8G8B8A8_SNORM;
            case VK_FORMAT_R8G8B8A8_USCALED:
                return EImageFormat::R8G8B8A8_USCALED;
            case VK_FORMAT_R8G8B8A8_SSCALED:
                return EImageFormat::R8G8B8A8_SSCALED;
            case VK_FORMAT_R8G8B8A8_UINT:
                return EImageFormat::R8G8B8A8_UINT;
            case VK_FORMAT_R8G8B8A8_SINT:
                return EImageFormat::R8G8B8A8_SINT;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return EImageFormat::R8G8B8A8_SRGB;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return EImageFormat::B8G8R8A8_UNORM;
            case VK_FORMAT_B8G8R8A8_SNORM:
                return EImageFormat::B8G8R8A8_SNORM;
            case VK_FORMAT_B8G8R8A8_USCALED:
                return EImageFormat::B8G8R8A8_USCALED;
            case VK_FORMAT_B8G8R8A8_SSCALED:
                return EImageFormat::B8G8R8A8_SSCALED;
            case VK_FORMAT_B8G8R8A8_UINT:
                return EImageFormat::B8G8R8A8_UINT;
            case VK_FORMAT_B8G8R8A8_SINT:
                return EImageFormat::B8G8R8A8_SINT;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return EImageFormat::B8G8R8A8_SRGB;
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                return EImageFormat::A8B8G8R8_UNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
                return EImageFormat::A8B8G8R8_SNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
                return EImageFormat::A8B8G8R8_USCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
                return EImageFormat::A8B8G8R8_SSCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_UINT_PACK32:
                return EImageFormat::A8B8G8R8_UINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SINT_PACK32:
                return EImageFormat::A8B8G8R8_SINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                return EImageFormat::A8B8G8R8_SRGB_PACK32;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
                return EImageFormat::A2R10G10B10_UNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
                return EImageFormat::A2R10G10B10_SNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
                return EImageFormat::A2R10G10B10_USCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
                return EImageFormat::A2R10G10B10_SSCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:
                return EImageFormat::A2R10G10B10_UINT_PACK32;
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:
                return EImageFormat::A2R10G10B10_SINT_PACK32;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return EImageFormat::A2B10G10R10_UNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
                return EImageFormat::A2B10G10R10_SNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
                return EImageFormat::A2B10G10R10_USCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
                return EImageFormat::A2B10G10R10_SSCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:
                return EImageFormat::A2B10G10R10_UINT_PACK32;
            case VK_FORMAT_A2B10G10R10_SINT_PACK32:
                return EImageFormat::A2B10G10R10_SINT_PACK32;
            case VK_FORMAT_R16_UNORM:
                return EImageFormat::R16_UNORM;
            case VK_FORMAT_R16_SNORM:
                return EImageFormat::R16_SNORM;
            case VK_FORMAT_R16_USCALED:
                return EImageFormat::R16_USCALED;
            case VK_FORMAT_R16_SSCALED:
                return EImageFormat::R16_SSCALED;
            case VK_FORMAT_R16_UINT:
                return EImageFormat::R16_UINT;
            case VK_FORMAT_R16_SINT:
                return EImageFormat::R16_SINT;
            case VK_FORMAT_R16_SFLOAT:
                return EImageFormat::R16_SFLOAT;
            case VK_FORMAT_R16G16_UNORM:
                return EImageFormat::R16G16_UNORM;
            case VK_FORMAT_R16G16_SNORM:
                return EImageFormat::R16G16_SNORM;
            case VK_FORMAT_R16G16_USCALED:
                return EImageFormat::R16G16_USCALED;
            case VK_FORMAT_R16G16_SSCALED:
                return EImageFormat::R16G16_SSCALED;
            case VK_FORMAT_R16G16_UINT:
                return EImageFormat::R16G16_UINT;
            case VK_FORMAT_R16G16_SINT:
                return EImageFormat::R16G16_SINT;
            case VK_FORMAT_R16G16_SFLOAT:
                return EImageFormat::R16G16_SFLOAT;
            case VK_FORMAT_R16G16B16_UNORM:
                return EImageFormat::R16G16B16_UNORM;
            case VK_FORMAT_R16G16B16_SNORM:
                return EImageFormat::R16G16B16_SNORM;
            case VK_FORMAT_R16G16B16_USCALED:
                return EImageFormat::R16G16B16_USCALED;
            case VK_FORMAT_R16G16B16_SSCALED:
                return EImageFormat::R16G16B16_SSCALED;
            case VK_FORMAT_R16G16B16_UINT:
                return EImageFormat::R16G16B16_UINT;
            case VK_FORMAT_R16G16B16_SINT:
                return EImageFormat::R16G16B16_SINT;
            case VK_FORMAT_R16G16B16_SFLOAT:
                return EImageFormat::R16G16B16_SFLOAT;
            case VK_FORMAT_R16G16B16A16_UNORM:
                return EImageFormat::R16G16B16A16_UNORM;
            case VK_FORMAT_R16G16B16A16_SNORM:
                return EImageFormat::R16G16B16A16_SNORM;
            case VK_FORMAT_R16G16B16A16_USCALED:
                return EImageFormat::R16G16B16A16_USCALED;
            case VK_FORMAT_R16G16B16A16_SSCALED:
                return EImageFormat::R16G16B16A16_SSCALED;
            case VK_FORMAT_R16G16B16A16_UINT:
                return EImageFormat::R16G16B16A16_UINT;
            case VK_FORMAT_R16G16B16A16_SINT:
                return EImageFormat::R16G16B16A16_SINT;
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return EImageFormat::R16G16B16A16_SFLOAT;
            case VK_FORMAT_R32_UINT:
                return EImageFormat::R32_UINT;
            case VK_FORMAT_R32_SINT:
                return EImageFormat::R32_SINT;
            case VK_FORMAT_R32_SFLOAT:
                return EImageFormat::R32_SFLOAT;
            case VK_FORMAT_R32G32_UINT:
                return EImageFormat::R32G32_UINT;
            case VK_FORMAT_R32G32_SINT:
                return EImageFormat::R32G32_SINT;
            case VK_FORMAT_R32G32_SFLOAT:
                return EImageFormat::R32G32_SFLOAT;
            case VK_FORMAT_R32G32B32_UINT:
                return EImageFormat::R32G32B32_UINT;
            case VK_FORMAT_R32G32B32_SINT:
                return EImageFormat::R32G32B32_SINT;
            case VK_FORMAT_R32G32B32_SFLOAT:
                return EImageFormat::R32G32B32_SFLOAT;
            case VK_FORMAT_R32G32B32A32_UINT:
                return EImageFormat::R32G32B32A32_UINT;
            case VK_FORMAT_R32G32B32A32_SINT:
                return EImageFormat::R32G32B32A32_SINT;
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return EImageFormat::R32G32B32A32_SFLOAT;
            case VK_FORMAT_R64_UINT:
                return EImageFormat::R64_UINT;
            case VK_FORMAT_R64_SINT:
                return EImageFormat::R64_SINT;
            case VK_FORMAT_R64_SFLOAT:
                return EImageFormat::R64_SFLOAT;
            case VK_FORMAT_R64G64_UINT:
                return EImageFormat::R64G64_UINT;
            case VK_FORMAT_R64G64_SINT:
                return EImageFormat::R64G64_SINT;
            case VK_FORMAT_R64G64_SFLOAT:
                return EImageFormat::R64G64_SFLOAT;
            case VK_FORMAT_R64G64B64_UINT:
                return EImageFormat::R64G64B64_UINT;
            case VK_FORMAT_R64G64B64_SINT:
                return EImageFormat::R64G64B64_SINT;
            case VK_FORMAT_R64G64B64_SFLOAT:
                return EImageFormat::R64G64B64_SFLOAT;
            case VK_FORMAT_R64G64B64A64_UINT:
                return EImageFormat::R64G64B64A64_UINT;
            case VK_FORMAT_R64G64B64A64_SINT:
                return EImageFormat::R64G64B64A64_SINT;
            case VK_FORMAT_R64G64B64A64_SFLOAT:
                return EImageFormat::R64G64B64A64_SFLOAT;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return EImageFormat::B10G11R11_UFLOAT_PACK32;
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
                return EImageFormat::E5B9G9R9_UFLOAT_PACK32;
            case VK_FORMAT_D16_UNORM:
                return EImageFormat::D16_UNORM;
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return EImageFormat::X8_D24_UNORM_PACK32;
            case VK_FORMAT_D32_SFLOAT:
                return EImageFormat::D32_SFLOAT;
            case VK_FORMAT_S8_UINT:
                return EImageFormat::S8_UINT;
            case VK_FORMAT_D16_UNORM_S8_UINT:
                return EImageFormat::D16_UNORM_S8_UINT;
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return EImageFormat::D24_UNORM_S8_UINT;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return EImageFormat::D32_SFLOAT_S8_UINT;
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
                return EImageFormat::BC1_RGB_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
                return EImageFormat::BC1_RGB_SRGB_BLOCK;
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return EImageFormat::BC1_RGBA_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
                return EImageFormat::BC1_RGBA_SRGB_BLOCK;
            case VK_FORMAT_BC2_UNORM_BLOCK:
                return EImageFormat::BC2_UNORM_BLOCK;
            case VK_FORMAT_BC2_SRGB_BLOCK:
                return EImageFormat::BC2_SRGB_BLOCK;
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return EImageFormat::BC3_UNORM_BLOCK;
            case VK_FORMAT_BC3_SRGB_BLOCK:
                return EImageFormat::BC3_SRGB_BLOCK;
            case VK_FORMAT_BC4_UNORM_BLOCK:
                return EImageFormat::BC4_UNORM_BLOCK;
            case VK_FORMAT_BC4_SNORM_BLOCK:
                return EImageFormat::BC4_SNORM_BLOCK;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return EImageFormat::BC5_UNORM_BLOCK;
            case VK_FORMAT_BC5_SNORM_BLOCK:
                return EImageFormat::BC5_SNORM_BLOCK;
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:
                return EImageFormat::BC6H_UFLOAT_BLOCK;
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:
                return EImageFormat::BC6H_SFLOAT_BLOCK;
            case VK_FORMAT_BC7_UNORM_BLOCK:
                return EImageFormat::BC7_UNORM_BLOCK;
            case VK_FORMAT_BC7_SRGB_BLOCK:
                return EImageFormat::BC7_SRGB_BLOCK;
            default:
                break;
        }
    }
}