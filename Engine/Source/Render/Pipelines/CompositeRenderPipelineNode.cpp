#include "CompositeRenderPipelineNode.h"

#include "RHI/Vulkan/CVkDevice.h"

#include "Model/Vertex.h"

#include "Render/Graph/ResourceGeneralInfo.h"

namespace psm
{
    namespace graph
    {
        CompositeRenderPipelineNode::CompositeRenderPipelineNode(const foundation::Name& name,
                                                     const DevicePtr& device,
                                                     const RenderPassPtr& renderPass,
                                                     const ResourceMediatorPtr& resourceMediator,
                                                     const SResourceExtent3D viewportSize) 
            : RenderPipelineNode(name)
        {
            
            constexpr uint32_t descriptorSetLayoutsSize = 2;
            DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
            {
                resourceMediator->GetDescriptorSetLayoutByName(GBUFFER_RESULT_COMPOSITE_INPUT_SET),
                resourceMediator->GetDescriptorSetLayoutByName(COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET),
            };

            SPipelineLayoutConfig pipelingLayoutConfig =
            {
                .pLayouts = descriptorSetLayouts,
                .LayoutsSize = static_cast<uint32_t>(descriptorSetLayoutsSize),
                .pPushConstants = nullptr,
                .PushConstantsSize = 0
            };

            mPipelineLayout = device->CreatePipelineLayout(pipelingLayoutConfig);

            //shader stages (describe all shader stages used in pipeline)

            ShaderPtr vertexShader = device->CreateShaderFromFilename("../Engine/Shaders/composite.vert.spirv", EShaderStageFlag::VERTEX_BIT);
            ShaderPtr fragmentShader = device->CreateShaderFromFilename("../Engine/Shaders/composite.frag.spirv", EShaderStageFlag::FRAGMENT_BIT);

            constexpr size_t modulesSize = 2;
            SShaderModuleConfig modules[modulesSize] =
            {
                {
                    .Shader = vertexShader,                 // shader module 
                    .Type = EShaderStageFlag::VERTEX_BIT,   // VkShaderStageFlag
                    .EntryPoint = "main"                        // entry point
                },
                {
                    .Shader = fragmentShader,               // shader module 
                    .Type = EShaderStageFlag::FRAGMENT_BIT, // VkShaderStageFlag
                    .EntryPoint = "main"                        // entry point
                },
            };

            SInputAssemblyConfig inputAssembly =
            {
                .Topology = EPrimitiveTopology::TRIANGLE_LIST,
                .RestartPrimitives = false
            };

            constexpr size_t dynamicStatesCount = 2;
            EDynamicState dynamicStates[dynamicStatesCount] =
            {
                EDynamicState::SCISSOR,
                EDynamicState::VIEWPORT
            };

            SRasterizationConfig rasterization =
            {
                .DepthClampEnable = false,
                .RasterizerDiscardEnable = false,
                .CullMode = ECullMode::BACK_BIT,
                .PolygonMode = EPolygonMode::FILL,
                .FrontFace = EFrontFace::COUNTER_CLOCKWISE,
                .DepthBiasEnable = false,
                .DepthBiasConstantFactor = 0.0f,
                .DepthBiasClamp = 0.0f,
                .DepthBiasSlopeFactor = 0.0f,
                .LineWidth = 1.0f,
            };

            SGraphicsPipelineConfig pipelineConfig =
            {
                .RenderPass = renderPass,
                .ViewPortExtent = {viewportSize.width, viewportSize.height},
                .PipelineLayout = mPipelineLayout,
                .pVertexInputAttributes = nullptr,
                .VertexInputAttributeCount = 0,
                .pVertexInputBindings = nullptr,
                .VertexInputBindingCount = 0,
                .pShaderModules = modules,
                .ShaderModulesCount = modulesSize,
                .pDynamicStates = dynamicStates,
                .DynamicStatesCount = dynamicStatesCount,
                .InputAssembly = inputAssembly,
                .Rasterization = rasterization,
                .DepthStencilCompareOp = ECompareOp::COMPARE_OP_GREATER,
            };

            mPipeline = device->CreateGraphicsPipeline(pipelineConfig);

            resourceMediator->RegisterPipeline(graph::COMPOSITE_GRAPHICS_PIPELINE, mPipeline);
        }

        CompositeRenderPipelineNode::~CompositeRenderPipelineNode()
        {
            LogMessage(MessageSeverity::Info, "CompositeRenderPipelineNode destructor");
        }
    }
}