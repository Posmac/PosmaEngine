#pragma once

#include <cstdint>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/RenderPassFormats.h"
#include "RHI/Enums/CommandBuffer.h"

namespace psm
{
    struct SCommandBufferBeginConfig
    {
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
        void* Queue;
        uint32_t SubmitCount;
        std::vector<EPipelineStageFlags> WaitStageFlags;
        std::vector<SemaphorePtr> WaitSemaphores;
        std::vector <CommandBufferPtr> CommandBuffers;
        std::vector <SemaphorePtr> SignalSemaphores;
        FencePtr Fence;
    };
}