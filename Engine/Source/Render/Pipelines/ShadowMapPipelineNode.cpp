#include "ShadowMapPipelineNode.h"

#include "RHI/Vulkan/CVkDevice.h"

#include "Model/Vertex.h"

namespace psm
{
    namespace graph
    {
        ShadowMapPipelineNode::ShadowMapPipelineNode(const foundation::Name& name,
                                                     const DevicePtr& device,
                                                     const RenderPassPtr& renderPass,
                                                     const ResourceMediatorPtr& resourceMediator,
                                                     const SResourceExtent3D viewportSize) :
            RenderPipelineNode(name)
        {
            constexpr uint32_t descriptorSetLayoutsSize = 2;
            DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
            {
                resourceMediator->GetDescriptorSetLayoutByName(DEPTH_PASS_SET),
                resourceMediator->GetDescriptorSetLayoutByName(MODEL_DATA_SET_LAYOUT),
            };

            SPipelineLayoutConfig pipelineLayoutConfig =
            {
                .pLayouts = descriptorSetLayouts,
                .LayoutsSize = static_cast<uint32_t>(descriptorSetLayoutsSize),
                .pPushConstants = nullptr,
                .PushConstantsSize = 0,
            };

            mPipelineLayout = device->CreatePipelineLayout(pipelineLayoutConfig);

            ShaderPtr vertexShader = device->CreateShaderFromFilename("../Engine/Shaders/shadow2D.vert.txt", EShaderStageFlag::VERTEX_BIT);

            constexpr size_t modulesSize = 1;
            SShaderModuleConfig modules[modulesSize] =
            {
                {
                    .Shader = vertexShader,                 // shader module 
                    .Type = EShaderStageFlag::VERTEX_BIT,   // VkShaderStageFlag
                    .EntryPoint = "main"                        // entry point
                },
            };

            constexpr int vertexInputAttributesSize = 7;
            SVertexInputAttributeDescription vertexInputAttributes[vertexInputAttributesSize] =
            {
                //vertex attribs input (per vertex input data)
                {
                    .Location = 0,                              // location
                    .Binding = 0,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                    .Offset = offsetof(Vertex, Position)      // offset
                },
                {
                    .Location = 1,                              // location
                    .Binding = 0,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                    .Offset = offsetof(Vertex, Normal)        // offset
                },
                {
                    .Location = 2,                              // location
                    .Binding = 0,                              // binding
                    .Format = EFormat::R32G32_SFLOAT,        // format
                    .Offset = offsetof(Vertex, TexCoord)      // offset
                },

                //instance attribs input (per instance input data)
                {
                    .Location = 3,                              // location
                    .Binding = 1,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                    .Offset = 0               // offset
                },
                {
                    .Location = 4,                              // location
                    .Binding = 1,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                    .Offset = sizeof(glm::vec4)              // offset
                },
                {
                    .Location = 5,                              // location
                    .Binding = 1,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,        // format
                    .Offset = sizeof(glm::vec4) * 2     // offset
                },
                {
                    .Location = 6,                              // location
                    .Binding = 1,                              // binding
                    .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                    .Offset = sizeof(glm::vec4) * 3     // offset
                },
            };

            constexpr size_t bindingsSize = 2;
            SVertexInputBindingDescription bindingDescriptions[bindingsSize] =
            {
                {
                    .Binding = 0,                          // binding
                    .Stride = sizeof(Vertex),             // stride
                    .InputRate = EVertexInputRate::VERTEX // input rate
                },
                {
                    .Binding = 1,                          // binding
                    .Stride = sizeof(glm::mat4),             // stride
                    .InputRate = EVertexInputRate::INSTANCE // input rate
                },
            };

            SInputAssemblyConfig inputAssembly =
            {
                .Topology = EPrimitiveTopology::TRIANGLE_LIST,
                .RestartPrimitives = false
            };

            constexpr size_t dynamicStatesCount = 3;
            EDynamicState dynamicStates[dynamicStatesCount] =
            {
                EDynamicState::SCISSOR,
                EDynamicState::VIEWPORT,
                EDynamicState::DEPTH_BIAS
            };

            SRasterizationConfig rasterization =
            {
                .DepthClampEnable = false,
                .RasterizerDiscardEnable = false,
                .CullMode = ECullMode::FRONT_BIT,
                .PolygonMode = EPolygonMode::FILL,
                .FrontFace = EFrontFace::COUNTER_CLOCKWISE,
                .DepthBiasEnable = true,
                .DepthBiasConstantFactor = 0.0f,
                .DepthBiasClamp = 0.0f,
                .DepthBiasSlopeFactor = 0.0f,
                .LineWidth = 1.0f,
            };

            SPipelineConfig pipelineConfig =
            {
                .RenderPass = renderPass,
                .ViewPortExtent = {viewportSize.width, viewportSize.height},
                .PipelineLayout = mPipelineLayout,
                .pVertexInputAttributes = vertexInputAttributes,
                .VertexInputAttributeCount = vertexInputAttributesSize,
                .pVertexInputBindings = bindingDescriptions,
                .VertexInputBindingCount = bindingsSize,
                .pShaderModules = modules,
                .ShaderModulesCount = modulesSize,
                .pDynamicStates = dynamicStates,
                .DynamicStatesCount = dynamicStatesCount,
                .InputAssembly = inputAssembly,
                .Rasterization = rasterization,
                .DepthStencilCompareOp = ECompareOp::COMPARE_OP_GREATER,
            };

            mPipeline = device->CreateRenderPipeline(pipelineConfig);
        }

        ShadowMapPipelineNode::~ShadowMapPipelineNode()
        {
            LogMessage(MessageSeverity::Info, "ShadowMapPipelineNode destructor");
        }
    }
}

