#include "Descriptors.h"

namespace psm
{
    namespace vk
    {
        void CreateDestriptorSetLayout(VkDevice device, const std::vector<DescriptorLayoutInfo>& descriptorLayoutsInfo,
            VkDescriptorSetLayoutCreateFlags flags, VkDescriptorSetLayout* layout)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings(descriptorLayoutsInfo.size());
            for (int i = 0; i < descriptorLayoutsInfo.size(); i++)
            {
                bindings[i].binding = descriptorLayoutsInfo[i].Binding;
                bindings[i].descriptorType = descriptorLayoutsInfo[i].DescriptorType;
                bindings[i].descriptorCount = descriptorLayoutsInfo[i].DescriptorCount;
                bindings[i].stageFlags = descriptorLayoutsInfo[i].StageFlags;
                bindings[i].pImmutableSamplers = nullptr;
            }

            VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
            descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorLayoutInfo.pNext = nullptr;
            descriptorLayoutInfo.bindingCount = bindings.size();
            descriptorLayoutInfo.pBindings = bindings.data();
            descriptorLayoutInfo.flags = flags;

            VkResult result = vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr,
                layout);

            VK_CHECK_RESULT(result);
        }

        void CreateDescriptorPool(VkDevice device, const std::vector<DescriptorPoolSize>& descriptorsInfo,
            uint32_t maximumDescriptorSets, VkDescriptorPoolCreateFlags poolFlags, 
            VkDescriptorPool* pool)
        {
            std::vector<VkDescriptorPoolSize> poolSize(descriptorsInfo.size());

            for (int i = 0; i < descriptorsInfo.size(); i++)
            {
                poolSize[i].type = descriptorsInfo[i].Type;
                poolSize[i].descriptorCount = descriptorsInfo[i].Size;
            }

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.pNext = nullptr;
            poolInfo.poolSizeCount = poolSize.size();
            poolInfo.pPoolSizes = poolSize.data();
            poolInfo.maxSets = maximumDescriptorSets;
            poolInfo.flags = poolFlags;

            vkCreateDescriptorPool(device, &poolInfo, nullptr, pool);
        }

        void AllocateDescriptorSets(VkDevice device, VkDescriptorPool pool, 
            const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, 
            uint32_t descriptorSetCount, VkDescriptorSet* descriptorSet)
        {
            VkDescriptorSetAllocateInfo setsAllocInfo{};
            setsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            setsAllocInfo.pNext = nullptr;
            setsAllocInfo.descriptorPool = pool;
            setsAllocInfo.descriptorSetCount = descriptorSetCount;
            setsAllocInfo.pSetLayouts = descriptorSetLayouts.data();

            vkAllocateDescriptorSets(device, &setsAllocInfo, descriptorSet);
        }

        void UpdateDescriptorSets(VkDevice device, const std::vector<VkWriteDescriptorSet>& writeDescriptors)
        {
            vkUpdateDescriptorSets(device, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
        }

        void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout layout,
            const std::vector<VkDescriptorSet>& sets)
        {
            vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, 0, sets.size(), sets.data(), 0, nullptr);
        }

        void DestroyDescriptorPool(VkDevice device, VkDescriptorPool pool)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
    }
}