#include "Image.h"

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
            VkDeviceMemory* imageMemory)
        {
            VkImageCreateInfo icInfo{};
            icInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            icInfo.pNext = nullptr;

            icInfo.imageType = imageType;
            icInfo.extent = extent;
            icInfo.mipLevels = mipLevels;
            icInfo.arrayLayers = arrayLevels;
            icInfo.format = imageFormat;
            icInfo.tiling = tiling;
            icInfo.initialLayout = initialLayout;
            icInfo.usage = usage;
            icInfo.sharingMode = sharingMode;
            icInfo.samples = samples;
            icInfo.flags = flags;

            VkResult res = vkCreateImage(device, &icInfo, nullptr, image);
            VK_CHECK_RESULT(res);

            VkMemoryRequirements memReq{};
            vkGetImageMemoryRequirements(device, *image, &memReq);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.pNext = nullptr;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex = vk::FindMemoryType(physicalDevice, memReq.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            res = vkAllocateMemory(device, &allocInfo, nullptr, imageMemory);
            VK_CHECK_RESULT(res);

            vkBindImageMemory(device, *image, *imageMemory, 0);
            VK_CHECK_RESULT(res);
        }

        void CreateImageView(VkDevice device,
            VkImage image,
            VkFormat imageViewFormat,
            VkImageViewType viewType,
            VkImageAspectFlags aspectFlags,
            uint32_t mipMaps,
            VkImageView* imageView)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.image = image;
            createInfo.format = imageViewFormat;
            createInfo.viewType = viewType;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            createInfo.subresourceRange.aspectMask = aspectFlags;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.levelCount = mipMaps;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(device, &createInfo,
                nullptr, imageView);
            VK_CHECK_RESULT(result);
        }

        void CreateImageViews(VkDevice device,
            const std::vector<VkImage> images,
            VkFormat imageViewFormat,
            VkImageViewType viewType,
            VkImageAspectFlags aspectFlags,
            std::vector<VkImageView>* imageViews)
        {
            imageViews->resize(images.size());

            for(int i = 0; i < images.size(); i++)
            {
                VkImageViewCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.pNext = nullptr;
                createInfo.flags = 0;
                createInfo.image = images[i];
                createInfo.format = imageViewFormat;
                createInfo.viewType = viewType;
                createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
                createInfo.subresourceRange.aspectMask = aspectFlags;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.layerCount = 1;

                VkResult result = vkCreateImageView(device, &createInfo,
                    nullptr, &(*imageViews)[i]);
                VK_CHECK_RESULT(result);

                if(result != VK_SUCCESS)
                {
                    std::cout << "Failed to create swapchain image view" << std::endl;
                }
            }
        }

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
                                   uint32_t mipLevels)
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = imageAspectFlags;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = sourceMask;
            barrier.dstAccessMask = destinationMask;

            vkCmdPipelineBarrier(commandBuffer,
                sourceStage,
                destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }

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
            VkImageView* imageView)
        {
            CreateImage(device, physicalDevice, extent, mipLevels, arrayLevels, imageType, imageFormat,
                tiling, initialLayout, usage, sharingMode, samples, flags, image, imageMemory);

            CreateImageView(device, *image, imageViewFormat, imageViewType,
                imageViewAspectFlags, mipLevels, imageView);
        }

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
            VkImage* dstImage)
        {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;

            CreateBuffer(device, physicalDevice, dataToLoadSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &stagingBuffer, &stagingBufferMemory);

            void* data;
            VkResult result = vkMapMemory(device, stagingBufferMemory, 0, dataToLoadSize, 0, &data);
            VK_CHECK_RESULT(result);

            memcpy(data, dataToLoad, static_cast<size_t>(dataToLoadSize));
            vkUnmapMemory(device, stagingBufferMemory);

            VkCommandBuffer commandBuffer = putils::BeginSingleTimeCommandBuffer(device, commandPool);

            vk::ImageLayoutTransition(device, commandBuffer, *dstImage,
                                      imageFormatBeforeTransition,
                                      imageLayoutBeforeTransition,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      0,
                                      VK_ACCESS_TRANSFER_WRITE_BIT,
                                      VK_IMAGE_ASPECT_COLOR_BIT,
                                      1);

            vk::CopyBufferToImage(device, commandBuffer, commandQueue, stagingBuffer, *dstImage, size);

            if(mipLevels > 1)
            {
                GenerateMipMaps(physicalDevice, commandBuffer, *dstImage,
                     VK_FORMAT_R8G8B8A8_SRGB, size.width, size.height, mipLevels);
            }
            else
            {
                vk::ImageLayoutTransition(device, commandBuffer, *dstImage,
                                      imageFormatBeforeTransition,
                                      imageLayoutBeforeTransition,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT,
                                      VK_ACCESS_SHADER_READ_BIT,
                                      VK_IMAGE_ASPECT_COLOR_BIT,
                                      1);
            }

            putils::EndSingleTimeCommandBuffer(device, commandPool, commandBuffer, commandQueue);

            vk::FreeMemory(device, stagingBufferMemory);
            vk::DestroyBuffer(device, stagingBuffer);
        }

        void GenerateMipMaps(VkPhysicalDevice physicalDevice,
                             VkCommandBuffer commandBuffer,
                             VkImage image,
                             VkFormat imageFormat,
                             int32_t texWidth,
                             int32_t texHeight,
                             int32_t mipLevels)
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

            if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            {
                throw std::runtime_error("texture image format does not support linear blitting!");
            }

            int32_t mipWidth = texWidth;
            int32_t mipHeight = texHeight;

            for(uint32_t i = 1; i < mipLevels; i++)
            {
                VkImageMemoryBarrier dstBaseBarrier{};
                dstBaseBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                dstBaseBarrier.image = image;
                dstBaseBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                dstBaseBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                dstBaseBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                dstBaseBarrier.subresourceRange.baseArrayLayer = 0;
                dstBaseBarrier.subresourceRange.layerCount = 1;
                dstBaseBarrier.subresourceRange.levelCount = 1;
                dstBaseBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                dstBaseBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                dstBaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstBaseBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                dstBaseBarrier.subresourceRange.baseMipLevel = i;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &dstBaseBarrier);

                VkImageMemoryBarrier sourceMipBarrier{};
                sourceMipBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                sourceMipBarrier.image = image;
                sourceMipBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                sourceMipBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                sourceMipBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                sourceMipBarrier.subresourceRange.baseArrayLayer = 0;
                sourceMipBarrier.subresourceRange.layerCount = 1;
                sourceMipBarrier.subresourceRange.levelCount = 1;
                sourceMipBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                sourceMipBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                sourceMipBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                sourceMipBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                sourceMipBarrier.subresourceRange.baseMipLevel = i - 1;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &sourceMipBarrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(commandBuffer,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                //after blit restore state
                /*imageMemoryBarrier(cmd, image, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   i, 1);*/

                dstBaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                dstBaseBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                dstBaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstBaseBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &dstBaseBarrier);


                /*imageMemoryBarrier(cmd, image, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                  i - 1, 1);*/

                sourceMipBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                sourceMipBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                sourceMipBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                sourceMipBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &sourceMipBarrier);

                if(mipWidth > 1)
                    mipWidth /= 2;
                if(mipHeight > 1)
                    mipHeight /= 2;
            }
        }

        void DestroyImage(VkDevice device, VkImage image)
        {
            vkDestroyImage(device, image, nullptr);
        }

        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews)
        {
            for(auto& view : imageViews)
            {
                vkDestroyImageView(device, view, nullptr);
            }
        }
    }
}