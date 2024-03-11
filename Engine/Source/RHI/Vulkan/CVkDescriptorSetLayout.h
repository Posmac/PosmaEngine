#pragma once

#include <memory>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/DescriptorPool.h"

#include "RHI/Configs/ShadersConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkDescriptorSetLayout : public IDescriptorSetLayout, std::enable_shared_from_this<CVkDescriptorSetLayout>
    {
    public:
        CVkDescriptorSetLayout(DevicePtr device, const SDescriptorSetLayoutConfig& config);
        virtual ~CVkDescriptorSetLayout();

        virtual void* GetPointer() override;
    private:
        VkDevice mDeviceInternal;
        VkDescriptorSetLayout mLayout;
    };

    class CVkDescriptorSet : public IDescriptorSet, std::enable_shared_from_this<CVkDescriptorSet>
    {
    public:
        CVkDescriptorSet(DevicePtr device, const SDescriptorSetLayoutConfig& config);
        virtual ~CVkDescriptorSet();

        virtual void* GetPointer() override;
    private:
        VkDevice mDeviceInternal;
        VkDescriptorSetLayout mLayout;
    };
}