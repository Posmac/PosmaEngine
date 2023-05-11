#pragma once

#include <iostream>

#define VK_CHECK_RESULT(vkresult) if( (vkresult) == 0) {return;} \
                                  fprintf(stdout, "[Vulkan Error]: VkResult = %d\n", (vkresult) ); \
                                  if( (vkresult) < 0) { abort; }