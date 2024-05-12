#pragma once

namespace psm
{
    class IObject
    {
    public:
        IObject() = default;
        virtual ~IObject() = default;

        virtual void* Raw() = 0;
        virtual void* Raw() const = 0;
    };
}