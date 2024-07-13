#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class ShadowMapPipelineNode : public RenderPipelineNode, std::enable_shared_from_this<ShadowMapPipelineNode>
        {
        public:
            ShadowMapPipelineNode(const foundation::Name& name,
                                  const DevicePtr& device,
                                  const RenderPassPtr& renderPass,
                                  const ResourceMediatorPtr& resourceMediator,
                                  const SResourceExtent3D viewportSize);
            virtual ~ShadowMapPipelineNode();
        };

        using ShadowMapPipelineNodePtr = std::shared_ptr<ShadowMapPipelineNode>;
    }
}