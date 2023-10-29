#pragma once

#include <vector>
#include <array>

#include "Core.h"

namespace psm
{
    namespace vk
    {
        struct DescriptorLayoutInfo
        {
            uint32_t Binding;
            VkDescriptorType DescriptorType;
            uint32_t DescriptorCount;
            VkShaderStageFlagBits StageFlags;
        };

        struct DescriptorPoolSize
        {
            VkDescriptorType Type;
            uint32_t Size;
        };

        struct UpdateBuffersInfo
        {
            VkDescriptorBufferInfo BufferInfo;
            uint32_t DstBinding;
            VkDescriptorType DescriptorType;
        };

        struct UpdateTextureInfo
        {
            VkDescriptorImageInfo ImageInfo;
            uint32_t DstBinding;
            VkDescriptorType DescriptorType;
        };

        void CreateDestriptorSetLayout(VkDevice device, 
            const std::vector<DescriptorLayoutInfo>& descriptorLayoutsInfo,
            VkDescriptorSetLayoutCreateFlags flags, 
            VkDescriptorSetLayout* layout);

        void CreateDescriptorPool(VkDevice device, 
            const std::vector<DescriptorPoolSize>& descriptorsInfo,
            uint32_t maximumDescriptorSets, 
            VkDescriptorPoolCreateFlags poolFlags,
            VkDescriptorPool* pool);

        void AllocateDescriptorSets(VkDevice device, 
            VkDescriptorPool pool,
            const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
            uint32_t descriptorSetCount, 
            VkDescriptorSet* descriptorSet);

        void UpdateDescriptorSets(VkDevice device,
            VkDescriptorSet descriptorSet,
            const std::vector<UpdateBuffersInfo>& buffersInfo,
            const std::vector<UpdateTextureInfo>& imagesInfo,
            uint32_t size);

        void BindDescriptorSets(VkCommandBuffer commandBuffer, 
            VkPipelineBindPoint bindPoint, 
            VkPipelineLayout layout,
            const std::vector<VkDescriptorSet>& sets);

        void DestroyDescriptorPool(VkDevice device, VkDescriptorPool pool);

    }
}