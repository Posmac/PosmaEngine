#include "Image.h"

namespace psm
{
    namespace vk
    {
        void CreateImageView(VkDevice device, VkImage image, VkFormat imageViewFormat, 
            VkImageViewType viewType, VkImageAspectFlags aspectFlags,
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

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create swapchain image view" << std::endl;
            }
        }

        void CreateImageViews(VkDevice device, const std::vector<VkImage> images, 
            VkFormat imageViewFormat, VkImageViewType viewType, 
            VkImageAspectFlags aspectFlags, std::vector<VkImageView>* imageViews)
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

                if (result != VK_SUCCESS)
                {
                    std::cout << "Failed to create swapchain image view" << std::endl;
                }
            }
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