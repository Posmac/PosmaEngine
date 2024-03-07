#pragma once

#include <cstdint>

namespace psm
{
    enum class EImageCreateFlags : uint8_t
    {
        NONE = 0,
    };

    enum class ESamplesCount : uint8_t
    {
        COUNT_1 = 1,
        COUNT_2 = 2,
        COUNT_4 = 4,
        COUNT_8 = 8,
        COUNT_16 = 16,
        COUNT_32 = 32,
        COUNT_64 = 64,
    };

    enum class EImageLayout : uint8_t
    {
        UNDEFINED = 0,
        GENERAL = 1,
        COLOR_ATTACHMENT_OPTIMAL = 2,
        DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
        DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
        SHADER_READ_ONLY_OPTIMAL = 5,
        TRANSFER_SRC_OPTIMAL = 6,
        TRANSFER_DST_OPTIMAL = 7,
        PREINITIALIZED = 8,
        DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 9,
        DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 10,
        DEPTH_ATTACHMENT_OPTIMAL = 11,
        DEPTH_READ_ONLY_OPTIMAL = 12,
        STENCIL_ATTACHMENT_OPTIMAL = 13,
        STENCIL_READ_ONLY_OPTIMAL = 14,
        READ_ONLY_OPTIMAL = 15,
        ATTACHMENT_OPTIMAL = 16,
        PRESENT_SRC_KHR = 17,
    };

    enum class EFormat : uint8_t
    {
        UNDEFINED = 0,
        R4G4_UNORM_PACK8 = 1,
        R4G4B4A4_UNORM_PACK16 = 2,
        B4G4R4A4_UNORM_PACK16 = 3,
        R5G6B5_UNORM_PACK16 = 4,
        B5G6R5_UNORM_PACK16 = 5,
        R5G5B5A1_UNORM_PACK16 = 6,
        B5G5R5A1_UNORM_PACK16 = 7,
        A1R5G5B5_UNORM_PACK16 = 8,
        R8_UNORM = 9,
        R8_SNORM = 10,
        R8_USCALED = 11,
        R8_SSCALED = 12,
        R8_UINT = 13,
        R8_SINT = 14,
        R8_SRGB = 15,
        R8G8_UNORM = 16,
        R8G8_SNORM = 17,
        R8G8_USCALED = 18,
        R8G8_SSCALED = 19,
        R8G8_UINT = 20,
        R8G8_SINT = 21,
        R8G8_SRGB = 22,
        R8G8B8_UNORM = 23,
        R8G8B8_SNORM = 24,
        R8G8B8_USCALED = 25,
        R8G8B8_SSCALED = 26,
        R8G8B8_UINT = 27,
        R8G8B8_SINT = 28,
        R8G8B8_SRGB = 29,
        B8G8R8_UNORM = 30,
        B8G8R8_SNORM = 31,
        B8G8R8_USCALED = 32,
        B8G8R8_SSCALED = 33,
        B8G8R8_UINT = 34,
        B8G8R8_SINT = 35,
        B8G8R8_SRGB = 36,
        R8G8B8A8_UNORM = 37,
        R8G8B8A8_SNORM = 38,
        R8G8B8A8_USCALED = 39,
        R8G8B8A8_SSCALED = 40,
        R8G8B8A8_UINT = 41,
        R8G8B8A8_SINT = 42,
        R8G8B8A8_SRGB = 43,
        B8G8R8A8_UNORM = 44,
        B8G8R8A8_SNORM = 45,
        B8G8R8A8_USCALED = 46,
        B8G8R8A8_SSCALED = 47,
        B8G8R8A8_UINT = 48,
        B8G8R8A8_SINT = 49,
        B8G8R8A8_SRGB = 50,
        A8B8G8R8_UNORM_PACK32 = 51,
        A8B8G8R8_SNORM_PACK32 = 52,
        A8B8G8R8_USCALED_PACK32 = 53,
        A8B8G8R8_SSCALED_PACK32 = 54,
        A8B8G8R8_UINT_PACK32 = 55,
        A8B8G8R8_SINT_PACK32 = 56,
        A8B8G8R8_SRGB_PACK32 = 57,
        A2R10G10B10_UNORM_PACK32 = 58,
        A2R10G10B10_SNORM_PACK32 = 59,
        A2R10G10B10_USCALED_PACK32 = 60,
        A2R10G10B10_SSCALED_PACK32 = 61,
        A2R10G10B10_UINT_PACK32 = 62,
        A2R10G10B10_SINT_PACK32 = 63,
        A2B10G10R10_UNORM_PACK32 = 64,
        A2B10G10R10_SNORM_PACK32 = 65,
        A2B10G10R10_USCALED_PACK32 = 66,
        A2B10G10R10_SSCALED_PACK32 = 67,
        A2B10G10R10_UINT_PACK32 = 68,
        A2B10G10R10_SINT_PACK32 = 69,
        R16_UNORM = 70,
        R16_SNORM = 71,
        R16_USCALED = 72,
        R16_SSCALED = 73,
        R16_UINT = 74,
        R16_SINT = 75,
        R16_SFLOAT = 76,
        R16G16_UNORM = 77,
        R16G16_SNORM = 78,
        R16G16_USCALED = 79,
        R16G16_SSCALED = 80,
        R16G16_UINT = 81,
        R16G16_SINT = 82,
        R16G16_SFLOAT = 83,
        R16G16B16_UNORM = 84,
        R16G16B16_SNORM = 85,
        R16G16B16_USCALED = 86,
        R16G16B16_SSCALED = 87,
        R16G16B16_UINT = 88,
        R16G16B16_SINT = 89,
        R16G16B16_SFLOAT = 90,
        R16G16B16A16_UNORM = 91,
        R16G16B16A16_SNORM = 92,
        R16G16B16A16_USCALED = 93,
        R16G16B16A16_SSCALED = 94,
        R16G16B16A16_UINT = 95,
        R16G16B16A16_SINT = 96,
        R16G16B16A16_SFLOAT = 97,
        R32_UINT = 98,
        R32_SINT = 99,
        R32_SFLOAT = 100,
        R32G32_UINT = 101,
        R32G32_SINT = 102,
        R32G32_SFLOAT = 103,
        R32G32B32_UINT = 104,
        R32G32B32_SINT = 105,
        R32G32B32_SFLOAT = 106,
        R32G32B32A32_UINT = 107,
        R32G32B32A32_SINT = 108,
        R32G32B32A32_SFLOAT = 109,
        R64_UINT = 110,
        R64_SINT = 111,
        R64_SFLOAT = 112,
        R64G64_UINT = 113,
        R64G64_SINT = 114,
        R64G64_SFLOAT = 115,
        R64G64B64_UINT = 116,
        R64G64B64_SINT = 117,
        R64G64B64_SFLOAT = 118,
        R64G64B64A64_UINT = 119,
        R64G64B64A64_SINT = 120,
        R64G64B64A64_SFLOAT = 121,
        B10G11R11_UFLOAT_PACK32 = 122,
        E5B9G9R9_UFLOAT_PACK32 = 123,
        D16_UNORM = 124,
        X8_D24_UNORM_PACK32 = 125,
        D32_SFLOAT = 126,
        S8_UINT = 127,
        D16_UNORM_S8_UINT = 128,
        D24_UNORM_S8_UINT = 129,
        D32_SFLOAT_S8_UINT = 130,
        BC1_RGB_UNORM_BLOCK = 131,
        BC1_RGB_SRGB_BLOCK = 132,
        BC1_RGBA_UNORM_BLOCK = 133,
        BC1_RGBA_SRGB_BLOCK = 134,
        BC2_UNORM_BLOCK = 135,
        BC2_SRGB_BLOCK = 136,
        BC3_UNORM_BLOCK = 137,
        BC3_SRGB_BLOCK = 138,
        BC4_UNORM_BLOCK = 139,
        BC4_SNORM_BLOCK = 140,
        BC5_UNORM_BLOCK = 141,
        BC5_SNORM_BLOCK = 142,
        BC6H_UFLOAT_BLOCK = 143,
        BC6H_SFLOAT_BLOCK = 144,
        BC7_UNORM_BLOCK = 145,
        BC7_SRGB_BLOCK = 146,
        //not complete
    };

