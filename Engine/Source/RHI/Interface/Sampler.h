#pragma once

namespace psm
{
    class ISampler
    {
    public:
        ISampler() = default;
        virtual ~ISampler() = default;

        virtual void* GetPointer() = 0;
    };
}
