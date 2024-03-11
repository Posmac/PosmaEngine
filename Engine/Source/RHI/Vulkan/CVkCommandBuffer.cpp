#include "CVkCommandBuffer.h"

#include "CVkDevice.h"
#include "CVkCommandPool.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkCommandBuffer::CVkCommandBuffer(DevicePtr device, CommandPoolPtr commandPool, const SCommandBufferConfig& config)
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

    void CVkCommandBuffer::ResetAtIndex(uint32_t index)
    {
        if(mCommandBuffers.size() > index)
        {
            VkResult result = vkResetCommandBuffer(mCommandBuffers[index], 0);
            VK_CHECK_RESULT(result);
        }
    }

    void CVkCommandBuffer::BeginAtIndex(const SCommandBufferBeginConfig& config)
    {
        VkCommandBufferBeginInfo begin{};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = ToVulkan(config.Usage);
        begin.pNext = nullptr;
        begin.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(mCommandBuffers[config.BufferIndex], &begin);
        VK_CHECK_RESULT(result);

        mCurrentCommandBuffer = config.BufferIndex;
    }

    void CVkCommandBuffer::EndCommandBuffer(uint32_t index)
    {
        vkEndCommandBuffer(mCommandBuffers[index]);
    }

    void* CVkCommandBuffer::GetRawPointer()
    {
        return mCommandBuffers[mCurrentCommandBuffer];
    }
}