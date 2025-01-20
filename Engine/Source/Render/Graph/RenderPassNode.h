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

            virtual void PreRender();
            virtual void Render();
            virtual void PostRender();
            virtual void SetState(CommandBufferPtr& commandBuffer, uint32_t framebufferIndex);
            virtual void ResetState();
            virtual void AddResourceReferences(uint32_t framesCount) = 0;
            virtual void CollectReferences(uint32_t framesCount) = 0;
            virtual void AddRenderCallback(const std::function<void()>& callback);
            virtual void AddPreRenderCallback(const std::function<void()>& callback);
            virtual void AddPostRenderCallback(const std::function<void()>& callback);
            virtual RenderPassPtr& GetRenderPass();
            virtual void RecreateFramebuffers(const SwapchainPtr swapchain) = 0;

        protected:
            foundation::Name mName;
            DevicePtr mDeviceInternal;
            ResourceMediatorPtr mResourceMediator;
            RenderPassPtr mRenderPass;
            SResourceExtent3D mFramebuffersSize;
            std::function<void()> mPreRenderCallback;
            std::function<void()> mRenderCallback;
            std::function<void()> mPostRenderCallback;

            CommandBufferPtr mCurrentCommandBuffer;
            uint32_t mCurrentFramebufferIndex;
        };

        using RenderPassNodePtr = std::shared_ptr<RenderPassNode>;
    }
}