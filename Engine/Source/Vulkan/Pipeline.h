#pragma once

#include <vector>
#include <array>

#include "Vulkan/Core.h"
#include "Include/vulkan/vulkan.h"
#include "Shader.h"

namespace psm
{
    namespace vk
    {
        void CreateGraphicsPipeline(VkDevice logicalDevice,
            VkExtent2D viewPortExtent,
            VkRenderPass renderPass,
            VkPipelineLayout pipelineLayout,
            const VkPipelineShaderStageCreateInfo* pShaderStages,
            uint32_t shaderStagesCount,
            VkPipelineMultisampleStateCreateInfo msaa,
            VkPipelineVertexInputStateCreateInfo vertexInput,
            VkPipelineInputAssemblyStateCreateInfo inputAssembly,
            VkPipeline* pipeline);

        void DestroyPipeline(VkDevice device, VkPipeline pipeline);
        void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout);

        void CreatePipelineLayout(VkDevice device,
            const VkDescriptorSetLayout* pLayouts,
            const uint32_t layoutsArraySize,
            const VkPushConstantRange* pPushConstants,
            const uint32_t pushConstantsArraySize,
            VkPipelineLayout* pipelineLayout);

        void GetPipelineShaderStages(const vk::ShaderModuleInfo* pShadersInfo,
            uint32_t shadersStagesSize,
            VkPipelineShaderStageCreateInfo* pipelineShaderStages);

        void GetVertexInputInfo(const VkVertexInputAttributeDescription* pVertexInputAttribs,
            uint32_t vertexInputAttribsSize,
            const VkVertexInputBindingDescription* pVertexInputBindings,
            uint32_t vertexInputBindingsSize,
            VkPipelineVertexInputStateCreateInfo* info);

        void GetInputAssembly(VkPrimitiveTopology topology, 
            bool restartPrimitives,
            VkPipelineInputAssemblyStateCreateInfo* info);
    }
}