#pragma once

#include <stdint.h>

#include "../VkCommon.h"
#include "../Formats/ImageFormats.h"

namespace psm
{
    class IImage /*maybe some inheritance needed*/
    {
    public:
        IImage() = default;
        virtual ~IImage() = default;
    public:
        virtual EImageType GetImageType() const = 0;
        virtual EImageFormat GetImageFormat() const = 0;
        virtual SResourceExtent3D GetImageSize() const = 0;
        virtual uint32_t GetImageWidth() const = 0;
        virtual uint32_t GetImageHeight() const = 0;
        virtual uint32_t GetImageDepth() const = 0;
        virtual uint32_t GetImageMips() const = 0;
        virtual uint32_t GetArrayLayers() const = 0;
        virtual uint32_t GetSampleCount() const = 0;
    };
}