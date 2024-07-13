#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class DefaultRenderPipelineNode : public RenderPipelineNode, std::enable_shared_from_this<DefaultRenderPipelineNode>
        {
        public:
            DefaultRenderPipelineNode(const foundation::Name& name,
                                      const DevicePtr& device,
                                      const RenderPassPtr& renderPass,
                                      const ResourceMediatorPtr& resourceMediator,
                                      const SResourceExtent3D viewportSize);
            virtual ~DefaultRenderPipelineNode();
        };

        using DefaultRenderPipelineNodePtr = std::shared_ptr<DefaultRenderPipelineNode>;
    }
}