#include "RHI.h"

#include "Interface/Device.h"

#ifdef RHI_VULKAN
#include "VkCommon.h"
#include "RHIVulkan.h"
#include "Vulkan/CVkDevice.h"
#endif

psm::API gCurrentAPI;

namespace psm
{
    API CurrentAPI()
    {
        return gCurrentAPI;
    }

    void InitRHI(const InitOptions& options)
    {
        gCurrentAPI = options.CurrentApi;
#ifdef RHI_VULKAN
        InitVk(options);
#endif
    }

    void ShutdownRHI()
    {
#ifdef RHI_VULKAN
        DeinitVk();
#endif
    }

    void IDevice::CreateSystemDefaultDevice(DevicePtr& device, PlatformConfig& config)
    {
#ifdef RHI_VULKAN
        device = CreateDefaultDeviceVk(config);
#endif
    }
}