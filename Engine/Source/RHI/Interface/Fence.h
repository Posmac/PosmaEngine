#pragma once

namespace psm
{
    struct SFenceWaitConfig;

    class IFence
    {
    public:
        IFence() = default;
        virtual ~IFence() = default;
        virtual void Wait(const SFenceWaitConfig& config) = 0;
        virtual void Reset() = 0;
        virtual void Signal() = 0;

        virtual void* GetPointer() = 0;
    };
}