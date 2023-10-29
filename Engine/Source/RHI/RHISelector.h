#pragma once

#define RHI_VULKAN

#if defined(RHI_VULKAN)
    #include "Include/vulkan/vulkan.h"
    #define RHI_RESULT VkResult

    using SResourceSize = VkDeviceSize;
#endif