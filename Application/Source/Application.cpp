#include "Application.h"

namespace psm
{
    void Application::Init()
    {
        LoadModelData();
    }

    void Application::Update()
    {
        time += 0.01f;
        Renderer::Instance()->Render(time);
    }

    void Application::Deinit()
    {

    }

    void Application::LoadModelData()
    {
        Renderer::Instance()->LoadDataIntoBuffer();
    }
}