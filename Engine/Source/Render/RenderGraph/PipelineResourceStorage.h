#pragma once

#include "Foundation/Name.h"

#include "RHI/Interface/Device.h"

namespace psm
{
    namespace graph
    {
        class RenderPassGraph;

        class PipelineResourceStorage
        {
        public:
            using ResourceName = foundation::Name;
            using PassName = foundation::Name;

            PipelineResourceStorage(const DevicePtr& device, 
                                    )
        };
    }
}