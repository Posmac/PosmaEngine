#include "OpaqueInstances.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "../GlobalBuffer.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkPipeline.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

namespace psm
{
    OpaqueInstances* OpaqueInstances::s_Instance = nullptr;

    OpaqueInstances* OpaqueInstances::GetInstance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new OpaqueInstances();
        }

        return s_Instance;
    }

    void OpaqueInstances::Init(DevicePtr device,
                               const RenderPassPtr& defaultRenderPass,
                               const RenderPassPtr& shadowRenderPass,
                               SResourceExtent2D windowSize,
                               SResourceExtent2D shadowMapSize)
    {
        mDeviceInternal = device;

        constexpr uint32_t maxUniformBuffers = 500;
        constexpr uint32_t maxCombinedImageSamples = 500;
        constexpr uint32_t maxDescriptorSets = 500;

        //create descriptor pool for everything
        std::vector<SDescriptorPoolSize> shaderDescriptors =
        {
            {
                EDescriptorType::UNIFORM_BUFFER, //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                maxUniformBuffers
            },
            {
                EDescriptorType::COMBINED_IMAGE_SAMPLER, //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                maxCombinedImageSamples
            }
        };

        SDescriptorPoolConfig descriptorPoolConfig =
        {
            .DesciptorPoolSizes = shaderDescriptors,
            .MaxDesciptorPools = maxDescriptorSets
        };

        mDescriptorPool = mDeviceInternal->CreateDescriptorPool(descriptorPoolConfig);

        SSamplerConfig samplerConfig =
        {
            .MagFilter = EFilterMode::FILTER_LINEAR,
            .MinFilter = EFilterMode::FILTER_LINEAR,
            .UAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .VAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .WAddress = ESamplerAddressMode::SAMPLER_MODE_MIRRORED_REPEAT,
            .BorderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .EnableComparision = false,
            .CompareOp = ECompareOp::COMPARE_OP_ALWAYS,
            .SamplerMode = ESamplerMipmapMode::SAMPLER_MIPMAP_MODE_LINEAR,
            .EnableAniso = false,
            .MaxAniso = 0.0f,
            .MaxLod = 0.0,
            .MinLod = 0.0,
            .MipLodBias = 0.0,
            .UnnormalizedCoords = false,
        };

        mSampler = mDeviceInternal->CreateSampler(samplerConfig);

        SetupDescriptors();

        CreateDefaultPipeline(defaultRenderPass, windowSize);
        CreateDepthPipeline(shadowRenderPass, shadowMapSize);
    }

    void OpaqueInstances::Deinit()
    {
        for(auto& model : m_Models)
            model.first->Deinit();
        m_Models.clear();

        m_PerModels.clear();

        mInstanceBuffer = nullptr;

        mGlobalBufferSetLayout = nullptr;
        mGlobalBufferSet = nullptr;
        mModelDataSetLayout = nullptr;
        mInstanceToWorldConstantBuffer = nullptr;
        mMaterialSetLayout = nullptr;
        mMaterialSet = nullptr;
        mDepthPassSetLayout = nullptr;
        mDepthPassSet = nullptr;
        mDefaultPassDepthDataSetLayout = nullptr;
        mDefaultPassDepthDataSet = nullptr;
        mDepthPassPipeline = nullptr;
        mDepthPassPipelineLayout = nullptr;
        mDefaultPassPipeline = nullptr;
        mDefaultPassPipelineLayout = nullptr;
        mSampler = nullptr;

        mDescriptorPool = nullptr;
    }

    void OpaqueInstances::Render(const CommandBufferPtr& commandBuffer)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        mDefaultPassPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

        SVertexBufferBindConfig vertexBufferBind =
        {
            .FirstSlot = 1,
            .BindingCount = 1,
            .Offsets = {0},
            .Buffers = &mInstanceBuffer
        };

        mDeviceInternal->BindVertexBuffers(commandBuffer, vertexBufferBind);

        uint32_t firstInstance = 0;

        for(auto& perModel : m_PerModels)
        {
            perModel.Model->BindBuffers(mDeviceInternal, commandBuffer);

            for(int i = 0; i < perModel.Meshes.size(); i++)
            {
                PerMesh& perMesh = perModel.Meshes[i];
                Mesh& modelMesh = perModel.Model->Meshes[i];

                for(auto& material : perMesh.PerMaterials)
                {
                    uint32_t totalInstances = material.Instances.size();

                    mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, mDefaultPassPipelineLayout,
                                                        { mGlobalBufferSet, perMesh.MeshToModelData, material.MaterialDescriptorSet, mDefaultPassDepthDataSet });

                    MeshRange range = perModel.Model->Meshes[i].Range;
                    mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);
                    firstInstance += totalInstances;
                }
            }
        }
    }

    void OpaqueInstances::RenderDepth(const CommandBufferPtr& commandBuffer)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        mDepthPassPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

        SVertexBufferBindConfig vertexBufferBind =
        {
            .FirstSlot = 1,
            .BindingCount = 1,
            .Offsets = {0},
            .Buffers = &mInstanceBuffer
        };

        mDeviceInternal->BindVertexBuffers(commandBuffer, vertexBufferBind);

        uint32_t firstInstance = 0;

        for(auto& perModel : m_PerModels)
        {
            perModel.Model->BindBuffers(mDeviceInternal, commandBuffer);

            for(int i = 0; i < perModel.Meshes.size(); i++)
            {
                PerMesh& perMesh = perModel.Meshes[i];
                Mesh& modelMesh = perModel.Model->Meshes[i];

                for(auto& material : perMesh.PerMaterials)
                {
                    uint32_t totalInstances = material.Instances.size();

                    mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, mDepthPassPipelineLayout,
                                                        { mDepthPassSet, perMesh.MeshToModelData });

                    MeshRange range = perModel.Model->Meshes[i].Range;
                        mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);
                    firstInstance += totalInstances;
                }
            }
        }
    }

    void OpaqueInstances::AddInstance(std::shared_ptr<Model> model,
                                      const OpaqModelMeshMaterials& materials,
                                      const OpaqModelMeshInstances& instances)
    {
        for(int j = 0; j < instances.size(); j++)
        {
            if(m_Models.count(model) == 0)
            {
                int index = m_PerModels.size();
                m_Models.insert({ model, index });

                PerModel perModel =
                {
                    .Model = model,
                };
                m_PerModels.push_back(perModel);

                for(int i = 0; i < model->Meshes.size(); i++)
                {
                    PerMaterial perMat =
                    {
                        .Material = materials[i],
                        .Instances = {instances[j]},
                    };
                    SetupMaterialDescriptor(perMat.MaterialDescriptorSet, perMat.Material);

                    PerMesh perMesh =
                    {
                        .PerMaterials = {perMat},
                        .MaterialsData = {{{materials[i], 0}}},
                    };

                    SDescriptorSetAllocateConfig modelAlloc =
                    {
                         .DescriptorPool = mDescriptorPool,
                         .DescriptorSetLayouts = {mModelDataSetLayout},
                         .MaxSets = 1,
                    };

                    perMesh.MeshToModelData = mDeviceInternal->AllocateDescriptorSets(modelAlloc);

                    m_PerModels[index].Meshes.push_back(perMesh);
                }
            }
            else
            {
                int index = m_Models[model];
                PerModel& perModel = m_PerModels[index];

                for(int i = 0; i < perModel.Meshes.size(); i++)
                {
                    auto& mesh = perModel.Meshes[i];
                    if(mesh.MaterialsData.count(materials[i]) == 0)
                    {
                        mesh.MaterialsData.insert({ materials[i], mesh.PerMaterials.size() });
                        PerMaterial perMat = {};
                        perMat.Material = materials[i];
                        perMat.Instances.emplace_back(instances[j]);

                        mesh.PerMaterials.push_back(perMat);
                    }
                    else
                    {
                        int matIndex = mesh.MaterialsData[materials[i]];
                        mesh.PerMaterials[matIndex].Instances.push_back(instances[j]);
                    }
                }
            }
        }
    }

    void OpaqueInstances::CreateDefaultPipeline(const RenderPassPtr& renderPass, SResourceExtent2D extent)
    {
        constexpr uint32_t descriptorSetLayoutsSize = 4;
        DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            mGlobalBufferSetLayout,
            mModelDataSetLayout,
            mMaterialSetLayout,
            mDefaultPassDepthDataSetLayout
        };

        SPipelineLayoutConfig pipelingLayoutConfig =
        {
            .pLayouts = descriptorSetLayouts,
            .LayoutsSize = descriptorSetLayoutsSize,
            .pPushConstants = nullptr,
            .PushConstantsSize = 0
        };

        mDefaultPassPipelineLayout = mDeviceInternal->CreatePipelineLayout(pipelingLayoutConfig);

        //shader stages (describe all shader stages used in pipeline)

        ShaderPtr vertexShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.vert.txt", EShaderStageFlag::VERTEX_BIT);
        ShaderPtr fragmentShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.frag.txt", EShaderStageFlag::FRAGMENT_BIT);

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

        constexpr size_t perVertexAttribsSize = 7;
        SVertexInputAttributeDescription vertexAttribDescr[perVertexAttribsSize] =
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

        SPipelineConfig pipelineConfig =
        {
            .RenderPass = renderPass,
            .ViewPortExtent = extent,
            .PipelineLayout = mDefaultPassPipelineLayout,
            .pVertexInputAttributes = vertexAttribDescr,
            .VertexInputAttributeCount = perVertexAttribsSize,
            .pVertexInputBindings = bindingDescriptions,
            .VertexInputBindingCount = bindingsSize,
            .pShaderModules = modules,
            .ShaderModulesCount = modulesSize,
            .pDynamicStates = dynamicStates,
            .DynamicStatesCount = dynamicStatesCount,
            .InputAssembly = inputAssembly,
            .Rasterization = rasterization,
        };

        mDefaultPassPipeline = mDeviceInternal->CreateRenderPipeline(pipelineConfig);
    }

    void OpaqueInstances::SetupDescriptors()
    {
        //global buffer only (set 0)
        std::vector<SDescriptorLayoutInfo> globalInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig globalConfig =
        {
            .pLayoutsInfo = globalInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(globalInfo.size())
        };

        mGlobalBufferSetLayout = mDeviceInternal->CreateDescriptorSetLayout(globalConfig);

        SDescriptorSetAllocateConfig globalAlloc =
        {
             .DescriptorPool = mDescriptorPool,
             .DescriptorSetLayouts = {mGlobalBufferSetLayout},
             .MaxSets = 1,
        };

        mGlobalBufferSet = mDeviceInternal->AllocateDescriptorSets(globalAlloc);

        // model instance matrix (set 1)
        std::vector<SDescriptorLayoutInfo> modelInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig modelConfig =
        {
            .pLayoutsInfo = modelInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(modelInfo.size())
        };

        mModelDataSetLayout = mDeviceInternal->CreateDescriptorSetLayout(modelConfig);

        // material albedo (set 2)
        std::vector<SDescriptorLayoutInfo> materialInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig materialConfig =
        {
            .pLayoutsInfo = materialInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(materialInfo.size())
        };

        mMaterialSetLayout = mDeviceInternal->CreateDescriptorSetLayout(materialConfig);

        //depth pass light projection matrices buffer(set 3)
        std::vector<SDescriptorLayoutInfo> depthInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig depthConfig =
        {
            .pLayoutsInfo = depthInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(depthInfo.size())
        };

        mDepthPassSetLayout = mDeviceInternal->CreateDescriptorSetLayout(depthConfig);

        SDescriptorSetAllocateConfig depthAlloc =
        {
             .DescriptorPool = mDescriptorPool,
             .DescriptorSetLayouts = {mDepthPassSetLayout},
             .MaxSets = 1,
        };

        mDepthPassSet = mDeviceInternal->AllocateDescriptorSets(depthAlloc);

        // depth pass light projection matrices buffer(b0) and shadow map(b1) (set 4)
        std::vector<SDescriptorLayoutInfo> depthInputInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            },
            {
                .Binding = 1, //binding
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig depthInputConfig =
        {
            .pLayoutsInfo = depthInputInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(depthInputInfo.size())
        };

        mDefaultPassDepthDataSetLayout = mDeviceInternal->CreateDescriptorSetLayout(depthInputConfig);

        SDescriptorSetAllocateConfig depthDataAlloc =
        {
             .DescriptorPool = mDescriptorPool,
             .DescriptorSetLayouts = {mDefaultPassDepthDataSetLayout},
             .MaxSets = 1,
        };

        mDefaultPassDepthDataSet = mDeviceInternal->AllocateDescriptorSets(depthDataAlloc);
    }

    void OpaqueInstances::SetupMaterialDescriptor(DescriptorSetPtr& descriptorSet, const Material& material)
    {
        SDescriptorSetAllocateConfig allocateConfig =
        {
            .DescriptorPool = mDescriptorPool,
            .DescriptorSetLayouts = {mMaterialSetLayout},
            .MaxSets = 1
        };

        descriptorSet = mDeviceInternal->AllocateDescriptorSets(allocateConfig);

        std::vector<SUpdateTextureConfig> texturesUpdateInfo =
        {
            {
                .Sampler = mSampler,
                .Image = material.Albedo,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 0,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            }
        };

        mDeviceInternal->UpdateDescriptorSets(descriptorSet, texturesUpdateInfo, {});
    }

    void OpaqueInstances::CreateDepthPipeline(const RenderPassPtr& renderPass, SResourceExtent2D viewportSize)
    {
        constexpr uint32_t descriptorSetLayoutsSize = 2;
        DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            mDepthPassSetLayout,
            mModelDataSetLayout
        };

        SPipelineLayoutConfig pipelineLayoutConfig =
        {
            .pLayouts = descriptorSetLayouts,
            .LayoutsSize = descriptorSetLayoutsSize,
            .pPushConstants = nullptr,
            .PushConstantsSize = 0,
        };

        mDepthPassPipelineLayout = mDeviceInternal->CreatePipelineLayout(pipelineLayoutConfig);

        ShaderPtr vertexShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/shadow2D.vert.txt", EShaderStageFlag::VERTEX_BIT);

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
            .ViewPortExtent = viewportSize,
            .PipelineLayout = mDepthPassPipelineLayout,
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
        };

        mDepthPassPipeline = mDeviceInternal->CreateRenderPipeline(pipelineConfig);
    }

    void OpaqueInstances::UpdateInstanceBuffer()
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        uint32_t totalInstances = 0;

        for(auto& models : m_PerModels)
        {
            for(auto& mesh : models.Meshes)
            {
                for(auto& material : mesh.PerMaterials)
                {
                    totalInstances += material.Instances.size();
                }
            }
        }

        if(mInstanceBuffer != nullptr)
        {
            mInstanceBuffer = nullptr;
        }

        VkDeviceSize bufferSize = sizeof(Instance) * totalInstances;

        void* instanceBufferMapping = nullptr;

        SBufferConfig bufferConfig =
        {
            .Size = bufferSize,
            .Usage = EBufferUsage::USAGE_VERTEX_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mInstanceBuffer = mDeviceInternal->CreateBuffer(bufferConfig);

        SBufferMapConfig mapConfig =
        {
            .Size = bufferSize,
            .Offset = 0,
            .pData = &instanceBufferMapping,
            .Flags = 0 //0 everythime for now
        };

        mInstanceBuffer->Map(mapConfig);

        Instance* perInstance = reinterpret_cast<Instance*>(instanceBufferMapping);

        for(auto& model : m_PerModels)
        {
            for(auto& mesh : model.Meshes)
            {
                for(auto& material : mesh.PerMaterials)
                {
                    for(int i = 0; i < material.Instances.size(); i++)
                    {
                        *perInstance = material.Instances[i];
                        perInstance++;
                    }
                }

            }
        }

        mInstanceBuffer->Unmap();
    }

    //can be merged with UpdateInstanceBuffer
    void OpaqueInstances::UpdateMeshToModelData()
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        uint32_t totalMatrices = 0;

        for(auto& models : m_PerModels)
        {
            for(auto& mesh : models.Meshes)
            {
                totalMatrices++;
            }
        }

        if(mInstanceToWorldConstantBuffer != nullptr)
        {
            mInstanceToWorldConstantBuffer = nullptr;
        }

        static const uint64_t range = sizeof(glm::mat4);

        VkDeviceSize bufferSize = range * totalMatrices;

        void* instanceBufferMapping = nullptr;

        SBufferConfig bufferConfig =
        {
            .Size = bufferSize,
            .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mInstanceToWorldConstantBuffer = mDeviceInternal->CreateBuffer(bufferConfig);

        SBufferMapConfig mapConfig =
        {
            .Size = bufferSize,
            .Offset = 0,
            .pData = &instanceBufferMapping,
            .Flags = 0 //0 everythime for now
        };

        mInstanceToWorldConstantBuffer->Map(mapConfig);

        glm::mat4* perInstance = reinterpret_cast<glm::mat4*>(instanceBufferMapping);
        uint64_t offset = 0;
        static const uint64_t step = sizeof(glm::mat4);

        for(auto& model : m_PerModels)
        {
            for(int i = 0; i < model.Meshes.size(); i++)
            {
                PerMesh& perMesh = model.Meshes[i];
                Mesh& modelMesh = model.Model->Meshes[i];

                *perInstance = modelMesh.LocalMatrix;
                perInstance++;

                std::vector<SUpdateBuffersConfig> buffersInfo =
                {
                   {
                       .Buffer = mInstanceToWorldConstantBuffer,
                       .Offset = offset,
                       .Range = range,
                       .DstBinding = 0,
                       .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
                   },
                };

                mDeviceInternal->UpdateDescriptorSets(perMesh.MeshToModelData, {}, buffersInfo);

                offset += step;
            }
        }

        mInstanceToWorldConstantBuffer->Unmap();
    }

    void OpaqueInstances::UpdateDefaultDescriptors(const BufferPtr& globalBuffer, const BufferPtr& shadowMapBuffer, const ImagePtr& dirDepthShadowMap)
    {
        std::vector<SUpdateBuffersConfig> buffersInfo =
        {
           {
               .Buffer = globalBuffer,
               .Offset = 0,
               .Range = globalBuffer->Size(),
               .DstBinding = 0,
               .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
           },
        };

        mDeviceInternal->UpdateDescriptorSets(mGlobalBufferSet, {}, buffersInfo);

        std::vector<SUpdateBuffersConfig> shadowBuffersInfo =
        {
            {
               .Buffer = shadowMapBuffer,
               .Offset = 0,
               .Range = shadowMapBuffer->Size(),
               .DstBinding = 0,
               .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
           },
        };

        std::vector<SUpdateTextureConfig> textureUpdateInfo =
        {
            {
                .Sampler = mSampler,
                .Image = dirDepthShadowMap,
                .ImageLayout = EImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .DstBinding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            }
        };

        mDeviceInternal->UpdateDescriptorSets(mDefaultPassDepthDataSet, textureUpdateInfo, shadowBuffersInfo);
    }

    void OpaqueInstances::UpdateDepthDescriptors(const BufferPtr& lightSources)
    {
        SUpdateBuffersConfig buffers =
        {
            .Buffer = lightSources,
            .Offset = 0,
            .Range = lightSources->Size(),
            .DstBinding = 0,
            .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
        };

        mDeviceInternal->UpdateDescriptorSets(mDepthPassSet, {}, { buffers });
    }

    bool OpaqueInstances::Material::operator==(const Material& lhs)
    {
        return this->Albedo.get() == lhs.Albedo.get();
    }

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs)
    {
        return lhs.Albedo.get() == rhs.Albedo.get();
    }
}