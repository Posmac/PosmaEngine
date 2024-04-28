#include "Application.h"

#include "Systems/InputSystem.h"
#include "Systems/TimeSystem.h"

#include "Materials/PbrMaterial.h"

#include "RHI/VkCommon.h"

namespace psm
{
    void Application::Init(uint32_t width, uint32_t height)
    {
        m_Camera = Camera(60.0f, width / height, 0.1f, 1000.0f);
        m_CameraDefaultMoveSpeed = 20.0f;
        m_CameraDefaultRotateSpeed = 150.0f;

        std::vector<MeshPbrMaterial> sponzaModelMeshMaterials;
        std::shared_ptr<Model>  sponzaModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/Sponza/glTF/", "Sponza.gltf", sponzaModel.get(), sponzaModelMeshMaterials);

        std::vector<MeshPbrMaterial> damagedHelmetMeshMaterials;
        std::shared_ptr<Model>  damagedHelmetModel = std::make_shared<Model>();
        ModelLoader::Instance()->LoadModel("../Engine/Models/DamagedHelmet/glTF/", "DamagedHelmet.gltf", damagedHelmetModel.get(), damagedHelmetMeshMaterials);

        {//add sponze instance(1)
            glm::mat4 instanceMatrix = glm::mat4(1.0);

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 0, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));

            OpaqueInstances::Instance instance = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshInstances opaqModelMeshInstances = { instance };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(sponzaModelMeshMaterials.size());
            for(int i = 0; i < sponzaModelMeshMaterials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = sponzaModelMeshMaterials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(sponzaModel, opaqModelMeshMaterials, opaqModelMeshInstances);
        }

        {//add damaged helmet (10)
            constexpr unsigned helmetsCount = 10;

            OpaqueInstances::OpaqModelMeshInstances opaqModelMeshInstances(10);

            glm::mat4 instanceMatrix = glm::mat4(1.0);
            for(unsigned i = 0; i < helmetsCount; i++)
            {
                instanceMatrix = glm::mat4(1.0);
                instanceMatrix = glm::translate(instanceMatrix, glm::vec3(20, i * 3, 0));
                instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));

                opaqModelMeshInstances[i] = { instanceMatrix };
            }

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(damagedHelmetMeshMaterials.size());
            for(int i = 0; i < damagedHelmetMeshMaterials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = damagedHelmetMeshMaterials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(damagedHelmetModel, opaqModelMeshMaterials, opaqModelMeshInstances);
        }


        OpaqueInstances::GetInstance()->UpdateInstanceBuffer();
        OpaqueInstances::GetInstance()->UpdateMeshToModelData();

        GlobalTimer::Instance()->Init(60);
    }

    void Application::Update()
    {
        GlobalTimer::Instance()->IsTimeElapsed();
        InputSystem::Instance()->Update();

        ProcessInput();

        GlobalBuffer data{};
        data.Time = GlobalTimer::Instance()->TotalTime();
        data.ViewMatrix = m_Camera.GetViewMatrix();
        data.ProjectionMatrix = m_Camera.GetProjectionMatrix();
        data.ViewProjectionMatrix = m_Camera.GetViewProjectionMatrix();

        Renderer::Instance()->Render(data);
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
        if(InputSystem::Instance()->IsKeyDown(KEY_LEFT))
        {
            cameraRotated = true;
            eulerRotation += glm::vec3(0, 1, 0);
        }
        if(InputSystem::Instance()->IsKeyDown(KEY_RIGHT))
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