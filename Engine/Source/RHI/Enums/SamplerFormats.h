#pragma once

namespace psm
{
    enum class EBorderColor
    {
        BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
        BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
        BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
        BORDER_COLOR_INT_OPAQUE_BLACK = 3,
        BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
        BORDER_COLOR_INT_OPAQUE_WHITE = 5,
        //not complete
    };

    enum class EFilterMode
    {
        FILTER_NEAREST = 0,
        FILTER_LINEAR = 1,
        //not complete
    };

    enum class ESamplerAddressMode
    {
        SAMPLER_MODE_REPEAT = 0,
        SAMPLER_MODE_MIRRORED_REPEAT = 1,
        SAMPLER_MODE_CLAMP_TO_EDGE = 2,
        SAMPLER_MODE_CLAMP_TO_BORDER = 3,
        SAMPLER_MODE_MIRROR_CLAMP_TO_EDGE = 4,
        //not complete
    };

    enum class ESamplerMipmapMode
    {
        SAMPLER_MIPMAP_MODE_NEAREST = 0,
        SAMPLER_MIPMAP_MODE_LINEAR = 1,
        SAMPLER_MIPMAP_MODE_MAX_ENUM = 0x7FFFFFFF //forces it to be 32 bit
    };

    enum class ECompareOp
    {
        COMPARE_OP_NEVER = 0,
        COMPARE_OP_LESS = 1,
        COMPARE_OP_EQUAL = 2,
        COMPARE_OP_LESS_OR_EQUAL = 3,
        COMPARE_OP_GREATER = 4,
        COMPARE_OP_NOT_EQUAL = 5,
        COMPARE_OP_GREATER_OR_EQUAL = 6,
        COMPARE_OP_ALWAYS = 7,
        COMPARE_OP_MAX_ENUM = 0x7FFFFFFF
    };
}