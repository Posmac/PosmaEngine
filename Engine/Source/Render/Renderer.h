#pragma once

#include "Vulkan/BaseVulkan.h"

namespace psm
{
    class Renderer
    {
    //singleton realization
    public:
        Renderer(Renderer&) = delete;
        void operator=(const Renderer&) = delete;
        static Renderer* Instance();
    private:
        Renderer();
        static Renderer* s_Instance;
    //class specific
    public:
        void Init(HINSTANCE hInstance, HWND hWnd);
        void Deinit();
        void Render();
    };
}