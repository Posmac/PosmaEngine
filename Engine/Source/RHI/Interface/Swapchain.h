#pragma once

#include <Windows.h>
#include "../VkCommon.h"
#include "../Configs/SwapchainConfig.h"
#include "Types.h"
#include "RHI/Enums/ImageFormats.h"

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
        virtual void Present(const SSwapchainPresentConfig& config) = 0;
        virtual void SetVsyncMode(bool enabled) = 0;
        virtual uint32_t GetImagesCount() = 0;
        virtual EImageFormat GetSwapchainImageFormat() = 0;
        virtual SResourceExtent3D GetSwapchainSize() = 0;
    };
}