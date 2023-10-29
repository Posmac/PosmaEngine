#pragma once

#include <Windows.h>

#include "../RHISelector.h"
#include "../Context/ImageContext.h"

namespace psm
{
    class CSwapchain;

    class CDevice
    {
    public:
        CDevice() = default;
        virtual ~CDevice() = default;
    public:
        virtual RHI_RESULT InitInternals() = 0;
        virtual RHI_RESULT InitDevice(CSwapchain* pSwapchain) = 0;
        virtual RHI_RESULT CreateImage() = 0;
    };
}