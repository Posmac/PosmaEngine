#pragma once

#include "Foundation/Name.h"
#include "RenderPassMetadata.h"

namespace psm
{
    namespace graph
    {
        class RenderSubPass
        {
            RenderSubPass(foundation::Name name)
                : mMetadata{ name, RenderPassPurporse::Default } {};

            virtual ~RenderSubPass() = 0;

            virtual void ScheduleResources() = 0;
            virtual void Render() = 0;

        private:
            RenderPassMetadata mMetadata;

        public:
            inline const RenderPassMetadata& Metadata() const { return mMetadata; }
        };
    }
}