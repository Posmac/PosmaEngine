#pragma once

#include "RHI/RHICommon.h"
#include "IObject.h"
#include "Types.h"

namespace psm
{
    class ISurface : public IObject
    {
    public:
        ISurface() = default;
        virtual ~ISurface() = default;
    };
}