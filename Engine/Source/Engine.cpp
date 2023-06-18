#include "Engine.h"

namespace psm
{
    Engine* Engine::s_Instance = nullptr;;

    Engine* Engine::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Engine();
        }

        return s_Instance;
    }

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