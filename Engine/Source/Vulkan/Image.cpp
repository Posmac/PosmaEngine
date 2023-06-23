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
            createInfo.subresourceRange.levelCount = 1;
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

            for (int i = 0; i < images.size(); i++)
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

                if (result != VK_SUCCESS)
                {
                    std::cout << "Failed to create swapchain image view" << std::endl;
                }
            }
        }

        void ImageLayoutTransition(VkDevice device, 
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkImage image, 
            VkFormat format, 
            VkImageLayout oldLayout, 
            VkImageLayout newLayout)
        {
            VkCommandBuffer commandBuffer = putils::BeginSingleTimeCommandBuffer(device, commandPool);

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
                newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && 
                newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else 
            {
                throw std::invalid_argument("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(commandBuffer,
                sourceStage, 
                destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            putils::EndSingleTimeCommandBuffer(device, commandPool, commandBuffer, graphicsQueue);
        }

        void DestroyImage(VkDevice device, VkImage image)
        {
            vkDestroyImage(device, image, nullptr);
        }

        void DestroyImageViews(VkDevice device, const std::vector<VkImageView>& imageViews)
        {
            for (auto& view : imageViews)
            {
                vkDestroyImageView(device, view, nullptr);
            }
        }
    }
}