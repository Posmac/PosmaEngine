#include "OpaqueInstances.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "../GlobalBuffer.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkPipeline.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

#include "Render/Graph/RenderPipelineNode.h"
#include "Render/Graph/GraphResourceNames.h"
#include "Render/Graph/ResourceMediator.h"

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

    void OpaqueInstances::Init(DevicePtr device, graph::ResourceMediatorPtr& resourceMediator, DescriptorPoolPtr descriptorPool)
    {
        mDeviceInternal = device;
        mResourceMediator = resourceMediator;
        mDescriptorPoolInternal = descriptorPool;
        SetupDescriptors();
        RegisterResources();
    }

    void OpaqueInstances::Deinit()
    {
        m_Models.clear();

        m_PerModels.clear();

        mInstanceBuffer = nullptr;

        mModelDataSetLayout = nullptr;
        mInstanceToWorldConstantBuffer = nullptr;

        mDeviceInternal = nullptr;
        mResourceMediator = nullptr;
        mDescriptorPoolInternal = nullptr;

        mGlobalBufferSetLayout = nullptr;
        mGlobalBufferSet = nullptr;

        mMaterialSetLayout = nullptr;
        mMaterialSet = nullptr;

        mPerViewBufferSetLayout = nullptr;
        mPerViewBufferSet = nullptr;

        mShadowBufferShadowMapSetLayout = nullptr;
        mShadowBufferShadowMapSet = nullptr;

        mGbufferTargetsResultSetLayout = nullptr;
        mGbufferTargetsResultSet = nullptr;
    }

    void OpaqueInstances::Render(const CommandBufferPtr& commandBuffer, graph::RenderPipelineNodePtr& pipelineNode)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        pipelineNode->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

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

                    mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, pipelineNode->GetPipelineLayout(),
                                                        { mGlobalBufferSet, perMesh.MeshToModelData, material.MaterialDescriptorSet });

                    MeshRange range = perModel.Model->Meshes[i].Range;
                    mDeviceInternal->DrawIndexed(commandBuffer, range, totalInstances, firstInstance);
                    firstInstance += totalInstances;
                }
            }
        }
    }

    void OpaqueInstances::RenderDepth(const CommandBufferPtr& commandBuffer, graph::RenderPipelineNodePtr& pipelineNode)
    {
        if(m_Models.size() == 0)
        {
            return;
        }

        pipelineNode->Bind(commandBuffer, EPipelineBindPoint::GRAPHICS);

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

                    mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, pipelineNode->GetPipelineLayout(),
                                                        { mPerViewBufferSet, perMesh.MeshToModelData });

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
                         .DescriptorPool = mDescriptorPoolInternal,
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

    void OpaqueInstances::SetupDescriptors()
    {
        //global buffer only (set 0)
        std::vector<SDescriptorLayoutInfo> globalBufferInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig globalBufferConfig =
        {
            .pLayoutsInfo = globalBufferInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(globalBufferInfo.size())
        };

        mGlobalBufferSetLayout = mDeviceInternal->CreateDescriptorSetLayout(globalBufferConfig);

        SDescriptorSetAllocateConfig globalBufferAlloc =
        {
             .DescriptorPool = mDescriptorPoolInternal,
             .DescriptorSetLayouts = {mGlobalBufferSetLayout},
             .MaxSets = 1,
        };

        mGlobalBufferSet = mDeviceInternal->AllocateDescriptorSets(globalBufferAlloc);

        // model instance matrix (set 1)
        std::vector<SDescriptorLayoutInfo> modelInstanceMatrixBufferInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig modelInstanceMatrixBufferConfig =
        {
            .pLayoutsInfo = modelInstanceMatrixBufferInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(modelInstanceMatrixBufferInfo.size())
        };

        mModelDataSetLayout = mDeviceInternal->CreateDescriptorSetLayout(modelInstanceMatrixBufferConfig);

        // material PBR (set 2)
        std::vector<SDescriptorLayoutInfo> meshMaterialInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT //vertex stage
            },
            //add more
        };

        SDescriptorSetLayoutConfig meshMaterialConfig =
        {
            .pLayoutsInfo = meshMaterialInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(meshMaterialInfo.size())
        };

        mMaterialSetLayout = mDeviceInternal->CreateDescriptorSetLayout(meshMaterialConfig);

        //depth pass light projection matrices buffer(set 3)
        std::vector<SDescriptorLayoutInfo> depthRenderPassPerViewBufferInfo =
        {
            {
                .Binding = 0, //binding
                .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                .DescriptorCount = 1, //count
                .ShaderStage = EShaderStageFlag::VERTEX_BIT //vertex stage
            }
        };

        SDescriptorSetLayoutConfig depthRenderPassPerViewBufferConfig =
        {
            .pLayoutsInfo = depthRenderPassPerViewBufferInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(depthRenderPassPerViewBufferInfo.size())
        };

        mPerViewBufferSetLayout = mDeviceInternal->CreateDescriptorSetLayout(depthRenderPassPerViewBufferConfig);

        SDescriptorSetAllocateConfig depthAlloc =
        {
             .DescriptorPool = mDescriptorPoolInternal,
             .DescriptorSetLayouts = {mPerViewBufferSetLayout},
             .MaxSets = 1,
        };

        mPerViewBufferSet = mDeviceInternal->AllocateDescriptorSets(depthAlloc);

        // depth pass light projection matrices buffer(b0) and shadow map(b1) (set 4)
        std::vector<SDescriptorLayoutInfo> compositeDepthBufferSamplerInfo =
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

        SDescriptorSetLayoutConfig compositeDepthBufferSamplerConfig =
        {
            .pLayoutsInfo = compositeDepthBufferSamplerInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(compositeDepthBufferSamplerInfo.size())
        };

        mShadowBufferShadowMapSetLayout = mDeviceInternal->CreateDescriptorSetLayout(compositeDepthBufferSamplerConfig);

        SDescriptorSetAllocateConfig depthDataAlloc =
        {
             .DescriptorPool = mDescriptorPoolInternal,
             .DescriptorSetLayouts = {mShadowBufferShadowMapSetLayout},
             .MaxSets = 1,
        };

        mShadowBufferShadowMapSet = mDeviceInternal->AllocateDescriptorSets(depthDataAlloc);

        //composite pass <- gbuffer result 
        std::vector<SDescriptorLayoutInfo> gbufferInputInfo =
        {
            {
                .Binding = 0,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 2,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 3,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 4,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 5,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
            {
                .Binding = 6,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                .DescriptorCount = 1,
                .ShaderStage = EShaderStageFlag::FRAGMENT_BIT
            },
        };

        SDescriptorSetLayoutConfig gbufferInputConfig =
        {
            .pLayoutsInfo = gbufferInputInfo.data(),
            .LayoutsCount = static_cast<uint32_t>(gbufferInputInfo.size())
        };

        mGbufferTargetsResultSetLayout = mDeviceInternal->CreateDescriptorSetLayout(gbufferInputConfig);

        SDescriptorSetAllocateConfig gbufferInputAllocConfig =
        {
            .DescriptorPool = mDescriptorPoolInternal,
            .DescriptorSetLayouts = {mGbufferTargetsResultSetLayout},
            .MaxSets = 1,
        };

        mGbufferTargetsResultSet = mDeviceInternal->AllocateDescriptorSets(gbufferInputAllocConfig);
    }

    void OpaqueInstances::SetupMaterialDescriptor(DescriptorSetPtr& descriptorSet, const Material& material)
    {
        SDescriptorSetAllocateConfig allocateConfig =
        {
            .DescriptorPool = mDescriptorPoolInternal,
            .DescriptorSetLayouts = {mMaterialSetLayout},
            .MaxSets = 1
        };

        descriptorSet = mDeviceInternal->AllocateDescriptorSets(allocateConfig);

        SamplerPtr& sampler = mResourceMediator->GetSamplerByName(graph::DEFAULT_SAMPLER);

        std::vector<SUpdateTextureConfig> texturesUpdateInfo =
        {
            {
                .Sampler = sampler,
                .Image = material.Albedo,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 0,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            }
        };

        mDeviceInternal->UpdateDescriptorSets(descriptorSet, texturesUpdateInfo, {});
    }

    void OpaqueInstances::RegisterResources()
    {
        mResourceMediator->RegisterDescriptorSetLayout(graph::MODEL_DATA_SET, mModelDataSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::GLOBAL_CBUFFER_SET, mGlobalBufferSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::GLOBAL_CBUFFER_SET, mGlobalBufferSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::OPAQUE_INSTANCES_MATERIALS_SET, mMaterialSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::OPAQUE_INSTANCES_MATERIALS_SET, mMaterialSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::SHADOW_MAP_GENERATION_PER_VIEW_CBUFFER_SET, mPerViewBufferSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::SHADOW_MAP_GENERATION_PER_VIEW_CBUFFER_SET, mPerViewBufferSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET, mShadowBufferShadowMapSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET, mShadowBufferShadowMapSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::GBUFFER_RESULT_COMPOSITE_INPUT_SET, mGbufferTargetsResultSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::GBUFFER_RESULT_COMPOSITE_INPUT_SET, mGbufferTargetsResultSetLayout);
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

    void OpaqueInstances::UpdateDefaultDescriptors(uint32_t imageIndex)
    {
        BufferPtr& globalBuffer = mResourceMediator->GetBufferByName(graph::GLOBAL_CBUFFER);
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
    }

    void OpaqueInstances::UpdateDepthDescriptors(uint32_t imageIndex)
    {
        BufferPtr& shadowMapBuffer = mResourceMediator->GetBufferByName(graph::SHADOW_CBUFFER);
        SUpdateBuffersConfig buffers =
        {
            .Buffer = shadowMapBuffer,
            .Offset = 0,
            .Range = shadowMapBuffer->Size(),
            .DstBinding = 0,
            .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
        };

        mDeviceInternal->UpdateDescriptorSets(mPerViewBufferSet, {}, { buffers });
    }

    void OpaqueInstances::UpdateGBufferDescriptors(uint32_t imageIndex)
    {
        /* resourceMediator->GetDescriptorSetLayoutByName(GBUFFER_RESULT_COMPOSITE_INPUT_SET),
                resourceMediator->GetDescriptorSetLayoutByName(COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET),*/
        SamplerPtr& sampler = mResourceMediator->GetSamplerByName(graph::DEFAULT_SAMPLER);

        //update gbuffer resulted render target images
        foundation::Name albedoRefName = graph::GetResourceIndexedName(graph::GBUFFER_ALBEDO_RENDERTARGET_NAME, imageIndex);
        foundation::Name normalRefName = graph::GetResourceIndexedName(graph::GBUFFER_NORMAL_RENDERTARGET_NAME, imageIndex);
        foundation::Name depthRefName = graph::GetResourceIndexedName(graph::GBUFFER_DEPTH_RENDERTARGET_NAME, imageIndex);
        foundation::Name worldPosRefName = graph::GetResourceIndexedName(graph::GBUFFER_WORLDPOS_RENDERTARGET_NAME, imageIndex);
        foundation::Name emissionRefName = graph::GetResourceIndexedName(graph::GBUFFER_EMISSION_RENDERTARGET_NAME, imageIndex);
        foundation::Name specularGlossRefName = graph::GetResourceIndexedName(graph::GBUFFER_SPECULAR_GLOSS_RENDERTARGET_NAME, imageIndex);
        foundation::Name roughMetallRefName = graph::GetResourceIndexedName(graph::GBUFFER_ROUGH_METAL_RENDERTARGET_NAME, imageIndex);

        ImagePtr& albedoImage = mResourceMediator->GetImageByName(albedoRefName);
        ImagePtr& normalImage = mResourceMediator->GetImageByName(normalRefName);
        ImagePtr& depthImage = mResourceMediator->GetImageByName(depthRefName);
        ImagePtr& worldPosImage = mResourceMediator->GetImageByName(worldPosRefName);
        ImagePtr& emissionImage = mResourceMediator->GetImageByName(emissionRefName);
        ImagePtr& specularGlossImage = mResourceMediator->GetImageByName(specularGlossRefName);
        ImagePtr& roughMetallImage = mResourceMediator->GetImageByName(roughMetallRefName);

        std::vector<SUpdateTextureConfig> gbufferTextureUpdateInfo =
        {
            {
                .Sampler = sampler,
                .Image = albedoImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 0,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = normalImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = depthImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 2,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = worldPosImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 3,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = emissionImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 4,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = specularGlossImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 5,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            },
            {
                .Sampler = sampler,
                .Image = roughMetallImage,
                .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .DstBinding = 6,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            }
        };

        mDeviceInternal->UpdateDescriptorSets(mGbufferTargetsResultSet, gbufferTextureUpdateInfo, {});

        //update shadow buffer and shadow map
        BufferPtr& shadowMapBuffer = mResourceMediator->GetBufferByName(graph::SHADOW_CBUFFER);
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

        foundation::Name shadowImageRefName = graph::GetResourceIndexedName(graph::SHADOWMAP_RENDERTARGET_NAME, imageIndex);
        ImagePtr& shadowMapImage = mResourceMediator->GetImageByName(shadowImageRefName);
        std::vector<SUpdateTextureConfig> textureUpdateInfo =
        {
            {
                .Sampler = sampler,
                .Image = shadowMapImage,
                .ImageLayout = EImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .DstBinding = 1,
                .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER
            }
        };

        mDeviceInternal->UpdateDescriptorSets(mShadowBufferShadowMapSet, textureUpdateInfo, shadowBuffersInfo);
    }

    void OpaqueInstances::BindCompositeDescriptors(const CommandBufferPtr& commandBuffer, graph::RenderPipelineNodePtr& pipelineNode)
    {
        mDeviceInternal->BindDescriptorSets(commandBuffer, EPipelineBindPoint::GRAPHICS, pipelineNode->GetPipelineLayout(),
                                            { mGbufferTargetsResultSet, mShadowBufferShadowMapSet });
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