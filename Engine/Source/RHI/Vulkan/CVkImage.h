#pragma once

#include "../Interface/Image.h"
#include "../Formats/ImageFormats.h"

#include "RenderBackend/Image.h"

namespace psm
{
    class CVkImage final : IImage
    {
    public:
        CVkImage() = default;
        virtual ~CVkImage() = default;
    public:
        virtual EImageType GetImageType() const override;
        virtual EImageFormat GetImageFormat() const override;
        virtual SResourceExtent3D GetImageSize() const override;
        virtual uint32_t GetImageWidth() const override;
        virtual uint32_t GetImageHeight() const override;
        virtual uint32_t GetImageDepth() const override;
        virtual uint32_t GetImageMips() const override;
        virtual uint32_t GetArrayLayers() const override;
        virtual uint32_t GetSampleCount() const override;
    private:
        VkImageCreateInfo m_ImageInfo;
        VkImage m_Image;
        VkDeviceMemory m_ImageMemory;
        VkImageView m_ImageView;
    };
}