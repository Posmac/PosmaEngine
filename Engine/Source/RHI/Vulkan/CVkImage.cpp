#include "CVkImage.h"

#include "TypeConvertor.h"

namespace psm
{
    EImageType CVkImage::GetImageType() const
    {
        return FromVulkan(m_ImageInfo.imageType);
    }

    EImageFormat CVkImage::GetImageFormat() const
    {
        return FromVulkan(m_ImageInfo.format);
    }

    SResourceExtent3D CVkImage::GetImageSize() const
    {
        return m_ImageInfo.extent;
    }

    uint32_t CVkImage::GetImageWidth() const
    {
        return m_ImageInfo.extent.width;
    }

    uint32_t CVkImage::GetImageHeight() const
    {
        return m_ImageInfo.extent.height;
    }

    uint32_t CVkImage::GetImageDepth() const
    {
        return m_ImageInfo.extent.depth;
    }

    uint32_t CVkImage::GetImageMips() const
    {
        return m_ImageInfo.mipLevels;
    }

    uint32_t CVkImage::GetArrayLayers() const
    {
        return m_ImageInfo.arrayLayers;
    }

    uint32_t CVkImage::GetSampleCount() const
    {
        return m_ImageInfo.samples;
    }
}

