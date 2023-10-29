#pragma once

#include <Windows.h>

#include "../RHISelector.h"

namespace psm
{
    class CDevice;

    class CSwapchain
    {
    public:
        CSwapchain() = default;
        virtual ~CSwapchain() = default;
    public:
        virtual RHI_RESULT Init(CDevice* device, HINSTANCE hInstance, HWND hWnd) = 0;
    };
}