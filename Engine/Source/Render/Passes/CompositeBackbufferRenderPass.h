#pragma once

#include <array>

#include "../Graph/RenderPassNode.h"
#include "GbuffferRenderPass.h"

#include "RHI/Interface/Device.h"
#include "RHI/Interface/Swapchain.h"

namespace psm
{
    namespace graph
    {
        class CompositeBackbufferRenderPassNode : public RenderPassNode, std::enable_shared_from_this<CompositeBackbufferRenderPassNode>
        {
        public:
            CompositeBackbufferRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            const SwapchainPtr swapchain,
                                            EFormat swapchainImagesFormat);
            virtual ~CompositeBackbufferRenderPassNode();

            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index) override;
            virtual void PostRender(CommandBufferPtr& commandBuffer) override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences(uint32_t framesCount) override;
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) override;

        private:
            void CreateRenderPass(EFormat swapchainImagesFormat);
            void CreateFramebuffers(const SwapchainPtr swapchain);
        private:
            std::vector<FramebufferPtr> mFramebuffers;

            //struct internalGbufferRefImages
            //{
            //    ImagePtr AlbedoTargetImage;
            //    ImagePtr NormalTargetImage;
            //    ImagePtr DepthTargetImage;
            //    ImagePtr WorldPositionsTargetImage;
            //    ImagePtr EmissionTargetImage;
            //    ImagePtr SpecularGlosinessTargetImage;
            //    ImagePtr RoughnessMetalnessTargetImage;
            //};
            //
            //std::vector<ImagePtr> mShadowMapRefs;
            //std::vector<internalGbufferRefImages> mGbufferTargetsRefs;
        };

        using CompositeBackbufferRenderPassNodePtr = std::shared_ptr<graph::CompositeBackbufferRenderPassNode>;
    }
}