#pragma once

namespace psm
{
    class IDescriptorPool
    {
    public:
        IDescriptorPool() = default;
        virtual ~IDescriptorPool() = default;

        virtual void* GetPointer() = 0;
    };

    class IDescriptorSetLayout
    {
    public:
        IDescriptorSetLayout() = default;
        virtual ~IDescriptorSetLayout() = default;

        virtual void* GetPointer() = 0;
    };

    class IDescriptorSet
    {
    public:
        IDescriptorSet() = default;
        virtual ~IDescriptorSet() = default;

        virtual void* GetPointer() = 0;
    };
}