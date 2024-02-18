#pragma once

namespace
{
    class IFence
    {
        IFence() = default;
        virtual ~IFence() = default;
        virtual void Wait() = 0;
        virtual void Reset() = 0;
        virtual void Signal() = 0;
    };
}