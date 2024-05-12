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
    class CVkPipelineLayout : public IPipelineLayout, std::enable_shared_from_this<CVkPipelineLayout>
    {
    public:
        CVkPipelineLayout(DevicePtr device, const SPipelineLayoutConfig& config);
        virtual ~CVkPipelineLayout();

        virtual void* Raw() override;
        virtual void* Raw() const override;

    private:
        VkDevice mDeviceInternal;
        VkPipelineLayout mPipelineLayout;
    };
}