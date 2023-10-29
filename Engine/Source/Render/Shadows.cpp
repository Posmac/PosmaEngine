#include "Shadows.h"

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

    void Shadows::Init(uint32_t swapchainImages)
    {
        m_DepthFormat = vk::Vk::GetInstance()->FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*| VK_IMAGE_USAGE_SAMPLED_BIT*/);
        m_DepthSize = { 2048, 2048, 1 };

        InitShadowsBuffer();
        InitDirectionalLightData(swapchainImages);
        InitPointLightsData(swapchainImages);
        InitSpotLightData(swapchainImages);
    }

    void Shadows::InitShadowsBuffer()
    {
        VkDeviceSize bufferSize = sizeof(m_ShadowsBuffer) - offsetof(ShadowsBuffer, ShadowBuffer);
        vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice, 
                                     bufferSize,
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     &m_ShadowsBuffer.ShadowBuffer, 
                                     &m_ShadowsBuffer.ShadowBufferMemory, 
                                     &m_ShadowsBuffer.ShadowBufferMapping);
    }

    void Shadows::InitDirectionalLightData(uint32_t swapchainImages)
    {
        m_DirDepthImage.resize(swapchainImages);
        m_DirDepthImageMemory.resize(swapchainImages);
        m_DirDepthImageView.resize(swapchainImages);

        for(int i = 0; i < swapchainImages; i++)
        {
            vk::CreateImageAndView(vk::Device, vk::PhysicalDevice, m_DepthSize, 1, 1, VK_IMAGE_TYPE_2D, m_DepthFormat,
                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                       VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                       m_DepthFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT,
                       &m_DirDepthImage[i], &m_DirDepthImageMemory[i], &m_DirDepthImageView[i]);
        }

        glm::mat4 lightView = glm::lookAt(glm::vec3(10.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));

        float nearFarPlanes = 100.0f;
        glm::mat4 lightProjection = glm::orthoRH_ZO(-100.0f, 100.0f, -100.0f, 100.0f, -nearFarPlanes, nearFarPlanes);
        m_ShadowsBuffer.DirectionalLightViewProjectionMatrix = lightProjection * lightView;

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

        ShadowsBuffer* mat = reinterpret_cast<ShadowsBuffer*>(m_ShadowsBuffer.ShadowBufferMapping);
        mat->DirectionalLightViewProjectionMatrix = m_ShadowsBuffer.DirectionalLightViewProjectionMatrix;
    }

    void Shadows::InitPointLightsData(uint32_t swapchainImages)
    {

    }

    void Shadows::InitSpotLightData(uint32_t swapchainImages)
    {

    }

    Shadows::ShadowsBuffer& Shadows::GetBufferData()
    {
        return m_ShadowsBuffer;
    }

    void Shadows::RenderDepth()
    {
        
    }

    void Shadows::Update()
    {
        glm::mat4 lightView = glm::lookAt(position, lookAt, up);
        glm::mat4 lightProjection = glm::orthoZO(-range, range, -range, range, nearPlane, farPlane);
        m_ShadowsBuffer.DirectionalLightViewProjectionMatrix = lightProjection * lightView;

        ShadowsBuffer* mat = reinterpret_cast<ShadowsBuffer*>(m_ShadowsBuffer.ShadowBufferMapping);
        mat->DirectionalLightViewProjectionMatrix = m_ShadowsBuffer.DirectionalLightViewProjectionMatrix;
    }
}