#include "Renderer.h"

namespace psm
{
    Renderer* Renderer::s_Instance = nullptr;;

    Renderer* Renderer::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Renderer();
        }

        return s_Instance;
    }

    Renderer::Renderer()
    {
    }

    void Renderer::Init(HINSTANCE hInstance, HWND hWnd)
    {
        Vulkan::Instance()->Init(hInstance, hWnd);
    }

    void Renderer::Deinit()
    {
        Vulkan::Instance()->Deinit();
    }

    void Renderer::Render()
    {
        Vulkan::Instance()->Render();
    }
}