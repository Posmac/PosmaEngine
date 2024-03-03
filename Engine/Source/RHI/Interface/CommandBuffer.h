#pragma once

#include <cstdint>

namespace psm
{
    struct CommandBufferBeginConfig;

    class ICommandBuffer
    {
    public:
        ICommandBuffer() = default;
        virtual ~ICommandBuffer() = default;

        virtual void ResetAtIndex(uint32_t index) = 0;
        virtual void BeginAtIndex(const SCommandBufferBeginConfig& config) = 0;
        virtual void EndCommandBuffer(uint32_t index) = 0;
        virtual void GetCommandBuffer(uint32_t index) = 0;
        virtual void* GetRawPointer() = 0;
    };
}