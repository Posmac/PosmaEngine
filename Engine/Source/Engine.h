#pragma once

#include <Windows.h>

#include "Render/Vk.h"
#include "Render/Renderer.h"
#include "Core/Log.h"

namespace psm
{
    class Engine
    {
        //singleton realization
    public:
        Engine(Engine&) = delete;
        void operator=(const Engine&) = delete;
        static Engine* Instance();
    private:
        Engine() = default;
        static Engine* s_Instance;
    //class specific
    public:
        void Init(HWND hWnd, HINSTANCE hInstance);
        void Dispose();
    };
}