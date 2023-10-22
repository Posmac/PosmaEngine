#include "Application.h"

#include "Systems/InputSystem.h"

namespace psm
{
    void Application::Init()
    {
        m_Camera = Camera(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
        m_Time = 0.0f;

        std::shared_ptr<Model> cubeModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/untitled.obj", cubeModel.get());

        std::shared_ptr<Model> skullModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/Skull/Skull.obj", skullModel.get());

        RawTextureData skullData{ Rgb_alpha };
        TextureLoader::Instance()->LoadRawTextureData("../Engine/Models/Skull/Skull.jpg", &skullData);

        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(skullData.Height, skullData.Width)))) + 1;

        std::shared_ptr<Texture> skullTexture = std::make_shared<Texture>();
        Renderer::Instance()->LoadTextureIntoMemory(skullData, mipLevels, skullTexture.get());

        RawTextureData cubeData{ Rgb_alpha };
        TextureLoader::Instance()->LoadRawTextureData("../Engine/Models/untitled.png", &cubeData);

        std::shared_ptr<Texture> cubeTexture = std::make_shared<Texture>();
        Renderer::Instance()->LoadTextureIntoMemory(cubeData, 1, cubeTexture.get());

        OpaqueInstances::Material material {};
        OpaqueInstances::Instance instance;

        //cube
        material.Tex = cubeTexture;
        glm::mat4 instanceMatrix = glm::mat4(1.0);
        //small cubes
        for(int x = -100; x < 100; x++)
        {
            for(int z = -100; z < 100; z++)
            {
                instanceMatrix = glm::mat4(1.0);
                instanceMatrix = glm::translate(instanceMatrix, glm::vec3(x, -5, z));
                instanceMatrix = glm::rotate(instanceMatrix, glm::radians(90.0f), glm::vec3(-1, 0, 0));
                instance.InstanceMatrix = instanceMatrix;
                OpaqueInstances::GetInstance()->AddInstance(cubeModel, material, instance);
            }
        }

        //skull
        instanceMatrix = glm::mat4(1.0);
        instanceMatrix = glm::translate(instanceMatrix, glm::vec3(30, 0, -50));
        instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1));
        instanceMatrix = glm::rotate(instanceMatrix, glm::radians(90.0f), glm::vec3(-1, 0, 0));

        instance.InstanceMatrix = instanceMatrix;

        material.Tex = skullTexture;

        OpaqueInstances::GetInstance()->AddInstance(skullModel, material, instance);
        OpaqueInstances::GetInstance()->PrepareInstances();
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

        if(InputSystem::Instance()->IsKeyDown(KEY_A))
        {
            //std::cout << "Key A is Down" << std::endl;
        }

        if(InputSystem::Instance()->IsKeyUp(KEY_A))
        {
            //std::cout << "Key A is Up" << std::endl;
        }

        if(InputSystem::Instance()->IsKeyPressed(KEY_A))
        {
            //std::cout << "Key A is Pressed" << std::endl;
        }

        if(InputSystem::Instance()->IsKeyReleased(KEY_A))
        {
            std::cout << "Key A is Released" << std::endl;
        }
    }

    void Application::Deinit()
    {

    }

    void Application::ResizeWindow(HWND hWnd, uint32_t width, uint32_t height)
    {
        Renderer::Instance()->ResizeWindow(hWnd);
    }
}