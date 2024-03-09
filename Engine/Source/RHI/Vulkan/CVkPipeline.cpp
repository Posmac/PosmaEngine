#include "CVkPipeline.h"

namespace psm
{
    CVkPipeline::CVkPipeline(DevicePtr device, const SPipelineConfig& config)
    {
        //move inside vulkan class
        {
            VkPipelineVertexInputStateCreateInfo vertexInputState{};
            vk::GetVertexInputInfo(vertexAttribDescr, perVertexAttribsSize,
                bindingDescriptions, bindingsSize, &vertexInputState);

            //input assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
            vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

            VkPipelineMultisampleStateCreateInfo msState{};
            vk::GetPipelineMultisampleState(false, false, 1, nullptr, vk::MaxMsaaSamples, &msState);

            VkPipelineShaderStageCreateInfo stages[modulesSize];
            vk::GetPipelineShaderStages(modules, modulesSize, stages);
        }

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        vk::GetRasterizationStateInfo(VK_FALSE,
                                      VK_FALSE,
                                      VK_POLYGON_MODE_FILL,
                                      VK_CULL_MODE_BACK_BIT,
                                      VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                      VK_FALSE,
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      1.0f,
                                      &rasterizationStateInfo);

        //create graphics pipeline (a lot of default things)
        vk::CreateGraphicsPipeline(vk::Device, extent, renderPass,
                                   m_InstancedPipelineLayout, stages,
                                   modulesSize, dynamicStates, dynamicStatesCount,
                                   &msState, &vertexInputState,
                                   &inputAssemblyInfo, &rasterizationStateInfo,
                                   &m_InstancedPipeline);
    }

    void CVkPipeline::Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_InstancedPipeline);
    }
}
