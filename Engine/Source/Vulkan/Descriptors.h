#pragma once

#include <vector>
#include <array>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

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

        struct DescriptorSize
        {
            VkDescriptorType Type;
            uint32_t Size;
        };

        struct UpdateBuffersInfo
        {
            VkBuffer Buffer;
            VkDeviceSize Offset;
            VkDeviceSize Range;
        };

        void CreateDestriptorSetLayout(VkDevice device, const std::vector<DescriptorLayoutInfo>& descriptorLayoutsInfo,
            VkDescriptorSetLayoutCreateFlags flags, VkDescriptorSetLayout* layout);
        void CreateDescriptorPool(VkDevice device, const std::vector<DescriptorSize>& descriptorsInfo,
            uint32_t maximumDescriptorSets, VkDescriptorPoolCreateFlags poolFlags,
            VkDescriptorPool* pool);
        void AllocateDescriptorSets(VkDevice device, VkDescriptorPool pool,
            const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
            uint32_t descriptorSetCount, VkDescriptorSet* descriptorSet);
        void UpdateDescriptorSets(VkDevice device, const std::vector<VkWriteDescriptorSet>& writeDescriptors);
        void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout layout,
            const std::vector<VkDescriptorSet>& sets);
        void DestroyDescriptorPool(VkDevice device, VkDescriptorPool pool);

    }
}