#pragma once

namespace psm
{
    class IDescriptorPool
    {
    public:
        IDescriptorPool() = default;
        virtual ~IDescriptorPool() = default;
        virtual DescriptorSetPtr AllocateDescriptorSets(const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts, uint32_t maxSets) = 0;
    };

    class IDescriptorSetLayout
    {
    public:
        IDescriptorSetLayout() = default;
        virtual ~IDescriptorSetLayout() = default;
    };
}