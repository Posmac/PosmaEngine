#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "Foundation/Name.h"
#include "RHI/Interface/Device.h"
#include "RHI/Interface/RenderPass.h"
#include "RHI/Interface/Framebuffer.h"
#include "RHI/Interface/Image.h"
#include "RHI/Interface/Buffer.h"
#include "RHI/Interface/CommandBuffer.h"
#include "ResourceAliases.h"
#include "ResourceMediator.h"

namespace psm
{
    namespace graph
    {
        class RenderPassNode
        {
        public:
            RenderPassNode(const foundation::Name& name, const ResourceMediatorPtr& resourceMediator);
            virtual ~RenderPassNode();
            foundation::Name GetName() const;

            virtual void PreRender(CommandBufferPtr& commandBuffer, uint32_t index);
            virtual void Render();
            virtual void PostRender(CommandBufferPtr& commandBuffer) = 0;
            virtual void AddResourceReferences(uint32_t framesCount) = 0;
            virtual void CollectReferences() = 0;
            virtual void AddRenderCallback(const std::function<void()>& callback);
            virtual void AddPreRenderCallback(const std::function<void()>& callback);
            virtual RenderPassPtr& GetRenderPass();
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain);
        protected:
            foundation::Name mName;
            DevicePtr mDeviceInternal;
            ResourceMediatorPtr mResourceMediator;
            RenderPassPtr mRenderPass;
            SResourceExtent3D mFramebuffersSize;
            std::vector<FramebufferPtr> mFramebuffers;
            std::function<void()> mRenderCallback;
            std::function<void()> mPreRenderCallback;
        };

        using RenderPassNodePtr = std::shared_ptr<RenderPassNode>;
    }
}