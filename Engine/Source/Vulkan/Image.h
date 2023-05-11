#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"

namespace psm
{
    namespace vk
    {
        void CreateImageView(VkDevice device, VkImage image, VkFormat imageViewFormat,
            VkImageViewType viewType, VkImageAspectFlags aspectFlags,
            VkImageView* imageView);
        void CreateImageViews(VkDevice device, const std::vector<VkImage> images, VkFormat imageViewFormat,
            VkImageViewType viewType, VkImageAspectFlags aspectFlags,
            std::vector<VkImageView>* imageViews);

        void DestroyImage(VkDevice device, VkImage image);
        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews);
    }
}