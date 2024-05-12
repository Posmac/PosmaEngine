#pragma once

#include "IObject.h"

namespace psm
{
    class ISemaphore : public IObject
    {
    public:
        ISemaphore() = default;
        virtual ~ISemaphore() = default;
        virtual void Wait() = 0;
        virtual void Signal() = 0;
    };
}