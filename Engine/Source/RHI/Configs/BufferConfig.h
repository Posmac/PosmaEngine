#pragma once

#include "../VkCommon.h"
#include "../Enums/BufferFormats.h"

namespace psm
{
    struct SBufferConfig
    {
        SResourceSize Size;
        EBufferUsage Usage;
        EMemoryProperties MemoryProperties;
    };
}