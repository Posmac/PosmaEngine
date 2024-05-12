#pragma once

#include "IObject.h"

namespace psm
{
    class ISampler : public IObject
    {
    public:
        ISampler() = default;
        virtual ~ISampler() = default;
    };
}
