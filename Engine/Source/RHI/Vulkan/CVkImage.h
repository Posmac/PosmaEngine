#pragma once

#include "../Interface/Image.h"
#include "../Context/ImageContext.h"

#include "RenderBackend/Core.h"
#include "RenderBackend/Image.h"

namespace psm
{
    class CVkImage : CImage
    {
    public:
        CVkImage();
        virtual ~CVkImage();
    public:
        virtual EImageType GetImageType() const override;
        virtual EImageFormat GetImageFormat() const override;
        virtual SImageSize GetImageSize() const override;
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