#pragma once

#include <unordered_map>
#include <memory>

#include "RHI/VkCommon.h"
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
        
        void Init(DevicePtr device, RenderPassPtr renderPass, RenderPassPtr shadowRenderPass, SResourceExtent2D windowSize, SResourceExtent2D shadowMapSize);
        void Deinit();

        void Render(CommandBufferPtr commandBuffer);
        void RenderDepth(CommandBufferPtr commandBuffer);
        void AddInstance(std::shared_ptr<Model> model, const Material& material, const Instance& instance);

        void UpdateInstanceDescriptorSets(BufferPtr globalBuffer, BufferPtr shadowMapBuffer, ImagePtr dirDepthShadowMap);
        void UpdateShadowMapDescriptorSets(BufferPtr globalBuffer);

        void UpdateInstanceBuffer();
    private:
        void CreateInstanceDescriptorSets();
        void CreateInstancePipeline(RenderPassPtr renderPass, SResourceExtent2D viewportSize);
        void CreateMaterialDescriptors();
        void AllocateAndUpdateDescriptors(DescriptorSetPtr& descriptorSet, const Material& material);

        void CreateShadowMapDescriptorSets();
        void CreateShadowMapPipeline(RenderPassPtr renderPass, SResourceExtent2D viewportSize);

    private:
        std::unordered_map<std::shared_ptr<Model>, uint32_t> m_Models;
        std::vector<PerModel> m_PerModels;

        DevicePtr mDeviceInternal;
        PipelinePtr mInstancedPipeline;
        PipelineLayoutPtr mInstancedPipelineLayout;
        BufferPtr mInstanceBuffer;
        DescriptorPoolPtr mDescriptorPool;

        DescriptorSetLayoutPtr mMaterilaSetLayout;
        DescriptorSetLayoutPtr mInstanceDescriptorSetLayout;
        DescriptorSetPtr mInstanceDescriptorSet;

        DescriptorSetLayoutPtr mInstanceShadowDescriptorSetLayout;
        DescriptorSetPtr mInstanceShadowDescriptorSet;

        DescriptorSetLayoutPtr mShadowMapDescriptorSetLayout;
        DescriptorSetPtr mShadowMapDescriptorSet;
        PipelinePtr mShadowMapPipeline;
        PipelineLayoutPtr mShadowMapPipelineLayout;

        SamplerPtr mSampler; //temporary
    };

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs);

}