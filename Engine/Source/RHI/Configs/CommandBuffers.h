#pragma once

#include <cstdint>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/RenderPassFormats.h"
#include "RHI/Enums/CommandBuffer.h"

namespace psm
{
    struct SCommandBufferBeginConfig
    {
        uint32_t BufferIndex;
        ECommandBufferUsage Usage;
    };

    struct SCommandPoolConfig
    {
        uint32_t QueueFamilyIndex;
        EQueueType QueueType;
    };

    struct SCommandBufferConfig
    {
        uint32_t Size;
        bool IsBufferLevelPrimary;
    };

    struct SSubmitConfig
    {
        void* Queue; //not sure if Queue should be abstracted to CVk(IQueue)
        uint32_t SubmitCount;
        EPipelineStageFlags WaitStageFlags;
        uint32_t WaitSemaphoresCount;
        SemaphorePtr* pWaitSemaphores;
        uint32_t CommandBuffersCount;
        CommandBufferPtr* pCommandBuffers;
        uint32_t SignalSemaphoresCount;
        SemaphorePtr* pSignalSemaphores;
        FencePtr Fence;
    };
}