#pragma once

namespace psm
{
    class ICommandPool
    {
    public:
        ICommandPool() = default;
        virtual ~ICommandPool() = default;

        virtual void* GetCommandPool() = 0;
    };
}