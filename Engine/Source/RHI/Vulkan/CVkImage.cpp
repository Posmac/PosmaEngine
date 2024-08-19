#include "CVkImage.h"

#include "CVkDevice.h"

#include "TypeConvertor.h"

namespace psm
{
    CVkImage::CVkImage(const DevicePtr& device, const SImageConfig& config)
    {
        mImageConfig = config;

        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        assert(mDeviceInternal != nullptr);
        VkPhysicalDevice gpu = reinterpret_cast<VkPhysicalDevice>(device->GetDeviceData().vkData.PhysicalDevice);
        assert(gpu != nullptr);

        mImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        mImageInfo.pNext = nullptr;

        mImageInfo.imageType = ToVulkan(config.Type);
        mImageInfo.extent = config.ImageSize;
        mImageInfo.mipLevels = config.MipLevels;
        mImageInfo.arrayLayers = config.ArrayLevels;
        mImageInfo.format = ToVulkan(config.Format);
        mImageInfo.tiling = ToVulkan(config.Tiling);
        mImageInfo.initialLayout = ToVulkan(config.InitialLayout);
        mImageInfo.usage = ToVulkan(config.Usage);
        mImageInfo.sharingMode = ToVulkan(config.SharingMode);
        mImageInfo.samples = ToVulkan(config.SamplesCount);
        mImageInfo.flags = ToVulkan(config.Flags);
        mImageInfo.pQueueFamilyIndices = nullptr;

        VkResult result = vkCreateImage(mDeviceInternal, &mImageInfo, nullptr, &mImage);
        VK_CHECK_RESULT(result);

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(mDeviceInternal, mImage, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(gpu, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(mDeviceInternal, &allocInfo, nullptr, &mImageMemory);
        VK_CHECK_RESULT(result);

        vkBindImageMemory(mDeviceInternal, mImage, mImageMemory, 0);
        VK_CHECK_RESULT(result);

        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.format = ToVulkan(config.ViewFormat);
        imageViewCreateInfo.viewType = ToVulkan(config.ViewType);
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask = ToVulkan(config.ViewAspect);
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.levelCount = config.MipLevels;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(mDeviceInternal, &imageViewCreateInfo, nullptr, &mImageView);
        VK_CHECK_RESULT(result);
    }

    CVkImage::~CVkImage()
    {
        vkDestroyImage(mDeviceInternal, mImage, nullptr);
        vkDestroyImageView(mDeviceInternal, mImageView, nullptr);
        vkFreeMemory(mDeviceInternal, mImageMemory, nullptr);
    }

    EImageType CVkImage::GetImageType() const
    {
        return mImageConfig.Type;
    }

    EFormat CVkImage::GetImageFormat() const
    {
        return mImageConfig.Format;
    }

    SResourceExtent3D CVkImage::GetImageSize() const
    {
        return mImageConfig.ImageSize;
    }

    uint32_t CVkImage::GetImageWidth() const
    {
        return mImageConfig.ImageSize.width;
    }

    uint32_t CVkImage::GetImageHeight() const
    {
        return mImageConfig.ImageSize.height;
    }

    uint32_t CVkImage::GetImageDepth() const
    {
        return mImageConfig.ImageSize.depth;
    }

    uint32_t CVkImage::GetImageMips() const
    {
        return mImageConfig.MipLevels;
    }

    uint32_t CVkImage::GetArrayLayers() const
    {
        return mImageConfig.ArrayLevels;
    }

    uint32_t CVkImage::GetSampleCount() const
    {
        return static_cast<uint32_t>(mImageConfig.SamplesCount);
    }

    uint32_t CVkImage::SubresourceCount() const
    {
        return 1;
    }

    void* CVkImage::Raw()
    {
        return mImage;
    }

    void* CVkImage::Raw() const
    {
        return mImage;
    }

    void* CVkImage::RawImageView()
    {
        return mImageView;
    }

    void* CVkImage::RawImageView() const
    {
        return mImageView;
    }

    uint32_t CVkImage::FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props)
    {
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

        for(uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if(typeFilter & (1 << i) &&
                (memProps.memoryTypes[i].propertyFlags & props) == props)
            {
                return i;
            }
        }
    }

    //void CVkImage::GenerateMipMaps(void* pMipMapsData)
    //{
    //    /*VkFormatProperties formatProperties;
    //    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    //    if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    //    {
    //        throw std::runtime_error("texture image format does not support linear blitting!");
    //    }*/

    //    int32_t mipWidth = mImageConfig.ImageSize.width;
    //    int32_t mipHeight = mImageConfig.ImageSize.height;

    //    for(uint32_t i = 1; i < mipLevels; i++)
    //    {
    //        VkImageMemoryBarrier dstBaseBarrier{};
    //        dstBaseBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //        dstBaseBarrier.image = image;
    //        dstBaseBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //        dstBaseBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //        dstBaseBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //        dstBaseBarrier.subresourceRange.baseArrayLayer = 0;
    //        dstBaseBarrier.subresourceRange.layerCount = 1;
    //        dstBaseBarrier.subresourceRange.levelCount = 1;
    //        dstBaseBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //        dstBaseBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //        dstBaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //        dstBaseBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    //        dstBaseBarrier.subresourceRange.baseMipLevel = i;

    //        vkCmdPipelineBarrier(commandBuffer,
    //            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
    //            0, nullptr,
    //            0, nullptr,
    //            1, &dstBaseBarrier);

    //        VkImageMemoryBarrier sourceMipBarrier{};
    //        sourceMipBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //        sourceMipBarrier.image = image;
    //        sourceMipBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //        sourceMipBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //        sourceMipBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //        sourceMipBarrier.subresourceRange.baseArrayLayer = 0;
    //        sourceMipBarrier.subresourceRange.layerCount = 1;
    //        sourceMipBarrier.subresourceRange.levelCount = 1;
    //        sourceMipBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //        sourceMipBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //        sourceMipBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //        sourceMipBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    //        sourceMipBarrier.subresourceRange.baseMipLevel = i - 1;

    //        vkCmdPipelineBarrier(commandBuffer,
    //            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
    //            0, nullptr,
    //            0, nullptr,
    //            1, &sourceMipBarrier);

    //        VkImageBlit blit{};
    //        blit.srcOffsets[0] = { 0, 0, 0 };
    //        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
    //        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //        blit.srcSubresource.mipLevel = i - 1;
    //        blit.srcSubresource.baseArrayLayer = 0;
    //        blit.srcSubresource.layerCount = 1;
    //        blit.dstOffsets[0] = { 0, 0, 0 };
    //        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
    //        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //        blit.dstSubresource.mipLevel = i;
    //        blit.dstSubresource.baseArrayLayer = 0;
    //        blit.dstSubresource.layerCount = 1;

    //        vkCmdBlitImage(commandBuffer,
    //            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    //            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //            1, &blit,
    //            VK_FILTER_LINEAR);

    //        //after blit restore state
    //        /*imageMemoryBarrier(cmd, image, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
    //                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    //                           i, 1);*/

    //        dstBaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //        dstBaseBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //        dstBaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //        dstBaseBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    //        vkCmdPipelineBarrier(commandBuffer,
    //            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
    //            0, nullptr,
    //            0, nullptr,
    //            1, &dstBaseBarrier);


    //        /*imageMemoryBarrier(cmd, image, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
    //          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    //          i - 1, 1);*/

    //        sourceMipBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //        sourceMipBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //        sourceMipBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    //        sourceMipBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    //        vkCmdPipelineBarrier(commandBuffer,
    //            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
    //            0, nullptr,
    //            0, nullptr,
    //            1, &sourceMipBarrier);

    //        if(mipWidth > 1)
    //            mipWidth /= 2;
    //        if(mipHeight > 1)
    //            mipHeight /= 2;
    //    }
    //}
}

