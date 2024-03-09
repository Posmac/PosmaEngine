#include "CVkDescriptorSetLayout.h"

namespace psm
{
    CVkDescriptorSetLayout::CVkDescriptorSetLayout(DevicePtr device, const SDesriptorSetLayoutConfig& config)
    {

        vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0,
                                      &m_ShadowDescriptorSetLayout);

        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool, { m_ShadowDescriptorSetLayout },
            1, &m_ShadowDescriptorSet);
    }
}