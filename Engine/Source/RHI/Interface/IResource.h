#pragma once

#include "Render/RenderGraph/ResourceState.h"

namespace psm
{
    class IResource
    {
    public:

        virtual ~IResource() = default;
        virtual uint32_t SubresourceCount() const = 0;

    private:
        graph::ResourceState mInitialState = graph::ResourceState::Common;
        graph::ResourceState mExpectedState = graph::ResourceState::Common;

    public:
        inline auto InitialStates() const
        {
            return mInitialState;
        };
        inline auto ExpectedStates() const
        {
            return mExpectedState;
        };

   /* public:
        virtual void SetDebugName(const std::string& name);

        GraphicAPIObject() = default;
        GraphicAPIObject(GraphicAPIObject&& that) = default;
        GraphicAPIObject& operator=(GraphicAPIObject&& that) = default;

        virtual ~GraphicAPIObject() = 0;

    protected:
        GraphicAPIObject(const GraphicAPIObject& that) = default;
        GraphicAPIObject& operator=(const GraphicAPIObject& that) = default;*/
    };
}