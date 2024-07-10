#pragma once

#include "Foundation/Name.h"
#include <cstdint>

namespace psm
{
    namespace graph
    {
        inline foundation::Name GetResourceIndexedName(const foundation::Name& name, uint32_t index)
        {
            return name.ToString() + '_' + std::to_string(index);
        }

        //render targets (out)
        const foundation::Name BACKBUFFER_RENDERTARGET_NAME = foundation::Name("BackbufferRenderTarget");
        const foundation::Name SHADOWMAP_RENDERTARGET_NAME = foundation::Name("ShadowMapRenderTarget");
        const foundation::Name DEPTHSTECIL_RENDERTARGET_NAME = foundation::Name("DepthStencilRenderTarget");

        //render passes
        const foundation::Name DEFAULT_RENDERPASS = foundation::Name("DefaultRenderPass");
        const foundation::Name SHADOWMAP_RENDERPASS = foundation::Name("ShadowMapRenderPass");

        //general buffers input
        const foundation::Name GLOBAL_CBUFFER = foundation::Name("GlobalConstantBuffer");
        const foundation::Name SHADOW_CBUFFER = foundation::Name("ShadowConstantBuffer");

        //pipelines
        const foundation::Name DEFAULT_GRAPHICS_PIPELINE = foundation::Name("DefaultGraphicsPipeline");
        const foundation::Name SHADOWMAP_GRAPHICS_PIPELINE = foundation::Name("ShadowMapGraphicsPipeline");

        //samplers
        const foundation::Name DEFAULT_SAMPLER = foundation::Name("DefaultSampler");

        //descriptor sets and layouts
        //single 
        const foundation::Name MODEL_DATA_SET_LAYOUT = foundation::Name("ModelDataSetLayout");
        //combination
        const foundation::Name GLOBAL_BUFFER_SET = foundation::Name("GlobalBufferSet(Layout)");
        const foundation::Name OPAQUE_MATERIAL_SET = foundation::Name("OpaqueInstancesMaterialSet(Layout)");
        const foundation::Name DEPTH_PASS_SET = foundation::Name("DepthPassSet(Layout)");
        const foundation::Name DEFAULT_PASS_SET = foundation::Name("DefaultPassSet(Layout)");
    }
}