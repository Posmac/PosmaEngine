#pragma once

#include "Common.h"

namespace psm
{
    void LogMessage(MessageSeverity severity, const std::string& message);
    void FatalError(const std::string& message);

    template<typename T>
    static void Assert(bool condition, const T& errorMsg)
    {
        if(!condition)
        {
            FatalError(errorMsg);
        }
    }
}


#if defined(RHI_VULKAN)
#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vk_enum_string_helper.h"

using SResourceSize = VkDeviceSize;
using SResourceExtent2D = VkExtent2D;
using SResourceExtent3D = VkExtent3D;
using SResourceOffset2D = VkOffset2D;
using SResourceOffset3D = VkOffset3D;

#define VK_CHECK_RESULT(vkresult) {auto VK_CHECK_RESULT = vkresult; Assert(VK_CHECK_RESULT == VK_SUCCESS, std::string("Vulkan assertion failed: ") + # vkresult + " -> " + string_VkResult(VK_CHECK_RESULT));}

// XXX use it
constexpr bool enableValidationLayer =
#ifdef _DEBUG
true;
#else
false;
#endif

#endif