
#include "BufferFormats.h"
#include "ImageFormats.h"
#include "PipelineFormats.h"
#include "RenderPassFormats.h"
#include "ResourceFormats.h"
#include "SamplerFormats.h"

namespace psm
{
    EBufferUsage operator | (const EBufferUsage rhs, const EBufferUsage lhs)
    {
        return static_cast<EBufferUsage>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    uint32_t operator & (const EBufferUsage rhs, const EBufferUsage lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    EMemoryProperties operator | (const EMemoryProperties rhs, const EMemoryProperties lhs)
    {
        return static_cast<EMemoryProperties>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    uint32_t operator & (const EMemoryProperties rhs, const EMemoryProperties lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    uint32_t operator &(const EImageUsageType& rhs, const EImageUsageType& lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    EImageUsageType operator | (const EImageUsageType& rhs, const EImageUsageType& lhs)
    {
        return static_cast<EImageUsageType>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    bool operator != (const EImageUsageType& rhs, const uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    uint32_t operator & (const EFeatureFormat& rhs, const EFeatureFormat& lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    bool operator != (const EFeatureFormat& rhs, const uint32_t& lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    EPipelineStageFlags operator | (const EPipelineStageFlags& rhs, const EPipelineStageFlags& lhs)
    {
        return static_cast<EPipelineStageFlags>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    uint32_t operator & (const EPipelineStageFlags& rhs, const EPipelineStageFlags& lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    bool operator != (const EPipelineStageFlags rhs, uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }

    EAccessFlags operator | (const EAccessFlags& rhs, const EAccessFlags& lhs)
    {
        return static_cast<EAccessFlags>(static_cast<uint32_t>(rhs) | static_cast<uint32_t>(lhs));
    }

    uint32_t operator & (const EAccessFlags& rhs, const EAccessFlags& lhs)
    {
        return static_cast<uint32_t>(rhs) & static_cast<uint32_t>(lhs);
    }

    bool operator != (const EAccessFlags rhs, uint32_t lhs)
    {
        return static_cast<uint32_t>(rhs) != lhs;
    }
}