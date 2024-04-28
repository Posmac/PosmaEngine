#include "CVkCommandBuffer.h"

#include "CVkDevice.h"
#include "CVkCommandPool.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkCommandBuffer::CVkCommandBuffer(DevicePtr device, VkCommandBuffer commandBuffer)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);
        mCommandBuffer = commandBuffer;
        assert(mCommandBuffer != nullptr);
    }

    CVkCommandBuffer::~CVkCommandBuffer()
    {
        assert(mCommandBuffer != nullptr);
    }

    void CVkCommandBuffer::Reset()
    {
        VkResult result = vkResetCommandBuffer(mCommandBuffer, 0);
        VK_CHECK_RESULT(result);
    }

    void CVkCommandBuffer::Begin(const SCommandBufferBeginConfig& config)
    {
        VkCommandBufferBeginInfo begin{};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = ToVulkan(config.Usage);
        begin.pNext = nullptr;
        begin.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(mCommandBuffer, &begin);
        VK_CHECK_RESULT(result);
    }

    void CVkCommandBuffer::End()
    {
        vkEndCommandBuffer(mCommandBuffer);
    }

    void* CVkCommandBuffer::GetRawPointer()
    {
        return mCommandBuffer;
    }
}