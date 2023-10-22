#include "OpaqueInstances.h"

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

    void OpaqueInstances::Init(VkRenderPass defaultRenderPass,
                               VkRenderPass shadowRenderPass,
                               VkExtent2D windowSize)
    {
        constexpr uint32_t maxUniformBuffers = 50;
        constexpr uint32_t maxCombinedImageSamples = 50;
        constexpr uint32_t maxDescriptorSets = 50;

        //create descriptor pool for everything
        std::vector<vk::DescriptorPoolSize> shaderDescriptors =
        {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                maxUniformBuffers
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                maxCombinedImageSamples
            }
        };

        vk::CreateDescriptorPool(vk::Device,
                                 shaderDescriptors,
                                 maxDescriptorSets, //maximum descriptor sets 
                                 0, //flags
                                 &m_DescriptorPool);

        CreateMaterialDescriptors();

        CreateInstanceDescriptorSets();
        CreateInstancePipelineLayout(defaultRenderPass, windowSize);

        CreateShadowDescriptorSets();
        CreateShadowPipeline(windowSize, shadowRenderPass);
    }

    void OpaqueInstances::Deinit()
    {
        vk::DestroyPipeline(vk::Device, m_InstancedPipeline);
        vk::DestroyPipelineLayout(vk::Device, m_InstancedPipelineLayout);
        vkDestroyDescriptorSetLayout(vk::Device, m_InstanceDescriptorSetLayout, nullptr);

        vk::DestroyDescriptorPool(vk::Device, m_DescriptorPool);

        vkDestroyDescriptorPool(vk::Device, m_DescriptorPool, nullptr);
    }

    void OpaqueInstances::Render(VkCommandBuffer commandBuffer)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        //bind all necessary things
        //(pipeline, descriptor sets, push constants, uniform buffers)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_InstancedPipeline);
        VkDeviceSize offset = { 0 };
        vkCmdBindVertexBuffers(commandBuffer,
                               1, //first binding 
                               1, //binding count
                               &m_InstanceBuffer,
                               &offset);

        for(auto& perModel : m_PerModels)
        {
            perModel.Model->BindBuffers(commandBuffer);

            uint32_t firstInstance = 0;

            for(int i = 0; i < perModel.PerMaterials.size(); i++)
            {
                uint32_t totalInstances = perModel.PerMaterials[i].Instances.size();

                vk::BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                           m_InstancedPipelineLayout, { m_InstanceDescriptorSet,
                                           perModel.PerMaterials[i].MaterialDescriptorSet });

                for(int i = 0; i < perModel.Model->Meshes.size(); i++)
                {
                    MeshRange range = perModel.Model->Meshes[i].Range;
                    vkCmdDrawIndexed(commandBuffer, range.IndicesCount, totalInstances,
                        range.IndicesOffset, range.VerticesOffset, firstInstance);
                }
            }
        }
    }

    void OpaqueInstances::RenderDepth2D(VkCommandBuffer commandBuffer,
                                        float depthBias,
                                        float depthSlope)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        //bind all necessary things
        //(pipeline, descriptor sets, push constants, uniform buffers)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ShadowPipeline);

        vkCmdSetDepthBias(commandBuffer,
                     depthBias,
                     1.0f,
                     depthSlope);

        VkDeviceSize offset = { 0 };
        vkCmdBindVertexBuffers(commandBuffer,
                               1, //first binding 
                               1, //binding count
                               &m_InstanceBuffer,
                               &offset);

        for(auto& perModel : m_PerModels)
        {
            perModel.Model->BindBuffers(commandBuffer);

            uint32_t firstInstance = 0;

            for(int i = 0; i < perModel.PerMaterials.size(); i++)
            {
                uint32_t totalInstances = perModel.PerMaterials[i].Instances.size();

                vk::BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                           m_ShadowPipelineLayout, { m_ShadowDescriptorSet });

                for(int i = 0; i < perModel.Model->Meshes.size(); i++)
                {
                    MeshRange range = perModel.Model->Meshes[i].Range;
                    vkCmdDrawIndexed(commandBuffer, range.IndicesCount, totalInstances,
                        range.IndicesOffset, range.VerticesOffset, firstInstance);
                }
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
            AllocateAndUpdateDescriptors(&perMat.MaterialDescriptorSet, perMat.Material);

            m_Models.insert({ model, index });

            PerModel perModel = {};
            perModel.Model = model;
            perModel.PerMaterials = { perMat };
            perModel.MaterialsData.insert({ material, 0 });

            m_PerModels.emplace_back(perModel);

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
        CreateInstanceBuffer();
    }

    void OpaqueInstances::CreateInstancePipelineLayout(VkRenderPass renderPass,
        VkExtent2D extent)
    {
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

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
        VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            m_InstanceDescriptorSetLayout,
            m_MaterialSetLayout
        };

        vk::CreatePipelineLayout(vk::Device,
            descriptorSetLayouts,
            descriptorSetLayoutsSize,
            nullptr, //pPushConstants
            0, //push constants size 
            &m_InstancedPipelineLayout);

        //shader stages (describe all shader stages used in pipeline)
        constexpr size_t modulesSize = 2;
        vk::ShaderModuleInfo modules[modulesSize] =
        {
            {
                vertexShader,                 // shader module 
                VK_SHADER_STAGE_VERTEX_BIT,   // VkShaderStageFlag
                "main"                        // entry point
            },
            {
                fragmentShader,               // shader module 
                VK_SHADER_STAGE_FRAGMENT_BIT, // VkShaderStageFlag
                "main"                        // entry point
            },
        };

        VkPipelineShaderStageCreateInfo stages[modulesSize];
        vk::GetPipelineShaderStages(modules, modulesSize, stages);

        constexpr size_t perVertexAttribsSize = 7;
        VkVertexInputAttributeDescription vertexAttribDescr[perVertexAttribsSize] =
        {
            //vertex attribs input (per vertex input data)
            {
                0,                              // location
                0,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                offsetof(Vertex, Position)      // offset
            },
            {
                1,                              // location
                0,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                offsetof(Vertex, Normal)        // offset
            },
            {
                2,                              // location
                0,                              // binding
                VK_FORMAT_R32G32_SFLOAT,        // format
                offsetof(Vertex, TexCoord)      // offset
            },

            //instance attribs input (per instance input data)
            {
                3,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 0               // offset
            },
            {
                4,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 1              // offset
            },
            {
                5,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,        // format
                sizeof(glm::vec4) * 2     // offset
            },
            {
                6,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 3     // offset
            },
        };

        constexpr size_t bindingsSize = 2;
        VkVertexInputBindingDescription bindingDescriptions[bindingsSize] =
        {
            {
                0,                          // binding
                sizeof(Vertex),             // stride
                VK_VERTEX_INPUT_RATE_VERTEX // input rate
            },
            {
                1,                          // binding
                sizeof(glm::mat4),             // stride
                VK_VERTEX_INPUT_RATE_INSTANCE // input rate
            },
        };



        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vk::GetVertexInputInfo(vertexAttribDescr, perVertexAttribsSize,
            bindingDescriptions, bindingsSize, &vertexInputState);

        //input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        VkPipelineMultisampleStateCreateInfo msState{};
        vk::GetPipelineMultisampleState(false, false, 1, nullptr, vk::MaxMsaaSamples, &msState);

        constexpr size_t dynamicStatesCount = 2;
        VkDynamicState dynamicStates[dynamicStatesCount] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        vk::GetRasterizationStateInfo(VK_FALSE,
                                      VK_FALSE,
                                      VK_POLYGON_MODE_FILL,
                                      VK_CULL_MODE_BACK_BIT,
                                      VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                      VK_FALSE,
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      1.0f,
                                      &rasterizationStateInfo);

        //create graphics pipeline (a lot of default things)
        vk::CreateGraphicsPipeline(vk::Device, extent, renderPass,
                                   m_InstancedPipelineLayout, stages,
                                   modulesSize, dynamicStates, dynamicStatesCount,
                                   &msState, &vertexInputState,
                                   &inputAssemblyInfo, &rasterizationStateInfo,
                                   &m_InstancedPipeline);

        vk::DestroyShaderModule(vk::Device, vertexShader);
        vk::DestroyShaderModule(vk::Device, fragmentShader);
    }

    void OpaqueInstances::CreateShadowDescriptorSets()
    {
        //descriptor set layout 
        std::vector<vk::DescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                0, //binding
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
                1, //count
                VK_SHADER_STAGE_VERTEX_BIT //vertex stage
            },
            //{
            //    1, //binding
            //    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
            //    1, //count
            //    VK_SHADER_STAGE_VERTEX_BIT //vertex stage
            //}
        };

        vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0,
                                      &m_ShadowDescriptorSetLayout);

        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool, { m_ShadowDescriptorSetLayout },
            1, &m_ShadowDescriptorSet);
    }

    void OpaqueInstances::CreateShadowPipeline(VkExtent2D size,
                                               VkRenderPass renderPass)
    {
        VkShaderModule vertexShader;
        vk::CreateShaderModule(vk::Device,
                               "../Engine/Shaders/shadow2D.vert.txt",
                               &vertexShader);

        //pipeline layout
        constexpr uint32_t descriptorSetLayoutsSize = 1;
        VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            m_ShadowDescriptorSetLayout
        };

        vk::CreatePipelineLayout(vk::Device,
            descriptorSetLayouts,
            descriptorSetLayoutsSize,
            nullptr,                    //push constants pointer
            0,                          //push constants size
            &m_ShadowPipelineLayout);

        //shader stages
        constexpr size_t modulesSize = 1;
        vk::ShaderModuleInfo modules[modulesSize] =
        {
            {
                vertexShader,                 // shader module 
                VK_SHADER_STAGE_VERTEX_BIT,   // VkShaderStageFlag
                "main"                        // entry point
            },
        };

        VkPipelineShaderStageCreateInfo stages[modulesSize];
        vk::GetPipelineShaderStages(modules, modulesSize, stages);

        VkPipelineMultisampleStateCreateInfo msState{};
        vk::GetPipelineMultisampleState(false, false, 1, nullptr, VK_SAMPLE_COUNT_1_BIT, &msState);

        constexpr size_t attribsSize = 5;
        VkVertexInputAttributeDescription vertexAttribDescr[attribsSize] =
        {
            //vertex input (per vertex)
            {
                0,                              // location
                0,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                offsetof(Vertex, Position)      // offset
            },
            //instance attribs input (per instance)
            {
                1,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 0     // offset
            },
            {
                2,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 1       // offset
            },
            {
                3,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,        // format
                sizeof(glm::vec4) * 2     // offset
            },
            {   
                4,                              // location
                1,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                sizeof(glm::vec4) * 3     // offset
            },
        };

        constexpr size_t bindingsSize = 2;
        VkVertexInputBindingDescription bindingDescriptions[bindingsSize] =
        {
            {
                0,                          // binding
                sizeof(Vertex),             // string
                VK_VERTEX_INPUT_RATE_VERTEX // input rate
            },
            {
                1,                          // binding
                sizeof(glm::mat4),             // string
                VK_VERTEX_INPUT_RATE_INSTANCE // input rate
            },
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vk::GetVertexInputInfo(vertexAttribDescr, attribsSize,
            bindingDescriptions, bindingsSize, &vertexInputState);

        //input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        constexpr size_t dynamicStatesCount = 3;
        VkDynamicState dynamicStates[dynamicStatesCount] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_DEPTH_BIAS,
        };

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        vk::GetRasterizationStateInfo(VK_FALSE,
                                      VK_FALSE,
                                      VK_POLYGON_MODE_FILL,
                                      VK_CULL_MODE_FRONT_BIT,//for depth only rendering it is better
                                      VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                      VK_TRUE,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      &rasterizationStateInfo);

        vk::CreateGraphicsPipeline(vk::Device, size, renderPass,
                                   m_ShadowPipelineLayout, stages, modulesSize,
                                   dynamicStates, dynamicStatesCount,
                                   &msState, &vertexInputState, &inputAssemblyInfo, &rasterizationStateInfo,
                                   &m_ShadowPipeline);
    }

    void OpaqueInstances::CreateMaterialDescriptors()
    {
        //descriptor set layout 
        std::vector<vk::DescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                0, //binding
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, //descriptor type
                1, //count
                VK_SHADER_STAGE_FRAGMENT_BIT //vertex stage
            }
        };

        vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0,
                                      &m_MaterialSetLayout);
    }

    void OpaqueInstances::AllocateAndUpdateDescriptors(VkDescriptorSet* descriptorSet,
                                                       const Material& material)
    {
        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool,
                                           { m_MaterialSetLayout },
                                           1, descriptorSet);

        std::vector<vk::UpdateTextureInfo> imagesInfo =
        {
            {
                {
                    //VkDescriptorImageInfo
                    vk::Sampler,                                // sampler
                    material.Tex->ImageView,                  // image view
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,   // image layout
                },

                0,                                              // binding
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // descriptor type
            }
        };

        vk::UpdateDescriptorSets(vk::Device, *descriptorSet, {}, imagesInfo,
            imagesInfo.size());
    }

    void OpaqueInstances::UpdateShadowDescriptors(VkBuffer dirLightBuffer)
    {
        //update descriptor sets
        std::vector<vk::UpdateBuffersInfo> buffersInfo =
        {
             {
                 {
                     //VkDescriptorBufferInfo
                     dirLightBuffer,              // buffer
                     0 , // offset
                     sizeof(glm::mat4),           // range
                  },

                  0,                                 // binding
                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
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

        vk::UpdateDescriptorSets(vk::Device, m_ShadowDescriptorSet, buffersInfo, {},
            buffersInfo.size() /*+ imagesInfo.size()*/);
    }

    void OpaqueInstances::CreateInstanceBuffer()
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

        if(m_InstanceBuffer != VK_NULL_HANDLE)
        {
            vk::DestroyBuffer(vk::Device, m_InstanceBuffer);
            vk::FreeMemory(vk::Device, m_InstanceBufferMemory);
        }

        VkDeviceSize bufferSize = sizeof(Instance) * totalInstances;

        void* instanceBufferMapping = nullptr;

        vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice,
            bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &m_InstanceBuffer, &m_InstanceBufferMemory, &instanceBufferMapping);

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
        vk::UnmapMemory(vk::Device, m_InstanceBufferMemory);


    }

    void OpaqueInstances::UpdateDescriptorSets(VkImageView shadowMapView,
                                               VkBuffer lightsBuffer)
    {
        //update descriptor sets
        std::vector<vk::UpdateBuffersInfo> buffersInfo =
        {
             {
                 {
                     //VkDescriptorBufferInfo
                     vk::PerFrameBuffer,              // buffer
                     0 , // offset 
                     sizeof(PerFrameData),           // range
                 },

                  0,                                 // binding
                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
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
                {
                    lightsBuffer,
                    0,
                    sizeof(glm::mat4)
                },

                2,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            },
        };

        std::vector<vk::UpdateTextureInfo> imagesInfo =
        {
            {
                {
                    vk::Sampler,
                    shadowMapView,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                },

                1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            },
        };

        vk::UpdateDescriptorSets(vk::Device, m_InstanceDescriptorSet, buffersInfo, imagesInfo,
            buffersInfo.size() + imagesInfo.size());
    }

    void OpaqueInstances::CreateInstanceDescriptorSets()
    {
        //descriptor set layout 
        std::vector<vk::DescriptorLayoutInfo> shaderDescriptorInfo =
        {
            {
                0, //binding
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
                1, //count
                VK_SHADER_STAGE_VERTEX_BIT //vertex stage
            },
            {
                1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT
            },
            {
                2, //binding
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
                1, //count
                VK_SHADER_STAGE_FRAGMENT_BIT //vertex stage
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

        vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo },
                                      0, &m_InstanceDescriptorSetLayout);

        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool,
                                   { m_InstanceDescriptorSetLayout },
                                   1, &m_InstanceDescriptorSet);
    }

    bool OpaqueInstances::Material::operator==(const Material& lhs)
    {
        return this->Tex.get() == lhs.Tex.get();
    }

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs)
    {
        return lhs.Tex.get() == rhs.Tex.get();
    }
}