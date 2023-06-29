#include "Application.h"

namespace psm
{
    void Application::Init()
    {
        m_Camera = Camera(60.0f, 1.77f, 0.1f, 100.0f);
    }

    void Application::Update()
    {
        m_Time += 0.01f;

        PerFrameData data{};
        data.Time = m_Time;
        data.ViewMatrix = m_Camera.GetViewMatrix();
        data.ProjectionMatrix = m_Camera.GetProjectionMatrix();
        data.ViewProjectionMatrix = m_Camera.GetViewProjectionMatrix();

        Renderer::Instance()->Render(data);
    }

    void Application::Deinit()
    {

    }
}