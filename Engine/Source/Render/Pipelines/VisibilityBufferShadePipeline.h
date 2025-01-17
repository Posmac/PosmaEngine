#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class VisibilityBufferShadePipelineNode : public RenderPipelineNode, std::enable_shared_from_this<VisibilityBufferShadePipelineNode>
        {
        public:
            VisibilityBufferShadePipelineNode(const foundation::Name& name,
                                  const DevicePtr& device,
                                  const ResourceMediatorPtr& resourceMediator);
            virtual ~VisibilityBufferShadePipelineNode();
        };

        using VisibilityBufferShadePipelineNodePtr = std::shared_ptr<VisibilityBufferShadePipelineNode>;
    }
}