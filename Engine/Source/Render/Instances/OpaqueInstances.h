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
        void Init(Model* pModel,
            VkRenderPass renderPass, 
            VkExtent2D windowSize);
        void Deinit();
        void Render(VkCommandBuffer commandBuffer);
        void AddInstance(const glm::mat4& instance);

        void UpdateDescriptorSets(VkImageView albedo, VkImageView emission);
    private:
        void UpdateBuffers(VkCommandBuffer commandBuffer);
        void Bind(VkCommandBuffer commandBuffer);
        void PrepareDescriptorSets();
        void PreparePipelineLayout(VkRenderPass renderPass,
            VkExtent2D extent);
        void InitBuffers();

    private:
        Model* m_pModel;
        std::vector<glm::mat4> m_Instances;

        //pipeline data
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        VkDescriptorSetLayout m_DescriptorSetLayout;

        //shaders data
        VkBuffer m_ShaderBuffer;
        VkDeviceMemory m_ShaderBufferMemory;
        void* m_ShaderBufferMapping;

        //descriptos
        VkDescriptorPool m_DescriptorPool;
        VkDescriptorSet m_DescriptorSet;
    };
}