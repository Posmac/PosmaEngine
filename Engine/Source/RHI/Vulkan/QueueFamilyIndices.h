#pragma once

#include <optional>

#include "Include/vulkan/vulkan.h"

namespace psm
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;
        VkQueue GraphicsQueue;
        VkQueue PresentQueue;

        bool IsComplete()
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }

        bool IdenticalQueues()
        {
            return GraphicsFamily.value() == PresentFamily.value();
        }
    };
}