#include "CVkSemaphore.h"

#include "CVkDevice.h"

namespace psm
{
    CVkSemaphore::CVkSemaphore(DevicePtr device, const SemaphoreConfig& config/*not used for now*/)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        VkResult result = vkCreateSemaphore(mDeviceInternal, &semaphoreInfo, nullptr, &mSemaphore);
        VK_CHECK_RESULT(result);
    }

    CVkSemaphore::~CVkSemaphore()
    {
        vkDestroySemaphore(mDeviceInternal, mSemaphore, nullptr);
    }

    void CVkSemaphore::Wait()
    {
        //not used
    }

    void CVkSemaphore::Signal()
    {
       
    }
}

