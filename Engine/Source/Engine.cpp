#include "Engine.h"

namespace psm
{
    void Engine::Init(HWND hWnd, HINSTANCE hInstance)
    {
        vk::Vk::GetInstance()->Init(hInstance, hWnd);
        Renderer::Instance()->Init(hInstance, hWnd);
        Log::Init();
    }

    void Engine::Dispose()
    {
        Renderer::Instance()->Deinit();
    }
}