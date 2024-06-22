#pragma once

#include "Foundation/Name.h"
#include "RenderPassMetadata.h"

namespace psm
{
    namespace graph
    {
        class RenderPass
        {
        public:
            RenderPass(foundation::Name name, RenderPassPurporse purporse = RenderPassPurporse::Default)
                : mMetaData{ name, purporse }
            {

            }

            virtual ~RenderPass() = 0;

            virtual void Render() = 0;
            virtual void SetupDescriptorSets() = 0;
            virtual void SetupPipeline() = 0;
            virtual void ScheduleResources() = 0;
            virtual void ScheduleSubpasses() = 0;
            virtual void ScheduleSamplers() = 0;

            inline const RenderPassMetadata& MetaData() const
            {
                return mMetaData;
            }

        private:
            RenderPassMetadata mMetaData;
        };
    }
}
