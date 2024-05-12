#include "CVkCommandPool.h"

#include "CVkDevice.h"
#include "CVkCommandBuffer.h"

namespace psm
{
    CVkCommandPool::CVkCommandPool(DevicePtr device, const SCommandPoolConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);

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

    void CVkCommandPool::FreeCommandBuffers(const std::vector<CommandBufferPtr>& commandBuffers)
    {
        std::vector<VkCommandBuffer> vkCmdBuffers(commandBuffers.size());
        for(int i = 0; i < commandBuffers.size(); i++)
        {
            vkCmdBuffers[i] = reinterpret_cast<VkCommandBuffer>(commandBuffers[i]->Raw());
        }

        //we are not freeing all command buffers
        vkFreeCommandBuffers(mDeviceInternal, mCommandPool, vkCmdBuffers.size(), vkCmdBuffers.data());
    }

    void* CVkCommandPool::Raw()
    {
        return mCommandPool;
    }

    void* CVkCommandPool::Raw() const
    {
        return mCommandPool;
    }
}

