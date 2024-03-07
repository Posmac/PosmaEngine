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
        EFormat Format;
        EImageTiling Tiling;
        EImageLayout InitialLayout;
        EImageUsageType Usage;
        ESharingMode SharingMode;
        ESamplesCount SamplesCount;
        EImageCreateFlags Flags; //for now 0 everywhere

        EFormat ViewFormat;
        EImageViewType ViewType;
        EImageAspect ViewAspect;
    };
}