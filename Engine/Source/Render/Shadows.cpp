#include "Shadows.h"

#ifdef RHI_VULKAN
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkBuffer.h"
#endif

#include "imgui/imgui.h"

namespace psm
{
    Shadows* Shadows::s_Instance = nullptr;

    Shadows* Shadows::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new Shadows();
        }

        return s_Instance;
    }

    void Shadows::Init(DevicePtr device, uint32_t swapchainImages)
    {
        mDeviceInternal = device;

        mDepthFormat = mDeviceInternal->FindSupportedFormat({EFormat::D32_SFLOAT, EFormat::D32_SFLOAT_S8_UINT, EFormat::D32_SFLOAT_S8_UINT}, EImageTiling::OPTIMAL, EFeatureFormat::DEPTH_STENCIL_ATTACHMENT_BIT);

        mShadowParams.DirectionalShadowTextureSize = { 4096, 4096, 1 };

        InitShadowsBuffer();
        InitDirectionalLightData(swapchainImages);
        InitPointLightsData(swapchainImages);
        InitSpotLightData(swapchainImages);
    }

    void Shadows::Deinit()
    {
        mGPUShadowBuffer = nullptr;

        //dir light depth image
        mDirDepthShadowMaps.clear();;

        //point light depth images
        mPointLightsShadowMaps = {};

        //spot light depth image
        mSpotLightShadowMaps.clear();

        //renderer related thing
        mShadowFramebuffers.clear();
        mShadowRenderPass = nullptr;
    }

    void Shadows::InitShadowsBuffer()
    {
        uint64_t bufferSize = sizeof(mShadowsBuffer);

        SBufferConfig bufferConfig =
        {
            .Size = bufferSize,
            .Usage = EBufferUsage::USAGE_UNIFORM_BUFFER_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        mGPUShadowBuffer = mDeviceInternal->CreateBuffer(bufferConfig);
    }

    void Shadows::InitDirectionalLightData(uint32_t swapchainImages)
    {
        mDirDepthShadowMaps.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            SImageConfig imageConfig =
            {
                .ImageSize = mShadowParams.DirectionalShadowTextureSize,
                .MipLevels = 1,
                .ArrayLevels = 1,
                .Type = EImageType::TYPE_2D,
                .Format = mDepthFormat,
                .Tiling = EImageTiling::OPTIMAL,
                .InitialLayout = EImageLayout::UNDEFINED,
                .Usage = EImageUsageType::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
                .SharingMode = ESharingMode::EXCLUSIVE,
                .SamplesCount = ESamplesCount::COUNT_1,
                .Flags = EImageCreateFlags::NONE,
                .ViewFormat = mDepthFormat,
                .ViewType = EImageViewType::TYPE_2D,
                .ViewAspect = EImageAspect::DEPTH_BIT
            };

            mDirDepthShadowMaps[i] = mDeviceInternal->CreateImage(imageConfig);
        }

        glm::mat4 lightView = glm::lookAt(glm::vec3(100.0f, 100.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));

        float nearFarPlanes = 1000.0f;
        glm::mat4 lightProjection = glm::orthoRH_ZO(-100.0f, 100.0f, -100.0f, 100.0f, -nearFarPlanes, nearFarPlanes);
        mShadowsBuffer.DirectionalLightViewProjectionMatrix = lightProjection * lightView;

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

        void* mapping;

        SBufferMapConfig mapConfig =
        {
            .Size = sizeof(mShadowsBuffer),
            .Offset = 0,
            .pData = &mapping
        };

        mGPUShadowBuffer->Map(mapConfig);

        ShadowsBuffer* mat = reinterpret_cast<ShadowsBuffer*>(mapping);
        mat->DirectionalLightViewProjectionMatrix = mShadowsBuffer.DirectionalLightViewProjectionMatrix;

        mGPUShadowBuffer->Unmap();
    }

    void Shadows::InitPointLightsData(uint32_t swapchainImages)
    {

    }

    void Shadows::InitSpotLightData(uint32_t swapchainImages)
    {

    }

    Shadows::ShadowsBuffer& Shadows::GetBufferData()
    {
        return mShadowsBuffer;
    }

    BufferPtr& Shadows::GetGPUBuffer()
    {
        return mGPUShadowBuffer;
    }

    void Shadows::Update()
    {
        glm::mat4 lightView = glm::lookAt(mDirectionalLightData.Position, mDirectionalLightData.LookAt, mDirectionalLightData.Up);
        float range = mDirectionalLightData.Range;
        glm::mat4 lightProjection = glm::orthoZO(-range, range, -range, range, mDirectionalLightData.NearPlane, mDirectionalLightData.FarPlane);
        mShadowsBuffer.DirectionalLightViewProjectionMatrix = lightProjection * lightView;

        void* mapping;

        SBufferMapConfig mapConfig =
        {
            .Size = sizeof(mShadowsBuffer),
            .Offset = 0,
            .pData = &mapping
        };

        mGPUShadowBuffer->Map(mapConfig);

        ShadowsBuffer* mat = reinterpret_cast<ShadowsBuffer*>(mapping);
        mat->DirectionalLightViewProjectionMatrix = mShadowsBuffer.DirectionalLightViewProjectionMatrix;

        mGPUShadowBuffer->Unmap();
    }

    void Shadows::DrawShadowParams()
    {
        ImGui::Begin("Hello, world!");
        ImGui::Text("Shadow maps render params");

        ImGui::SliderFloat("range", &mDirectionalLightData.Range, -100.0f, 100.0f);
        ImGui::SliderFloat("nearPlane", &mDirectionalLightData.NearPlane, -500.0f, 500.0f);
        ImGui::SliderFloat("farPlane", &mDirectionalLightData.FarPlane, -500.0f, 500.0f);
        ImGui::SliderFloat("depthBias", &mShadowParams.DepthBias, -10.0f, 10.0f);
        ImGui::SliderFloat("depthSlope", &mShadowParams.DepthSlope, -10.0f, 10.0f);

        ImGui::SliderFloat3("position", &mDirectionalLightData.Position[0], -100, 100);
        ImGui::SliderFloat3("lookAt", &mDirectionalLightData.LookAt[0], -10, 10);
        ImGui::SliderFloat3("up", &mDirectionalLightData.Up[0], -1, 1);

        ImGui::End();
    }

    ShadowsParams& Shadows::GetShadowParams()
    {
        return mShadowParams;
    }
}