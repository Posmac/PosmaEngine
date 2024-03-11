#include "CVkPipeline.h"

#include "CVkCommandBuffer.h"

namespace psm
{
    CVkPipeline::CVkPipeline(DevicePtr device, const SPipelineConfig& config)
    {
        //move inside vulkan class
        {
            VkPipelineVertexInputStateCreateInfo vertexInputState{};

            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputState.pNext = nullptr;
            vertexInputState.flags = 0;
            vertexInputState.vertexAttributeDescriptionCount = vertexInputAttribsSize;
            vertexInputState.pVertexAttributeDescriptions = pVertexInputAttribs;
            vertexInputState.vertexBindingDescriptionCount = vertexInputBindingsSize;
            vertexInputState.pVertexBindingDescriptions = pVertexInputBindings;

            /*vk::GetVertexInputInfo(vertexAttribDescr, perVertexAttribsSize,
                bindingDescriptions, bindingsSize, &vertexInputState);*/

            //input assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
            inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyInfo.pNext = nullptr;
            inputAssemblyInfo.flags = 0;
            inputAssemblyInfo.primitiveRestartEnable = restartPrimitives;
            inputAssemblyInfo.topology = topology;

            //vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

            VkPipelineMultisampleStateCreateInfo msState{};
            msState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            msState.pNext = nullptr;
            msState.alphaToCoverageEnable = alphaToCoverageEnable;
            msState.alphaToOneEnable = alphaToOneEnable;
            msState.flags = 0;
            msState.minSampleShading = minSampleShading;
            msState.pSampleMask = pSampleMask;
            msState.rasterizationSamples = rasterizationSamples;
            //vk::GetPipelineMultisampleState(false, false, 1, nullptr, vk::MaxMsaaSamples, &msState);

            VkPipelineShaderStageCreateInfo stages[config.];
            //vk::GetPipelineShaderStages(modules, modulesSize, stages);
        }

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext = nullptr;
        rasterizationStateInfo.flags = 0;
        rasterizationStateInfo.depthClampEnable = depthClampEnable;
        rasterizationStateInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
        rasterizationStateInfo.polygonMode = polygonMode;
        rasterizationStateInfo.cullMode = cullMode;
        rasterizationStateInfo.frontFace = frontFace;
        rasterizationStateInfo.depthBiasEnable = depthBiasEnable;
        rasterizationStateInfo.depthBiasConstantFactor = depthBiasConstantFactor;
        rasterizationStateInfo.depthBiasClamp = depthBiasClamp;
        rasterizationStateInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
        rasterizationStateInfo.lineWidth = lineWidth;
        /*vk::GetRasterizationStateInfo(VK_FALSE,
                                      VK_FALSE,
                                      VK_POLYGON_MODE_FILL,
                                      VK_CULL_MODE_BACK_BIT,
                                      VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                      VK_FALSE,
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      1.0f,
                                      &rasterizationStateInfo);*/

        //create graphics pipeline (a lot of default things)
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

        /*vk::CreateGraphicsPipeline(vk::Device, extent, renderPass,
                                   m_InstancedPipelineLayout, stages,
                                   modulesSize, dynamicStates, dynamicStatesCount,
                                   &msState, &vertexInputState,
                                   &inputAssemblyInfo, &rasterizationStateInfo,
                                   &m_InstancedPipeline);*/
    }

    CVkPipeline::~CVkPipeline()
    {
        vkDestroyPipeline(mDeviceInternal, mPipeline, nullptr);
    }

    void CVkPipeline::Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->GetRawPointer());
        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
    }
}
