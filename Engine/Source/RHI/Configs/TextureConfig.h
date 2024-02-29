#pragma once

#include <optional>

#include "RHI/VkCommon.h"

#include "RHI/Enums/ImageFormats.h"
#include "RHI/Enums/ResourceFormats.h"

namespace psm
{
    struct SImageConfig
    {
        SResourceExtent3D ImageSize;
        int MipLevels;
        int ArrayLevels;
        EImageType Type;
        EImageFormat Format;
        EImageTiling Tiling;
        EImageLayout InitialLayout;
        EImageUsageType Usage;
        ESharingMode SharingMode;
        ESamplesCount SamplesCount;
        EImageCreateFlags Flags; //for now 0 everywhere

        EImageFormat ViewFormat;
        EImageViewType ViewType;
        EImageAspect ViewAspect;
    };
}