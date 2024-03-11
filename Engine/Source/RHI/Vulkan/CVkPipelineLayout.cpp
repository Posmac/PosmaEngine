#include "CVkPipelineLayout.h"

namespace psm
{
    CVkPipelineLayout::CVkPipelineLayout(DevicePtr device, const SPipelineLayoutConfig& config)
    {
        vk::CreatePipelineLayout(vk::Device,
           descriptorSetLayouts,
           descriptorSetLayoutsSize,
           nullptr, //pPushConstants
           0, //push constants size 
           &m_InstancedPipelineLayout);
    }

    CVkPipelineLayout::~CVkPipelineLayout()
    {}
}