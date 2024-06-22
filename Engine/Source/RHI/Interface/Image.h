#pragma once

#include <cstdint>

#include "RHI/RHICommon.h"
#include "IObject.h"
#include "IResource.h"

#include "RHI/Enums/ImageFormats.h"

namespace psm
{
    class IImage : public IObject, public IResource
    {
    public:
        IImage() = default;
        virtual ~IImage() = default;
    public:
        virtual EImageType GetImageType() const = 0;
        virtual EFormat GetImageFormat() const = 0;
        virtual SResourceExtent3D GetImageSize() const = 0;
        virtual uint32_t GetImageWidth() const = 0;
        virtual uint32_t GetImageHeight() const = 0;
        virtual uint32_t GetImageDepth() const = 0;
        virtual uint32_t GetImageMips() const = 0;
        virtual uint32_t GetArrayLayers() const = 0;
        virtual uint32_t GetSampleCount() const = 0;

        virtual void* RawImageView() = 0;
        virtual void* RawImageView() const  = 0;
    };
}