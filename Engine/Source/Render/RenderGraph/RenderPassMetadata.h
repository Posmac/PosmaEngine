#pragma once

#include "Foundation/Name.h"

namespace psm
{
    namespace graph
    {
        enum class RenderPassPurporse
        {
            //every frame render pass
            Default,
            //One-time render pass to setup things
            Setup,
            //One-time render pass for asset preprocessing. 
            //Asset resources will be specifically transitioned
            //to appropriate states before and after such passes
            AssetProcessing,
        };

        enum class RenderPassExecutionQueue : uint64_t
        {
            Graphics = 0,
            Compute = 1,
            AsyncCompute = 2,
            Transfer = 3
        };

        struct RenderPassMetadata
        {
            foundation::Name Name;
            RenderPassPurporse Purporse = RenderPassPurporse::Default;
        };
    }
}