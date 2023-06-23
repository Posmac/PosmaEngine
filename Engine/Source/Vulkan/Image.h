#pragma once

#include <vector>

#include "Core.h"
#include "Include/vulkan/vulkan.h"
#include "Memory.h"
#include "CommandBuffer.h"

namespace psm
{
    namespace vk
    {
        void CreateImage(VkDevice device, 
            VkPhysicalDevice physicalDevice, 
            VkExtent3D extent,
            int mipLevels, 
            int arrayLevels, 
            VkImageType imageType, 
            VkFormat imageFormat, 
            VkImageTiling tiling,
            VkImageLayout initialLayout, 
            VkImageUsageFlags usage, 
            VkSharingMode sharingMode,
            VkSampleCountFlagBits samples, 
            VkImageCreateFlags flags, 
            VkImage* image,
            VkDeviceMemory* imageMemory);

        void CreateImageView(VkDevice device, 
            VkImage image, 
            VkFormat imageViewFormat,
            VkImageViewType viewType, 
            VkImageAspectFlags aspectFlags,
            VkImageView* imageView);

        void CreateImageViews(VkDevice device, 
            const std::vector<VkImage> images, 
            VkFormat imageViewFormat,
            VkImageViewType viewType, 
            VkImageAspectFlags aspectFlags,
            std::vector<VkImageView>* imageViews);

        void ImageLayoutTransition(VkDevice device,
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkImage image,
            VkFormat format,
            VkImageLayout oldLayout,
            VkImageLayout newLayout);

        void DestroyImage(VkDevice device, VkImage image);

        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews);
    }
}