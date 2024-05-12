#pragma once

#include "IObject.h"

namespace psm
{
    class IFramebuffer : public IObject
    {
    public:
        IFramebuffer() = default;
        virtual ~IFramebuffer() = default;
    };
}