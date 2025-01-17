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
        class VisibilityBufferGeneratorRenderPassNode : public RenderPassNode, std::enable_shared_from_this<VisibilityBufferGeneratorRenderPassNode>
        {
        public:
            VisibilityBufferGeneratorRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            SResourceExtent3D framebufferSize,
                                            uint32_t framebuffersCount);
            virtual ~VisibilityBufferGeneratorRenderPassNode();

            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index) override;
            virtual void PostRender(CommandBufferPtr& commandBuffer) override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences(uint32_t framesCount) override;
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) override;

        private:
            void CreateRenderPass(uint32_t framesCount);
            void CreateDepthStencilRenderTarget(SResourceExtent3D size);
            void CreateFramebuffers(SResourceExtent3D framebufferSize, uint32_t framesCount);

        private:
            ImagePtr mDepthStencilRenderTarget;
            std::vector<ImagePtr> mVisBufferImages;
            std::vector<FramebufferPtr> mFramebuffers;
        };

        using VisibilityBufferGeneratorRenderPassNodePtr = std::shared_ptr<graph::VisibilityBufferGeneratorRenderPassNode>;
    }
}