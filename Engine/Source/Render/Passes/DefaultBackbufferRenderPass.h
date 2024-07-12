#pragma once

#include <array>

#include "../Graph/RenderPassNode.h"

#include "RHI/Interface/Device.h"
#include "RHI/Interface/Swapchain.h"

namespace psm
{
    namespace graph
    {
        class DefaultBackbufferRenderPassNode : public RenderPassNode, std::enable_shared_from_this<DefaultBackbufferRenderPassNode>
        {
        public:
            DefaultBackbufferRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            const SwapchainPtr swapchain,
                                            EFormat swapchainImagesFormat);
            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index) override;
            virtual void PostRender(CommandBufferPtr& commandBuffer) override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences() override;

        private:
            void CreateRenderPass(EFormat swapchainImagesFormat);
            void CreateDepthStencilRenderTarget(const SwapchainPtr& swapchain);
            void CreateFramebuffers(const SwapchainPtr swapchain);
        private:
            ImagePtr mDepthStencilRenderTarget;
            EFormat mDepthStencilFormat;

            std::vector<ImagePtr> mShadowMapRefs;
        };

        using DefaultBackbufferRenderPassNodePtr = std::shared_ptr<graph::DefaultBackbufferRenderPassNode>;
    }
}