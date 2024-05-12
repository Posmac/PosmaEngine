#include "CVkPipelineLayout.h"

#include "CVkDevice.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkPipelineLayout::CVkPipelineLayout(DevicePtr device, const SPipelineLayoutConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        std::vector<VkDescriptorSetLayout> layouts(config.LayoutsSize);

        for(int i = 0; i < config.LayoutsSize; i++)
        {
            layouts[i] = reinterpret_cast<VkDescriptorSetLayout>(config.pLayouts[i]->Raw());
        }

        std::vector<VkPushConstantRange> pushConstants(config.PushConstantsSize);

        for(int i = 0; i < config.PushConstantsSize; i++)
        {
            pushConstants[i] =
            {
                .stageFlags = static_cast<uint32_t>(ToVulkan(config.pPushConstants[i].StageFlags)),
                .offset = config.pPushConstants[i].Offset,
                .size = config.pPushConstants[i].Size,
            };
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pNext = nullptr;
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.flags = 0;
        pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        VkResult result = vkCreatePipelineLayout(mDeviceInternal, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    }

    CVkPipelineLayout::~CVkPipelineLayout()
    {
        vkDestroyPipelineLayout(mDeviceInternal, mPipelineLayout, nullptr);
    }

    void* CVkPipelineLayout::Raw()
    {
        return mPipelineLayout;
    }

    void* CVkPipelineLayout::Raw() const
    {
        return mPipelineLayout;
    }
}