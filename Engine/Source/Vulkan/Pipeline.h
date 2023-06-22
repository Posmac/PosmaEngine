#pragma once

#include <vector>
#include <array>

#include "Vulkan/Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreatePipeline(VkDevice logicalDevice, VkShaderModule vertexModule,
            VkShaderModule fragmentModule, uint32_t vertexInputBindingDescriptionStride,
            VkExtent2D viewPortExtent, VkRenderPass renderPass,
            const std::vector< VkPushConstantRange>& pushConstants,
            const std::vector<VkDescriptorSetLayout>& layouts,
            VkPipelineLayout* pipelineLayout,
            VkPipeline* pipeline);
        void DestroyPipeline(VkDevice device, VkPipeline pipeline);
        void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout);
    }
}