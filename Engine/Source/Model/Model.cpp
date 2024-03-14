#include "Model.h"

#include "RHI/Configs/PipelineConfig.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkCommandBuffer.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

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

    void Model::BindBuffers(DevicePtr device, CommandBufferPtr commandBuffer)
    {
        SVertexBufferBindConfig vertexBindConfig =
        {
            .FirstSlot = 0,
            .BindingCount = 1,
            .Offsets = {0},
            .Buffers = &mVertexBuffer,
        };

        device->BindVertexBuffers(commandBuffer, vertexBindConfig);

        SIndexBufferBindConfig indexBindConfig =
        {
            .Type = EIndexType::UINT32,
            .Buffer = mIndexBuffer
        };

        device->BindIndexBuffer(commandBuffer, indexBindConfig);

        /*VkDeviceSize offset = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer, &offset);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);*/
    }

    void Model::Deinit()
    {
        /*vk::DestroyBuffer(vk::Device, m_VertexBuffer);
        vk::FreeMemory(vk::Device, m_VertexBufferMemory);*/
    }

    void Model::Init(DevicePtr device, CommandPoolPtr commandPool)
    {
        uint32_t totalVertices = 0;
        uint32_t totalIndices = 0;

        for (auto& mesh : Meshes)
        {
            totalVertices += mesh.MeshVertices.size();
            totalIndices += mesh.MeshIndices.size();
        }

        SBufferConfig vertexBufferConfig =
        {
            .Size = sizeof(Vertex) * totalVertices,
            .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_VERTEX_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };

        SBufferConfig stagingVertexBufferConfig =
        {
            .Size = sizeof(Vertex) * totalVertices,
            .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mVertexBuffer = device->CreateBuffer(vertexBufferConfig);
        BufferPtr vertexStagingBuffer = device->CreateBuffer(stagingVertexBufferConfig);

        ////vertex buffer
        //VkDeviceSize vertexBufferSize = sizeof(Vertex) * totalVertices;
        //VkBuffer vertexStagingBuffer;
        //VkDeviceMemory vertexStagingBufferMemory;

        //vk::CreateBuffer(vk::Device, vk::PhysicalDevice, vertexBufferSize,
        //    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        //    &vertexStagingBuffer, &vertexStagingBufferMemory);

        //vk::CreateBuffer(vk::Device, vk::PhysicalDevice, vertexBufferSize,
        //    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        //    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        //    &m_VertexBuffer, &m_VertexBufferMemory);

        //index buffer

        SBufferConfig indexBufferConfig =
        {
            .Size = sizeof(Vertex) * totalVertices,
            .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_INDEX_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };

        SBufferConfig stagingIndexBufferConfig =
        {
            .Size = sizeof(Vertex) * totalVertices,
            .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mIndexBuffer = device->CreateBuffer(vertexBufferConfig);
        BufferPtr indexStagingBuffer = device->CreateBuffer(stagingVertexBufferConfig);

       /* VkDeviceSize indexBufferSize = sizeof(uint32_t) * totalIndices;
        VkBuffer indexStagingBuffer;
        VkDeviceMemory indexStagingBufferMemory;

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &indexStagingBuffer, &indexStagingBufferMemory);

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_IndexBuffer, &m_IndexBufferMemory);*/

        //copy data
        uint64_t vertexOffset = 0;
        uint64_t indexOffset = 0;

        //VkDeviceSize vertexOffset = 0;
        //VkDeviceSize indexOffset = 0;

        for (auto& mesh : Meshes)
        {
            void* vertexData;
            uint64_t currentVerticesSize = sizeof(Vertex) * mesh.MeshVertices.size();

            SBufferMapConfig vMapConfig =
            {
                .Size = currentVerticesSize,
                .Offset = vertexOffset,
                .pData = &vertexData,
                .Flags = 0
            };

            vertexStagingBuffer->Map(vMapConfig);

            memcpy(vertexData, mesh.MeshVertices.data(), currentVerticesSize);

            SBufferFlushConfig vFlushConfig =
            {
                .Size = currentVerticesSize,
                .Offset = vertexOffset,
            };

            vertexStagingBuffer->Flush(vFlushConfig);
            vertexStagingBuffer->Unmap();

            vertexOffset += currentVerticesSize;

            void* indexData;
            uint64_t currentIndicesSize = sizeof(uint32_t) * mesh.MeshIndices.size();

            SBufferMapConfig iMapConfig =
            {
                .Size = currentIndicesSize,
                .Offset = indexOffset,
                .pData = &indexData,
                .Flags = 0
            };

            indexStagingBuffer->Map(iMapConfig);
           /* vkMapMemory(vk::Device, indexStagingBufferMemory, indexOffset,
                currentIndicesSize, 0, &indexData);*/
            memcpy(indexData, mesh.MeshIndices.data(), currentIndicesSize);

            SBufferFlushConfig iFlushConfig =
            {
                .Size = currentIndicesSize,
                .Offset = indexOffset,
            };
            indexStagingBuffer->Flush(iFlushConfig);

            /*range.memory = indexStagingBufferMemory;
            range.offset = indexOffset;
            range.size = currentIndicesSize;*/

            //vkFlushMappedMemoryRanges(vk::Device, 1, &range);
            
            indexStagingBuffer->Unmap();

            //vkUnmapMemory(vk::Device, indexStagingBufferMemory);
            indexOffset += currentIndicesSize;
        };

        SCommandBufferConfig commandBufferConfig =
        {
            .Size = 1,
            .IsBufferLevelPrimary = true
        };

        CommandBufferPtr commandBuffer = device->CreateCommandBuffers(commandPool, commandBufferConfig);

        SCommandBufferBeginConfig beginConfig =
        {
            .BufferIndex = 0,
            .Usage = ECommandBufferUsage::ONE_TIME_SUBMIT_BIT,
        };

        commandBuffer->BeginAtIndex(beginConfig);

        device->CopyBuffer(commandBuffer, vertexBufferConfig.Size, vertexStagingBuffer, mVertexBuffer);
        device->CopyBuffer(commandBuffer, indexBufferConfig.Size ,indexStagingBuffer, mIndexBuffer);

        commandBuffer->EndCommandBuffer(0);

        SFenceConfig fenceConfig =
        {
            .Signaled = false
        };

        FencePtr submitFence = device->CreateFence(fenceConfig);

        SSubmitConfig submitConfig =
        {
            .Queue = device->GetDeviceData().vkData.GraphicsQueue, //not sure if Queue should be abstracted to CVk(IQueue)
            .SubmitCount = 1,
            .WaitStageFlags = EPipelineStageFlags::NONE,
            .WaitSemaphoresCount = 0,
            .pWaitSemaphores = nullptr,
            .CommandBuffersCount = 1,
            .pCommandBuffers = &commandBuffer,
            .SignalSemaphoresCount = 0,
            .pSignalSemaphores = nullptr,
            .Fence = submitFence,
        };

        device->Submit(submitConfig);

        /*VkCommandBuffer commandBuffer = putils::BeginSingleTimeCommandBuffer(vk::Device, commandPool);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            vertexStagingBuffer, m_VertexBuffer, vertexBufferSize);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            indexStagingBuffer, m_IndexBuffer, indexBufferSize);

        putils::EndSingleTimeCommandBuffer(vk::Device, commandPool, commandBuffer, vk::Queues.GraphicsQueue);*/

        /*vk::DestroyBuffer(vk::Device, vertexStagingBuffer);
        vk::FreeMemory(vk::Device, vertexStagingBufferMemory);

        vk::DestroyBuffer(vk::Device, indexStagingBuffer);
        vk::FreeMemory(vk::Device, indexStagingBufferMemory);*/
    }
}