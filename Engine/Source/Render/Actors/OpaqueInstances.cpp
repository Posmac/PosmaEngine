#include "OpaqueInstances.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/RenderPassConfig.h"

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
                               RenderPassPtr shadowRenderPass,
                               SResourceExtent2D windowSize)
    {
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
        //vk::CreateDescriptorPool(vk::Device,
        //                         shaderDescriptors,
        //                         maxDescriptorSets, //maximum descriptor sets 
        //                         0, //flags
        //                         &m_DescriptorPool);

        CreateMaterialDescriptors();

        CreateInstanceDescriptorSets();
        CreateInstancePipelineLayout(defaultRenderPass, windowSize);

        CreateShadowDescriptorSets();
        CreateShadowPipeline(shadowRenderPass, windowSize);
    }

    void OpaqueInstances::Deinit()
    {
        /*vk::DestroyPipeline(vk::Device, m_InstancedPipeline);
        vk::DestroyPipelineLayout(vk::Device, m_InstancedPipelineLayout);
        vkDestroyDescriptorSetLayout(vk::Device, m_InstanceDescriptorSetLayout, nullptr);

        vk::DestroyDescriptorPool(vk::Device, m_DescriptorPool);

        vkDestroyDescriptorPool(vk::Device, m_DescriptorPool, nullptr);*/
    }

    void OpaqueInstances::Render(CommandBufferPtr commandBuffer)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        //bind all necessary things
        //(pipeline, descriptor sets, push constants, uniform buffers)

        mInstancedPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);
        //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_InstancedPipeline);

        SVertexBufferBindConfig vertexBufferBind =
        {
            .FirstSlot = 1,
            .BindingCount = 1,
            .Offsets = {0},
            .Buffers = &mInstanceBuffer
        };

        mDeviceInternal->BindVertexBuffers(vertexBufferBind);
        //VkDeviceSize offset = { 0 };
        //vkCmdBindVertexBuffers(commandBuffer,
        //                       1, //first binding 
        //                       1, //binding count
        //                       &m_InstanceBuffer,
        //                       &offset);

        uint32_t firstInstance = 0;

        for(auto& perModel : m_PerModels)
        {
            perModel.Model->BindBuffers(mDeviceInternal, commandBuffer);

            for(int i = 0; i < perModel.PerMaterials.size(); i++)
            {
                uint32_t totalInstances = perModel.PerMaterials[i].Instances.size();

                mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, mInstancedPipeline, {mInstanceDescriptorSet, perModel.PerMaterials[i].MaterialDescriptorSet});

                for(int i = 0; i < perModel.Model->Meshes.size(); i++)
                {
                    MeshRange range = perModel.Model->Meshes[i].Range;
                    mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);
                 
                }
                firstInstance += totalInstances;
            }
        }
    }

    void OpaqueInstances::RenderDepth2D(CommandBufferPtr commandBuffer,
                                        float depthBias,
                                        float depthSlope)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        mShadowsPipeline->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);
        //bind all necessary things
        //(pipeline, descriptor sets, push constants, uniform buffers)
        //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ShadowPipeline);

        mDeviceInternal->SetDepthBias(commandBuffer, depthBias, 1.0f, depthSlope);

        SVertexBufferBindConfig config =
        {
            .FirstSlot = 1, 
            .BindingCount = 1,
            .Offsets = {0},
            .Buffers = &mInstanceBuffer
        };

        mDeviceInternal->BindVertexBuffers(config);
        //VkDeviceSize offset = { 0 };
        //vkCmdBindVertexBuffers(commandBuffer,
        //                       1, //first binding 
        //                       1, //binding count
        //                       &m_InstanceBuffer,
        //                       &offset);

        uint32_t firstInstance = 0;

        for(auto& perModel : m_PerModels)
        {

            perModel.Model->BindBuffers(mDeviceInternal, commandBuffer);

            for(int i = 0; i < perModel.PerMaterials.size(); i++)
            {
                uint32_t totalInstances = perModel.PerMaterials[i].Instances.size();

                mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, mShadowsPipeline, { mShadowDescriptorSet });

                /*vk::BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                           m_ShadowPipelineLayout, { m_ShadowDescriptorSet });*/

                for(int i = 0; i < perModel.Model->Meshes.size(); i++)
                {
                    MeshRange range = perModel.Model->Meshes[i].Range;
                    mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);
                    /*vkCmdDrawIndexed(commandBuffer, range.IndicesCount, totalInstances,
                        range.IndicesOffset, range.VerticesOffset, firstInstance);*/
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
        //CreateInstanceBuffer();
    }

    void OpaqueInstances::CreateInstancePipelineLayout(RenderPassPtr renderPass, SResourceExtent2D extent)
    {
        ShaderPtr vertexShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.vert.txt", EShaderStageFlag::VERTEX_BIT);
        ShaderPtr fragmentShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/triangle.frag.txt", EShaderStageFlag::FRAGMENT_BIT);
        /*VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, , &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);*/

        //pipeline layout
        //constexpr uint32_t pushConstantsSize = 1;
        //VkPushConstantRange pushConstants[pushConstantsSize] =
        //{
        //    {
        //        VK_SHADER_STAGE_FRAGMENT_BIT, // stage flags
        //        0,                            // offset
        //        sizeof(float),                // size
        //    },
        //};

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
        //vk::CreatePipelineLayout(vk::Device,
        //    descriptorSetLayouts,
        //    descriptorSetLayoutsSize,
        //    nullptr, //pPushConstants
        //    0, //push constants size 
        //    &m_InstancedPipelineLayout);

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
                .Location =  0,                              // location
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
                .Stride =  sizeof(Vertex),             // stride
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

        ////move inside vulkan class
        //{
        //    VkPipelineVertexInputStateCreateInfo vertexInputState{};
        //    vk::GetVertexInputInfo(vertexAttribDescr, perVertexAttribsSize,
        //        bindingDescriptions, bindingsSize, &vertexInputState);

        //    //input assembly
        //    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        //    vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        //    VkPipelineMultisampleStateCreateInfo msState{};
        //    vk::GetPipelineMultisampleState(false, false, 1, nullptr, vk::MaxMsaaSamples, &msState);

        //    VkPipelineShaderStageCreateInfo stages[modulesSize];
        //    vk::GetPipelineShaderStages(modules, modulesSize, stages);
        //}

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

        //VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        //vk::GetRasterizationStateInfo(VK_FALSE,
        //                              VK_FALSE,
        //                              VK_POLYGON_MODE_FILL,
        //                              VK_CULL_MODE_BACK_BIT,
        //                              VK_FRONT_FACE_COUNTER_CLOCKWISE,
        //                              VK_FALSE,
        //                              0.0f,
        //                              0.0f,
        //                              0.0f,
        //                              1.0f,
        //                              &rasterizationStateInfo);

        ////create graphics pipeline (a lot of default things)
        //vk::CreateGraphicsPipeline(vk::Device, extent, renderPass,
        //                           m_InstancedPipelineLayout, stages,
        //                           modulesSize, dynamicStates, dynamicStatesCount,
        //                           &msState, &vertexInputState,
        //                           &inputAssemblyInfo, &rasterizationStateInfo,
        //                           &m_InstancedPipeline);
        
        SPipelineConfig pipelineConfig =
        {
            .RenderPass = renderPass,
            .Extent = extent,
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

        mDeviceInternal->CreateRenderPipeline(pipelineConfig);

        /*vk::DestroyShaderModule(vk::Device, vertexShader);
        vk::DestroyShaderModule(vk::Device, fragmentShader);*/
    }

    void OpaqueInstances::CreateShadowDescriptorSets()
    {
        //descriptor set layout 
        std::vector<SDescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            },
            //{
            //    1, //binding
            //    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
            //    1, //count
            //    VK_SHADER_STAGE_VERTEX_BIT //vertex stage
            //}
        };

        SDescriptorSetLayoutConfig layoutConfig =
        {
            .pLayoutsInfo = shaderDescriptorInfo.data(),
            .LayoutsCount = 1,
        };

        mDeviceInternal->CreateDescriptorSetLayout(layoutConfig);
    }

    void OpaqueInstances::CreateShadowPipeline(RenderPassPtr renderPass, SResourceExtent2D size)
    {
        ShaderPtr vertexShader = mDeviceInternal->CreateShaderFromFilename("../Engine/Shaders/shadow2D.vert.txt", EShaderStageFlag::VERTEX_BIT);
        /*VkShaderModule vertexShader;
        vk::CreateShaderModule(vk::Device,
                               "../Engine/Shaders/shadow2D.vert.txt",
                               &vertexShader);*/

        //pipeline layout
        constexpr uint32_t descriptorSetLayoutsSize = 1;
        DescriptorSetLayoutPtr descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            mShadowDescriptorSetLayout
        };

        SPipelineLayoutConfig pipelineLayoutConfig =
        {
            .pLayouts = descriptorSetLayouts,
            .LayoutsSize = descriptorSetLayoutsSize,
            .pPushConstants = nullptr,
            .PushConstantsSize = 0
        };

        mShadowsPipelineLayout = mDeviceInternal->CreatePipelineLayout(pipelineLayoutConfig);

        //vk::CreatePipelineLayout(vk::Device,
        //    descriptorSetLayouts,
        //    descriptorSetLayoutsSize,
        //    nullptr,                    //push constants pointer
        //    0,                          //push constants size
        //    &m_ShadowPipelineLayout);

        //shader stages
        constexpr size_t modulesSize = 1;
        SShaderModuleConfig modules[modulesSize] =
        {
            {
                .Shader = vertexShader,                 // shader module 
                .Type = EShaderStageFlag::VERTEX_BIT,   // VkShaderStageFlag
                .EntryPoint = "main"                        // entry point
            },
        };

     /*   VkPipelineShaderStageCreateInfo stages[modulesSize];
        vk::GetPipelineShaderStages(modules, modulesSize, stages);

        VkPipelineMultisampleStateCreateInfo msState{};
        vk::GetPipelineMultisampleState(false, false, 1, nullptr, VK_SAMPLE_COUNT_1_BIT, &msState);*/

        constexpr size_t attribsSize = 5;
        SVertexInputAttributeDescription vertexAttribDescr[attribsSize] =
        {
            //vertex input (per vertex)
            {
                .Location = 0,                              // location
                .Binding =  0,                              // binding
                .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                .Offset = offsetof(Vertex, Position)      // offset
            },
            //instance attribs input (per instance)
            {
                .Location = 1,                              // location
                .Binding = 1,                              // binding
                .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                .Offset = sizeof(glm::vec4) * 0     // offset
            },
            {
                .Location = 2,                              // location
                .Binding = 1,                              // binding
                .Format = EFormat::R32G32B32A32_SFLOAT,  // format
                .Offset = sizeof(glm::vec4) * 1       // offset
            },
            {
                .Location = 3,                              // location
                .Binding = 1,                              // binding
                .Format = EFormat::R32G32B32A32_SFLOAT,        // format
                .Offset = sizeof(glm::vec4) * 2     // offset
            },
            {   
                .Location = 4,                              // location
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
                .Stride = sizeof(Vertex),             // string
                .InputRate = EVertexInputRate::VERTEX // input rate
            },
            {
                .Binding = 1,                          // binding
                .Stride = sizeof(glm::mat4),             // string
                .InputRate = EVertexInputRate::INSTANCE // input rate
            },
        };

        /*VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vk::GetVertexInputInfo(vertexAttribDescr, attribsSize,
            bindingDescriptions, bindingsSize, &vertexInputState);*/

        SInputAssemblyConfig inputAssembly =
        {
            .Topology = EPrimitiveTopology::TRIANGLE_LIST,
            .RestartPrimitives = false
        };
        ////input assembly
        //VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        //vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        constexpr size_t dynamicStatesCount = 3;
        EDynamicState dynamicStates[dynamicStatesCount] =
        {
            EDynamicState::VIEWPORT,
            EDynamicState::SCISSOR,
            EDynamicState::DEPTH_BIAS,
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
            .DepthBiasClamp = 1.0f,
            .DepthBiasSlopeFactor = 0.0f,
            .LineWidth = 1.0f,
        };

        //VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        //vk::GetRasterizationStateInfo(VK_FALSE,
        //                              VK_FALSE,
        //                              VK_POLYGON_MODE_FILL,
        //                              VK_CULL_MODE_FRONT_BIT,//for depth only rendering it is better
        //                              VK_FRONT_FACE_COUNTER_CLOCKWISE,
        //                              VK_TRUE,
        //                              0.0f,
        //                              1.0f,
        //                              0.0f,
        //                              1.0f,
        //                              &rasterizationStateInfo);

        //vk::CreateGraphicsPipeline(vk::Device, size, renderPass,
        //                           m_ShadowPipelineLayout, stages, modulesSize,
        //                           dynamicStates, dynamicStatesCount,
        //                           &msState, &vertexInputState, &inputAssemblyInfo, &rasterizationStateInfo,
        //                           &m_ShadowPipeline);

        SPipelineConfig pipelineConfig =
        {
            .RenderPass = renderPass,
            .Extent = size,
            .PipelineLayout = mInstancedPipelineLayout,
            .pVertexInputAttributes = vertexAttribDescr,
            .VertexInputAttributeCount = attribsSize,
            .pVertexInputBindings = bindingDescriptions,
            .VertexInputBindingCount = bindingsSize,
            .pShaderModules = modules,
            .ShaderModulesCount = modulesSize,
            .pDynamicStates = dynamicStates,
            .DynamicStatesCount = dynamicStatesCount,
            .InputAssembly = inputAssembly,
            .Rasterization = rasterization,
        };

        mShadowsPipeline = mDeviceInternal->CreateRenderPipeline(pipelineConfig);
    }

    void OpaqueInstances::CreateMaterialDescriptors()
    {
        //descriptor set layout 
        std::vector<SDescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                .Binding =  0, //binding
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig config =
        {
            .pLayoutsInfo = shaderDescriptorInfo.data(),
            .LayoutsCount = shaderDescriptorInfo.size()
        };

        mMaterilaSetLayout = mDeviceInternal->CreateDescriptorSetLayout(config);
        /*vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0,
                                      &m_MaterialSetLayout);*/
    }

    void OpaqueInstances::AllocateAndUpdateDescriptors(DescriptorSetPtr descriptorSet, const Material& material)
    {
        descriptorSet = mDescriptorPool->AllocateDescriptorSets({ mMaterilaSetLayout }, 1);

        /*vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool,
                                           { m_MaterialSetLayout },
                                           1, descriptorSet);*/

        /*SamplerPtr Sampler;
        ImagePtr Image;
        EImageLayout ImageLayout;
        uint32_t DstBinding;
        EDescriptorType  DescriptorType;*/

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
        
        mDeviceInternal->UpdateDescriptorSets(&descriptorSet, 1, texturesUpdateInfo, {});

        //vk::UpdateDescriptorSets(vk::Device, *descriptorSet, {}, imagesInfo,
        //    imagesInfo.size());
    }

    void OpaqueInstances::UpdateShadowDescriptors(BufferPtr dirLightBuffer)
    {
        //update descriptor sets
        std::vector<SUpdateBuffersConfig> buffersInfo =
        {
             {
                .Buffer = dirLightBuffer,
                .Offset = 0,
                .Range = sizeof(glm::mat4),
                .DstBinding = 0,
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
             },

             //{
             //    {
             //        //VkDescriptorBufferInfo
             //        m_InstanceBuffer,              // buffer
             //        0 , // offset
             //        sizeof(glm::mat4),           // range
             //     },

             //     1,                                 // binding
             //     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
             //},
        };

        mDeviceInternal->UpdateDescriptorSets(&mShadowDescriptorSet, 1, {}, buffersInfo);

        //vk::UpdateDescriptorSets(vk::Device, m_ShadowDescriptorSet, buffersInfo, {},
        //    buffersInfo.size() /*+ imagesInfo.size()*/);
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
        /*vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice,
            bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &m_InstanceBuffer, &m_InstanceBufferMemory, &instanceBufferMapping);*/

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
        //vk::UnmapMemory(vk::Device, m_InstanceBufferMemory);
    }

    void OpaqueInstances::UpdateDescriptorSets(ImagePtr shadowMapView, BufferPtr lightsBuffer, BufferPtr matrixBuffer)
    {
        //update descriptor sets
        std::vector<SUpdateBuffersConfig> buffersInfo =
        {
             {
                .Buffer = matrixBuffer,  //vk::PerFrameBuffer
                .Offset = 0,
                .Range = sizeof(glm::mat4) * 3,
                .DstBinding = 0,
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
                 //{
                 //    //VkDescriptorBufferInfo
                 //    vk::PerFrameBuffer,              // buffer
                 //    0 , // offset 
                 //    sizeof(PerFrameData),           // range
                 //},

                 // 0,                                 // binding
                 // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
             },
             //{
             //    {
             //        //VkDescriptorBufferInfo
             //        m_InstanceBuffer,              // buffer
             //        0 , // offset
             //        sizeof(glm::mat4),           // range
             //    },

             //     1,                                 // binding
             //     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
             //},
            {
                 .Buffer = lightsBuffer,  //vk::PerFrameBuffer
                .Offset = 0,
                .Range = sizeof(glm::mat4),
                .DstBinding = 2,
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
                /*{
                    lightsBuffer,
                    0,
                    sizeof(glm::mat4)
                },

                2,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER*/
            },
        };

        std::vector<SUpdateTextureConfig> imagesInfo =
        {
            {
                .Sampler = mSampler,
                .Image = shadowMapView,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
                /*{
                    vk::Sampler,
                    shadowMapView,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                },

                1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER*/
            },
        };

        mDeviceInternal->UpdateDescriptorSets(&mInstanceDescriptorSet, 1, imagesInfo, buffersInfo);
        /*vk::UpdateDescriptorSets(vk::Device, m_InstanceDescriptorSet, buffersInfo, imagesInfo,
            buffersInfo.size() + imagesInfo.size());*/
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
            {
                .Binding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 2, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            },
            /* {
                 2,
                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                 1,
                 VK_SHADER_STAGE_FRAGMENT_BIT
             },*/

             /*{
                 4,
                 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                 1,
                 VK_SHADER_STAGE_FRAGMENT_BIT
             }*/
        };

        SDescriptorSetLayoutConfig descriptorSetLayoutConfig =
        {
            .pLayoutsInfo = shaderDescriptorInfo.data(),
            .LayoutsCount = 1,
        };

        mInstanceDescriptorSetLayout = mDeviceInternal->CreateDescriptorSetLayout(descriptorSetLayoutConfig);

        mDescriptorPool->AllocateDescriptorSets({ mInstanceDescriptorSetLayout }, 1);

        /*vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo },
                                      0, &m_InstanceDescriptorSetLayout);

        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool,
                                   { m_InstanceDescriptorSetLayout },
                                   1, &m_InstanceDescriptorSet);*/
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