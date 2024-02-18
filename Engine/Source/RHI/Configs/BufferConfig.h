#pragma once

#include "../VkCommon.h"
#include "../Formats/BufferFormats.h"

namespace psm
{
    struct BufferConfig
    {
        SResourceSize Size;
        EBufferUsage Usage;
        EMemoryProperties MemoryProperties;
    };
}