#pragma once

#include <cstdint>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/RenderPassFormats.h"

namespace psm
{
    enum class EQueueType : uint32_t
    {
        GRAHPICS,
        COMPUTE,
        TRANSFER,
        GENERAL
    };

    enum class ECommandBufferUsage : uint32_t
    {
        NONE = 0x00000000,
        ONE_TIME_SUBMIT_BIT = 0x00000001,
        RENDER_PASS_CONTINUE_BIT = 0x00000002,
        SIMULTANEOUS_USE_BIT = 0x00000004,
    };

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