#pragma once

#include <array>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/ImageFormats.h"

#include "RHI/RHICommon.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Passes/ShadowMapRenderPass.h"
#include "Graph/ResourceMediator.h"

namespace psm
{
    struct DirectionalShadowsData
    {
        float Range = 300.0f;
        float NearPlane = -1500.0f;
        float FarPlane = 1500.0f;
        glm::vec3 Position = glm::vec3(0.0f, 1000.0f, 600.0f);
        glm::vec3 LookAt = glm::vec3(0, 0, 0);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    };

    class ShadowsGenerator
    {
    public:
        //singleton realization
    public:
        ShadowsGenerator(ShadowsGenerator&) = delete;
        void operator=(const ShadowsGenerator&) = delete;
        static ShadowsGenerator* Instance();
    private:
        ShadowsGenerator() {};
        static ShadowsGenerator* s_Instance;
        //class specific
    public:
        void Init(DevicePtr device, graph::ResourceMediatorPtr resourceMediator);
        void InitDirectionalLightData();
        void InitPointLightsData();
        void InitSpotLightData();
        void Update();
        void DrawShadowParams();
        void Deinit();

    public:
        float DepthBias = -1;
        float DepthSlope = -1;
    private:
        DevicePtr mDeviceInternal;
        graph::ResourceMediatorPtr mResourceMediatorInternal;
        
        DirectionalShadowsData mDirectionalLightData;
    };
}