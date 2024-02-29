#include "CVkCommandBuffer.h"

#include "CVkDevice.h"
#include "CVkCommandPool.h"

namespace psm
{
    CVkCommandBuffer::CVkCommandBuffer(DevicePtr device, CommandPoolPtr commandPool, const CommandBufferConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkCommandPool vkPool = reinterpret_cast<VkCommandPool>(commandPool->GetCommandPool());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = vkPool;
        allocInfo.level = config.IsBufferLevelPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = config.Size;

        mCommandBuffers.resize(config.Size);

        VkResult result = vkAllocateCommandBuffers(mDeviceInternal, &allocInfo, mCommandBuffers.data());
        VK_CHECK_RESULT(result);
    }

    CVkCommandBuffer::~CVkCommandBuffer()
    {

    }
}