#pragma once

#include <memory>

#include "RHI/Interface/RenderPass.h"
#include "RHI/Interface/Types.h"

#include "RHI/Configs/RenderPassConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkRenderPass : public IRenderPass, std::enable_shared_from_this<CVkRenderPass>
    {
    public:
        CVkRenderPass(DevicePtr device, const SRenderPassConfig& config);
        virtual ~CVkRenderPass();

        virtual void BeginRenderPass() override;
        virtual void SetViewportAndScissors() override;
    private:
        VkDevice mDeviceInternal;
        VkRenderPass mRenderPass;
    };
}