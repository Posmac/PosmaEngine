#pragma once

#include <Windows.h>
#include "../VkCommon.h"
#include "../Configs/SwapchainConfig.h"
#include "Types.h"

namespace psm
{
    class ISwapchain
    {
    public:
        ISwapchain() = default;
        virtual ~ISwapchain() = default;
    public:
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void GetNextImage(uint32_t* index) = 0;
        virtual TexturePtr& ImageAtIndex(uint32_t index) = 0;
        virtual void Present(const SwapchainPresentConfig& config) = 0;
        virtual void SetVsyncMode(bool enabled) = 0;
    };
}