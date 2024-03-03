#include "CVkFence.h"

#include "CVkDevice.h"

#include "RenderBackend/Synchronization.h"

namespace psm
{
    CVkFence::CVkFence(DevicePtr device, const SFenceConfig& config)
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

    void CVkFence::Wait(const SFenceWaitConfig& config)
    {
        vkWaitForFences(mDeviceInternal, 1, &mFence, config.WaitAll, config.Timeout);
    }

    void CVkFence::Reset()
    {
        vkResetFences(mDeviceInternal, 1, &mFence);
    }

    void CVkFence::Signal()
    {
        ///not used in case of fences
    }
}

