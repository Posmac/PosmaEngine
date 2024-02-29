#include "CVkImage.h"

#include "CVkDevice.h"

#include "TypeConvertor.h"

#include "RenderBackend/Image.h"

namespace psm
{
    CVkImage::CVkImage(DevicePtr device, const SImageConfig& config)
    {
        mImageConfig = config;

        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);
        VkPhysicalDevice gpu = reinterpret_cast<VkPhysicalDevice>(device->GetDeviceData().vkData.PhysicalDevice);

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

        VkResult result = vkCreateImageView(mDeviceInternal, &imageViewCreateInfo, nullptr, &mImageView);
        VK_CHECK_RESULT(result);
    }

    CVkImage::~CVkImage()
    {

    }

    EImageType CVkImage::GetImageType() const
    {
        return mImageConfig.Type;
    }

    EImageFormat CVkImage::GetImageFormat() const
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
}

