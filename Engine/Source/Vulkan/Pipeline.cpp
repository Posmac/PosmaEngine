#include "Pipeline.h"

namespace psm
{
    namespace vk
    {
        void CreatePipeline(VkDevice logicalDevice, VkShaderModule vertexModule,
            VkShaderModule fragmentModule, uint32_t vertexInputBindingDescriptionStride,
            VkExtent2D viewPortExtent, VkRenderPass renderPass,
            VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout,
            VkPipeline* pipeline)
        {
            VkDescriptorSetLayoutCreateInfo descriptorSetInfo{};
            descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetInfo.pNext = nullptr;
            descriptorSetInfo.flags = 0;
            descriptorSetInfo.bindingCount = 0;
            descriptorSetInfo.pBindings = nullptr;

            vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetInfo, nullptr,
                descriptorSetLayout);

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.pNext = nullptr;
            pipelineLayoutInfo.flags = 0;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

            VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr,
                pipelineLayout);

            std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
            shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[0].pNext = nullptr;
            shaderStages[0].flags = 0;
            shaderStages[0].pName = "main";
            shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[0].pSpecializationInfo = nullptr;
            shaderStages[0].module = vertexModule;

            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].pNext = nullptr;
            shaderStages[1].flags = 0;
            shaderStages[1].pName = "main";
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].pSpecializationInfo = nullptr;
            shaderStages[1].module = fragmentModule;

            VkVertexInputAttributeDescription vertexAttribDescr{};
            vertexAttribDescr.binding = 0;
            vertexAttribDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
            vertexAttribDescr.location = 0;
            vertexAttribDescr.offset = 0;

            VkVertexInputBindingDescription bindingDescr{};
            bindingDescr.binding = 0;
            bindingDescr.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescr.stride = vertexInputBindingDescriptionStride;

            VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
            vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputStateCreateInfo.pNext = nullptr;
            vertexInputStateCreateInfo.flags = 0;
            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
            vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
            vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescr;
            vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexAttribDescr;

            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
            inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyInfo.pNext = nullptr;
            inputAssemblyInfo.flags = 0;
            inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
            inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

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

            VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
            rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationStateInfo.pNext = nullptr;
            rasterizationStateInfo.flags = 0;
            rasterizationStateInfo.depthClampEnable = VK_FALSE;
            rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizationStateInfo.depthBiasEnable = VK_FALSE;
            rasterizationStateInfo.depthBiasConstantFactor = 0;
            rasterizationStateInfo.depthBiasClamp = 1.0f;
            rasterizationStateInfo.depthBiasSlopeFactor = 0;
            rasterizationStateInfo.lineWidth = 1;

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
            dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateCreateInfo.pNext = nullptr;
            dynamicStateCreateInfo.flags = 0;
            dynamicStateCreateInfo.dynamicStateCount = 2;
            dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

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

            VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
            graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphicsPipelineInfo.flags = 0;
            graphicsPipelineInfo.pNext = nullptr;
            graphicsPipelineInfo.stageCount = shaderStages.size();
            graphicsPipelineInfo.pStages = shaderStages.data();
            graphicsPipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
            graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
            graphicsPipelineInfo.pTessellationState = nullptr;
            graphicsPipelineInfo.pViewportState = &viewPortInfo;
            graphicsPipelineInfo.pRasterizationState = &rasterizationStateInfo;
            graphicsPipelineInfo.pMultisampleState = nullptr;
            graphicsPipelineInfo.pDepthStencilState = nullptr;
            graphicsPipelineInfo.pColorBlendState = &colorBlending;
            graphicsPipelineInfo.pDynamicState = &dynamicStateCreateInfo;
            graphicsPipelineInfo.layout = *pipelineLayout;
            graphicsPipelineInfo.renderPass = renderPass;
            graphicsPipelineInfo.subpass = 0;
            graphicsPipelineInfo.basePipelineHandle = nullptr;
            graphicsPipelineInfo.basePipelineIndex = 0;

            result = vkCreateGraphicsPipelines(logicalDevice, nullptr, 1,
                &graphicsPipelineInfo, nullptr, pipeline);
        }

        void DestroyPipeline(VkDevice device, VkPipeline pipeline)
        {
            vkDestroyPipeline(device, pipeline, nullptr);
        }

        void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout)
        {
            vkDestroyPipelineLayout(device, layout, nullptr);
        }
    }
}
