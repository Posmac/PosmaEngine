#include "RenderGraph.h"

#include "RHI/RHICommon.h"

namespace psm
{
    namespace graph
    {
        SubresourceName RenderPassGraph::ConstructSubresourceName(foundation::Name resourceName, uint32_t subresourceIndex)
        {
            SubresourceName name = resourceName.ToId();
            name <<= 32;
            name |= subresourceIndex;
            return name;
        }

        std::pair<foundation::Name, uint32_t> RenderPassGraph::DecodeSubresourceName(SubresourceName name)
        {
            return { foundation::Name{name >> 32}, name & 0x0000FFFF };
        }

        uint64_t RenderPassGraph::NodeCountForQueue(uint64_t queueIndex) const
        {
            auto countIt = mQueueNodeCounters.find(queueIndex);
            return countIt != mQueueNodeCounters.end() ? countIt->second : 0;
        }

        const RenderPassGraph::ResourceUsageTimeline& RenderPassGraph::GetResourceUsageTimeline(foundation::Name resourceName) const
        {
            auto timelineIt = mResourceUsageTimelines.find(resourceName);
            if(timelineIt == mResourceUsageTimelines.end())
                LogMessage(MessageSeverity::Warning, "Resource timeline (" + resourceName.ToString() + ") doesn't exist");
            return timelineIt->second;
        }

        const Node* RenderPassGraph::GetNodeThatWritesToSubresource(SubresourceName subresourceName) const
        {
            auto it = mWrittenSubresourceToPassMap.find(subresourceName);
            if(it == mWrittenSubresourceToPassMap.end())
                LogMessage(MessageSeverity::Warning, "Subresource," + DecodeSubresourceName(subresourceName).first.ToString() + " is not registered for writing in the graph.");
            return it->second;
        }

        uint64_t RenderPassGraph::AddPass(const RenderPassMetadata& passMetadata)
        {
            EnsureRenderPassUniqueness(passMetadata.Name);
            mPassNodes.emplace_back(Node{ passMetadata, &mGlobalWriteDependencyRegistry });
            mPassNodes.back().mIndexInUnorderedList = mPassNodes.size() - 1;
            return mPassNodes.size() - 1;
        }

        void RenderPassGraph::Build()
        {
            BuildAdjacencyLists();
            TopologicalSort();
            BuildDependencyLevels();
            FinalizeDependencyLevels();
            CullRedundantSynchronizations();
        }

        void RenderPassGraph::Clear()
        {
            mGlobalWriteDependencyRegistry.clear();
            mDependencyLevels.clear();
            mResourceUsageTimelines.clear();
            mQueueNodeCounters.clear();
            mTopologicallySortedNodes.clear();
            mNodesInGlobalExecutionOrder.clear();
            mAdjacencyLists.clear();
            mDetectedQueueCount = 1;
            mNodesPerQueue.clear();
            mFirstNodesThatUseRayTracing.clear();

            for(Node& node : mPassNodes)
            {
                node.Clear();
            }
        }

        void RenderPassGraph::EnsureRenderPassUniqueness(foundation::Name passName)
        {}
        void RenderPassGraph::BuildAdjacencyLists()
        {}
        void RenderPassGraph::DepthFirstSearch(uint64_t nodeIndex, std::vector<bool>&visited, std::vector<bool>&onStack, bool& isCyclic)
        {}
        void RenderPassGraph::TopologicalSort()
        {}
        void RenderPassGraph::BuildDependencyLevels()
        {}
        void RenderPassGraph::FinalizeDependencyLevels()
        {}
        void RenderPassGraph::CullRedundantSynchronizations()
        {}
    }
}

