#include "Model.h"

#include "RHI/Configs/PipelineConfig.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkCommandBuffer.h"
#include "RHI/Vulkan/CVkBuffer.h"
#include "RHI/Vulkan/CVkFence.h"
#include "RHI/Vulkan/CVkCommandPool.h"
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
    }

    void Model::Deinit()
    {
        /*vk::DestroyBuffer(vk::Device, m_VertexBuffer);
        vk::FreeMemory(vk::Device, m_VertexBufferMemory);*/
    }

    void Model::Init(DevicePtr device, CommandPoolPtr commandPool)
    {
        uint32_t totalVertices = MeshVertices.size();
        uint32_t totalIndices = MeshIndices.size();

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

        SBufferConfig indexBufferConfig =
        {
            .Size = sizeof(uint32_t) * totalIndices,
            .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_INDEX_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };

        SBufferConfig stagingIndexBufferConfig =
        {
            .Size = sizeof(uint32_t) * totalIndices,
            .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mIndexBuffer = device->CreateBuffer(indexBufferConfig);
        BufferPtr indexStagingBuffer = device->CreateBuffer(stagingIndexBufferConfig);

        //copy data
        //map vertex buffer
        void* pVertexData;
        uint64_t totalVerticesSize = sizeof(Vertex) * totalVertices;

        SBufferMapConfig vMapConfig =
        {
            .Size = totalVerticesSize,
            .Offset = 0,
            .pData = &pVertexData,
            .Flags = 0
        };

        vertexStagingBuffer->Map(vMapConfig);

        //map index buffer
        void* pIndexData;
        uint64_t totalIndicesSize = sizeof(uint32_t) * totalIndices;

        SBufferMapConfig iMapConfig =
        {
            .Size = totalIndicesSize,
            .Offset = 0,
            .pData = &pIndexData,
            .Flags = 0
        };

        indexStagingBuffer->Map(iMapConfig);

        memcpy(pVertexData, MeshVertices.data(), sizeof(Vertex) * MeshVertices.size());
        memcpy(pIndexData, MeshIndices.data(), sizeof(uint32_t) * MeshIndices.size());

        SBufferFlushConfig vFlushConfig =
        {
            .Size = totalVerticesSize,
            .Offset = 0,
        };

        vertexStagingBuffer->Flush(vFlushConfig);
        vertexStagingBuffer->Unmap();

        SBufferFlushConfig iFlushConfig =
        {
            .Size = totalIndicesSize,
            .Offset = 0,
        };
        indexStagingBuffer->Flush(iFlushConfig);
        indexStagingBuffer->Unmap();

        SCommandBufferConfig commandBufferConfig =
        {
            .Size = 1,
            .IsBufferLevelPrimary = true
        };

        std::vector<CommandBufferPtr> commandBuffers = device->CreateCommandBuffers(commandPool, commandBufferConfig);
        CommandBufferPtr commandBuffer = commandBuffers[0];

        SCommandBufferBeginConfig beginConfig =
        {
            .BufferIndex = 0,
            .Usage = ECommandBufferUsage::ONE_TIME_SUBMIT_BIT,
        };

        commandBuffer->Begin(beginConfig);

        device->CopyBuffer(commandBuffer, vertexBufferConfig.Size, vertexStagingBuffer, mVertexBuffer);
        device->CopyBuffer(commandBuffer, indexBufferConfig.Size ,indexStagingBuffer, mIndexBuffer);

        commandBuffer->End();

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

        SFenceWaitConfig wait =
        {
            .WaitAll = true,
            .Timeout = 100'000'000,
        };

        submitFence->Wait(wait);
        commandPool->FreeCommandBuffers({ commandBuffer });
    }
}