#include "Application.h"

#include "Systems/InputSystem.h"
#include "Systems/TimeSystem.h"

#include "RHI/VkCommon.h"

namespace psm
{
    void Application::Init()
    {
        m_Camera = Camera(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
        m_CameraDefaultMoveSpeed = 20.0f;
        m_CameraDefaultRotateSpeed = 150.0f;

        std::shared_ptr<Model> cubeModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/untitled.obj", cubeModel.get());

        std::shared_ptr<Model> skullModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/Skull/Skull.obj", skullModel.get());

        RawTextureData skullData{ Rgb_alpha };
        TextureLoader::Instance()->LoadRawTextureData("../Engine/Models/Skull/Skull.jpg", &skullData);

        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(skullData.Height, skullData.Width)))) + 1;

        ImagePtr skullTexture;
        Renderer::Instance()->LoadTextureIntoMemory(skullData, mipLevels, skullTexture);

        RawTextureData cubeData{ Rgb_alpha };
        TextureLoader::Instance()->LoadRawTextureData("../Engine/Models/untitled.png", &cubeData);

        ImagePtr cubeTexture;
        Renderer::Instance()->LoadTextureIntoMemory(cubeData, 1, cubeTexture);

        OpaqueInstances::Material material {};
        OpaqueInstances::Instance instance;

        //cube
        material.Albedo = cubeTexture;
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

        material.Albedo = skullTexture;

        OpaqueInstances::GetInstance()->AddInstance(skullModel, material, instance);
        OpaqueInstances::GetInstance()->PrepareInstances();

        GlobalTimer::Instance()->Init(60);
    }

    void Application::Update()
    {
        GlobalTimer::Instance()->IsTimeElapsed();
        InputSystem::Instance()->Update();

        ProcessInput();

        PerFrameData data{};
        data.Time = GlobalTimer::Instance()->TotalTime();
        data.ViewMatrix = m_Camera.GetViewMatrix();
        data.ProjectionMatrix = m_Camera.GetProjectionMatrix();
        data.ViewProjectionMatrix = m_Camera.GetViewProjectionMatrix();

        Renderer::Instance()->Render();
    }

    void Application::Deinit()
    {
        //TODO: Implement some resource deinitialization (Memory Allocator)
    }

    void Application::ResizeWindow(HWND hWnd, uint32_t width, uint32_t height)
    {
        //TODO: not working ang the moment
        Renderer::Instance()->ResizeWindow(hWnd);
    }

    void Application::ProcessInput()
    {
        float deltaTime = GlobalTimer::Instance()->DeltaTime();

        //camera movement
        glm::vec4 offset = glm::vec4(0);
        bool cameraMoved = false;
        //Z
        if(InputSystem::Instance()->IsKeyDown(KEY_W))
        {
            offset -= m_Camera.GetForwardWorld();
            //offset -= glm::vec4(0, 0, 1, 0);
            cameraMoved = true;
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_S))
        {
            offset += m_Camera.GetForwardWorld();
            //offset += glm::vec4(0, 0, 1, 0);
            cameraMoved = true;
        }
        //X
        if(InputSystem::Instance()->IsKeyDown(KEY_A))
        {
            offset -= m_Camera.GetRightWorld();
            //offset -= glm::vec4(1, 0, 0, 0);
            cameraMoved = true;
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_D))
        {
            offset += m_Camera.GetRightWorld();
            //offset += glm::vec4(1, 0, 0, 0);
            cameraMoved = true;
        }
        //Y
        if(InputSystem::Instance()->IsKeyDown(KEY_SPACE))
        {
            offset += m_Camera.GetUpWorld();
            //offset += glm::vec4(0, 1, 0, 0);
            cameraMoved = true;
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_SHIFT))
        {
            offset -= m_Camera.GetUpWorld();
            //offset -= glm::vec4(0, 1, 0, 0);
            cameraMoved = true;
        }

        bool resetCamera = false;
        if(InputSystem::Instance()->IsKeyPressed(KEY_R))
        {
            resetCamera = true;
        }

        bool cameraRotated = false;
        glm::vec3 eulerRotation = glm::vec3(0);
        //Y
        if(InputSystem::Instance()->IsKeyDown(KEY_Q))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(0, 1, 0);
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_E))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(0, -1, 0);
        }
        //X
        if(InputSystem::Instance()->IsKeyDown(KEY_UP))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(1, 0, 0);
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_DOWN))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(-1, 0, 0);
        }
        //Z
        if(InputSystem::Instance()->IsKeyDown(KEY_LEFT))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(0, 0, -1);
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_RIGHT))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(0, 0, 1);
        }

        if(cameraRotated)
        {
            //m_Camera.RotateWorldEuler(eulerRotation * deltaTime * m_CameraDefaultRotateSpeed);
            m_Camera.RotateWorldEulerZConstrained(eulerRotation * deltaTime * m_CameraDefaultRotateSpeed);
        }
        if(cameraMoved)
        {
            m_Camera.TranslateWorld(offset * deltaTime * m_CameraDefaultMoveSpeed);
        }
        if(resetCamera)
        {
            m_Camera.SetWorldPosition(glm::vec4(0, 0, 0, 1));
            m_Camera.SetWorldRotationEuler(glm::vec3(0, 0, 0));
        }
        if(cameraMoved || resetCamera || cameraRotated)
        {
            m_Camera.RecalculateFromWorld();
        }
    }
}