    enum class EImageType : uint8_t
    {
        TYPE_1D = 0,
        TYPE_2D = 1,
        TYPE_3D = 2,
    };

    enum class EImageViewType : uint8_t
    {
        TYPE_1D = 0,
        TYPE_2D = 1,
        TYPE_3D = 2,
        TYPE_CUBE = 3,
        TYPE_1D_ARRAY = 4,
        TYPE_2D_ARRAY = 5,
        TYPE_CUBE_ARRAY = 6,
    };

    enum class EImageViewType : uint8_t
    {
        VIEW_TYPE_1D = 0,
        VIEW_TYPE_2D = 1,
        VIEW_TYPE_3D = 2,
        VIEW_TYPE_CUBE = 3,
        VIEW_TYPE_1D_ARRAY = 4,
        VIEW_TYPE_2D_ARRAY = 5,
        VIEW_TYPE_CUBE_ARRAY = 6,
    };

    enum class EImageUsageType : uint32_t
    {
        USAGE_TRANSFER_SRC_BIT = 0x00000001,
        USAGE_TRANSFER_DST_BIT = 0x00000002,
        USAGE_SAMPLED_BIT = 0x00000004,
        USAGE_STORAGE_BIT = 0x00000008,
        USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,
        USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000020,
        USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00000040,
        USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,
    };

    EImageUsageType operator &(const EImageUsageType& rhs, const EImageUsageType& lhs)
    {
        return static_cast<EImageUsageType>(static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs));
    }

    EImageUsageType operator | (const EImageUsageType& rhs, const EImageUsageType& lhs)
    {
        return static_cast<EImageUsageType>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    bool operator != (const EImageUsageType& rhs, const uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    enum class EImageTiling : uint8_t
    {
        OPTIMAL = 0,
        LINEAR = 1,
    };

    enum class EFeatureFormat : uint32_t
    {
        SAMPLED_IMAGE_BIT = 0x00000001,
        STORAGE_IMAGE_BIT = 0x00000002,
        STORAGE_IMAGE_ATOMIC_BIT = 0x00000004,
        UNIFORM_TEXEL_BUFFER_BIT = 0x00000008,
        STORAGE_TEXEL_BUFFER_BIT = 0x00000010,
        STORAGE_TEXEL_BUFFER_ATOMIC_BIT = 0x00000020,
        VERTEX_BUFFER_BIT = 0x00000040,
        COLOR_ATTACHMENT_BIT = 0x00000080,
        COLOR_ATTACHMENT_BLEND_BIT = 0x00000100,
        DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000200,
        BLIT_SRC_BIT = 0x00000400,
        BLIT_DST_BIT = 0x00000800,
        SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x00001000,
        TRANSFER_SRC_BIT = 0x00004000,
        TRANSFER_DST_BIT = 0x00008000,
    };

    EFeatureFormat operator & (const EFeatureFormat& rhs, const EFeatureFormat& lhs)
    {
        return static_cast<EFeatureFormat>(static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs));
    }

    bool operator != (const EFeatureFormat& rhs, const uint32_t& lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    enum class EImageAspect : uint8_t
    {
        COLOR_BIT = 0x00000001,
        DEPTH_BIT = 0x00000002,
        STENCIL_BIT = 0x00000004,
    };
}