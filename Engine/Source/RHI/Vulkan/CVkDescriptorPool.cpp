#include "CVkDescriptorPool.h"

#include "CVkDevice.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkDescriptorPool::CVkDescriptorPool(const DevicePtr& device, const SDescriptorPoolConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        std::vector<VkDescriptorPoolSize> poolSize(config.DesciptorPoolSizes.size());

        for(int i = 0; i < config.DesciptorPoolSizes.size(); i++)
        {
            poolSize[i].type = ToVulkan(config.DesciptorPoolSizes[i].DescriptorType);
            poolSize[i].descriptorCount = config.DesciptorPoolSizes[i].MaxSize;
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pNext = nullptr;
        poolInfo.poolSizeCount = poolSize.size();
        poolInfo.pPoolSizes = poolSize.data();
        poolInfo.maxSets = config.MaxDesciptorPools;
        poolInfo.flags = 0; //for now 

        VkResult result = vkCreateDescriptorPool(mDeviceInternal, &poolInfo, nullptr, &mDescriptorPool);
        VK_CHECK_RESULT(result);
    }

    CVkDescriptorPool::~CVkDescriptorPool()
    {
        vkDestroyDescriptorPool(mDeviceInternal, mDescriptorPool, nullptr);
    }

    void* CVkDescriptorPool::Raw()
    {
        return mDescriptorPool;
    }

    void* CVkDescriptorPool::Raw() const
    {
        return mDescriptorPool;
    }
}