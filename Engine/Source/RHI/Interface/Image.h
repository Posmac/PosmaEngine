#pragma once

#include <stdint.h>

#include "../Context/ImageContext.h"
#include "../RHIObjects.h"

namespace psm
{
    class CImage /*maybe some inheritance needed*/
    {
    public:
        CImage() = default;
        virtual ~CImage() = default;
    public:
        virtual EImageType GetImageType() const = 0;
        virtual EImageFormat GetImageFormat() const = 0;
        virtual SImageSize GetImageSize() const = 0;
        virtual uint32_t GetImageWidth() const = 0;
        virtual uint32_t GetImageHeight() const = 0;
        virtual uint32_t GetImageDepth() const = 0;
        virtual uint32_t GetImageMips() const = 0;
        virtual uint32_t GetArrayLayers() const = 0;
        virtual uint32_t GetSampleCount() const = 0;
    };
}