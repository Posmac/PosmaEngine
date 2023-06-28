#include "Application.h"

namespace psm
{
    void Application::Init()
    {
       
    }

    void Application::Update()
    {
        time += 0.01f;
        Renderer::Instance()->Render(time);
    }

    void Application::Deinit()
    {

    }
}