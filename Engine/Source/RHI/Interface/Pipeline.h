#pragma once

#pragma once

#include "RHI/Interface/Types.h"

#include "RHI/Enums/RenderPassFormats.h"

#include "RHI/Configs/PipelineConfig.h"
#include "RHI/Configs/PipelineLayoutConfig.h"

namespace psm
{
    class IPipelineLayout
    {
    public:
        IPipelineLayout() = default;
        virtual ~IPipelineLayout() = default;
    };

    class IPipeline
    {
    public:
        IPipeline() = default;
        virtual ~IPipeline() = default;

        virtual void Bind(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint) = 0;
    };
}