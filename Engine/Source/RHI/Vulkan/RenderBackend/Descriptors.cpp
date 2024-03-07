#include "Descriptors.h"


namespace psm
{
    namespace vk
    {
        void CreateDestriptorSetLayout(VkDevice device, 
            const std::vector<DescriptorLayoutInfo>& descriptorLayoutsInfo,
            VkDescriptorSetLayoutCreateFlags flags, 
            VkDescriptorSetLayout* layout)
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

        void CreateDescriptorPool(VkDevice device, 
            const std::vector<DescriptorPoolSize>& descriptorsInfo,
            uint32_t maximumDescriptorSets, 
            VkDescriptorPoolCreateFlags poolFlags,
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

            VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, pool);
            VK_CHECK_RESULT(result);
        }

        void AllocateDescriptorSets(VkDevice device, 
            VkDescriptorPool pool,
            const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
            uint32_t descriptorSetCount, 
            VkDescriptorSet* descriptorSet)
        {
            VkDescriptorSetAllocateInfo setsAllocInfo{};
            setsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            setsAllocInfo.pNext = nullptr;
            setsAllocInfo.descriptorPool = pool;
            setsAllocInfo.descriptorSetCount = descriptorSetCount;
            setsAllocInfo.pSetLayouts = descriptorSetLayouts.data();

            VkResult result = vkAllocateDescriptorSets(device, &setsAllocInfo, descriptorSet);
            VK_CHECK_RESULT(result);
        }

        void UpdateDescriptorSets(VkDevice device,
            VkDescriptorSet descriptorSet,
            const std::vector<UpdateBuffersInfo>& buffersInfo,
            const std::vector<UpdateTextureInfo>& imagesInfo, 
            uint32_t size)
        {
            std::vector<VkWriteDescriptorSet> writeDescriptors(size);

            for (int i = 0; i < buffersInfo.size(); i++)
            {
                writeDescriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptors[i].pNext = nullptr;
                writeDescriptors[i].dstBinding = buffersInfo[i].DstBinding;
                writeDescriptors[i].dstArrayElement = 0;
                writeDescriptors[i].descriptorType = buffersInfo[i].DescriptorType;
                writeDescriptors[i].descriptorCount = 1;
                writeDescriptors[i].pBufferInfo = &buffersInfo[i].BufferInfo;
                writeDescriptors[i].pImageInfo = nullptr;
                writeDescriptors[i].pTexelBufferView = nullptr;
                writeDescriptors[i].dstSet = descriptorSet;
            }

            for (int i = 0; i < imagesInfo.size(); i++)
            {
                writeDescriptors[i + buffersInfo.size()].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptors[i + buffersInfo.size()].pNext = nullptr;
                writeDescriptors[i + buffersInfo.size()].dstBinding = imagesInfo[i].DstBinding;
                writeDescriptors[i + buffersInfo.size()].dstArrayElement = 0;
                writeDescriptors[i + buffersInfo.size()].descriptorType = imagesInfo[i].DescriptorType;
                writeDescriptors[i + buffersInfo.size()].descriptorCount = 1;
                writeDescriptors[i + buffersInfo.size()].pBufferInfo = nullptr;
                writeDescriptors[i + buffersInfo.size()].pImageInfo = &imagesInfo[i].ImageInfo;
                writeDescriptors[i + buffersInfo.size()].pTexelBufferView = nullptr;
                writeDescriptors[i + buffersInfo.size()].dstSet = descriptorSet;
            }

            vkUpdateDescriptorSets(device, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
        }

        void BindDescriptorSets(VkCommandBuffer commandBuffer, 
            VkPipelineBindPoint bindPoint, 
            VkPipelineLayout layout,
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