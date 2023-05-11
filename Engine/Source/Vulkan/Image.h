#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void DestroyImage(VkDevice device, VkImage image);
        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews);
    }
}