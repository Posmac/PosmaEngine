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
        virtual void SetViewport(CommandBufferPtr commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth) override;
        virtual void SetScissors(CommandBufferPtr commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent) override;
        virtual void* GetNativeRawPtr() override;
    private:
        VkDevice mDeviceInternal;
        VkRenderPass mRenderPass;
    };
}