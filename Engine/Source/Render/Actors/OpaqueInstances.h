#pragma once

#include <unordered_map>
#include <memory>

#include "RHI/RHICommon.h"

#include "RHI/Interface/Types.h"

#include "Model/Model.h"

#include "glm/glm.hpp"
#include "Utilities/TextureLoader.h"

#include "Render/Graph/RenderPipelineNode.h"
#include "Render/Graph/ResourceMediator.h"

namespace psm
{
    class OpaqueInstances
    {
    public:
        struct Material
        {
            ImagePtr Albedo;

            bool operator==(const Material& lhs);
        };

        struct Instance
        {
            glm::mat4 InstanceMatrix;
        };

        using OpaqModelMeshMaterials = std::vector<Material>;
        using OpaqModelMeshInstances = std::vector<Instance>;

        struct MaterialHash
        {
            std::size_t operator()(const Material& mat) const
            {
                std::hash<std::shared_ptr<void>> func;
                return func(mat.Albedo);
            }
        };

    private:
        struct PerMaterial
        {
            Material Material;
            std::vector<Instance> Instances;

            //api related data
            DescriptorSetPtr MaterialDescriptorSet;
        };

        struct PerMesh
        {
            std::vector<PerMaterial> PerMaterials;
            DescriptorSetPtr MeshToModelData;
            std::unordered_map<Material, int, MaterialHash> MaterialsData;
        };

        struct PerModel
        {
            std::shared_ptr<Model> Model;
            std::vector<PerMesh> Meshes;
        };

        struct InstanceInternal
        {
            glm::mat4 MeshToModel;
        };

    public:
        struct PushConstant
        {
            float Time;
        };

        struct ShaderUBO
        {
            glm::vec4 Offset;
            glm::vec4 Color;
        };

        //singleton realization
    public:
        OpaqueInstances(OpaqueInstances&) = delete;
        void operator=(const OpaqueInstances&) = delete;
        static OpaqueInstances* GetInstance();
    private:
        OpaqueInstances() = default;
        static OpaqueInstances* s_Instance;

        //class related
    public:

        void Init(DevicePtr device, graph::ResourceMediatorPtr& resourceMediator, DescriptorPoolPtr descriptorPool);
        void Deinit();

        void Render(const CommandBufferPtr& commandBuffer, graph::RenderPipelineNodePtr& pipelineNode);
        void RenderDepth(const CommandBufferPtr& commandBuffer, graph::RenderPipelineNodePtr& pipelineNode);
        void AddInstance(std::shared_ptr<Model> model, const OpaqModelMeshMaterials& materials, const OpaqModelMeshInstances& instances);

        void UpdateDefaultDescriptors(uint32_t imageIndex);
        void UpdateDepthDescriptors(uint32_t imageIndex);

        void UpdateInstanceBuffer();
        void UpdateMeshToModelData();
    private:
        void SetupDescriptors();
        void SetupMaterialDescriptor(DescriptorSetPtr& descriptorSet, const Material& material);
        void RegisterResources();

    private:
        std::unordered_map<std::shared_ptr<Model>, uint32_t> m_Models;
        std::vector<PerModel> m_PerModels;

        DevicePtr mDeviceInternal;
        graph::ResourceMediatorPtr mResourceMediator;
        DescriptorPoolPtr mDescriptorPoolInternal;

        BufferPtr mInstanceBuffer;

        DescriptorSetLayoutPtr mModelDataSetLayout; // model instance matrix (set 1)
        BufferPtr mInstanceToWorldConstantBuffer; //hold all instances of all models
        
        DescriptorSetLayoutPtr mGlobalBufferSetLayout;
        DescriptorSetPtr mGlobalBufferSet;

        DescriptorSetLayoutPtr mMaterialSetLayout;
        DescriptorSetPtr mMaterialSet;

        DescriptorSetLayoutPtr mDepthPassSetLayout;
        DescriptorSetPtr mDepthPassSet;

        DescriptorSetLayoutPtr mDefaultSetLayout;
        DescriptorSetPtr mDefaultSet;
    };

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs);

}