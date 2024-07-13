#pragma once

#include <array>

#include "../Graph/RenderPassNode.h"

#include "RHI/Interface/Device.h"

#include "glm/glm.hpp"

constexpr int MAX_POINT_LIGHT_SOURCES = 16;

namespace psm
{
    namespace graph
    {
        class RenderGraph;

        struct ShadowMapCbuffer
        {
            glm::mat4 DirectionalLightViewProjectionMatrix;
            glm::mat4 SpotViewProjectionMatrix;
            std::array<glm::mat4, MAX_POINT_LIGHT_SOURCES> PointLightViewProjectionMatrices;
        };

        class ShadowMapRenderPassNode : public RenderPassNode, std::enable_shared_from_this<ShadowMapRenderPassNode>
        {
        public:
            ShadowMapRenderPassNode(const foundation::Name& name,
                                    const ResourceMediatorPtr& resourceMediator,
                                    const DevicePtr& device,
                                    SResourceExtent3D framebufferSize,
                                    uint32_t framebuffersCount);
            virtual ~ShadowMapRenderPassNode();
            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index) override;
            virtual void PostRender(CommandBufferPtr& commandBuffer) override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences() override;

        private:
            void CreateRenderPass(const DevicePtr& device);
            void CreateFramebuffers(const DevicePtr& device,
                                    SResourceExtent3D framebufferSize,
                                    uint32_t framebuffersCount);
            void CreateBuffer();
        private:
            std::vector<ImagePtr> mRenderTargets;

            ShadowMapCbuffer mShadowsBuffer;
            BufferPtr mGPUShadowBuffer;
            EFormat mDepthFormat;
        };

        using ShadowMapRenderPassNodePtr = std::shared_ptr<graph::ShadowMapRenderPassNode>;
    }
}