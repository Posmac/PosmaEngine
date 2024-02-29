#pragma once

namespace psm
{
    class IRenderPass
    {
    public:
        IRenderPass() = default;
        virtual ~IRenderPass() = default;

        virtual void BeginRenderPass() = 0;
        virtual void SetViewportAndScissors() = 0;
    };
}