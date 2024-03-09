#pragma once

#include <unordered_map>
#include <memory>

#include "RHI/VkCommon.h"
#include "RHI/Interface/Types.h"

//#include "Core/Log.h"
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

        struct MaterialHash
        {
            std::size_t operator()(const Material& mat) const
            {
                std::hash<std::shared_ptr<void>> func;
                return func(mat.Albedo);
            }
        };

        struct Instance
        {
            glm::mat4 InstanceMatrix;
        };
    private:
        struct PerMaterial
        {
            Material Material;
            std::vector<Instance> Instances;

            //api related data
            DescriptorSetPtr MaterialDescriptorSet;
        };

        struct PerModel
        {
            std::shared_ptr<Model> Model;
            std::vector<PerMaterial> PerMaterials;
            std::unordered_map<Material, int, MaterialHash> MaterialsData;
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
        void Init(DevicePtr device, RenderPassPtr renderPass, RenderPassPtr shadowRenderPass, SResourceExtent2D windowSize);
        void Deinit();
        void Render(CommandBufferPtr commandBuffer);
        void RenderDepth2D(CommandBufferPtr commandBuffer, float depthBias, float depthSlope);
        void AddInstance(std::shared_ptr<Model> model, const Material& material, const Instance& instance);

        void UpdateDescriptorSets(ImagePtr shadowMapView, BufferPtr lightsBuffer);
        void UpdateShadowDescriptors(BufferPtr lightsBuffer);

        void PrepareInstances();
    private:
        void CreateInstanceDescriptorSets();
        void CreateInstancePipelineLayout(RenderPassPtr renderPass, SResourceExtent2D extent);
        void CreateShadowDescriptorSets();
        void CreateShadowPipeline(RenderPassPtr renderPass, SResourceExtent2D size);
        void CreateMaterialDescriptors();
        void AllocateAndUpdateDescriptors(DescriptorSetPtr descriptorSet, const Material& material);

    private:
        std::unordered_map<std::shared_ptr<Model>, uint32_t> m_Models;
        std::vector<PerModel> m_PerModels;

        DevicePtr mDeviceInternal;
        //pipeline data
        PipelinePtr mInstancedPipeline;
        PipelineLayoutPtr mInstancedPipelineLayout;
        //VkPipelineLayout m_InstancedPipelineLayout;
        //VkPipeline m_InstancedPipeline;

        //shadow generation pipeline
        PipelinePtr mShadowsPipeline;
        PipelineLayoutPtr mShadowsPipelineLayout;
        //VkPipelineLayout m_ShadowPipelineLayout;
        //VkPipeline m_ShadowPipeline;

        //instance buffer
        BufferPtr mInstanceBuffer;
        //VkBuffer m_InstanceBuffer;
        //VkDeviceMemory m_InstanceBufferMemory;

        //descriptos
        DescriptorPoolPtr mDescriptorPool;
        //VkDescriptorPool m_DescriptorPool;

        //model material descriptor layout
        DescriptorSetLayoutPtr mMaterilaSetLayout;
        //VkDescriptorSetLayout m_MaterialSetLayout;

        //general data for all models
        DescriptorSetLayoutPtr mInstanceDescriptorSetLayout;
        DescriptorSetPtr mInstanceDescriptorSet;
        //VkDescriptorSetLayout m_InstanceDescriptorSetLayout;
        //VkDescriptorSet m_InstanceDescriptorSet;

        //shadow data (also for all models)
        DescriptorSetLayoutPtr mShadowDescriptorSetLayout;
        DescriptorSetPtr mShadowDescriptorSet;
        //VkDescriptorSetLayout m_ShadowDescriptorSetLayout;
        //VkDescriptorSet m_ShadowDescriptorSet;

        SamplerPtr mSampler; //temporary
    };

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs);

}