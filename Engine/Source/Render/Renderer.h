#pragma once

#include "Vulkan/Vulkan.h"

namespace psm
{
    class Renderer
    {
    public:
        Renderer();
        void Init(HINSTANCE hInstance, HWND hWnd);
        void Deinit();
    private:
        Vulkan m_Vk;
    };
}