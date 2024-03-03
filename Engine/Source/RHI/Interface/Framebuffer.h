#pragma once

namespace psm
{
    class IFramebuffer
    {
    public:
        IFramebuffer() = default;
        virtual ~IFramebuffer() = default;

        virtual void* GetRawPointer() = 0;
    };
}