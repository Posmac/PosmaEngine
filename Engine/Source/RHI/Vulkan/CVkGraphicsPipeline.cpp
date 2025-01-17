#include "CVkGraphicsPipeline.h"

#include "CVkDevice.h"
#include "CVkCommandBuffer.h"
#include "CVkShader.h"
#include "CVkPipelineLayout.h"
#include "CVkRenderPass.h"
#include "TypeConvertor.h"

namespace psm
{
    CVkGraphicsPipeline::CVkGraphicsPipeline(const DevicePtr& device, const SGraphicsPipelineConfig& config)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);

        //move inside vulkan class
        VkPipelineVertexInputStateCreateInfo vertexInputState{};

        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(config.VertexInputBindingCount);
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(config.VertexInputAttributeCount);

        for(int i = 0; i < vertexBindingDescriptions.size(); i++)
        {
            vertexBindingDescriptions[i] =
            {
                .binding = config.pVertexInputBindings[i].Binding,
                .stride = config.pVertexInputBindings[i].Stride,
                .inputRate = ToVulkan(config.pVertexInputBindings[i].InputRate),
            };
        }

        for(int i = 0; i < vertexAttributeDescriptions.size(); i++)
        {
            vertexAttributeDescriptions[i] =
            {
                .location = config.pVertexInputAttributes[i].Location,
                .binding = config.pVertexInputAttributes[i].Binding,
                .format = ToVulkan(config.pVertexInputAttributes[i].Format),
                .offset = config.pVertexInputAttributes[i].Offset,
            };
        }

        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.pNext = nullptr;
        vertexInputState.flags = 0;
        vertexInputState.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        vertexInputState.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.data();

        //input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = nullptr;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.primitiveRestartEnable = config.InputAssembly.RestartPrimitives;
        inputAssemblyInfo.topology = ToVulkan(config.InputAssembly.Topology);

        VkPipelineMultisampleStateCreateInfo msState{};
        msState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msState.pNext = nullptr;
        msState.alphaToCoverageEnable = false;
        msState.alphaToOneEnable = false;
        msState.flags = 0;
        msState.minSampleShading = 1.0f;
        msState.pSampleMask = nullptr;
        msState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(config.ShaderModulesCount);

        for(int i = 0; i < shaderStages.size(); i++)
        {
            shaderStages[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = ToVulkan(config.pShaderModules[i].Type),
                .module = reinterpret_cast<VkShaderModule>(config.pShaderModules[i].Shader->Raw()),
                .pName = config.pShaderModules[i].EntryPoint,
                .pSpecializationInfo = nullptr,
            };
        }

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext = nullptr;
        rasterizationStateInfo.flags = 0;
        rasterizationStateInfo.depthClampEnable = config.Rasterization.DepthClampEnable;
        rasterizationStateInfo.rasterizerDiscardEnable = config.Rasterization.RasterizerDiscardEnable;
        rasterizationStateInfo.polygonMode = ToVulkan(config.Rasterization.PolygonMode);
        rasterizationStateInfo.cullMode = ToVulkan(config.Rasterization.CullMode);
        rasterizationStateInfo.frontFace = ToVulkan(config.Rasterization.FrontFace);
        rasterizationStateInfo.depthBiasEnable = config.Rasterization.DepthBiasEnable;
        rasterizationStateInfo.depthBiasConstantFactor = config.Rasterization.DepthBiasConstantFactor;
        rasterizationStateInfo.depthBiasClamp = config.Rasterization.DepthBiasClamp;
        rasterizationStateInfo.depthBiasSlopeFactor = config.Rasterization.DepthBiasSlopeFactor;
        rasterizationStateInfo.lineWidth = config.Rasterization.LineWidth;

        VkViewport viewPort{};
        viewPort.x = 0;
        viewPort.y = 0;
        viewPort.width = config.ViewPortExtent.width;
        viewPort.height = config.ViewPortExtent.height;
        viewPort.minDepth = 0;
        viewPort.maxDepth = 1;

        VkRect2D scriccors{};
        scriccors.extent = config.ViewPortExtent;
        scriccors.offset = { 0, 0 };

        VkPipelineViewportStateCreateInfo viewPortInfo{};
        viewPortInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewPortInfo.pNext = nullptr;
        viewPortInfo.flags = 0;
        viewPortInfo.viewportCount = 1;
        viewPortInfo.pViewports = &viewPort;
        viewPortInfo.scissorCount = 1;
        viewPortInfo.pScissors = &scriccors;

        std::vector<VkDynamicState> dynamicStates(config.DynamicStatesCount);
        for(int i = 0; i < config.DynamicStatesCount; i++)
        {
            dynamicStates[i] = ToVulkan(config.pDynamicStates[i]);
        }

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.pNext = nullptr;
        dynamicStateCreateInfo.flags = 0;
        dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(config.RenderPass->GetColorAttachmentsCount(), colorBlendAttachment);

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();
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
        depthInfo.depthCompareOp = ToVulkan(config.DepthStencilCompareOp);
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
        graphicsPipelineInfo.stageCount = shaderStages.size();
        graphicsPipelineInfo.pStages = shaderStages.data();
        graphicsPipelineInfo.pVertexInputState = &vertexInputState;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        graphicsPipelineInfo.pTessellationState = nullptr;
        graphicsPipelineInfo.pViewportState = &viewPortInfo;
        graphicsPipelineInfo.pRasterizationState = &rasterizationStateInfo;
        graphicsPipelineInfo.pMultisampleState = &msState;
        graphicsPipelineInfo.pDepthStencilState = &depthInfo;
        graphicsPipelineInfo.pColorBlendState = &colorBlending;
        graphicsPipelineInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineInfo.layout = reinterpret_cast<VkPipelineLayout>(config.PipelineLayout->Raw());
        graphicsPipelineInfo.renderPass = reinterpret_cast<VkRenderPass>(config.RenderPass->Raw());
        graphicsPipelineInfo.subpass = 0;
        graphicsPipelineInfo.basePipelineHandle = nullptr;
        graphicsPipelineInfo.basePipelineIndex = 0;

        VkResult result = vkCreateGraphicsPipelines(mDeviceInternal, nullptr, 1, &graphicsPipelineInfo, nullptr, &mPipeline);

        VK_CHECK_RESULT(result);
    }

    CVkGraphicsPipeline::~CVkGraphicsPipeline()
    {
        vkDestroyPipeline(mDeviceInternal, mPipeline, nullptr);
    }

    void CVkGraphicsPipeline::Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw());
        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
    }

    void* CVkGraphicsPipeline::Raw()
    {
        return mPipeline;
    }

    void* CVkGraphicsPipeline::Raw() const
    {
        return mPipeline;
    }
}
