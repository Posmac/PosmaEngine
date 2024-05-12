#pragma once

#include "IObject.h"

namespace psm
{
    class IShader : public IObject
    {
    public:
        IShader() = default;
        virtual ~IShader() = default;
    };
}