#pragma once

#include <memory>
#include "RHI/Interface/Types.h"
#include "RHI/Interface/Framebuffer.h"
#include "RHI/Configs/FramebuffersConfig.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkFramebuffer : public IFramebuffer, std::enable_shared_from_this<CVkFramebuffer>
    {
    public:
        CVkFramebuffer(DevicePtr device, const SFramebufferConfig& config);
        virtual ~CVkFramebuffer();

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mDeviceInternal;
        VkFramebuffer mFramebuffer;
    };
}