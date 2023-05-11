#pragma once

#include <vector>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreatePipeline(VkDevice logicalDevice, VkShaderModule vertexModule,
            VkShaderModule fragmentModule, uint32_t vertexInputBindingDescriptionStride,
            VkExtent2D viewPortExtent, VkRenderPass renderPass,
            VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout,
            VkPipeline* pipeline);
        void DestroyPipeline(VkDevice device, VkPipeline pipeline);
        void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout);
    }
}