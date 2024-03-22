#include "OpaqueInstances.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "../PerFrameData.h"

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
                               RenderPassPtr defaultRenderPass,
                               SResourceExtent2D windowSize)
    {
        mDeviceInternal = device;

        constexpr uint32_t maxUniformBuffers = 50;
        constexpr uint32_t maxCombinedImageSamples = 50;
        constexpr uint32_t maxDescriptorSets = 50;

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

        CreateMaterialDescriptors();

        CreateInstanceDescriptorSets();
        CreateInstancePipelineLayout(defaultRenderPass, windowSize);
    }

    void OpaqueInstances::Deinit()
    {

    }

    void OpaqueInstances::Render(CommandBufferPtr commandBuffer)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        mInstancedPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

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

            for(int i = 0; i < perModel.PerMaterials.size(); i++)
            {
                uint32_t totalInstances = perModel.PerMaterials[i].Instances.size();

                mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, mInstancedPipelineLayout, { mInstanceDescriptorSet, perModel.PerMaterials[i].MaterialDescriptorSet });

                for(int i = 0; i < perModel.Model->Meshes.size(); i++)
                {
                    MeshRange range = perModel.Model->Meshes[i].Range;
                    mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);

                }
                firstInstance += totalInstances;
            }
        }
    }

    void OpaqueInstances::AddInstance(std::shared_ptr<Model> model,
                                      const Material& material,
                                      const Instance& instance)
    {
        if(m_Models.count(model) == 0)
        {
            int index = m_PerModels.size();

            PerMaterial perMat = {};
            perMat.Material = material;
            perMat.Instances.push_back(instance);
            AllocateAndUpdateDescriptors(perMat.MaterialDescriptorSet, perMat.Material);

            m_Models.insert({ model, index });

            PerModel perModel = {};
            perModel.Model = model;
            perModel.PerMaterials = { perMat };
            perModel.MaterialsData.insert({ material, 0 });

            m_PerModels.push_back(perModel);
        }
        else
        {
            int index = m_Models[model];
            PerModel& perModel = m_PerModels[index];

            if(perModel.MaterialsData.count(material) == 0)
            {
                perModel.MaterialsData.insert({ material, perModel.PerMaterials.size() });
                PerMaterial perMat = {};
                perMat.Material = material;
                perMat.Instances.emplace_back(instance);

                perModel.PerMaterials.push_back(perMat);
            }
            else
            {
                int matIndex = perModel.MaterialsData[material];
                perModel.PerMaterials[matIndex].Instances.push_back(instance);
            }
        }
    }

    void OpaqueInstances::CreateInstancePipelineLayout(RenderPassPtr renderPass, SResourceExtent2D extent)
    {
        ShaderPtr vertexShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.vert.txt", EShaderStageFlag::VERTEX_BIT);
        ShaderPtr fragmentShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.frag.txt", EShaderStageFlag::FRAGMENT_BIT);

        constexpr uint32_t descriptorSetLayoutsSize = 2;
        DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            mInstanceDescriptorSetLayout,
            mMaterilaSetLayout
        };

        SPipelineLayoutConfig pipelingLayoutConfig =
        {
            .pLayouts = descriptorSetLayouts,
            .LayoutsSize = descriptorSetLayoutsSize,
            .pPushConstants = nullptr,
            .PushConstantsSize = 0
        };

        mInstancedPipelineLayout = mDeviceInternal->CreatePipelineLayout(pipelingLayoutConfig);

        //shader stages (describe all shader stages used in pipeline)
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
                .Offset = sizeof(glm::vec4) * 0               // offset
            },
            {
                .Location = 4,                              // location
                .Binding = 1,                              // binding
                .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                .Offset = sizeof(glm::vec4) * 1              // offset
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
            .PipelineLayout = mInstancedPipelineLayout,
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

        mInstancedPipeline = mDeviceInternal->CreateRenderPipeline(pipelineConfig);
    }

    void OpaqueInstances::CreateMaterialDescriptors()
    {
        //descriptor set layout 
        std::vector<SDescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig config =
        {
            .pLayoutsInfo = shaderDescriptorInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(shaderDescriptorInfo.size())
        };

        mMaterilaSetLayout = mDeviceInternal->CreateDescriptorSetLayout(config);
    }

    void OpaqueInstances::AllocateAndUpdateDescriptors(DescriptorSetPtr& descriptorSet, const Material& material)
    {
        SDescriptorSetAllocateConfig allocateConfig =
        {
            .DescriptorPool = mDescriptorPool,
            .DescriptorSetLayouts = {mMaterilaSetLayout},
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

    void OpaqueInstances::PrepareInstances()
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        uint32_t totalInstances = 0;

        for(auto& models : m_PerModels)
        {
            for(auto& material : models.PerMaterials)
            {
                totalInstances += material.Instances.size();
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
            for(auto& material : model.PerMaterials)
            {
                for(int i = 0; i < material.Instances.size(); i++)
                {
                    *perInstance = material.Instances[i];
                    perInstance++;
                }
            }
        }

        mInstanceBuffer->Unmap();
    }

    void OpaqueInstances::UpdateDescriptorSets(BufferPtr globalBuffer)
    {
        std::vector<SUpdateBuffersConfig> buffersInfo =
        {
           {
               .Buffer = globalBuffer,
               .Offset = 0,
               .Range = sizeof(GlobalBuffer),
               .DstBinding = 0,
               .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
           },
        };

        mDeviceInternal->UpdateDescriptorSets(mInstanceDescriptorSet, {}, buffersInfo);
    }

    void OpaqueInstances::CreateInstanceDescriptorSets()
    {
        //descriptor set layout 
        std::vector<SDescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT//vertex stage
            },
        };

        SDescriptorSetLayoutConfig descriptorSetLayoutConfig =
        {
            .pLayoutsInfo = shaderDescriptorInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(shaderDescriptorInfo.size()),
        };

        mInstanceDescriptorSetLayout = mDeviceInternal->CreateDescriptorSetLayout(descriptorSetLayoutConfig);

        SDescriptorSetAllocateConfig allocateConfig =
        {
            .DescriptorPool = mDescriptorPool,
            .DescriptorSetLayouts = {mInstanceDescriptorSetLayout},
            .MaxSets = 1
        };

        mInstanceDescriptorSet = mDeviceInternal->AllocateDescriptorSets(allocateConfig);
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