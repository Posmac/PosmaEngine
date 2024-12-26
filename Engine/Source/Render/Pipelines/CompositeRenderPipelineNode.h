#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class CompositeRenderPipelineNode : public RenderPipelineNode, std::enable_shared_from_this<CompositeRenderPipelineNode>
        {
        public:
            CompositeRenderPipelineNode(const foundation::Name& name,
                                      const DevicePtr& device,
                                      const RenderPassPtr& renderPass,
                                      const ResourceMediatorPtr& resourceMediator,
                                      const SResourceExtent3D viewportSize);
            virtual ~CompositeRenderPipelineNode();
        };

        using CompositeRenderPipelineNodePtr = std::shared_ptr<CompositeRenderPipelineNode>;
    }
}