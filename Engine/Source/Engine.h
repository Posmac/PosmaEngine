#pragma once

#include <Windows.h>

#include "Render/Renderer.h"
//#include "Core/Log.h"

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
        Engine() {};
        static Engine* s_Instance;
    public:
        void Init(HWND hWnd, HINSTANCE hInstance);
        void Dispose();
    };
}