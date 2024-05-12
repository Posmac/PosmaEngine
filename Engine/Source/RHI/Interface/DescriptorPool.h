#pragma once

#include "IObject.h"

namespace psm
{
    class IDescriptorPool : public IObject
    {
    public:
        IDescriptorPool() = default;
        virtual ~IDescriptorPool() = default;
    };

    class IDescriptorSetLayout : public IObject
    {
    public:
        IDescriptorSetLayout() = default;
        virtual ~IDescriptorSetLayout() = default;
    };

    class IDescriptorSet : public IObject
    {
    public:
        IDescriptorSet() = default;
        virtual ~IDescriptorSet() = default;
    };
}