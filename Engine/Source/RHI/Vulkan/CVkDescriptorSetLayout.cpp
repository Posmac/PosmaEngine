#include "CVkDescriptorSetLayout.h"

#include "CVkDevice.h"

#include "RHI/VkCommon.h"
#include "RHI/Vulkan/TypeConvertor.h"

namespace psm
{
    CVkDescriptorSetLayout::CVkDescriptorSetLayout(DevicePtr device, const SDescriptorSetLayoutConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        std::vector<VkDescriptorSetLayoutBinding> bindings(config.LayoutsCount);

        for(int i = 0; i < config.LayoutsCount; i++)
        {
            bindings[i].binding = config.pLayoutsInfo[i].Binding;
            bindings[i].descriptorType = ToVulkan(config.pLayoutsInfo[i].DescriptorType);
            bindings[i].descriptorCount = config.pLayoutsInfo[i].DescriptorCount;
            bindings[i].stageFlags = ToVulkan(config.pLayoutsInfo[i].ShaderStage);
            bindings[i].pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
        descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutInfo.pNext = nullptr;
        descriptorLayoutInfo.bindingCount = bindings.size();
        descriptorLayoutInfo.pBindings = bindings.data();
        descriptorLayoutInfo.flags = 0; //for now 0

        VkResult result = vkCreateDescriptorSetLayout(mDeviceInternal, &descriptorLayoutInfo, nullptr, &mLayout);

        VK_CHECK_RESULT(result);
    }

    CVkDescriptorSetLayout::~CVkDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(mDeviceInternal, mLayout, nullptr);
    }

    void* CVkDescriptorSetLayout::GetPointer()
    {
        return mLayout;
    }
}