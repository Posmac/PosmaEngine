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
        struct Particle
        {
            glm::vec2 Position;
            glm::vec2 Velocity;
            glm::vec4 Color;
        };

        const uint32_t PARTICLE_COUNT = 8192;

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

            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index) override;
            virtual void PostRender(CommandBufferPtr& commandBuffer) override;
            virtual void AddResourceReferences(uint32_t framesCount) override;
            virtual void CollectReferences(uint32_t framesCount) override;
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) override;

        private:
            void InitializeBuffers(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount);
            void InitializeDescriptors(DescriptorPoolPtr& descriptorPool);
            void UpdateDescriptors(uint32_t framesCount);

        private:
            std::vector<BufferPtr> mShaderStorageBuffers;
                                                                                            
            DescriptorSetLayoutPtr mShaderStorageSetLayout;
            DescriptorSetPtr mShaderStorageSet;
        };

        using VisibilityBufferShadeRenderPassNodePtr = std::shared_ptr<graph::VisibilityBufferShadeRenderPassNode>;
    }
}