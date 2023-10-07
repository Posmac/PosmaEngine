#include "Pipeline.h"

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
                                    VkPipeline* pipeline)
        {
            VkViewport viewPort{};
            viewPort.x = 0;
            viewPort.y = 0;
            viewPort.width = viewPortExtent.width;
            viewPort.height = viewPortExtent.height;
            viewPort.minDepth = 0;
            viewPort.maxDepth = 1;

            VkRect2D scriccors{};
            scriccors.extent = viewPortExtent;
            scriccors.offset = { 0, 0 };

            VkPipelineViewportStateCreateInfo viewPortInfo{};
            viewPortInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewPortInfo.pNext = nullptr;
            viewPortInfo.flags = 0;
            viewPortInfo.viewportCount = 1;
            viewPortInfo.pViewports = &viewPort;
            viewPortInfo.scissorCount = 1;
            viewPortInfo.pScissors = &scriccors;

            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
            dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateCreateInfo.pNext = nullptr;
            dynamicStateCreateInfo.flags = 0;
            dynamicStateCreateInfo.dynamicStateCount = dynamincStatesCount;
            dynamicStateCreateInfo.pDynamicStates = pDynamicStates;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            VkPipelineDepthStencilStateCreateInfo depthInfo{};
            depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthInfo.pNext = nullptr;
            depthInfo.flags = 0;
            depthInfo.depthTestEnable = VK_TRUE;
            depthInfo.depthWriteEnable = VK_TRUE;
            depthInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            depthInfo.depthBoundsTestEnable = VK_FALSE;
            depthInfo.minDepthBounds = 0.0f; // Optional
            depthInfo.maxDepthBounds = 1.0f; // Optional
            depthInfo.stencilTestEnable = VK_FALSE;
            depthInfo.front = {}; // Optional
            depthInfo.back = {}; // Optional

            VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
            graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphicsPipelineInfo.flags = 0;
            graphicsPipelineInfo.pNext = nullptr;
            graphicsPipelineInfo.stageCount = shaderStagesCount;
            graphicsPipelineInfo.pStages = pShaderStages;
            graphicsPipelineInfo.pVertexInputState = vertexInput;
            graphicsPipelineInfo.pInputAssemblyState = inputAssembly;
            graphicsPipelineInfo.pTessellationState = nullptr;
            graphicsPipelineInfo.pViewportState = &viewPortInfo;
            graphicsPipelineInfo.pRasterizationState = rasterizationState;
            graphicsPipelineInfo.pMultisampleState = msaa;
            graphicsPipelineInfo.pDepthStencilState = &depthInfo;
            graphicsPipelineInfo.pColorBlendState = &colorBlending;
            graphicsPipelineInfo.pDynamicState = &dynamicStateCreateInfo;
            graphicsPipelineInfo.layout = pipelineLayout;
            graphicsPipelineInfo.renderPass = renderPass;
            graphicsPipelineInfo.subpass = 0;
            graphicsPipelineInfo.basePipelineHandle = nullptr;
            graphicsPipelineInfo.basePipelineIndex = 0;

            VkResult result = vkCreateGraphicsPipelines(logicalDevice, nullptr, 1,
                &graphicsPipelineInfo, nullptr, pipeline);

            VK_CHECK_RESULT(result);
        }

        void DestroyPipeline(VkDevice device, VkPipeline pipeline)
        {
            vkDestroyPipeline(device, pipeline, nullptr);
        }

        void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout)
        {
            vkDestroyPipelineLayout(device, layout, nullptr);
        }

        void GetInputAssembly(VkPrimitiveTopology topology, 
            bool restartPrimitives, 
            VkPipelineInputAssemblyStateCreateInfo* info)
        {
            info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            info->pNext = nullptr;
            info->flags = 0;
            info->primitiveRestartEnable = restartPrimitives;
            info->topology = topology;
        }

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
                                       VkPipelineRasterizationStateCreateInfo* rasterizationStateInfo)
        {
            rasterizationStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationStateInfo->pNext = nullptr;
            rasterizationStateInfo->flags = 0;
            rasterizationStateInfo->depthClampEnable = depthClampEnable;
            rasterizationStateInfo->rasterizerDiscardEnable = rasterizerDiscardEnable;
            rasterizationStateInfo->polygonMode = polygonMode;
            rasterizationStateInfo->cullMode = cullMode;
            rasterizationStateInfo->frontFace = frontFace;
            rasterizationStateInfo->depthBiasEnable = depthBiasEnable;
            rasterizationStateInfo->depthBiasConstantFactor = depthBiasConstantFactor;
            rasterizationStateInfo->depthBiasClamp = depthBiasClamp;
            rasterizationStateInfo->depthBiasSlopeFactor = depthBiasSlopeFactor;
            rasterizationStateInfo->lineWidth = lineWidth;
        }

        void GetPipelineMultisampleState(VkBool32 alphaToCoverageEnable, 
                                         VkBool32 alphaToOneEnable, 
                                         float minSampleShading,
                                         VkSampleMask* pSampleMask,
                                         VkSampleCountFlagBits rasterizationSamples,
                                         VkPipelineMultisampleStateCreateInfo* multisamplesState)
        {
            multisamplesState->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisamplesState->pNext = nullptr;
            multisamplesState->alphaToCoverageEnable = alphaToCoverageEnable;
            multisamplesState->alphaToOneEnable = alphaToOneEnable;
            multisamplesState->flags = 0;
            multisamplesState->minSampleShading = minSampleShading;
            multisamplesState->pSampleMask = pSampleMask;
            multisamplesState->rasterizationSamples = rasterizationSamples;
        }

        void GetPipelineShaderStages(const vk::ShaderModuleInfo* pShadersInfo,
            uint32_t shadersStagesSize,
            VkPipelineShaderStageCreateInfo* pipelineShaderStages)
        {
            for (int i = 0; i < shadersStagesSize; i++)
            {
                pipelineShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                pipelineShaderStages[i].pNext = nullptr;
                pipelineShaderStages[i].flags = 0;
                pipelineShaderStages[i].pName = pShadersInfo[i].Name.c_str();
                pipelineShaderStages[i].stage = pShadersInfo[i].Stage;
                pipelineShaderStages[i].pSpecializationInfo = nullptr;
                pipelineShaderStages[i].module = pShadersInfo[i].Module;
            }
        }

        void GetVertexInputInfo(const VkVertexInputAttributeDescription* pVertexInputAttribs,
            uint32_t vertexInputAttribsSize,
            const VkVertexInputBindingDescription* pVertexInputBindings,
            uint32_t vertexInputBindingsSize,
            VkPipelineVertexInputStateCreateInfo* info)
        {
            info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            info->pNext = nullptr;
            info->flags = 0;
            info->vertexAttributeDescriptionCount = vertexInputAttribsSize;
            info->pVertexAttributeDescriptions = pVertexInputAttribs;
            info->vertexBindingDescriptionCount = vertexInputBindingsSize;
            info->pVertexBindingDescriptions = pVertexInputBindings;
        }

        void CreatePipelineLayout(VkDevice device,
            const VkDescriptorSetLayout* pLayouts,
            const uint32_t layoutsSize,
            const VkPushConstantRange* pPushConstants,
            const uint32_t pushConstantsSize,
            VkPipelineLayout* pipelineLayout)
        {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.pNext = nullptr;
            pipelineLayoutInfo.setLayoutCount = layoutsSize;
            pipelineLayoutInfo.pSetLayouts = pLayouts;
            pipelineLayoutInfo.flags = 0;
            pipelineLayoutInfo.pushConstantRangeCount = pushConstantsSize;
            pipelineLayoutInfo.pPushConstantRanges = pPushConstants;

            VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                pipelineLayout);
        }
    }
}
