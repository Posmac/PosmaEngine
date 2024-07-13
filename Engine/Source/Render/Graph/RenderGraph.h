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

            void AddRenderPass(RenderPassNodePtr& renderPass);
            void GenerateResourceDependencies(uint32_t framesCount);

            std::vector<RenderPassNodePtr>::iterator begin();
            std::vector<RenderPassNodePtr>::const_iterator begin() const;

            std::vector<RenderPassNodePtr>::iterator end();
            std::vector<RenderPassNodePtr>::const_iterator end() const;

        private:
            ResourceMediatorPtr mResourceMediator;

            std::set<foundation::Name> mRenderPassNodesMeta;
            std::vector<RenderPassNodePtr> mRenderPassNodes;
        };

        using RenderGraphPtr = std::shared_ptr<RenderGraph>;
    }
}