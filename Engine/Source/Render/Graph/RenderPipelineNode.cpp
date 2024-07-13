#include "RenderPipelineNode.h"

namespace psm
{
    namespace graph
    {
        RenderPipelineNode::RenderPipelineNode(const foundation::Name& name)
            : mRenderPipelineName(name)
        {

        }

        RenderPipelineNode::~RenderPipelineNode()
        {
            LogMessage(MessageSeverity::Info, "RenderPipelineNode destructor");

            mPipelineLayout = nullptr;
            mPipeline = nullptr;
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