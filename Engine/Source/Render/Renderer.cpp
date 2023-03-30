#include "Renderer.h"

namespace psm
{
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
}