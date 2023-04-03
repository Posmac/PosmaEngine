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
        void Render();
    private:
        Vulkan m_Vk;
    };
}