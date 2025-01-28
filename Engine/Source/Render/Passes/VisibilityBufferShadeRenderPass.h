#pragma once

#include <array>

#include "../Graph/RenderPassNode.h"

#include "RHI/Interface/Device.h"
#include "RHI/Interface/Swapchain.h"
#include "RHI/Interface/Framebuffer.h"

#include "glm/glm.hpp"

namespace psm
{
    namespace graph
    {
        class VisibilityBufferShadeRenderPassNode : public RenderPassNode, std::enable_shared_from_this<VisibilityBufferShadeRenderPassNode>
        {
        public:
            VisibilityBufferShadeRenderPassNode(const foundation::Name& name,
                                            const DevicePtr& device,
                                            const ResourceMediatorPtr& resourceMediator,
                                            CommandPoolPtr& computeCommandPool,
                                            CommandPoolPtr& graphicsCommandPool,
                                            DescriptorPoolPtr& descriptorPool,
                                            SResourceExtent3D framebufferSize,
                                            uint32_t framebuffersCount);
            virtual ~VisibilityBufferShadeRenderPassNode();

            virtual void PreRender() override;
            virtual void PostRender() override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences(uint32_t framesCount) override;
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) override;

            void UpdateDescriptors();

        private:
            void InitializeBuffers(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount);
            void InitializeDescriptors(DescriptorPoolPtr& descriptorPool);
            void InitializeRenderTargets(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount);

        private:
            std::vector<ImagePtr> mShaderStorageAlbedoResult;
            std::vector<ImagePtr> mShaderStorageNormalResult;
            std::vector<ImagePtr> mShaderStorageWorldPosResult;

            //store indices and vertices of all models
            BufferPtr mVertices;
            BufferPtr mIndices;
            BufferPtr mInstances;

            DescriptorSetLayoutPtr mSetLayout;
            DescriptorSetPtr mSet;

            SamplerPtr mSampler;
        };

        using VisibilityBufferShadeRenderPassNodePtr = std::shared_ptr<graph::VisibilityBufferShadeRenderPassNode>;
    }
}