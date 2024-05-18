#pragma once

#include <array>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/ImageFormats.h"

#include "RHI/RHICommon.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

constexpr int MAX_POINT_LIGHT_SOURCES = 16;

namespace psm
{
    struct ShadowsParams
    {
        SResourceExtent3D DirectionalShadowTextureSize;

        float DepthBias = 0.25f;
        float DepthSlope = 0.25f;
    };

    struct DirectionalShadowsData
    {
        float Range = 25.0f;
        float NearPlane = -500.0f;
        float FarPlane = 500.0f;
        glm::vec3 Position = glm::vec3(100.0f, 100.0f, 0.0f);
        glm::vec3 LookAt = glm::vec3(0, 0, 0);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    };

    class Shadows
    {
    public:
        struct ShadowsBuffer
        {
            glm::mat4 DirectionalLightViewProjectionMatrix;
            glm::mat4 SpotViewProjectionMatrix;
            std::array<glm::mat4, MAX_POINT_LIGHT_SOURCES> PointLightViewProjectionMatrices;
        };

        //singleton realization
    public:
        Shadows(Shadows&) = delete;
        void operator=(const Shadows&) = delete;
        static Shadows* Instance();
    private:
        Shadows() {};
        static Shadows* s_Instance;
        //class specific
    public:
        void Init(DevicePtr device, uint32_t swapchainImages);
        void Deinit();
        void InitShadowsBuffer();
        void InitDirectionalLightData(uint32_t swapchainImages);
        void InitPointLightsData(uint32_t swapchainImages);
        void InitSpotLightData(uint32_t swapchainImages);
        ShadowsBuffer& GetBufferData();
        BufferPtr& GetGPUBuffer();
        void Update();
        void DrawShadowParams();

        ShadowsParams& GetShadowParams();

    private:

        DevicePtr mDeviceInternal;

        ShadowsBuffer mShadowsBuffer;
        BufferPtr mGPUShadowBuffer;
        EFormat mDepthFormat;

        //dir light depth image
        std::vector<ImagePtr> mDirDepthShadowMaps;

        //point light depth images
        std::array<std::vector<ImagePtr>, MAX_POINT_LIGHT_SOURCES> mPointLightsShadowMaps;

        //spot light depth image
        std::vector<ImagePtr> mSpotLightShadowMaps;

        //renderer related thing
        RenderPassPtr mShadowRenderPass;
        std::vector<FramebufferPtr> mShadowFramebuffers;

        //
        DirectionalShadowsData mDirectionalLightData;
        ShadowsParams mShadowParams;
    };
}