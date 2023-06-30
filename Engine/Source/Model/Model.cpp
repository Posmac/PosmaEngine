#include "Model.h"

namespace psm
{
    Model::Model()
    {
        Meshes = {};
    }

    Model::Model(std::vector<Mesh>& meshes)
    {
        this->Meshes = std::move(meshes);
    }

    void Model::BindBuffers(VkCommandBuffer commandBuffer) const
    {
        VkDeviceSize offset = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer, &offset);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void Model::Deinit()
    {
        vk::DestroyBuffer(vk::Device, m_VertexBuffer);
        vk::FreeMemory(vk::Device, m_VertexBufferMemory);
    }

    void Model::Init(VkCommandPool commandPool)
    {
        uint32_t totalVertices = 0;
        uint32_t totalIndices = 0;

        for (auto& mesh : Meshes)
        {
            totalVertices += mesh.MeshVertices.size();
            totalIndices += mesh.MeshIndices.size();
        }

        //vertex buffer
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * totalVertices;
        VkBuffer vertexStagingBuffer;
        VkDeviceMemory vertexStagingBufferMemory;

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, vertexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &vertexStagingBuffer, &vertexStagingBufferMemory);

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, vertexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_VertexBuffer, &m_VertexBufferMemory);

        //index buffer
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * totalIndices;
        VkBuffer indexStagingBuffer;
        VkDeviceMemory indexStagingBufferMemory;

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &indexStagingBuffer, &indexStagingBufferMemory);

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_IndexBuffer, &m_IndexBufferMemory);

        //copy data
        VkDeviceSize vertexOffset = 0;
        VkDeviceSize indexOffset = 0;

        for (auto& mesh : Meshes)
        {
            void* vertexData;
            VkDeviceSize currentVerticesSize = sizeof(Vertex) * mesh.MeshVertices.size();
            vkMapMemory(vk::Device, vertexStagingBufferMemory, vertexOffset,
                currentVerticesSize, 0, &vertexData);
            memcpy(vertexData, mesh.MeshVertices.data(), currentVerticesSize);

            VkMappedMemoryRange range{};
            range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range.pNext = nullptr;
            range.memory = vertexStagingBufferMemory;
            range.offset = vertexOffset;
            range.size = currentVerticesSize;

            vkFlushMappedMemoryRanges(vk::Device, 1, &range);

            vkUnmapMemory(vk::Device, vertexStagingBufferMemory);
            vertexOffset += currentVerticesSize;

            void* indexData;
            VkDeviceSize currentIndicesSize = sizeof(uint32_t) * mesh.MeshIndices.size();
            vkMapMemory(vk::Device, indexStagingBufferMemory, indexOffset,
                currentIndicesSize, 0, &indexData);
            memcpy(indexData, mesh.MeshIndices.data(), currentIndicesSize);

            range.memory = indexStagingBufferMemory;
            range.offset = indexOffset;
            range.size = currentIndicesSize;

            vkFlushMappedMemoryRanges(vk::Device, 1, &range);

            vkUnmapMemory(vk::Device, indexStagingBufferMemory);
            indexOffset += currentIndicesSize;
        };

        VkCommandBuffer commandBuffer = putils::BeginSingleTimeCommandBuffer(vk::Device, commandPool);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            vertexStagingBuffer, m_VertexBuffer, vertexBufferSize);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            indexStagingBuffer, m_IndexBuffer, indexBufferSize);

        putils::EndSingleTimeCommandBuffer(vk::Device, commandPool, commandBuffer, vk::Queues.GraphicsQueue);

        vk::DestroyBuffer(vk::Device, vertexStagingBuffer);
        vk::FreeMemory(vk::Device, vertexStagingBufferMemory);

        vk::DestroyBuffer(vk::Device, indexStagingBuffer);
        vk::FreeMemory(vk::Device, indexStagingBufferMemory);
    }
}