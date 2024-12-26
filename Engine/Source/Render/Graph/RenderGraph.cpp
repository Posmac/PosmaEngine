#include "RenderGraph.h"

#include "RHI/RHICommon.h"

namespace psm
{
    namespace graph
    {
        RenderGraph::RenderGraph()
        {

        }

        RenderGraph::RenderGraph(const ResourceMediatorPtr& mediator)
            : mResourceMediator(mediator)
        {
        }

        RenderGraph::~RenderGraph()
        {
            LogMessage(MessageSeverity::Info, "RenderGraph destructor");

            mResourceMediator = nullptr;
            for(auto& rp : mRenderPassNodes)
                rp = nullptr;

            mRenderPassNodes.clear();
        }

        void RenderGraph::AddRenderPass(RenderPassNodePtr& renderPass)
        {
            auto it = mRenderPassNodesMeta.find(renderPass->GetName());
            if(it != mRenderPassNodesMeta.end())
            {
                LogMessage(MessageSeverity::Warning, "RenderPass with name: " + renderPass->GetName().ToString() + " already exists!");
                return;
            }

            mRenderPassNodesMeta.insert(renderPass->GetName());
            mRenderPassNodes.push_back(renderPass);
        }

        void RenderGraph::GenerateResourceDependencies(uint32_t framesCount)
        {
            if(mRenderPassNodes.size() == 0)
                return;

            for(auto& renderPass : mRenderPassNodes)
            {
                renderPass->CollectReferences(framesCount);
            }

            for(auto& renderPass : mRenderPassNodes)
            {
                renderPass->AddResourceReferences(framesCount);
            }
        }

        std::vector<RenderPassNodePtr>::iterator RenderGraph::begin()
        {
            return mRenderPassNodes.begin();
        }

        std::vector<RenderPassNodePtr>::const_iterator RenderGraph::begin() const
        {
            return mRenderPassNodes.begin();
        }

        std::vector<RenderPassNodePtr>::iterator RenderGraph::end()
        {
            return mRenderPassNodes.end();
        }

        std::vector<RenderPassNodePtr>::const_iterator RenderGraph::end() const
        {
            return mRenderPassNodes.end();
        }
    }
}