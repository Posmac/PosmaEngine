#pragma once

#include <array>

#include "../Graph/RenderPassNode.h"

#include "RHI/Interface/Device.h"
#include "RHI/Interface/Swapchain.h"
#include "RHI/Interface/Framebuffer.h"

namespace psm
{
    namespace graph
    {
        struct GbufferTargetInfo
        {
            ImagePtr Image;
            EFormat Format;
        };

        struct GbufferTargets
        {
            GbufferTargetInfo AlbedoTarget;
            GbufferTargetInfo NormalTarget;
            GbufferTargetInfo DepthTarget;
            GbufferTargetInfo WorldPositionsTarget;
            GbufferTargetInfo EmissionTarget;
            GbufferTargetInfo SpecularGlosinessTarget;
            GbufferTargetInfo RoughnessMetalnessTarget;
            FramebufferPtr Framebuffer;
        };

        class GbuffferRenderPassNode : public RenderPassNode, std::enable_shared_from_this<GbuffferRenderPassNode>
        {
        public:
            GbuffferRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            SResourceExtent3D framebufferSize,
                                            uint32_t framebuffersCount);
            virtual ~GbuffferRenderPassNode();

            virtual void PreRender() override;
            virtual void PostRender() override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences(uint32_t framesCount) override;
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) override;

        private:
            void CreateRenderPass(uint32_t framesCount);
            void CreateDepthStencilRenderTarget(SResourceExtent3D size);
            void CreateFramebuffers(SResourceExtent3D framebufferSize, uint32_t framesCount);

        private:
            GbufferTargetInfo mDepthStencilRenderTarget;

            uint32_t mTargetsCount;
            std::vector<GbufferTargets> mTargets;
        };

        using GbuffferRenderPassNodePtr = std::shared_ptr<graph::GbuffferRenderPassNode>;
    }
}