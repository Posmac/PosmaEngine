#pragma once

namespace psm
{
    class ISemaphore
    {
    public:
        ISemaphore() = default;
        virtual ~ISemaphore() = default;
        virtual void Wait() = 0;
        virtual void Signal() = 0;
    };
}