#include "Application.h"

namespace psm
{
    void Application::Init()
    {
        m_Camera = Camera(60.0f, 1.77f, 0.1f, 100.0f);
        m_Time = 0.0f;

        std::shared_ptr<Model> m_SkullModel;
        ModelLoader::Instance()->LoadModel("../Engine/Models/untitled.obj", m_SkullModel.get());

        glm::mat4 instanceMatrix = glm::mat4(1.0);
        instanceMatrix = glm::translate(instanceMatrix, glm::vec3(-30, 10, -50));
        instanceMatrix = glm::scale(instanceMatrix, glm::vec3(5));
        instanceMatrix = glm::rotate(instanceMatrix, glm::radians(180.0f), glm::vec3(0, 0, 1));
        instanceMatrix = glm::rotate(instanceMatrix, glm::radians(90.0f), glm::vec3(-1, 0, 0));

        RawTextureData rawData{ Rgb_alpha };
        TextureLoader::Instance()->LoadRawTextureData("../Engine/Models/Skull/Skull.jpg", &rawData);

        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(rawData.Height, rawData.Width)))) + 1;

        std::shared_ptr<Texture> m_SkullTexture;
        Renderer::Instance()->LoadTextureIntoMemory(rawData, mipLevels , m_SkullTexture.get());

        OpaqueInstances::Material material {};
        material.Tex = m_SkullTexture;

        OpaqueInstances::Instance instance;
        instance.InstanceMatrix = instanceMatrix;

        OpaqueInstances::GetInstance()->AddInstance(m_SkullModel, material, instance);
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

    void Application::ResizeWindow(HWND hWnd, uint32_t width, uint32_t height)
    {
        Renderer::Instance()->ResizeWindow(hWnd);
    }
}