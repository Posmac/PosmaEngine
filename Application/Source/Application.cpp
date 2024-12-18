#include "Application.h"

#include "Systems/InputSystem.h"
#include "Systems/TimeSystem.h"

#include "Materials/PbrMaterial.h"

#include "RHI/RHICommon.h"

#include <filesystem>
#include <algorithm>
#include <vector>
#include <set>

namespace psm
{
    void Application::Init(uint32_t width, uint32_t height)
    {
        m_Camera = Camera(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
        m_CameraDefaultMoveSpeed = 50.0f;
        m_CameraDefaultRotateSpeed = 300.0f;
        m_CameraStartingPosition = glm::vec4(0, 30, 80, 1);
        m_Camera.SetWorldPosition(m_CameraStartingPosition);
        m_Camera.RecalculateFromWorld();

        m_isMouseLBPressed = false;

        m_Resolution = { width, height };

        TextureLoader::Instance()->AddWhiteDefaultTexture("../Engine/Textures/DefaultMagentaTexture.png");

        {//add sponze instance(1)

            std::vector<MeshPbrMaterial> materials;
            std::shared_ptr<Model>  sponzaModel = ModelLoader::Instance()->LoadModel("../Engine/Models/Sponza/glTF/", "Sponza.gltf", materials, true);

            glm::mat4 instanceMatrix = glm::mat4(1.0);

            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(0, 0, 0));
            instanceMatrix = glm::rotate(instanceMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(10, 10, 10));

            OpaqueInstances::Instance instance = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshInstances instances = { instance };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(materials.size());
            for(int i = 0; i < materials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = materials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(sponzaModel, opaqModelMeshMaterials, instances);
        }

        constexpr int startingYPos = 5;
        constexpr int yPosStep = 10;
        int startingXPos = 5;

        {//add Abautiful game

            std::vector<MeshPbrMaterial> materials;
            std::shared_ptr<Model> model = ModelLoader::Instance()->LoadModel("../Engine/Models/ABeautifulGame/glTF/", "ABeautifulGame.gltf", materials, true);

            constexpr unsigned modelsCount = 1;

            OpaqueInstances::OpaqModelMeshInstances instances(modelsCount);

            startingXPos = -5;
            int curYPos = startingYPos;
            glm::mat4 instanceMatrix = glm::mat4(1.0);
            glm::vec3 scale = glm::vec3(30);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, 0));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[0] = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(materials.size());
            for(int i = 0; i < materials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = materials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(model, opaqModelMeshMaterials, instances);
        }

        {//add damaged helmet

            std::vector<MeshPbrMaterial> materials;
            std::shared_ptr<Model> model = ModelLoader::Instance()->LoadModel("../Engine/Models/DamagedHelmet/glTF/", "DamagedHelmet.gltf", materials, false);

            constexpr unsigned modelsCount = 1;

            OpaqueInstances::OpaqModelMeshInstances instances(modelsCount);

            startingXPos = -5;
            int curYPos = startingYPos + 10;
            glm::mat4 instanceMatrix = glm::mat4(1.0);
            glm::vec3 scale = glm::vec3(3);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, 0));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[0] = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(materials.size());
            for(int i = 0; i < materials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = materials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(model, opaqModelMeshMaterials, instances);
        }

        {//add knights

            std::vector<MeshPbrMaterial> knightsMaterials;
            std::shared_ptr<Model> knightModel = ModelLoader::Instance()->LoadModel("../Engine/Models/Knight/", "Knight.fbx", knightsMaterials, true);

            constexpr unsigned knightsCount = 4;

            OpaqueInstances::OpaqModelMeshInstances opaqModelMeshInstances(knightsCount);

            startingXPos = 10;
            int curYPos = startingYPos;

            glm::mat4 instanceMatrix = glm::mat4(1.0);
            glm::vec3 scale = glm::vec3(3);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            opaqModelMeshInstances[0] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            opaqModelMeshInstances[1] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            opaqModelMeshInstances[2] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            opaqModelMeshInstances[3] = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(knightsMaterials.size());
            for(int i = 0; i < knightsMaterials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = knightsMaterials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(knightModel, opaqModelMeshMaterials, opaqModelMeshInstances);
        }

        {//add horses

            std::vector<MeshPbrMaterial> materials;
            std::shared_ptr<Model> model = ModelLoader::Instance()->LoadModel("../Engine/Models/KnightHorse/", "KnightHorse.fbx", materials, true);

            constexpr unsigned modelsCount = 4;

            OpaqueInstances::OpaqModelMeshInstances instances(modelsCount);

            startingXPos = -5;
            int curYPos = startingYPos;
            glm::vec3 scale = glm::vec3(3);
            glm::mat4 instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[0] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[1] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[2] = { instanceMatrix };

            curYPos += yPosStep;
            instanceMatrix = glm::mat4(1.0);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(startingXPos, curYPos, -20));
            instanceMatrix = glm::scale(instanceMatrix, scale);
            instances[3] = { instanceMatrix };

            OpaqueInstances::OpaqModelMeshMaterials opaqModelMeshMaterials(materials.size());
            for(int i = 0; i < materials.size(); i++)
            {
                opaqModelMeshMaterials[i].Albedo = materials[i].Albedo;
            }

            OpaqueInstances::GetInstance()->AddInstance(model, opaqModelMeshMaterials, instances);
        }

        TextureLoader::Instance()->LoadAllTextures();

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
            std::swap(eulerRotation.x, eulerRotation.y);

            cameraRotated = true;
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