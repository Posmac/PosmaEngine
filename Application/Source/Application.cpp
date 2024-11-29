#include "Application.h"

#include "Systems/InputSystem.h"
#include "Systems/TimeSystem.h"

#include "Materials/PbrMaterial.h"

#include "RHI/RHICommon.h"

namespace psm
{
    void Application::Init(uint32_t width, uint32_t height)
    {
        m_Camera = Camera(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
        m_CameraDefaultMoveSpeed = 10.0f;
        m_CameraDefaultRotateSpeed = 100.0f;

        m_isMouseLBPressed = false;

        m_Resolution = { width, height };

        TextureLoader::Instance()->AddWhiteDefaultTexture("../Engine/Textures/DefaultMagentaTexture.png");

        ImagePtr ddsTexture = TextureLoader::Instance()->LoadDDSTexture("../Engine/Models/Knight/Cape_BaseColor.dds");

        {//add sponze instance(1)

            /*std::vector<MeshPbrMaterial> sponzaModelMeshMaterials;
            std::shared_ptr<Model>  sponzaModel = std::make_shared<Model>();
            ModelLoader::Instance()->LoadModel("../Engine/Models/Sponza/glTF/", "Sponza.gltf", sponzaModel.get(), sponzaModelMeshMaterials);

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

            OpaqueInstances::GetInstance()->AddInstance(sponzaModel, opaqModelMeshMaterials, opaqModelMeshInstances);*/
        }

        {//add damaged helmet (10)

            std::vector<MeshPbrMaterial> damagedHelmetMeshMaterials;
            std::shared_ptr<Model>  damagedHelmetModel = ModelLoader::Instance()->LoadModel("../Engine/Models/DamagedHelmet/glTF/", "DamagedHelmet.gltf", damagedHelmetMeshMaterials);
            //damagedHelmetMeshMaterials[0].Albedo = ddsTexture;

            constexpr unsigned helmetsCount = 4;

            OpaqueInstances::OpaqModelMeshInstances opaqModelMeshInstances(helmetsCount);

            glm::mat4 instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 0, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));
            opaqModelMeshInstances[0] = { instanceMatrix };

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 5, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));
            opaqModelMeshInstances[1] = { instanceMatrix };

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(5, 0, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));
            opaqModelMeshInstances[2] = { instanceMatrix };

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 0, 5));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));
            opaqModelMeshInstances[3] = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(damagedHelmetMeshMaterials.size());
            for(int i = 0; i < damagedHelmetMeshMaterials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = damagedHelmetMeshMaterials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(damagedHelmetModel, opaqModelMeshMaterials, opaqModelMeshInstances);
        }

        {//add cubes

            std::vector<MeshPbrMaterial> boxMaterials;
            std::shared_ptr<Model>  boxModel = ModelLoader::Instance()->LoadModel("../Engine/Models/Box/", "Box.gltf", boxMaterials);

            constexpr unsigned boxCount = 2;

            OpaqueInstances::OpaqModelMeshInstances opaqModelMeshInstances(boxCount);

            glm::mat4 instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 0, -4));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(1, 1, 1));
            opaqModelMeshInstances[0] = { instanceMatrix };

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, -5, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(20, 1, 20));

            opaqModelMeshInstances[1] = { instanceMatrix };

            /*for(int i = 0; i < boxCount; i++)
            {
                instanceMatrix = glm::mat4(1.0);
                instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, -15, 0));
                instanceMatrix = glm::scale(instanceMatrix, glm::vec3(50, 1, 50));

                opaqModelMeshInstances[i] = { instanceMatrix };
            }*/

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(boxMaterials.size());
            for(int i = 0; i < boxMaterials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = boxMaterials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(boxModel, opaqModelMeshMaterials, opaqModelMeshInstances);
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
        m_Camera.OnWindowResize(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
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

        if(InputSystem::Instance()->IsMouseButtonPressed(MouseKeys::LEFT))
        {
            m_PivotMousePosition = InputSystem::Instance()->GetMousePosition();
            m_isMouseLBPressed = true;
        }

        if(InputSystem::Instance()->IsMouseButtonDown(MouseKeys::LEFT))
        {
            glm::vec2 currentMousePos = InputSystem::Instance()->GetMousePosition();
            glm::vec2 difference = currentMousePos - m_PivotMousePosition;

            difference /= (m_Resolution * 0.5f);

            eulerRotation += glm::vec3(difference, 0.0);

            //std::cout << eulerRotation.x << " | " << eulerRotation.y << std::endl;

            std::swap(eulerRotation.x, eulerRotation.y);

            cameraRotated = true;
        }

        /*if(InputSystem::Instance()->IsMouseButtonReleased(MouseKeys::LEFT))
        {
            
        }*/

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