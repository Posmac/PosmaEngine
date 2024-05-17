#pragma once

#include "IObject.h"

namespace psm
{
    struct SFenceWaitConfig;

    class IFence : public IObject
    {
    public:
        IFence() = default;
        virtual ~IFence() = default;
        virtual bool Wait(const SFenceWaitConfig& config) = 0;
        virtual void Reset() = 0;
        virtual void Signal() = 0;
    };
}