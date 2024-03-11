#include "CVkCommandPool.h"

#include "CVkDevice.h"

namespace psm
{
    CVkCommandPool::CVkCommandPool(DevicePtr device, const SCommandPoolConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = config.QueueFamilyIndex;

        VkResult result = vkCreateCommandPool(mDeviceInternal, &commandPoolCreateInfo, nullptr, &mCommandPool);
        VK_CHECK_RESULT(result);
    }

    CVkCommandPool::~CVkCommandPool()
    {
        vkDestroyCommandPool(mDeviceInternal, mCommandPool, nullptr);
    }

    void* CVkCommandPool::GetCommandPool()
    {
        return mCommandPool;
    }
}

