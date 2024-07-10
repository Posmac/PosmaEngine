#pragma once

#include <memory>

#include "Foundation/Name.h"
#include "RHI/Interface/Pipeline.h"
#include "ResourceMediator.h"
#include "GraphResourceNames.h"

namespace psm
{
    namespace graph
    {
        class RenderPipelineNode
        {
        public:
            RenderPipelineNode(const foundation::Name& name);
            void Bind(const CommandBufferPtr& commandBuffer, EPipelineBindPoint bindPoint);
            PipelineLayoutPtr& GetPipelineLayout();
        protected:
            foundation::Name mRenderPipelineName;
            PipelinePtr mPipeline;
            PipelineLayoutPtr mPipelineLayout;
        };

        using RenderPipelineNodePtr = std::shared_ptr<RenderPipelineNode>;
    }
}