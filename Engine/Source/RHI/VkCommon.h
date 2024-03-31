#pragma once

//#if defined(RHI_VULKAN)

#include "Include/vulkan/vulkan.h"
#include "Include/vulkan/vk_enum_string_helper.h"

#include "RHICommon.h"

#define RESULT VkResult

using SResourceSize = VkDeviceSize;
using SResourceExtent2D = VkExtent2D;
using SResourceExtent3D = VkExtent3D;
using SResourceOffset2D = VkOffset2D;
using SResourceOffset3D = VkOffset3D;

struct QueueFamilyIndices
{

};

#define VK_CHECK_RESULT(vkresult) {auto VK_CHECK_RESULT = vkresult; Assert(VK_CHECK_RESULT == VK_SUCCESS, std::string("Vulkan assertion failed: ") + # vkresult + " -> " + string_VkResult(VK_CHECK_RESULT));}

// XXX use it
constexpr bool enableValidationLayer =
#ifdef _DEBUG
true;
#else
false;
#endif

//#endif