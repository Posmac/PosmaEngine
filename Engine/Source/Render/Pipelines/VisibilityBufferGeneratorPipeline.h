#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class VisibilityBufferGeneratorPipelineNode : public RenderPipelineNode, std::enable_shared_from_this<VisibilityBufferGeneratorPipelineNode>
        {
        public:
            VisibilityBufferGeneratorPipelineNode(const foundation::Name& name,
                                  const DevicePtr& device,
                                  const RenderPassPtr& renderPass,
                                  const ResourceMediatorPtr& resourceMediator,
                                  const SResourceExtent3D viewportSize);
            virtual ~VisibilityBufferGeneratorPipelineNode();
        };

        using VisibilityBufferGeneratorPipelineNodePtr = std::shared_ptr<VisibilityBufferGeneratorPipelineNode>;
    }
}