#include "ShadowsGenerator.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

#include "imgui/imgui.h"

#include "Graph/GraphResourceNames.h"

#include <iostream>
#include <string>

namespace psm
{
    ShadowsGenerator* ShadowsGenerator::s_Instance = nullptr;

    ShadowsGenerator* ShadowsGenerator::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new ShadowsGenerator();
        }

        return s_Instance;
    }

    void ShadowsGenerator::Init(DevicePtr device, graph::ResourceMediatorPtr resourceMediator)
    {
        mDeviceInternal = device;
        mResourceMediatorInternal = resourceMediator;

        InitDirectionalLightData();
        InitPointLightsData();
        InitSpotLightData();
    }

    void ShadowsGenerator::InitDirectionalLightData()
    {
        /*glm::mat4 lightView = glm::lookAt(glm::vec3(100.0f, 100.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));

        float nearFarPlanes = 1000.0f;
        glm::mat4 lightProjection = glm::orthoRH_ZO(-100.0f, 100.0f, -100.0f, 100.0f, -nearFarPlanes, nearFarPlanes);*/

        //auto logVec = [](const glm::mat4& mat)
        //{
        //    std::cout << mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << ' ' << mat[0][3] << ' ' << '\n'
        //        << mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' ' << mat[1][3] << ' ' << '\n'
        //        << mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2] << ' ' << mat[2][3] << ' ' << '\n'
        //        << mat[3][0] << ' ' << mat[3][1] << ' ' << mat[3][2] << ' ' << mat[3][3] << ' ' << std::endl;
        //    std::cout << std::endl;
        //};

        //logVec(lightView);
        //logVec(lightProjection);
        //logVec(m_DirViewProjMatrix);

        //update buffer
    }

    void ShadowsGenerator::InitPointLightsData()
    {}

    void ShadowsGenerator::InitSpotLightData()
    {}

    void ShadowsGenerator::Update()
    {
        glm::mat4 lightView = glm::lookAt(mDirectionalLightData.Position, mDirectionalLightData.LookAt, mDirectionalLightData.Up);
        float range = mDirectionalLightData.Range;
        glm::mat4 lightProjection = glm::orthoZO(-range, range, -range, range, mDirectionalLightData.FarPlane, mDirectionalLightData.NearPlane);

        glm::mat4 viewproj = lightProjection * lightView;

        //update buffer
        BufferPtr& shadowBuffer = mResourceMediatorInternal->GetBufferByName(graph::SHADOW_CBUFFER);
        mDeviceInternal->UpdateBuffer(shadowBuffer, &viewproj);

       // auto logVec = [](const glm::mat4& mat)
       //{
       //    std::cout << mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << ' ' << mat[0][3] << ' ' << '\n'
       //        << mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' ' << mat[1][3] << ' ' << '\n'
       //        << mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2] << ' ' << mat[2][3] << ' ' << '\n'
       //        << mat[3][0] << ' ' << mat[3][1] << ' ' << mat[3][2] << ' ' << mat[3][3] << ' ' << std::endl;
       //    std::cout << std::endl;
       //};

       //logVec(lightProjection);
    }

    void ShadowsGenerator::DrawShadowParams()
    {
        ImGui::Begin("Hello, world!");
        ImGui::Text("Shadow maps render params");

        ImGui::SliderFloat("range", &mDirectionalLightData.Range, -1000.0f, 1000.0f);
        ImGui::SliderFloat("nearPlane", &mDirectionalLightData.NearPlane, -5000.0f, 5000.0f);
        ImGui::SliderFloat("farPlane", &mDirectionalLightData.FarPlane, -5000.0f, 5000.0f);
        ImGui::SliderFloat("depthBias", &DepthBias, -10.0f, 10.0f);
        ImGui::SliderFloat("depthSlope", &DepthSlope, -10.0f, 10.0f);

        ImGui::SliderFloat3("position", &mDirectionalLightData.Position[0], -1000, 1000);
        ImGui::SliderFloat3("lookAt", &mDirectionalLightData.LookAt[0], -10, 10);
        ImGui::SliderFloat3("up", &mDirectionalLightData.Up[0], -1, 1);

        ImGui::End();
    }

    void ShadowsGenerator::Deinit()
    {
        mDeviceInternal = nullptr;
        mResourceMediatorInternal = nullptr;
    }
}