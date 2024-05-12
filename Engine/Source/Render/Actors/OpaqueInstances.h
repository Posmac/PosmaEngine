#pragma once

#include <unordered_map>
#include <memory>

#include "RHI/RHICommon.h"

#include "RHI/Interface/Types.h"

#include "Model/Model.h"

#include "glm/glm.hpp"
#include "Utilities/TextureLoader.h"

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
        
        void Init(DevicePtr device, RenderPassPtr renderPass, RenderPassPtr shadowRenderPass, SResourceExtent2D windowSize, SResourceExtent2D shadowMapSize);
        void Deinit();

        void Render(CommandBufferPtr commandBuffer);
        void RenderDepth(CommandBufferPtr commandBuffer);
        void AddInstance(std::shared_ptr<Model> model, const OpaqModelMeshMaterials& materials, const OpaqModelMeshInstances& instances);

        void UpdateDefaultDescriptors(BufferPtr globalBuffer, BufferPtr shadowMapBuffer, ImagePtr dirDepthShadowMap);
        void UpdateDepthDescriptors(BufferPtr globalBuffer);

        void UpdateInstanceBuffer();
        void UpdateMeshToModelData();
    private:
        void SetupDescriptors();
        void SetupMaterialDescriptor(DescriptorSetPtr& descriptorSet, const Material& material);

        void CreateDepthPipeline(RenderPassPtr renderPass, SResourceExtent2D viewportSize);
        void CreateDefaultPipeline(RenderPassPtr renderPass, SResourceExtent2D viewportSize);

    private:
        std::unordered_map<std::shared_ptr<Model>, uint32_t> m_Models;
        std::vector<PerModel> m_PerModels;

        DevicePtr mDeviceInternal;
        
        BufferPtr mInstanceBuffer;
        DescriptorPoolPtr mDescriptorPool;

        //shader descriptor sets
        DescriptorSetLayoutPtr mGlobalBufferSetLayout; //global buffer only (set 0)
        DescriptorSetPtr mGlobalBufferSet;

        DescriptorSetLayoutPtr mModelDataSetLayout; // model instance matrix (set 1)
        BufferPtr mInstanceToWorldConstantBuffer; //hold all instances of all models

        DescriptorSetLayoutPtr mMaterialSetLayout; // material albedo (set 2)
        DescriptorSetPtr mMaterialSet;

        DescriptorSetLayoutPtr mDepthPassSetLayout; // depth pass light projection matrices buffer(set 3)
        DescriptorSetPtr mDepthPassSet;

        DescriptorSetLayoutPtr mDefaultPassDepthDataSetLayout; // depth pass light projection matrices buffer(b0) and shadow map(b1) (set 4)
        DescriptorSetPtr mDefaultPassDepthDataSet;

        //shadow map pipeline
        PipelinePtr mDepthPassPipeline;
        PipelineLayoutPtr mDepthPassPipelineLayout;

        //default pipeline
        PipelinePtr mDefaultPassPipeline;
        PipelineLayoutPtr mDefaultPassPipelineLayout;


        SamplerPtr mSampler; //temporary
    };

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs);

}