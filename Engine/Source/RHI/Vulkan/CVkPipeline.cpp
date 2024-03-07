#include "CVkPipeline.h"

namespace psm
{
    CVkPipeline::CVkPipeline(DevicePtr device, const SPipelineConfig& config)
    {

    }

    void CVkPipeline::Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_InstancedPipeline);
    }
}
