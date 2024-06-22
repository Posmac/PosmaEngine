#pragma once

#include <cstdint>
#include <unordered_map>

#include "Foundation/Name.h"

namespace psm
{
    namespace graph
    {
        class RenderPass;
        class RenderSubPass;
        class RenderPassGraph;

        class RenderPassContainer
        {
        public:
            template<class RenderPassT>
            struct PassHelper
            {
                PassHelper(
                RenderPassT* renderPass,
                RenderPassGraph* graph,
                uint64_t graphNodeIndex
                );

                RenderPassT* Pass;
                RenderPassGraph* Graph;
                uint64_t GraphNodeIndex;
            };

            void AddRenderPass(RenderPass* pass);
            void AddRenderSubPass(RenderSubPass* pass);

            PassHelper<RenderPass>* GetRenderPass(foundation::Name passName);
            PassHelper<RenderSubPass>* GetRenderSubPass(foundation::Name passName);

        private:
            std::unordered_map<foundation::Name, PassHelper<RenderPass>> mRenderPasses;
            std::unordered_map<foundation::Name, PassHelper<RenderSubPass>> mRenderSubPasses;

            RenderPassGraph* mRenderPassGraph = nullptr;

        public:
            inline const auto& RenderPasses() const
            {
                return mRenderPasses;
            }
            inline const auto& RenderSubPasses() const
            {
                return mRenderSubPasses;
            }
            inline auto& RenderPasses()
            {
                return mRenderPasses;
            }
            inline auto& RenderSubPasses()
            {
                return mRenderSubPasses;
            }
        };
    
    }
}