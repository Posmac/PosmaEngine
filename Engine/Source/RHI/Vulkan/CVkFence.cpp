#include "CVkFence.h"

#include "CVkDevice.h"

namespace psm
{
    CVkFence::CVkFence(const DevicePtr& device, const SFenceConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = config.Signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VkResult result = vkCreateFence(mDeviceInternal, &fenceInfo, nullptr, &mFence);
        VK_CHECK_RESULT(result);
    }

    CVkFence::~CVkFence()
    {
        vkDestroyFence(mDeviceInternal, mFence, nullptr);
    }

    bool CVkFence::Wait(const SFenceWaitConfig& config)
    {
        VkResult result = vkWaitForFences(mDeviceInternal, 1, &mFence, config.WaitAll, config.Timeout);
        return result == VK_SUCCESS;
    }

    void CVkFence::Reset()
    {
        vkResetFences(mDeviceInternal, 1, &mFence);
    }

    void CVkFence::Signal()
    {
        ///not used in case of fences
    }

    void* CVkFence::Raw()
    {
        return mFence;
    }

    void* CVkFence::Raw() const
    {
        return mFence;
    }
}

