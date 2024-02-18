#pragma once

#include <vector>

//#include "Core.h"
#include "Memory.h"
#include "CommandBuffer.h"
#include "Buffer.h"

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
                             uint32_t mipMaps,
                             VkImageView* imageView);

        void CreateImageViews(VkDevice device,
                              const std::vector<VkImage> images,
                              VkFormat imageViewFormat,
                              VkImageViewType viewType,
                              VkImageAspectFlags aspectFlags,
                              std::vector<VkImageView>* imageViews);

        void ImageLayoutTransition(VkDevice device,
                                   VkCommandBuffer commandBuffer,
                                   VkImage image,
                                   VkFormat format,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout,
                                   VkPipelineStageFlags sourceStage,
                                   VkPipelineStageFlags destinationStage,
                                   VkAccessFlags sourceMask,
                                   VkAccessFlags destinationMask,
                                   VkImageAspectFlags imageAspectFlags,
                                   uint32_t mipLevels);

        void CreateImageAndView(VkDevice device,
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
                                VkFormat imageViewFormat,
                                VkImageViewType imageViewType,
                                VkImageAspectFlags imageViewAspectFlags,
                                VkImage* image,
                                VkDeviceMemory* imageMemory,
                                VkImageView* imageView);

        void LoadDataIntoImageUsingBuffer(VkDevice device,
                                          VkPhysicalDevice physicalDevice,
                                          VkDeviceSize dataToLoadSize,
                                          void* dataToLoad,
                                          VkCommandPool commandPool,
                                          VkQueue commandQueue,
                                          VkExtent3D size,
                                          uint32_t mipLevels,
                                          VkFormat imageFormatBeforeTransition,
                                          VkImageLayout imageLayoutBeforeTransition,
                                          VkFormat imageFormatAfterTransition,
                                          VkImageLayout imageLayoutAfterTransition,
                                          VkImage* dstImage);

        void GenerateMipMaps(VkPhysicalDevice physicalDevice,
                             VkCommandBuffer commandBuffer,
                             VkImage image,
                             VkFormat imageFormat,
                             int32_t texWidth,
                             int32_t texHeight,
                             int32_t mipLevels);

        void DestroyImage(VkDevice device, VkImage image);

        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews);
    }
}