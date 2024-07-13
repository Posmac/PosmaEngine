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

        mDepthPassSetLayout = nullptr;
        mDepthPassSet = nullptr;

        mDefaultSetLayout = nullptr;
        mDefaultSet = nullptr;
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
                                                        { mGlobalBufferSet, perMesh.MeshToModelData, material.MaterialDescriptorSet, mDefaultSet });

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
             .DescriptorPool = mDescriptorPoolInternal,
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
             .DescriptorPool = mDescriptorPoolInternal,
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

        mDefaultSetLayout = mDeviceInternal->CreateDescriptorSetLayout(depthInputConfig);

        SDescriptorSetAllocateConfig depthDataAlloc =
        {
             .DescriptorPool = mDescriptorPoolInternal,
             .DescriptorSetLayouts = {mDefaultSetLayout},
             .MaxSets = 1,
        };

        mDefaultSet = mDeviceInternal->AllocateDescriptorSets(depthDataAlloc);
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
        mResourceMediator->RegisterDescriptorSetLayout(graph::MODEL_DATA_SET_LAYOUT, mModelDataSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::GLOBAL_BUFFER_SET, mGlobalBufferSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::GLOBAL_BUFFER_SET, mGlobalBufferSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::OPAQUE_MATERIAL_SET, mMaterialSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::OPAQUE_MATERIAL_SET, mMaterialSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::DEPTH_PASS_SET, mDepthPassSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::DEPTH_PASS_SET, mDepthPassSetLayout);

        mResourceMediator->RegisterDescriptorSet(graph::DEFAULT_PASS_SET, mDefaultSet);
        mResourceMediator->RegisterDescriptorSetLayout(graph::DEFAULT_PASS_SET, mDefaultSetLayout);
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

        SamplerPtr& sampler = mResourceMediator->GetSamplerByName(graph::DEFAULT_SAMPLER);
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

        mDeviceInternal->UpdateDescriptorSets(mDefaultSet, textureUpdateInfo, shadowBuffersInfo);
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