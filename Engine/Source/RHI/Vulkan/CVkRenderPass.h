#pragma once

#include <memory>

#include "RHI/VkCommon.h"

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

        virtual void BeginRenderPass(const SRenderPassBeginConfig& config) override;
        virtual void EndRenderPass(CommandBufferPtr commandBuffer) override;
        virtual void* GetNativeRawPtr() override;
    private:
        VkDevice mDeviceInternal;
        VkRenderPass mRenderPass;
    };
}