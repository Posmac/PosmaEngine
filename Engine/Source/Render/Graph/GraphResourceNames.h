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

        //gbuffer targets 
        const foundation::Name GBUFFER_ALBEDO_RENDERTARGET_NAME = foundation::Name("GbufferAlbedoTarget");
        const foundation::Name GBUFFER_NORMAL_RENDERTARGET_NAME = foundation::Name("GbufferNormalTarget");
        const foundation::Name GBUFFER_WORLDPOS_RENDERTARGET_NAME = foundation::Name("GbufferWorldPosTarget");
        const foundation::Name GBUFFER_DEPTH_RENDERTARGET_NAME = foundation::Name("GbufferDepthTarget");
        const foundation::Name GBUFFER_EMISSION_RENDERTARGET_NAME = foundation::Name("GbufferEmissionTarget");
        const foundation::Name GBUFFER_SPECULAR_GLOSS_RENDERTARGET_NAME = foundation::Name("GbufferSpecularGlossTarget");
        const foundation::Name GBUFFER_ROUGH_METAL_RENDERTARGET_NAME = foundation::Name("GbufferRoughMetallTarget");
        const foundation::Name GBUFFER_DEPTHSTECIL_RENDERTARGET_NAME = foundation::Name("GbufferDepthStencilRenderTarget");

        //vis buffer targets
        const foundation::Name VISBUF_GENERATION_RENDERTARGET_NAME = foundation::Name("VisibilityBufferGenerationRenderTarget");
        const foundation::Name VISBUF_DEPTHSTECIL_RENDERTARGET_NAME = foundation::Name("VisibilityBufferDepthStencilRenderTarget");

        //render passes
        const foundation::Name COMPOSITE_RENDERPASS = foundation::Name("CompositeBackbufferRenderPass");
        const foundation::Name SHADOWMAP_RENDERPASS = foundation::Name("ShadowMapRenderPass");
        const foundation::Name GBUFFER_RENDERPASS = foundation::Name("GbufferRenderPass");
        const foundation::Name VISBUF_GENERATION_RENDERPASS = foundation::Name("VisibilityBufferGenerationRenderPass");
        const foundation::Name VISBUF_SHADE_RENDERPASS = foundation::Name("VisibilityBufferShadeRenderPass");

        //general buffers input
        const foundation::Name GLOBAL_CBUFFER = foundation::Name("GlobalConstantBuffer");
        const foundation::Name SHADOW_CBUFFER = foundation::Name("ShadowConstantBuffer");

        //pipelines
        const foundation::Name COMPOSITE_GRAPHICS_PIPELINE = foundation::Name("CompositeGraphicsPipeline");
        const foundation::Name SHADOWMAP_GRAPHICS_PIPELINE = foundation::Name("ShadowMapGraphicsPipeline");
        const foundation::Name GBUFFER_GRAPHICS_PIPELINE = foundation::Name("GbufferGraphicsPipeline");
        const foundation::Name VISBUF_GENERATION_PIPELINE = foundation::Name("VisibilityBufferGenerationPipeline");
        const foundation::Name VISBUF_SHADE_PIPELINE = foundation::Name("VisibilityBufferShadePipeline");

        //samplers
        const foundation::Name DEFAULT_SAMPLER = foundation::Name("DefaultSampler");

        //descriptor sets and layouts
        const foundation::Name MODEL_DATA_SET = foundation::Name("MODEL_DATA_SET");
        const foundation::Name GLOBAL_CBUFFER_SET = foundation::Name("GLOBAL_CBUFFER_SET");
        const foundation::Name OPAQUE_INSTANCES_MATERIALS_SET = foundation::Name("OPAQUE_INSTANCES_MATERIALS_SET");
        const foundation::Name SHADOW_MAP_GENERATION_PER_VIEW_CBUFFER_SET = foundation::Name("SHADOW_MAP_GENERATION_PER_VIEW_CBUFFER_SET");
        const foundation::Name COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET = foundation::Name("COMPOSITE_PASS_SHADOW_CBUFFER_SHADOWMAP_SET");
        const foundation::Name GBUFFER_RESULT_COMPOSITE_INPUT_SET = foundation::Name("GBUFFER_RESULT_COMPOSITE_INPUT_SET");
        const foundation::Name VISBUF_SHADE_SET = foundation::Name("VISBUF_SHADE_SET");
        const foundation::Name VISBUF_SHADE_SET_LAYOUT = foundation::Name("VISBUF_SHADE_SET_LAYOUT");
    }
}