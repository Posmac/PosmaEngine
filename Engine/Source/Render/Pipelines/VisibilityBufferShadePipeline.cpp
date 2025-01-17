#include "VisibilityBufferShadePipeline.h"

#include "RHI/Vulkan/CVkDevice.h"

#include "Model/Vertex.h"

#include "Render/Graph/ResourceGeneralInfo.h"

namespace psm
{
    namespace graph
    {
        VisibilityBufferShadePipelineNode::VisibilityBufferShadePipelineNode(const foundation::Name& name, 
                                                                             const DevicePtr& device, 
                                                                             const ResourceMediatorPtr& resourceMediator)
            : RenderPipelineNode(name)
        {
            constexpr uint32_t descriptorSetLayoutSize = 1;
            DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutSize] =
            {
                resourceMediator->GetDescriptorSetLayoutByName(VISBUF_SHADE_SET_LAYOUT),
            };

            SPipelineLayoutConfig pipelineLayoutConfig =
            {
                .pLayouts = descriptorSetLayouts,
                .LayoutsSize = descriptorSetLayoutSize,
                .pPushConstants = nullptr,
                .PushConstantsSize = 0
            };

            mPipelineLayout = device->CreatePipelineLayout(pipelineLayoutConfig);

            ShaderPtr computeShader = device->CreateShaderFromFilename("../Engine/Shaders/visbufshade.csh.spirv", EShaderStageFlag::COMPUTE_BIT);

            SShaderModuleConfig module =
            {
                .Shader = computeShader,
                .Type = EShaderStageFlag::COMPUTE_BIT,
                .EntryPoint = "main"
            };

            SComputePipelineConfig pipelineConfig =
            {
                .PipelineLayout = mPipelineLayout,
                .ShaderModule = module,
            };

            mPipeline = device->CreateComputePipeline(pipelineConfig);

            resourceMediator->RegisterPipeline(graph::VISBUF_SHADE_PIPELINE, mPipeline);
        }

        VisibilityBufferShadePipelineNode::~VisibilityBufferShadePipelineNode()
        {
            LogMessage(MessageSeverity::Info, "VisibilityBufferShadePipelineNode destructor");
        }
    }
}