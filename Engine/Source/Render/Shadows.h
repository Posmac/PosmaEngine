#pragma once

#include <array>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

constexpr int MAX_POINT_LIGHT_SOURCES = 16;

namespace psm
{
    class Shadows
    {
    public:
        struct ShadowsBuffer
        {
            glm::mat4 DirectionalLightViewProjectionMatrix;
            glm::mat4 m_SpotViewProjectionMatrix;
            std::array<glm::mat4, MAX_POINT_LIGHT_SOURCES> PointLightViewProjectionMatrices;

            VkBuffer ShadowBuffer;
            VkDeviceMemory ShadowBufferMemory;
            void* ShadowBufferMapping;
        };

        //singleton realization
    public:
        Shadows(Shadows&) = delete;
        void operator=(const Shadows&) = delete;
        static Shadows* Instance();
    private:
        Shadows();
        static Shadows* s_Instance;
        //class specific
    public:
        void Init(uint32_t swapchainImages);
        void InitShadowsBuffer();
        void InitDirectionalLightData(uint32_t swapchainImages);
        void InitPointLightsData(uint32_t swapchainImages);
        void InitSpotLightData(uint32_t swapchainImages);
        ShadowsBuffer& GetBufferData();
        void RenderDepth();
        void Update();
    private:
        ShadowsBuffer m_ShadowsBuffer;
        VkFormat m_DepthFormat;
        VkExtent3D m_DepthSize;

        //dir light depth image
        std::vector<VkImage> m_DirDepthImage;
        std::vector<VkDeviceMemory> m_DirDepthImageMemory;
        std::vector<VkImageView> m_DirDepthImageView;

        //point light depth images
        std::array<std::vector<VkImage>, MAX_POINT_LIGHT_SOURCES> m_PointLightsDepthImages;
        std::array<std::vector<VkDeviceMemory>, MAX_POINT_LIGHT_SOURCES> m_PointLightsDepthImagesMemory;
        std::array<std::vector<VkImageView>, MAX_POINT_LIGHT_SOURCES> m_PointLightsDepthImageViews;

        //spot light depth image
        std::vector<VkImage> m_SpotDepthImage;
        std::vector<VkDeviceMemory> m_SpotDepthImageMemory;
        std::vector<VkImageView> m_SpotDepthImageView;

        //renderer related thing
        VkRenderPass m_ShadowRenderPass;
        std::vector<VkFramebuffer> m_ShadowFramebuffers;

        //imgui data
        float range = 100;
        float nearPlane = -200;
        float farPlane = 200;
        glm::vec3 position = glm::vec3(10.0f, 10.0f, 0.0f);
        glm::vec3 lookAt;
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        float depthBias = 1;
        float depthSlope = 1;
    };
}