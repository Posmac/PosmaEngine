#include "Image.h"

namespace psm
{
    namespace vk
    {
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