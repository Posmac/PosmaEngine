#pragma once

#include <vector>

#include "RHI/RHICommon.h"

#include "RHI/Interface/Types.h"
#include "RHI/Enums/PipelineFormats.h"
#include "RHI/Enums/RenderPassFormats.h"
#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/SamplerConfig.h"

namespace psm
{
    struct SVertexBufferBindConfig
    {
        uint32_t FirstSlot;
        uint32_t BindingCount;
        std::vector<uint64_t> Offsets;
        BufferPtr* Buffers;
    };

    struct SIndexBufferBindConfig
    {
        EIndexType Type;
        BufferPtr Buffer;
    };

    struct SPipelineConfig
    {
        RenderPassPtr RenderPass;
        SResourceExtent2D ViewPortExtent;
        PipelineLayoutPtr PipelineLayout;
        SVertexInputAttributeDescription* pVertexInputAttributes;
        uint32_t VertexInputAttributeCount;
        SVertexInputBindingDescription* pVertexInputBindings;
        uint32_t VertexInputBindingCount;
        SShaderModuleConfig* pShaderModules;
        uint32_t ShaderModulesCount;
        EDynamicState* pDynamicStates;
        uint32_t DynamicStatesCount;
        SInputAssemblyConfig InputAssembly;
        SRasterizationConfig Rasterization;
        ECompareOp DepthStencilCompareOp;
    };
}