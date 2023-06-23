#include "Memory.h"

namespace psm
{
    namespace vk
    {
        uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props)
        {
            VkPhysicalDeviceMemoryProperties memProps;
            vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

            for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
            {
                if (typeFilter & (1 << i) &&
                    (memProps.memoryTypes[i].propertyFlags & props) == props)
                {
                    return i;
                }
            }
        }
    }
}