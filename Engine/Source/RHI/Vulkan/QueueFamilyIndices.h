#pragma once

#include <optional>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsAndComputeFamily;
        std::optional<uint32_t> PresentFamily;
        VkQueue GraphicsQueue;
        VkQueue PresentQueue;
        VkQueue ComputeQueue;

        bool IsComplete()
        {
            return GraphicsAndComputeFamily.has_value() && PresentFamily.has_value();
        }

        bool IdenticalQueues()
        {
            return GraphicsAndComputeFamily.value() == PresentFamily.value();
        }
    };
}