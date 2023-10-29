#pragma once

#include <iostream>
#include <cassert>

#include "Include/vulkan/vulkan.h"

#define VK_CHECK_RESULT(vkresult) assert((vkresult) == VK_SUCCESS);