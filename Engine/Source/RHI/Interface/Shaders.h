#pragma once

namespace psm
{
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() = default;

        virtual void* GetPointer() = 0;
    };
}