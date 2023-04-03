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
        m_Vk.Init(hInstance, hWnd);
    }

    void Renderer::Deinit()
    {
        m_Vk.Deinit();
    }

    void Renderer::Render()
    {
        m_Vk.Render();
    }
}