#pragma once

#include <unordered_map>
#include <memory>

#include "Core/Log.h"
#include "Render/Vk.h"
#include "Model/Model.h"

#include "glm/glm.hpp"
#include "Render/Vk.h"
#include "Include/vulkan/vulkan.hpp"
#include "Utilities/TextureLoader.h"
#include "Vulkan/Descriptors.h"
#include "Vulkan/Sampler.h"
#include "Vulkan/Pipeline.h"

namespace psm
{
    class OpaqueInstances
    {
    public:
        struct Material
        {
            std::shared_ptr<Texture> Tex;

            bool operator==(const Material& lhs);
        };

        struct MaterialHash
        {
            std::size_t operator()(const Material& mat) const
            {
                std::hash<std::shared_ptr<void>> func;
                return func(mat.Tex);
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
            VkDescriptorSet MaterialDescriptorSet;
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
        void Init(VkRenderPass renderPass,
                  VkRenderPass shadowRenderPass, 
                  VkExtent2D windowSize);
        void Deinit();
        void Render(VkCommandBuffer commandBuffer);
        void RenderDepth2D(VkCommandBuffer commandBuffer,
                           float depthBias,
                           float depthSlope);
        void AddInstance(std::shared_ptr<Model> model, 
                         const Material& material,
                         const Instance& instance);

        void UpdateDescriptorSets(VkImageView shadowMapView, 
                                  VkBuffer lightsBuffer);
        void UpdateShadowDescriptors(VkBuffer lightsBuffer);

        void CreateInstanceBuffer();
    private:
        void CreateInstanceDescriptorSets();
        void CreateInstancePipelineLayout(VkRenderPass renderPass, 
                                          VkExtent2D extent);
        void CreateShadowDescriptorSets();
        void CreateShadowPipeline(VkExtent2D size,
                                  VkRenderPass renderPass);
        void CreateMaterialDescriptors();
        void AllocateAndUpdateDescriptors(VkDescriptorSet* descriptorSet,
                                          const Material& material);

    private:
        std::unordered_map<std::shared_ptr<Model>, uint32_t> m_Models;
        std::vector<PerModel> m_PerModels;

        //pipeline data
        VkPipelineLayout m_InstancedPipelineLayout;
        VkPipeline m_InstancedPipeline;

        //shadow generation pipeline
        VkPipelineLayout m_ShadowPipelineLayout;
        VkPipeline m_ShadowPipeline;

        //instance buffer
        VkBuffer m_InstanceBuffer;
        VkDeviceMemory m_InstanceBufferMemory;

        //descriptos
        VkDescriptorPool m_DescriptorPool;

        //model material descriptor layout
        VkDescriptorSetLayout m_MaterialSetLayout;

        //general data for all models
        VkDescriptorSetLayout m_InstanceDescriptorSetLayout;
        VkDescriptorSet m_InstanceDescriptorSet;

        //shadow data (also for all models)
        VkDescriptorSetLayout m_ShadowDescriptorSetLayout;
        VkDescriptorSet m_ShadowDescriptorSet;
    };

    bool operator==(const OpaqueInstances::Material& lhs, const OpaqueInstances::Material& rhs);

}