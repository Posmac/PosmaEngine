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
        static OpaqueInstances* Instance();
    private:
        OpaqueInstances() = default;
        static OpaqueInstances* s_Instance;
    public:
        void Init(VkRenderPass renderPass, 
            VkExtent2D windowSize);
        void Deinit();
        void Render(VkCommandBuffer commandBuffer);
        void AddInstance(const glm::mat4& instance);
        void AddModel(Model* model, Texture* modelTexture);

        void UpdateDescriptorSets();
    private:
        void PrepareDescriptorSets();
        void PreparePipelineLayout(VkRenderPass renderPass,
            VkExtent2D extent);

    private:
        Model* m_Model;
        Texture* m_ModelTexture;
        std::vector<glm::mat4> m_Instances;

        //pipeline data
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        VkDescriptorSetLayout m_DescriptorSetLayout;

        //instance buffer
        VkBuffer m_InstanceBuffer;
        VkDeviceMemory m_InstanceBufferMemory;
        void* m_InstanceBufferMapping;

        //descriptos
        VkDescriptorPool m_DescriptorPool;
        VkDescriptorSet m_DescriptorSet;
    };
}