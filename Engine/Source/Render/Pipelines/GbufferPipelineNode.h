#pragma once

#include "../Graph/RenderPipelineNode.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class GbufferPipelineNode : public RenderPipelineNode, std::enable_shared_from_this<GbufferPipelineNode>
        {
        public:
            GbufferPipelineNode(const foundation::Name& name,
                                  const DevicePtr& device,
                                  const RenderPassPtr& renderPass,
                                  const ResourceMediatorPtr& resourceMediator,
                                  const SResourceExtent3D viewportSize);
            virtual ~GbufferPipelineNode();
        };

        using GbufferPipelineNodePtr = std::shared_ptr<GbufferPipelineNode>;
    }
}