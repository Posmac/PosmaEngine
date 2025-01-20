#include "RenderPassNode.h"

namespace psm
{
    namespace graph
    {
        RenderPassNode::RenderPassNode(const foundation::Name& name, const ResourceMediatorPtr& resourceMediator)
            : mName(name), mResourceMediator(resourceMediator)
        {}

        RenderPassNode::~RenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "RenderPassNode destructor");

            mDeviceInternal = nullptr;
            mResourceMediator = nullptr;
            mRenderPass = nullptr;
            mRenderCallback = nullptr;
            mPreRenderCallback = nullptr;
        }

        foundation::Name RenderPassNode::GetName() const
        {
            return mName;
        }

        void RenderPassNode::PreRender()
        {
            mPreRenderCallback();
        }

        void RenderPassNode::Render()
        {
            mRenderCallback();
        }

        void RenderPassNode::PostRender()
        {
            mPostRenderCallback();
        }

        void RenderPassNode::SetState(CommandBufferPtr& commandBuffer, uint32_t framebufferIndex)
        {
            mCurrentCommandBuffer = commandBuffer;
            mCurrentFramebufferIndex = framebufferIndex;
        }

        void RenderPassNode::ResetState()
        {
            mCurrentCommandBuffer = nullptr;
            mCurrentFramebufferIndex = -1;
        }

        void RenderPassNode::AddRenderCallback(const std::function<void()>& callback)
        {
            mRenderCallback = callback;
        }

        void RenderPassNode::AddPreRenderCallback(const std::function<void()>& callback)
        {
            mPreRenderCallback = callback;
        }

        void RenderPassNode::AddPostRenderCallback(const std::function<void()>& callback)
        {
            mPostRenderCallback = callback;
        }

        RenderPassPtr& RenderPassNode::GetRenderPass()
        {
            return mRenderPass;
        }
    }
}