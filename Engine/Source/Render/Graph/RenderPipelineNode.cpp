#include "RenderPipelineNode.h"

namespace psm
{
    namespace graph
    {
        RenderPipelineNode::RenderPipelineNode(const foundation::Name& name)
            : mRenderPipelineName(name)
        {

        }

        void RenderPipelineNode::Bind(const CommandBufferPtr& commandBuffer, EPipelineBindPoint bindPoint)
        {
            mPipeline->Bind(commandBuffer, bindPoint);
        }

        PipelineLayoutPtr& RenderPipelineNode::GetPipelineLayout()
        {
            return mPipelineLayout;
        }
    }
}