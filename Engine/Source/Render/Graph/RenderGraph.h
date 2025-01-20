#pragma once

#include "Foundation/Name.h"
#include "RenderPassNode.h"
#include "RHI/Interface/Buffer.h"
#include "RHI/Interface/Image.h"
#include "ResourceAliases.h"
#include "ResourceMediator.h"

#include <unordered_map>
#include <set>
#include <vector>
#include <memory>

namespace psm
{
    namespace graph
    {
        class RenderGraph
        {
        public:
            RenderGraph();
            RenderGraph(const ResourceMediatorPtr& mediator);
            virtual ~RenderGraph();

            void AddGraphicsRenderPass(RenderPassNodePtr& renderPass);
            void AddComputeRenderPass(RenderPassNodePtr& renderPass);
            void GenerateResourceDependencies(uint32_t framesCount);

            void Update();
            void Render();

            std::vector<RenderPassNodePtr>& RenderPassNodes();
            std::vector<RenderPassNodePtr>& GraphicsNodes();
            std::vector<RenderPassNodePtr>& ComputeNodes();

        private:
            ResourceMediatorPtr mResourceMediator;

            std::set<foundation::Name> mRenderPassNodesMeta;
            std::vector<RenderPassNodePtr> mAllRenderPassNodes;
            std::vector<RenderPassNodePtr> mGraphicsRenderPassNodes;
            std::vector<RenderPassNodePtr> mComputeRenderPassNodes;
        };

        using RenderGraphPtr = std::shared_ptr<RenderGraph>;
    }
}