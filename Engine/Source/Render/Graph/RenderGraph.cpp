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
            for(auto& rp : mGraphicsRenderPassNodes)
                rp = nullptr;

            for(auto& rp : mComputeRenderPassNodes)
                rp = nullptr;

            mGraphicsRenderPassNodes.clear();
        }

        void RenderGraph::AddGraphicsRenderPass(RenderPassNodePtr& renderPass)
        {
            auto it = mRenderPassNodesMeta.find(renderPass->GetName());
            if(it != mRenderPassNodesMeta.end())
            {
                LogMessage(MessageSeverity::Warning, "Graphics RenderPass with name: " + renderPass->GetName().ToString() + " already exists!");
                return;
            }

            mRenderPassNodesMeta.insert(renderPass->GetName());
            mGraphicsRenderPassNodes.push_back(renderPass);
            mAllRenderPassNodes.push_back(renderPass);
        }

        void RenderGraph::AddComputeRenderPass(RenderPassNodePtr& renderPass)
        {
            auto it = mRenderPassNodesMeta.find(renderPass->GetName());
            if(it != mRenderPassNodesMeta.end())
            {
                LogMessage(MessageSeverity::Warning, "Compute RenderPass with name: " + renderPass->GetName().ToString() + " already exists!");
                return;
            }

            mRenderPassNodesMeta.insert(renderPass->GetName());
            mComputeRenderPassNodes.push_back(renderPass);
            mAllRenderPassNodes.push_back(renderPass);
        }

        void RenderGraph::GenerateResourceDependencies(uint32_t framesCount)
        {
            if(mGraphicsRenderPassNodes.size() == 0)
                return;

            for(auto& renderPass : mGraphicsRenderPassNodes)
            {
                renderPass->CollectReferences(framesCount);
            }

            for(auto& renderPass : mComputeRenderPassNodes)
            {
                renderPass->CollectReferences(framesCount);
            }

            for(auto& renderPass : mGraphicsRenderPassNodes)
            {
                renderPass->AddResourceReferences(framesCount);
            }

            for(auto& renderPass : mComputeRenderPassNodes)
            {
                renderPass->AddResourceReferences(framesCount);
            }
        }

        void RenderGraph::Update()
        {
        }

        void RenderGraph::Render()
        {
            for(auto& rp : mAllRenderPassNodes)
            {
                rp->PreRender();
                rp->Render();
                rp->PostRender();
            }
        }

        std::vector<RenderPassNodePtr>& RenderGraph::RenderPassNodes()
        {
            return mAllRenderPassNodes;
        }

        std::vector<RenderPassNodePtr>& RenderGraph::GraphicsNodes()
        {
            return mGraphicsRenderPassNodes;
        }

        std::vector<RenderPassNodePtr>& RenderGraph::ComputeNodes()
        {
            return mComputeRenderPassNodes;
        }
    }
}