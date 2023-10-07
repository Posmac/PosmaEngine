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
                                    const VkDynamicState* pDynamicStates,
                                    uint32_t dynamincStatesCount,
                                    VkPipelineMultisampleStateCreateInfo* msaa,
                                    VkPipelineVertexInputStateCreateInfo* vertexInput,
                                    VkPipelineInputAssemblyStateCreateInfo* inputAssembly,
                                    VkPipelineRasterizationStateCreateInfo* rasterizationState,
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

        void GetRasterizationStateInfo(VkBool32 depthClampEnable,
                                       VkBool32 rasterizerDiscardEnable,
                                       VkPolygonMode polygonMode,
                                       VkCullModeFlags cullMode,
                                       VkFrontFace frontFace,
                                       VkBool32 depthBiasEnable,
                                       float depthBiasConstantFactor,
                                       float depthBiasClamp,
                                       float depthBiasSlopeFactor,
                                       float lineWidth,
                                       VkPipelineRasterizationStateCreateInfo* rasterizationStateInfo);

        void GetPipelineMultisampleState(VkBool32 alphaToCoverageEnable,
                                         VkBool32 alphaToOneEnable,
                                         float minSampleShading,
                                         VkSampleMask* pSampleMask,
                                         VkSampleCountFlagBits rasterizationSamples,
                                         VkPipelineMultisampleStateCreateInfo* multisamplesState);
    }
}