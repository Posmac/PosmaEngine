#include "CVkComputePipeline.h"

#include "CVkDevice.h"
#include "CVkCommandBuffer.h"
#include "CVkShader.h"
#include "CVkPipelineLayout.h"
#include "CVkRenderPass.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkComputePipeline::CVkComputePipeline(const DevicePtr& device, const SComputePipelineConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);

        VkPipelineShaderStageCreateInfo shaderStage =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = ToVulkan(config.ShaderModule.Type),
            .module = reinterpret_cast<VkShaderModule>(config.ShaderModule.Shader->Raw()),
            .pName = config.ShaderModule.EntryPoint,
            .pSpecializationInfo = nullptr,
        };

        VkComputePipelineCreateInfo computePipelineInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0, //enable later for derivatives etc
            .stage = shaderStage,
            .layout = reinterpret_cast<VkPipelineLayout>(config.PipelineLayout->Raw()),
            .basePipelineHandle = nullptr,
            .basePipelineIndex = 0,
        };

        VkResult result = vkCreateComputePipelines(mDeviceInternal, nullptr, 1, &computePipelineInfo, nullptr, &mPipeline);

        VK_CHECK_RESULT(result);
    }

    CVkComputePipeline::~CVkComputePipeline()
    {
        vkDestroyPipeline(mDeviceInternal, mPipeline, nullptr);
    }

    void CVkComputePipeline::Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw());
        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);
    }

    void* CVkComputePipeline::Raw()
    {
        return mPipeline;
    }

    void* CVkComputePipeline::Raw() const
    {
        return mPipeline;
    }
}
