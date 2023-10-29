#pragma once

#include <Windows.h>

#include "Interface/System.h"

#include "Render/Vk.h"
#include "Render/Renderer.h"
#include "Core/Log.h"

namespace psm
{
    class Engine : public Singleton<Engine>
    {
    public:
        void Init(HWND hWnd, HINSTANCE hInstance);
        void Dispose();
    };
}