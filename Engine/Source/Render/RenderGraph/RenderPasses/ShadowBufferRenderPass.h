#pragma once

#include "../RenderPass.h"
#include "RHI/Common.h"

#if defined(RHI_VULKAN)
#include "RHI/Vulkan/CVkDevice.h"
#endif

namespace psm
{
    namespace graph
    {
        struct ShadowMapCBContent
        {

        };

        class ShadowMapRenderPass : public RenderPass //for now render pass, later pass content manager
        {
        public:
            ShadowMapRenderPass();
            ~ShadowMapRenderPass() = default;

            virtual void Render() override;
            virtual void SetupDescriptorSets() override;
            virtual void SetupPipeline() override;
            virtual void ScheduleResources() override;
            virtual void ScheduleSubpasses() override;
            virtual void ScheduleSamplers() override;
            
        private:

        };
    }
}