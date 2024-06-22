#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Foundation/Name.h"

#include "Node.h"
#include "DependencyLevel.h"

namespace psm
{
    namespace graph
    {
        class RenderPassGraph
        {
        public:
            using NodeList = std::vector<Node>;
            using NodeListIterator = NodeList::iterator;
            using ResourceUsageTimeline = std::pair<uint64_t, uint64_t>;
            using ResourceUsageTimelines = std::unordered_map<foundation::Name, ResourceUsageTimeline>;

            static SubresourceName ConstructSubresourceName(foundation::Name resourceName, uint32_t subresourceIndex);
            static std::pair<foundation::Name, uint32_t> DecodeSubresourceName(SubresourceName name);

            uint64_t NodeCountForQueue(uint64_t queueIndex) const;
            const ResourceUsageTimeline& GetResourceUsageTimeline(foundation::Name resourceName) const;
            const Node* GetNodeThatWritesToSubresource(SubresourceName subresourceName) const;

            uint64_t AddPass(const RenderPassMetadata& passMetadata);

            void Build();
            void Clear();
        private:
            using DependencyLevelList = std::vector<DependencyLevel>;
            using OrderedNodeList = std::vector<Node*>;
            using RenderPassRegistry = std::unordered_set<foundation::Name>;
            using QueueNodeCounters = std::unordered_map<uint64_t, uint64_t>;
            using AdjacencyLists = std::vector<std::vector<uint64_t>>;
            using WrittenSubresourceToPassMap = std::unordered_map<SubresourceName, const Node*>;

            struct SyncCoverage
            {
                const Node* NodeToSyncWith = nullptr;
                uint64_t NodeToSyncWithIndex = 0;
                std::vector<uint64_t> SyncedQueueIndices;
            };

            void EnsureRenderPassUniqueness(foundation::Name passName);
            void BuildAdjacencyLists();
            void DepthFirstSearch(uint64_t nodeIndex, std::vector<bool>& visited, std::vector<bool>& onStack, bool& isCyclic);
            void TopologicalSort();
            void BuildDependencyLevels();
            void FinalizeDependencyLevels();
            void CullRedundantSynchronizations();

            NodeList mPassNodes;
            AdjacencyLists mAdjacencyLists;
            DependencyLevelList mDependencyLevels;

            // In order to avoid any unambiguity in graph nodes execution order
            // and avoid cyclic dependencies to make graph builds fully automatic
            // we must ensure that there can only be one write dependency for each subresource in a frame
            WriteDependencyRegistry mGlobalWriteDependencyRegistry;

            ResourceUsageTimelines mResourceUsageTimelines;
            RenderPassRegistry mRenderPassRegistry;
            QueueNodeCounters mQueueNodeCounters;
            OrderedNodeList mTopologicallySortedNodes;
            OrderedNodeList mNodesInGlobalExecutionOrder;
            WrittenSubresourceToPassMap mWrittenSubresourceToPassMap;
            uint64_t mDetectedQueueCount = 1;
            std::vector<std::vector<const Node*>> mNodesPerQueue;
            std::vector<const Node*> mFirstNodesThatUseRayTracing;

        public:
            inline const auto& NodesInGlobalExecutionOrder() const
            {
                return mNodesInGlobalExecutionOrder;
            }
            inline const auto& Nodes() const
            {
                return mPassNodes;
            }
            inline auto& Nodes()
            {
                return mPassNodes;
            }
            inline const auto& DependencyLevels() const
            {
                return mDependencyLevels;
            }
            inline auto DetectedQueueCount() const
            {
                return mDetectedQueueCount;
            }
            inline const auto& NodesForQueue(Node::QueueIndex queueIndex) const
            {
                return mNodesPerQueue[queueIndex];
            }
            inline const Node* FirstNodeThatUsesRayTracingOnQueue(Node::QueueIndex queueIndex) const
            {
                return mFirstNodesThatUseRayTracing[queueIndex];
            }
        };
    }
}