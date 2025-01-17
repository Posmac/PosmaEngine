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
    class CVkGraphicsPipeline : public IPipeline, std::enable_shared_from_this<CVkGraphicsPipeline>
    {
    public:
        CVkGraphicsPipeline(const DevicePtr& device, const SGraphicsPipelineConfig& config);
        virtual ~CVkGraphicsPipeline();

        virtual void Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint) override;
        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mDeviceInternal;
        VkPipeline mPipeline;
    };
}