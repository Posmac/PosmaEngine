#pragma once

#include <cassert>

#include "Include/vulkan/vulkan.h"
#include "RHI/Enums/SamplerFormats.h"
#include "RHI/Enums/BufferFormats.h"
#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/RenderPassFormats.h"
#include "RHI/Enums/ResourceFormats.h"
#include "RHI/Enums/PipelineFormats.h"
#include "RHI/Configs/CommandBuffers.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "RHI/Configs/ShadersConfig.h"

namespace psm
{
    inline VkFilter ToVulkan(EFilterMode mode)
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

    inline VkSamplerMipmapMode ToVulkan(ESamplerMipmapMode mode)
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

    inline VkSamplerAddressMode ToVulkan(ESamplerAddressMode mode)
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

    inline VkBorderColor ToVulkan(EBorderColor color)
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

    inline VkCompareOp ToVulkan(ECompareOp operation)
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

    inline VkBufferUsageFlags ToVulkan(EBufferUsage usage)
    {
        VkBufferUsageFlags flags = 0;
        if((usage & EBufferUsage::USAGE_TRANSFER_SRC_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if((usage & EBufferUsage::USAGE_TRANSFER_DST_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if((usage & EBufferUsage::USAGE_UNIFORM_TEXEL_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_STORAGE_TEXEL_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_UNIFORM_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_STORAGE_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_INDEX_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_VERTEX_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if((usage & EBufferUsage::USAGE_INDIRECT_BUFFER_BIT) != 0)
        {
            flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return flags;
    }

    inline VkMemoryPropertyFlags ToVulkan(EMemoryProperties properties)
    {
        VkMemoryPropertyFlags flags = 0;

        if((properties & EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_PROTECTED_BIT) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_PROTECTED_BIT;
        }

        if((properties & EMemoryProperties::MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
        }
        if((properties & EMemoryProperties::MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV) != 0)
        {
            flags |= VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
        }
        return flags;
    }

    inline EImageType FromVulkan(VkImageType type)
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
                return EImageType::TYPE_2D;
        }
    }

    inline EFormat FromVulkan(VkFormat format)
    {
        switch(format)
        {
            case VK_FORMAT_UNDEFINED:
                return EFormat::UNDEFINED;
            case VK_FORMAT_R4G4_UNORM_PACK8:
                return EFormat::R4G4_UNORM_PACK8;
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
                return EFormat::R4G4B4A4_UNORM_PACK16;
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
                return EFormat::B4G4R4A4_UNORM_PACK16;
            case VK_FORMAT_R5G6B5_UNORM_PACK16:
                return EFormat::R5G6B5_UNORM_PACK16;
            case VK_FORMAT_B5G6R5_UNORM_PACK16:
                return EFormat::B5G6R5_UNORM_PACK16;
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
                return EFormat::R5G5B5A1_UNORM_PACK16;
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
                return EFormat::B5G5R5A1_UNORM_PACK16;
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
                return EFormat::A1R5G5B5_UNORM_PACK16;
            case VK_FORMAT_R8_UNORM:
                return EFormat::R8_UNORM;
            case VK_FORMAT_R8_SNORM:
                return EFormat::R8_SNORM;
            case VK_FORMAT_R8_USCALED:
                return EFormat::R8_USCALED;
            case VK_FORMAT_R8_SSCALED:
                return EFormat::R8_SSCALED;
            case VK_FORMAT_R8_UINT:
                return EFormat::R8_UINT;
            case VK_FORMAT_R8_SINT:
                return EFormat::R8_SINT;
            case VK_FORMAT_R8_SRGB:
                return EFormat::R8_SRGB;
            case VK_FORMAT_R8G8_UNORM:
                return EFormat::R8G8_UNORM;
            case VK_FORMAT_R8G8_SNORM:
                return EFormat::R8G8_SNORM;
            case VK_FORMAT_R8G8_USCALED:
                return EFormat::R8G8_USCALED;
            case VK_FORMAT_R8G8_SSCALED:
                return EFormat::R8G8_SSCALED;
            case VK_FORMAT_R8G8_UINT:
                return EFormat::R8G8_UINT;
            case VK_FORMAT_R8G8_SINT:
                return EFormat::R8G8_SINT;
            case VK_FORMAT_R8G8_SRGB:
                return EFormat::R8G8_SRGB;
            case VK_FORMAT_R8G8B8_UNORM:
                return EFormat::R8G8B8_UNORM;
            case VK_FORMAT_R8G8B8_SNORM:
                return EFormat::R8G8B8_SNORM;
            case VK_FORMAT_R8G8B8_USCALED:
                return EFormat::R8G8B8_USCALED;
            case VK_FORMAT_R8G8B8_SSCALED:
                return EFormat::R8G8B8_SSCALED;
            case VK_FORMAT_R8G8B8_UINT:
                return EFormat::R8G8B8_UINT;
            case VK_FORMAT_R8G8B8_SINT:
                return EFormat::R8G8B8_SINT;
            case VK_FORMAT_R8G8B8_SRGB:
                return EFormat::R8G8B8_SRGB;
            case VK_FORMAT_B8G8R8_UNORM:
                return EFormat::B8G8R8_UNORM;
            case VK_FORMAT_B8G8R8_SNORM:
                return EFormat::B8G8R8_SNORM;
            case VK_FORMAT_B8G8R8_USCALED:
                return EFormat::B8G8R8_USCALED;
            case VK_FORMAT_B8G8R8_SSCALED:
                return EFormat::B8G8R8_SSCALED;
            case VK_FORMAT_B8G8R8_UINT:
                return EFormat::B8G8R8_UINT;
            case VK_FORMAT_B8G8R8_SINT:
                return EFormat::B8G8R8_SINT;
            case VK_FORMAT_B8G8R8_SRGB:
                return EFormat::B8G8R8_SRGB;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return EFormat::R8G8B8A8_UNORM;
            case VK_FORMAT_R8G8B8A8_SNORM:
                return EFormat::R8G8B8A8_SNORM;
            case VK_FORMAT_R8G8B8A8_USCALED:
                return EFormat::R8G8B8A8_USCALED;
            case VK_FORMAT_R8G8B8A8_SSCALED:
                return EFormat::R8G8B8A8_SSCALED;
            case VK_FORMAT_R8G8B8A8_UINT:
                return EFormat::R8G8B8A8_UINT;
            case VK_FORMAT_R8G8B8A8_SINT:
                return EFormat::R8G8B8A8_SINT;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return EFormat::R8G8B8A8_SRGB;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return EFormat::B8G8R8A8_UNORM;
            case VK_FORMAT_B8G8R8A8_SNORM:
                return EFormat::B8G8R8A8_SNORM;
            case VK_FORMAT_B8G8R8A8_USCALED:
                return EFormat::B8G8R8A8_USCALED;
            case VK_FORMAT_B8G8R8A8_SSCALED:
                return EFormat::B8G8R8A8_SSCALED;
            case VK_FORMAT_B8G8R8A8_UINT:
                return EFormat::B8G8R8A8_UINT;
            case VK_FORMAT_B8G8R8A8_SINT:
                return EFormat::B8G8R8A8_SINT;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return EFormat::B8G8R8A8_SRGB;
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                return EFormat::A8B8G8R8_UNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
                return EFormat::A8B8G8R8_SNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
                return EFormat::A8B8G8R8_USCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
                return EFormat::A8B8G8R8_SSCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_UINT_PACK32:
                return EFormat::A8B8G8R8_UINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SINT_PACK32:
                return EFormat::A8B8G8R8_SINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                return EFormat::A8B8G8R8_SRGB_PACK32;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
                return EFormat::A2R10G10B10_UNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
                return EFormat::A2R10G10B10_SNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
                return EFormat::A2R10G10B10_USCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
                return EFormat::A2R10G10B10_SSCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:
                return EFormat::A2R10G10B10_UINT_PACK32;
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:
                return EFormat::A2R10G10B10_SINT_PACK32;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return EFormat::A2B10G10R10_UNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
                return EFormat::A2B10G10R10_SNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
                return EFormat::A2B10G10R10_USCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
                return EFormat::A2B10G10R10_SSCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:
                return EFormat::A2B10G10R10_UINT_PACK32;
            case VK_FORMAT_A2B10G10R10_SINT_PACK32:
                return EFormat::A2B10G10R10_SINT_PACK32;
            case VK_FORMAT_R16_UNORM:
                return EFormat::R16_UNORM;
            case VK_FORMAT_R16_SNORM:
                return EFormat::R16_SNORM;
            case VK_FORMAT_R16_USCALED:
                return EFormat::R16_USCALED;
            case VK_FORMAT_R16_SSCALED:
                return EFormat::R16_SSCALED;
            case VK_FORMAT_R16_UINT:
                return EFormat::R16_UINT;
            case VK_FORMAT_R16_SINT:
                return EFormat::R16_SINT;
            case VK_FORMAT_R16_SFLOAT:
                return EFormat::R16_SFLOAT;
            case VK_FORMAT_R16G16_UNORM:
                return EFormat::R16G16_UNORM;
            case VK_FORMAT_R16G16_SNORM:
                return EFormat::R16G16_SNORM;
            case VK_FORMAT_R16G16_USCALED:
                return EFormat::R16G16_USCALED;
            case VK_FORMAT_R16G16_SSCALED:
                return EFormat::R16G16_SSCALED;
            case VK_FORMAT_R16G16_UINT:
                return EFormat::R16G16_UINT;
            case VK_FORMAT_R16G16_SINT:
                return EFormat::R16G16_SINT;
            case VK_FORMAT_R16G16_SFLOAT:
                return EFormat::R16G16_SFLOAT;
            case VK_FORMAT_R16G16B16_UNORM:
                return EFormat::R16G16B16_UNORM;
            case VK_FORMAT_R16G16B16_SNORM:
                return EFormat::R16G16B16_SNORM;
            case VK_FORMAT_R16G16B16_USCALED:
                return EFormat::R16G16B16_USCALED;
            case VK_FORMAT_R16G16B16_SSCALED:
                return EFormat::R16G16B16_SSCALED;
            case VK_FORMAT_R16G16B16_UINT:
                return EFormat::R16G16B16_UINT;
            case VK_FORMAT_R16G16B16_SINT:
                return EFormat::R16G16B16_SINT;
            case VK_FORMAT_R16G16B16_SFLOAT:
                return EFormat::R16G16B16_SFLOAT;
            case VK_FORMAT_R16G16B16A16_UNORM:
                return EFormat::R16G16B16A16_UNORM;
            case VK_FORMAT_R16G16B16A16_SNORM:
                return EFormat::R16G16B16A16_SNORM;
            case VK_FORMAT_R16G16B16A16_USCALED:
                return EFormat::R16G16B16A16_USCALED;
            case VK_FORMAT_R16G16B16A16_SSCALED:
                return EFormat::R16G16B16A16_SSCALED;
            case VK_FORMAT_R16G16B16A16_UINT:
                return EFormat::R16G16B16A16_UINT;
            case VK_FORMAT_R16G16B16A16_SINT:
                return EFormat::R16G16B16A16_SINT;
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return EFormat::R16G16B16A16_SFLOAT;
            case VK_FORMAT_R32_UINT:
                return EFormat::R32_UINT;
            case VK_FORMAT_R32_SINT:
                return EFormat::R32_SINT;
            case VK_FORMAT_R32_SFLOAT:
                return EFormat::R32_SFLOAT;
            case VK_FORMAT_R32G32_UINT:
                return EFormat::R32G32_UINT;
            case VK_FORMAT_R32G32_SINT:
                return EFormat::R32G32_SINT;
            case VK_FORMAT_R32G32_SFLOAT:
                return EFormat::R32G32_SFLOAT;
            case VK_FORMAT_R32G32B32_UINT:
                return EFormat::R32G32B32_UINT;
            case VK_FORMAT_R32G32B32_SINT:
                return EFormat::R32G32B32_SINT;
            case VK_FORMAT_R32G32B32_SFLOAT:
                return EFormat::R32G32B32_SFLOAT;
            case VK_FORMAT_R32G32B32A32_UINT:
                return EFormat::R32G32B32A32_UINT;
            case VK_FORMAT_R32G32B32A32_SINT:
                return EFormat::R32G32B32A32_SINT;
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return EFormat::R32G32B32A32_SFLOAT;
            case VK_FORMAT_R64_UINT:
                return EFormat::R64_UINT;
            case VK_FORMAT_R64_SINT:
                return EFormat::R64_SINT;
            case VK_FORMAT_R64_SFLOAT:
                return EFormat::R64_SFLOAT;
            case VK_FORMAT_R64G64_UINT:
                return EFormat::R64G64_UINT;
            case VK_FORMAT_R64G64_SINT:
                return EFormat::R64G64_SINT;
            case VK_FORMAT_R64G64_SFLOAT:
                return EFormat::R64G64_SFLOAT;
            case VK_FORMAT_R64G64B64_UINT:
                return EFormat::R64G64B64_UINT;
            case VK_FORMAT_R64G64B64_SINT:
                return EFormat::R64G64B64_SINT;
            case VK_FORMAT_R64G64B64_SFLOAT:
                return EFormat::R64G64B64_SFLOAT;
            case VK_FORMAT_R64G64B64A64_UINT:
                return EFormat::R64G64B64A64_UINT;
            case VK_FORMAT_R64G64B64A64_SINT:
                return EFormat::R64G64B64A64_SINT;
            case VK_FORMAT_R64G64B64A64_SFLOAT:
                return EFormat::R64G64B64A64_SFLOAT;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return EFormat::B10G11R11_UFLOAT_PACK32;
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
                return EFormat::E5B9G9R9_UFLOAT_PACK32;
            case VK_FORMAT_D16_UNORM:
                return EFormat::D16_UNORM;
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return EFormat::X8_D24_UNORM_PACK32;
            case VK_FORMAT_D32_SFLOAT:
                return EFormat::D32_SFLOAT;
            case VK_FORMAT_S8_UINT:
                return EFormat::S8_UINT;
            case VK_FORMAT_D16_UNORM_S8_UINT:
                return EFormat::D16_UNORM_S8_UINT;
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return EFormat::D24_UNORM_S8_UINT;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return EFormat::D32_SFLOAT_S8_UINT;
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
                return EFormat::BC1_RGB_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
                return EFormat::BC1_RGB_SRGB_BLOCK;
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return EFormat::BC1_RGBA_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
                return EFormat::BC1_RGBA_SRGB_BLOCK;
            case VK_FORMAT_BC2_UNORM_BLOCK:
                return EFormat::BC2_UNORM_BLOCK;
            case VK_FORMAT_BC2_SRGB_BLOCK:
                return EFormat::BC2_SRGB_BLOCK;
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return EFormat::BC3_UNORM_BLOCK;
            case VK_FORMAT_BC3_SRGB_BLOCK:
                return EFormat::BC3_SRGB_BLOCK;
            case VK_FORMAT_BC4_UNORM_BLOCK:
                return EFormat::BC4_UNORM_BLOCK;
            case VK_FORMAT_BC4_SNORM_BLOCK:
                return EFormat::BC4_SNORM_BLOCK;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return EFormat::BC5_UNORM_BLOCK;
            case VK_FORMAT_BC5_SNORM_BLOCK:
                return EFormat::BC5_SNORM_BLOCK;
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:
                return EFormat::BC6H_UFLOAT_BLOCK;
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:
                return EFormat::BC6H_SFLOAT_BLOCK;
            case VK_FORMAT_BC7_UNORM_BLOCK:
                return EFormat::BC7_UNORM_BLOCK;
            case VK_FORMAT_BC7_SRGB_BLOCK:
                return EFormat::BC7_SRGB_BLOCK;
            default:
                return EFormat::UNDEFINED;
        }
    }

    inline VkFormat ToVulkan(EFormat imageFormat)
    {
        switch(imageFormat)
        {
            case EFormat::UNDEFINED:
                return VK_FORMAT_UNDEFINED;
            case EFormat::R4G4_UNORM_PACK8:
                return VK_FORMAT_R4G4_UNORM_PACK8;
            case EFormat::R4G4B4A4_UNORM_PACK16:
                return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            case EFormat::B4G4R4A4_UNORM_PACK16:
                return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            case EFormat::R5G6B5_UNORM_PACK16:
                return VK_FORMAT_R5G6B5_UNORM_PACK16;
            case EFormat::B5G6R5_UNORM_PACK16:
                return VK_FORMAT_B5G6R5_UNORM_PACK16;
            case EFormat::R5G5B5A1_UNORM_PACK16:
                return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            case EFormat::B5G5R5A1_UNORM_PACK16:
                return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            case EFormat::A1R5G5B5_UNORM_PACK16:
                return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            case EFormat::R8_UNORM:
                return VK_FORMAT_R8_UNORM;
            case EFormat::R8_SNORM:
                return VK_FORMAT_R8_SNORM;
            case EFormat::R8_USCALED:
                return VK_FORMAT_R8_USCALED;
            case EFormat::R8_SSCALED:
                return VK_FORMAT_R8_SSCALED;
            case EFormat::R8_UINT:
                return VK_FORMAT_R8_UINT;
            case EFormat::R8_SINT:
                return VK_FORMAT_R8_SINT;
            case EFormat::R8_SRGB:
                return VK_FORMAT_R8_SRGB;
            case EFormat::R8G8_UNORM:
                return VK_FORMAT_R8G8_UNORM;
            case EFormat::R8G8_SNORM:
                return VK_FORMAT_R8G8_SNORM;
            case EFormat::R8G8_USCALED:
                return VK_FORMAT_R8G8_USCALED;
            case EFormat::R8G8_SSCALED:
                return VK_FORMAT_R8G8_SSCALED;
            case EFormat::R8G8_UINT:
                return VK_FORMAT_R8G8_UINT;
            case EFormat::R8G8_SINT:
                return VK_FORMAT_R8G8_SINT;
            case EFormat::R8G8_SRGB:
                return VK_FORMAT_R8G8_SRGB;
            case EFormat::R8G8B8_UNORM:
                return VK_FORMAT_R8G8B8_UNORM;
            case EFormat::R8G8B8_SNORM:
                return VK_FORMAT_R8G8B8_SNORM;
            case EFormat::R8G8B8_USCALED:
                return VK_FORMAT_R8G8B8_USCALED;
            case EFormat::R8G8B8_SSCALED:
                return VK_FORMAT_R8G8B8_SSCALED;
            case EFormat::R8G8B8_UINT:
                return VK_FORMAT_R8G8B8_UINT;
            case EFormat::R8G8B8_SINT:
                return VK_FORMAT_R8G8B8_SINT;
            case EFormat::R8G8B8_SRGB:
                return VK_FORMAT_R8G8B8_SRGB;
            case EFormat::B8G8R8_UNORM:
                return VK_FORMAT_B8G8R8_UNORM;
            case EFormat::B8G8R8_SNORM:
                return VK_FORMAT_B8G8R8_SNORM;
            case EFormat::B8G8R8_USCALED:
                return VK_FORMAT_B8G8R8_USCALED;
            case EFormat::B8G8R8_SSCALED:
                return VK_FORMAT_B8G8R8_SSCALED;
            case EFormat::B8G8R8_UINT:
                return VK_FORMAT_B8G8R8_UINT;
            case EFormat::B8G8R8_SINT:
                return VK_FORMAT_B8G8R8_SINT;
            case EFormat::B8G8R8_SRGB:
                return VK_FORMAT_B8G8R8_SRGB;
            case EFormat::R8G8B8A8_UNORM:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case EFormat::R8G8B8A8_SNORM:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case EFormat::R8G8B8A8_USCALED:
                return VK_FORMAT_R8G8B8A8_USCALED;
            case EFormat::R8G8B8A8_SSCALED:
                return VK_FORMAT_R8G8B8A8_SSCALED;
            case EFormat::R8G8B8A8_UINT:
                return VK_FORMAT_R8G8B8A8_UINT;
            case EFormat::R8G8B8A8_SINT:
                return VK_FORMAT_R8G8B8A8_SINT;
            case EFormat::R8G8B8A8_SRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case EFormat::B8G8R8A8_UNORM:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case EFormat::B8G8R8A8_SNORM:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case EFormat::B8G8R8A8_USCALED:
                return VK_FORMAT_B8G8R8A8_USCALED;
            case EFormat::B8G8R8A8_SSCALED:
                return VK_FORMAT_B8G8R8A8_SSCALED;
            case EFormat::B8G8R8A8_UINT:
                return VK_FORMAT_B8G8R8A8_UINT;
            case EFormat::B8G8R8A8_SINT:
                return VK_FORMAT_B8G8R8A8_SINT;
            case EFormat::B8G8R8A8_SRGB:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case EFormat::A8B8G8R8_UNORM_PACK32:
                return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
            case EFormat::A8B8G8R8_SNORM_PACK32:
                return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
            case EFormat::A8B8G8R8_USCALED_PACK32:
                return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
            case EFormat::A8B8G8R8_SSCALED_PACK32:
                return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
            case EFormat::A8B8G8R8_UINT_PACK32:
                return VK_FORMAT_A8B8G8R8_UINT_PACK32;
            case EFormat::A8B8G8R8_SINT_PACK32:
                return VK_FORMAT_A8B8G8R8_SINT_PACK32;
            case EFormat::A8B8G8R8_SRGB_PACK32:
                return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
            case EFormat::A2R10G10B10_UNORM_PACK32:
                return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
            case EFormat::A2R10G10B10_SNORM_PACK32:
                return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
            case EFormat::A2R10G10B10_USCALED_PACK32:
                return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
            case EFormat::A2R10G10B10_SSCALED_PACK32:
                return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
            case EFormat::A2R10G10B10_UINT_PACK32:
                return VK_FORMAT_A2R10G10B10_UINT_PACK32;
            case EFormat::A2R10G10B10_SINT_PACK32:
                return VK_FORMAT_A2R10G10B10_SINT_PACK32;
            case EFormat::A2B10G10R10_UNORM_PACK32:
                return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            case EFormat::A2B10G10R10_SNORM_PACK32:
                return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
            case EFormat::A2B10G10R10_USCALED_PACK32:
                return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
            case EFormat::A2B10G10R10_SSCALED_PACK32:
                return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
            case EFormat::A2B10G10R10_UINT_PACK32:
                return VK_FORMAT_A2B10G10R10_UINT_PACK32;
            case EFormat::A2B10G10R10_SINT_PACK32:
                return VK_FORMAT_A2B10G10R10_SINT_PACK32;
            case EFormat::R16_UNORM:
                return VK_FORMAT_R16_UNORM;
            case EFormat::R16_SNORM:
                return VK_FORMAT_R16_SNORM;
            case EFormat::R16_USCALED:
                return VK_FORMAT_R16_USCALED;
            case EFormat::R16_SSCALED:
                return VK_FORMAT_R16_SSCALED;
            case EFormat::R16_UINT:
                return VK_FORMAT_R16_UINT;
            case EFormat::R16_SINT:
                return VK_FORMAT_R16_SINT;
            case EFormat::R16_SFLOAT:
                return VK_FORMAT_R16_SFLOAT;
            case EFormat::R16G16_UNORM:
                return VK_FORMAT_R16G16_UNORM;
            case EFormat::R16G16_SNORM:
                return VK_FORMAT_R16G16_SNORM;
            case EFormat::R16G16_USCALED:
                return VK_FORMAT_R16G16_USCALED;
            case EFormat::R16G16_SSCALED:
                return VK_FORMAT_R16G16_SSCALED;
            case EFormat::R16G16_UINT:
                return VK_FORMAT_R16G16_UINT;
            case EFormat::R16G16_SINT:
                return VK_FORMAT_R16G16_SINT;
            case EFormat::R16G16_SFLOAT:
                return VK_FORMAT_R16G16_SFLOAT;
            case EFormat::R16G16B16_UNORM:
                return VK_FORMAT_R16G16B16_UNORM;
            case EFormat::R16G16B16_SNORM:
                return VK_FORMAT_R16G16B16_SNORM;
            case EFormat::R16G16B16_USCALED:
                return VK_FORMAT_R16G16B16_USCALED;
            case EFormat::R16G16B16_SSCALED:
                return VK_FORMAT_R16G16B16_SSCALED;
            case EFormat::R16G16B16_UINT:
                return VK_FORMAT_R16G16B16_UINT;
            case EFormat::R16G16B16_SINT:
                return VK_FORMAT_R16G16B16_SINT;
            case EFormat::R16G16B16_SFLOAT:
                return VK_FORMAT_R16G16B16_SFLOAT;
            case EFormat::R16G16B16A16_UNORM:
                return VK_FORMAT_R16G16B16A16_UNORM;
            case EFormat::R16G16B16A16_SNORM:
                return VK_FORMAT_R16G16B16A16_SNORM;
            case EFormat::R16G16B16A16_USCALED:
                return VK_FORMAT_R16G16B16A16_USCALED;
            case EFormat::R16G16B16A16_SSCALED:
                return VK_FORMAT_R16G16B16A16_SSCALED;
            case EFormat::R16G16B16A16_UINT:
                return VK_FORMAT_R16G16B16A16_UINT;
            case EFormat::R16G16B16A16_SINT:
                return VK_FORMAT_R16G16B16A16_SINT;
            case EFormat::R16G16B16A16_SFLOAT:
                return VK_FORMAT_R16G16B16A16_SFLOAT;
            case EFormat::R32_UINT:
                return VK_FORMAT_R32_UINT;
            case EFormat::R32_SINT:
                return VK_FORMAT_R32_SINT;
            case EFormat::R32_SFLOAT:
                return VK_FORMAT_R32_SFLOAT;
            case EFormat::R32G32_UINT:
                return VK_FORMAT_R32G32_UINT;
            case EFormat::R32G32_SINT:
                return VK_FORMAT_R32G32_SINT;
            case EFormat::R32G32_SFLOAT:
                return VK_FORMAT_R32G32_SFLOAT;
            case EFormat::R32G32B32_UINT:
                return VK_FORMAT_R32G32B32_UINT;
            case EFormat::R32G32B32_SINT:
                return VK_FORMAT_R32G32B32_SINT;
            case EFormat::R32G32B32_SFLOAT:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case EFormat::R32G32B32A32_UINT:
                return VK_FORMAT_R32G32B32A32_UINT;
            case EFormat::R32G32B32A32_SINT:
                return VK_FORMAT_R32G32B32A32_SINT;
            case EFormat::R32G32B32A32_SFLOAT:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case EFormat::R64_UINT:
                return VK_FORMAT_R64_UINT;
            case EFormat::R64_SINT:
                return VK_FORMAT_R64_SINT;
            case EFormat::R64_SFLOAT:
                return VK_FORMAT_R64_SFLOAT;
            case EFormat::R64G64_UINT:
                return VK_FORMAT_R64G64_UINT;
            case EFormat::R64G64_SINT:
                return VK_FORMAT_R64G64_SINT;
            case EFormat::R64G64_SFLOAT:
                return VK_FORMAT_R64G64_SFLOAT;
            case EFormat::R64G64B64_UINT:
                return VK_FORMAT_R64G64B64_UINT;
            case EFormat::R64G64B64_SINT:
                return VK_FORMAT_R64G64B64_SINT;
            case EFormat::R64G64B64_SFLOAT:
                return VK_FORMAT_R64G64B64_SFLOAT;
            case EFormat::R64G64B64A64_UINT:
                return VK_FORMAT_R64G64B64A64_UINT;
            case EFormat::R64G64B64A64_SINT:
                return VK_FORMAT_R64G64B64A64_SINT;
            case EFormat::R64G64B64A64_SFLOAT:
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            case EFormat::B10G11R11_UFLOAT_PACK32:
                return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case EFormat::E5B9G9R9_UFLOAT_PACK32:
                return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            case EFormat::D16_UNORM:
                return VK_FORMAT_D16_UNORM;
            case EFormat::X8_D24_UNORM_PACK32:
                return VK_FORMAT_X8_D24_UNORM_PACK32;
            case EFormat::D32_SFLOAT:
                return VK_FORMAT_D32_SFLOAT;
            case EFormat::S8_UINT:
                return VK_FORMAT_S8_UINT;
            case EFormat::D16_UNORM_S8_UINT:
                return VK_FORMAT_D16_UNORM_S8_UINT;
            case EFormat::D24_UNORM_S8_UINT:
                return VK_FORMAT_D24_UNORM_S8_UINT;
            case EFormat::D32_SFLOAT_S8_UINT:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    inline VkAttachmentDescriptionFlags ToVulkan(EAttachmentDescriptionFlags flags)
    {
        switch(flags)
        {
            case EAttachmentDescriptionFlags::NONE:
                return 0;
            default:
                return 0;
        }
    }

    inline VkSampleCountFlagBits ToVulkan(ESamplesCount samples)
    {
        switch(samples)
        {
            case ESamplesCount::COUNT_1:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            case ESamplesCount::COUNT_2:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
            case ESamplesCount::COUNT_4:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
            case ESamplesCount::COUNT_8:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
            case ESamplesCount::COUNT_16:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
            case ESamplesCount::COUNT_32:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;
            case ESamplesCount::COUNT_64:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
            default:
                return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        }
    }

    inline VkAttachmentLoadOp ToVulkan(EAttachmentLoadOp loadOp)
    {
        switch(loadOp)
        {
            case EAttachmentLoadOp::LOAD_OP_LOAD:
                return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
            case EAttachmentLoadOp::LOAD_OP_CLEAR:
                return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            case EAttachmentLoadOp::LOAD_OP_DONT_CARE:
                return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }

    inline VkAttachmentStoreOp ToVulkan(EAttachmentStoreOp storeOp)
    {
        switch(storeOp)
        {
            case EAttachmentStoreOp::STORE_OP_STORE:
                return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
            case EAttachmentStoreOp::STORE_OP_DONT_CARE:
                return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

    inline VkImageLayout ToVulkan(EImageLayout layout)
    {
        switch(layout)
        {
            case EImageLayout::UNDEFINED:
                return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            case EImageLayout::GENERAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
            case EImageLayout::COLOR_ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case EImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case EImageLayout::SHADER_READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case EImageLayout::TRANSFER_SRC_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case EImageLayout::TRANSFER_DST_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case EImageLayout::PREINITIALIZED:
                return VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;
            case EImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
            case EImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
            case EImageLayout::DEPTH_ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case EImageLayout::DEPTH_READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            case EImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            case EImageLayout::STENCIL_READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
            case EImageLayout::READ_ONLY_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            case EImageLayout::ATTACHMENT_OPTIMAL:
                return VkImageLayout::VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            case EImageLayout::PRESENT_SRC_KHR:
                return VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            default:
                return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    inline VkPipelineBindPoint ToVulkan(EPipelineBindPoint bindPoint)
    {
        switch(bindPoint)
        {
            case EPipelineBindPoint::GRAPHICS:
                return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
            case EPipelineBindPoint::COMPUTE:
                return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE;
            default:
                return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
    }

    inline VkPipelineStageFlags ToVulkan(EPipelineStageFlags stageFlags)
    {
        VkPipelineStageFlags flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_NONE;

        if((stageFlags & EPipelineStageFlags::TOP_OF_PIPE_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        if((stageFlags & EPipelineStageFlags::DRAW_INDIRECT_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        }
        if((stageFlags & EPipelineStageFlags::VERTEX_INPUT_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        }
        if((stageFlags & EPipelineStageFlags::VERTEX_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::TESSELLATION_CONTROL_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::TESSELLATION_EVALUATION_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::GEOMETRY_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::FRAGMENT_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        if((stageFlags & EPipelineStageFlags::LATE_FRAGMENT_TESTS_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        if((stageFlags & EPipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        if((stageFlags & EPipelineStageFlags::COMPUTE_SHADER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::TRANSFER_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        if((stageFlags & EPipelineStageFlags::BOTTOM_OF_PIPE_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        if((stageFlags & EPipelineStageFlags::HOST_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT;
        }
        if((stageFlags & EPipelineStageFlags::ALL_GRAPHICS_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        }
        if((stageFlags & EPipelineStageFlags::ALL_COMMANDS_BIT) != 0)
        {
            flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        return flags;
    }

    inline VkAccessFlags ToVulkan(EAccessFlags accessFlags)
    {
        VkAccessFlags flags = VkAccessFlagBits::VK_ACCESS_NONE;

        if((accessFlags & EAccessFlags::INDIRECT_COMMAND_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        }
        if((accessFlags & EAccessFlags::INDEX_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_INDEX_READ_BIT;
        }
        if((accessFlags & EAccessFlags::VERTEX_ATTRIBUTE_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        }
        if((accessFlags & EAccessFlags::UNIFORM_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_UNIFORM_READ_BIT;
        }
        if((accessFlags & EAccessFlags::INPUT_ATTACHMENT_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        }
        if((accessFlags & EAccessFlags::SHADER_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        }
        if((accessFlags & EAccessFlags::SHADER_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT;
        }
        if((accessFlags & EAccessFlags::COLOR_ATTACHMENT_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }
        if((accessFlags & EAccessFlags::COLOR_ATTACHMENT_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
        if((accessFlags & EAccessFlags::DEPTH_STENCIL_ATTACHMENT_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        }
        if((accessFlags & EAccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        if((accessFlags & EAccessFlags::TRANSFER_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
        }
        if((accessFlags & EAccessFlags::TRANSFER_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        if((accessFlags & EAccessFlags::HOST_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_HOST_READ_BIT;
        }
        if((accessFlags & EAccessFlags::HOST_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;
        }
        if((accessFlags & EAccessFlags::MEMORY_READ_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
        }
        if((accessFlags & EAccessFlags::MEMORY_WRITE_BIT) != 0)
        {
            flags |= VkAccessFlagBits::VK_ACCESS_MEMORY_WRITE_BIT;
        }

        return flags;
    }

    inline VkDependencyFlags ToVulkan(EDependencyFlags dependencyFlags)
    {
        switch(dependencyFlags)
        {
            case EDependencyFlags::NONE:
                return 0;
            case EDependencyFlags::BY_REGION_BIT:
                VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;
            default:
                break;
        }
    }

    inline VkFormatFeatureFlags ToVulkan(EFeatureFormat feature)
    {
        VkFormatFeatureFlags flags = 0;

        if((feature & EFeatureFormat::SAMPLED_IMAGE_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
        }
        if((feature & EFeatureFormat::STORAGE_IMAGE_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
        }
        if((feature & EFeatureFormat::STORAGE_IMAGE_ATOMIC_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
        }
        if((feature & EFeatureFormat::UNIFORM_TEXEL_BUFFER_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        }
        if((feature & EFeatureFormat::STORAGE_TEXEL_BUFFER_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
        }
        if((feature & EFeatureFormat::STORAGE_TEXEL_BUFFER_ATOMIC_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
        }
        if((feature & EFeatureFormat::VERTEX_BUFFER_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
        }
        if((feature & EFeatureFormat::COLOR_ATTACHMENT_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
        }
        if((feature & EFeatureFormat::COLOR_ATTACHMENT_BLEND_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        }
        if((feature & EFeatureFormat::DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if((feature & EFeatureFormat::BLIT_SRC_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        }
        if((feature & EFeatureFormat::BLIT_DST_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_DST_BIT;
        }
        if((feature & EFeatureFormat::SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
        }
        if((feature & EFeatureFormat::TRANSFER_SRC_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
        }
        if((feature & EFeatureFormat::TRANSFER_DST_BIT) != 0)
        {
            flags |= VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
        }

        return flags;
    }

    inline VkImageType ToVulkan(EImageType type)
    {
        switch(type)
        {
            case EImageType::TYPE_1D:
                return VkImageType::VK_IMAGE_TYPE_1D;
            case EImageType::TYPE_2D:
                return VkImageType::VK_IMAGE_TYPE_2D;
            case EImageType::TYPE_3D:
                return VkImageType::VK_IMAGE_TYPE_3D;
            default:
                return VkImageType::VK_IMAGE_TYPE_2D;
        }
    }

    inline VkImageTiling ToVulkan(EImageTiling tiling)
    {
        switch(tiling)
        {
            case EImageTiling::OPTIMAL:
                return VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
            case EImageTiling::LINEAR:
                return VkImageTiling::VK_IMAGE_TILING_LINEAR;
            default:
                return VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        }
    }

    inline VkImageUsageFlags ToVulkan(EImageUsageType usage)
    {
        VkImageUsageFlags flags = 0;
        if((usage & EImageUsageType::USAGE_TRANSFER_SRC_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if((usage & EImageUsageType::USAGE_TRANSFER_DST_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if((usage & EImageUsageType::USAGE_SAMPLED_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if((usage & EImageUsageType::USAGE_STORAGE_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if((usage & EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if((usage & EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if((usage & EImageUsageType::USAGE_TRANSIENT_ATTACHMENT_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        }
        if((usage & EImageUsageType::USAGE_INPUT_ATTACHMENT_BIT) != 0)
        {
            flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }

        return flags;
    }

    inline VkSharingMode ToVulkan(ESharingMode mode)
    {
        switch(mode)
        {
            case ESharingMode::EXCLUSIVE:
                VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
            case ESharingMode::CONCURRENT:
                VkSharingMode::VK_SHARING_MODE_CONCURRENT;
            default:
                VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        }

        return VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
    }

    inline VkImageCreateFlags ToVulkan(EImageCreateFlags flags)
    {
        return 0;
    }

    inline VkImageViewType ToVulkan(EImageViewType type)
    {
        switch(type)
        {
            case EImageViewType::TYPE_1D:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
            case EImageViewType::TYPE_2D:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
            case EImageViewType::TYPE_3D:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
            case EImageViewType::TYPE_CUBE:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
            case EImageViewType::TYPE_1D_ARRAY:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            case EImageViewType::TYPE_2D_ARRAY:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case EImageViewType::TYPE_CUBE_ARRAY:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            default:
                return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        }
    }

    inline VkImageAspectFlags ToVulkan(EImageAspect aspect)
    {
        switch(aspect)
        {
            case EImageAspect::COLOR_BIT:
                return VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
            case EImageAspect::DEPTH_BIT:
                return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
            case EImageAspect::STENCIL_BIT:
                return VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
            default:
                return VkImageAspectFlagBits::VK_IMAGE_ASPECT_NONE;

        }
    }

    inline VkCommandBufferUsageFlags ToVulkan(ECommandBufferUsage usage)
    {
        VkCommandBufferUsageFlags flags = 0;

        switch(usage)
        {
            case ECommandBufferUsage::NONE:
                return 0;
            case ECommandBufferUsage::ONE_TIME_SUBMIT_BIT:
                VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            case ECommandBufferUsage::RENDER_PASS_CONTINUE_BIT:
                VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            case ECommandBufferUsage::SIMULTANEOUS_USE_BIT:
                VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            default:
                return 0;
        }

        return flags;
    }

    inline VkSubpassContents ToVulkan(ESubpassContents contents)
    {
        switch(contents)
        {
            case ESubpassContents::INLINE:
                return VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE;
            case ESubpassContents::SECONDARY_COMMAND_BUFFERS:
                return VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
            default:
                return VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE;
        }
    }

    inline VkDescriptorType ToVulkan(EDescriptorType DescriptorType)
    {
        switch(DescriptorType)
        {
            case EDescriptorType::SAMPLER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
            case EDescriptorType::COMBINED_IMAGE_SAMPLER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case EDescriptorType::SAMPLED_IMAGE:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case EDescriptorType::STORAGE_IMAGE:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case EDescriptorType::UNIFORM_TEXEL_BUFFER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            case EDescriptorType::STORAGE_TEXEL_BUFFER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            case EDescriptorType::UNIFORM_BUFFER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case EDescriptorType::STORAGE_BUFFER:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            case EDescriptorType::STORAGE_BUFFER_DYNAMIC:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            case EDescriptorType::INPUT_ATTACHMENT:
                return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        }
    }

    inline VkShaderStageFlagBits ToVulkan(EShaderStageFlag ShaderStage)
    {
        switch(ShaderStage)
        {
            case EShaderStageFlag::VERTEX_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            case EShaderStageFlag::TESSELLATION_CONTROL_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            case EShaderStageFlag::TESSELLATION_EVALUATION_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            case EShaderStageFlag::GEOMETRY_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
            case EShaderStageFlag::FRAGMENT_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            case EShaderStageFlag::COMPUTE_BIT:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
            case EShaderStageFlag::ALL_GRAPHICS:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL_GRAPHICS;
            case EShaderStageFlag::ALL:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
        }
    }

    inline VkVertexInputRate ToVulkan(EVertexInputRate rate)
    {
        switch(rate)
        {
            case EVertexInputRate::VERTEX:
                return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
            case EVertexInputRate::INSTANCE:
                return VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
            default:
                return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    inline VkPrimitiveTopology ToVulkan(EPrimitiveTopology topology)
    {
        switch(topology)
        {
            case EPrimitiveTopology::POINT_LIST:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case EPrimitiveTopology::LINE_LIST:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case EPrimitiveTopology::LINE_STRIP:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case EPrimitiveTopology::TRIANGLE_LIST:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case EPrimitiveTopology::TRIANGLE_STRIP:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case EPrimitiveTopology::TRIANGLE_FAN:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            default:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    inline VkCullModeFlagBits ToVulkan(ECullMode CullMode)
    {
        switch(CullMode)
        {
            case ECullMode::NONE:
                return VkCullModeFlagBits::VK_CULL_MODE_NONE;
            case ECullMode::FRONT_BIT:
                return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            case ECullMode::BACK_BIT:
                return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            case ECullMode::FRONT_AND_BACK:
                return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
            default:
                return VkCullModeFlagBits::VK_CULL_MODE_NONE;
        }
    }

    inline VkPolygonMode ToVulkan(EPolygonMode PolygonMode)
    {
        switch(PolygonMode)
        {
            case EPolygonMode::FILL:
                return VkPolygonMode::VK_POLYGON_MODE_FILL;
            case EPolygonMode::LINE:
                return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case EPolygonMode::POINT:
                return VkPolygonMode::VK_POLYGON_MODE_POINT;
            default:
                return VkPolygonMode::VK_POLYGON_MODE_FILL;
        }
    }

    inline VkFrontFace ToVulkan(EFrontFace FrontFace)
    {
        switch(FrontFace)
        {
            case EFrontFace::COUNTER_CLOCKWISE:
                return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            case EFrontFace::CLOCKWISE:
                return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            default:
                return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        }
    }

    inline VkDynamicState ToVulkan(EDynamicState dynamicState)
    {
        switch(dynamicState)
        {
            case EDynamicState::VIEWPORT:
                return VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT;
            case EDynamicState::SCISSOR:
                return VkDynamicState::VK_DYNAMIC_STATE_SCISSOR;
            case EDynamicState::LINE_WIDTH:
                return VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH;
            case EDynamicState::DEPTH_BIAS:
                return VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS;
        }
    }

    inline VkIndexType ToVulkan(EIndexType indexType)
    {
        switch(indexType)
        {
            case EIndexType::UINT16:
                return VkIndexType::VK_INDEX_TYPE_UINT16;
            case EIndexType::UINT32:
                return VkIndexType::VK_INDEX_TYPE_UINT32;
            default:
                return VkIndexType::VK_INDEX_TYPE_UINT32;
        }
    }
}