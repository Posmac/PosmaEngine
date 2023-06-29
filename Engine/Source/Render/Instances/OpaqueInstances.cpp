#include "OpaqueInstances.h"

namespace psm
{
    OpaqueInstances* OpaqueInstances::s_Instance = nullptr;

    OpaqueInstances* OpaqueInstances::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new OpaqueInstances();
        }

        return s_Instance;
    }

    void OpaqueInstances::Init(Model* pModel,
        VkRenderPass renderPass, 
        VkExtent2D windowSize)
    {
        if (pModel == nullptr)
        {
            LOG_ERROR("Model pointer is null");
        }

        m_pModel = pModel;

        PrepareDescriptorSets();
        PreparePipelineLayout(renderPass, windowSize);

        vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice,
            sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &m_InstanceBuffer, &m_InstanceBufferMemory, &m_InstanceBufferMapping);
    }

    void OpaqueInstances::Deinit()
    {
        vk::DestroyPipeline(vk::Device, m_Pipeline);
        vk::DestroyPipelineLayout(vk::Device, m_PipelineLayout);
        vkDestroyDescriptorSetLayout(vk::Device, m_DescriptorSetLayout, nullptr);

        vk::DestroyDescriptorPool(vk::Device, m_DescriptorPool);

        vkDestroyDescriptorPool(vk::Device, m_DescriptorPool, nullptr);
    }

    void OpaqueInstances::Render(VkCommandBuffer commandBuffer)
    {
        if (m_pModel == nullptr)
        {
            LOG_ERROR("Model pointer is null");
            return;
        }

        if (m_Instances.empty())
        {
            return;
        }

        //bind all necessary things
        //(pipeline, descriptor sets, push constants, uniform buffers)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        m_pModel->BindBuffers(commandBuffer);

        vk::BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_PipelineLayout, { m_DescriptorSet });

        uint32_t totalInstances = m_Instances.size();
        uint32_t firstInstance = 0;

        for (int i = 0; i < m_pModel->Meshes.size(); i++)
        {
            MeshRange range = m_pModel->Meshes[i].Range;
            vkCmdDrawIndexed(commandBuffer, range.IndicesCount, totalInstances,
                range.IndicesOffset, range.VerticesOffset, firstInstance);
        }
    }

    void OpaqueInstances::AddInstance(const glm::mat4& instance)
    {
        m_Instances.push_back(instance);
    }

    void OpaqueInstances::PreparePipelineLayout(VkRenderPass renderPass,
        VkExtent2D extent)
    {
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

        //pipeline layout
        constexpr uint32_t pushConstantsSize = 1;
        VkPushConstantRange pushConstants[pushConstantsSize] =
        {
            {
                VK_SHADER_STAGE_FRAGMENT_BIT, // stage flags
                0,                            // offset
                sizeof(float),                // size
            },
        };

        constexpr uint32_t descriptorSetLayoutsSize = 1;
        VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            m_DescriptorSetLayout
        };

        vk::CreatePipelineLayout(vk::Device,
            descriptorSetLayouts,
            descriptorSetLayoutsSize,
            pushConstants,
            pushConstantsSize,
            &m_PipelineLayout);

        //shader stages
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

        //vertex input
        constexpr size_t attribsSize = 3;
        VkVertexInputAttributeDescription vertexAttribDescr[attribsSize] =
        {
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
        };

        constexpr size_t bindingsSize = 1;
        VkVertexInputBindingDescription bindingDescriptions[bindingsSize] =
        {
            {
                0,                          // binding
                sizeof(Vertex),             // string
                VK_VERTEX_INPUT_RATE_VERTEX // input rate
            },
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vk::GetVertexInputInfo(vertexAttribDescr, attribsSize,
            bindingDescriptions, bindingsSize, &vertexInputState);

        //input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        //create graphics pipeline (a lot of default things)
        vk::CreateGraphicsPipeline(vk::Device, extent,
            renderPass, m_PipelineLayout, stages, modulesSize,
            vertexInputState, inputAssemblyInfo, &m_Pipeline);

        vk::DestroyShaderModule(vk::Device, vertexShader);
        vk::DestroyShaderModule(vk::Device, fragmentShader);
    }

    void OpaqueInstances::UpdateDescriptorSets(VkImageView albedo, VkImageView emission)
    {
        glm::mat4* instance = reinterpret_cast<glm::mat4*>(m_InstanceBufferMapping);
        *instance = m_Instances[0];

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

             {
                 {
                     //VkDescriptorBufferInfo
                     m_InstanceBuffer,              // buffer
                     0 , // offset
                     sizeof(glm::mat4),           // range
                  },

                  1,                                 // binding
                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
             },
        };

        std::vector<vk::UpdateTextureInfo> imagesInfo =
        {
            {
                {
                    //VkDescriptorImageInfo
                    vk::Sampler,                                // sampler
                    albedo,                                     // image view
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,   // image layout
                },

                2,                                              // binding
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // descriptor type
            },
        };

        vk::UpdateDescriptorSets(vk::Device, m_DescriptorSet, buffersInfo, imagesInfo,
            buffersInfo.size() + imagesInfo.size());
    }

    void OpaqueInstances::PrepareDescriptorSets()
    {
        //create descriptor pool
        std::vector<vk::DescriptorPoolSize> shaderDescriptors =
        {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                2
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1
            }
        };

        vk::CreateDescriptorPool(vk::Device, shaderDescriptors, 1, 0, &m_DescriptorPool);

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
                1, //binding
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor type
                1, //count
                VK_SHADER_STAGE_VERTEX_BIT //vertex stage
            },
            {
                2,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT
            }
        };

        vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0, &m_DescriptorSetLayout);

        vk::AllocateDescriptorSets(vk::Device, m_DescriptorPool, { m_DescriptorSetLayout },
            1, &m_DescriptorSet);
    }
}