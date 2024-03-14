#pragma once

#include <memory>
#include <vector>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/Pipeline.h"

#include "RHI/Configs/PipelineConfig.h"
#include "RHI/Configs/PipelineLayoutConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkPipeline : public IPipeline, std::enable_shared_from_this<CVkPipeline>
    {
    public:
        CVkPipeline(DevicePtr device, const SPipelineConfig& config);
        virtual ~CVkPipeline();

        virtual void Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint) override;
        virtual void* GetPointer() override;

    private:
        VkDevice mDeviceInternal;
        VkPipeline mPipeline;
    };
}