#include "RenderPassContainer.h"

#include "RenderPass.h"
#include "RenderSubpass.h"
#include "RenderGraph.h"

namespace psm
{
    namespace graph
    {
        void RenderPassContainer::AddRenderPass(RenderPass* pass)
        {
            if(mRenderPasses.contains(pass->MetaData().Name))
            {
                return;
            }

            auto nodeIndex = mRenderPassGraph->AddPass(pass->MetaData());
            mRenderPasses.emplace(pass->MetaData().Name, PassHelper(pass, mRenderPassGraph, nodeIndex));

            //mResourceStorage->CreatePerPassData(pass->MetaData().Name);
        }

        void RenderPassContainer::AddRenderSubPass(RenderSubPass * pass)
        {
            if(mRenderSubPasses.contains(pass->Metadata().Name))
            {
                return;
            }
            
            auto nodeIndex = mRenderPassGraph->AddPass(pass->Metadata());
            mRenderSubPasses.emplace(pass->Metadata().Name,  
                                     PassHelper(pass, mRenderPassGraph, nodeIndex));

            //mResourceStorage->CreatePerPassData(pass->MetaData().Name);
        }

        RenderPassContainer::PassHelper<RenderPass>* RenderPassContainer::GetRenderPass(foundation::Name passName)
        {
            auto it = mRenderPasses.find(passName);
            if(it == mRenderPasses.end())
            {
                return nullptr;
            }

            return &(it->second);
        }

        RenderPassContainer::PassHelper<RenderSubPass>* RenderPassContainer::GetRenderSubPass(foundation::Name passName)
        {
            auto it = mRenderSubPasses.find(passName);
            if(it == mRenderSubPasses.end())
            {
                return nullptr;
            }

            return &(it->second);
        }
    }
}