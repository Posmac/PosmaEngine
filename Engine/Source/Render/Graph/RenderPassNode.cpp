#include "RenderPassNode.h"

namespace psm
{
    namespace graph
    {
        RenderPassNode::RenderPassNode(const foundation::Name& name, const ResourceMediatorPtr& resourceMediator)
            : mName(name), mResourceMediator(resourceMediator)
        {}

        foundation::Name RenderPassNode::GetName() const
        {
            return mName;
        }

        void RenderPassNode::Render()
        {
            mRenderCallback();
        }

        void RenderPassNode::AddRenderCallback(const std::function<void()>& callback)
        {
            mRenderCallback = callback;
        }

        RenderPassPtr& RenderPassNode::GetRenderPass()
        {
            return mRenderPass;
        }
    }
}