#pragma once

#include <list>
#include <vector>
#include <unordered_set>

#include "Node.h"

namespace psm
{
    namespace graph
    {
        class RenderPassGraph;

        class DependencyLevel
        {
        public:
            friend RenderPassGraph;

            using NodeList = std::list<Node*>;
            using NodeIterator = NodeList::iterator;

        public:
            inline const auto& Nodes() const
            {
                return mNodes;
            }

            inline const auto& NodesForQueue(Node::QueueIndex queueIndex) const
            {
                return mNodesPerQueue[queueIndex];
            }

            inline const auto& QueuesInvoledInCrossQueueResourceReads() const
            {
                return mQueuesInvoledInCrossQueueResourceReads;
            }

            inline const auto& SubresourcesReadByMultipleQueues() const
            {
                return mSubresourcesReadByMultipleQueues;
            }

            inline auto LevelIndex() const
            {
                return mLevelIndex;
            }

        private:
            void AddNode(Node* node);
            Node* RemoveNode(NodeIterator it);

            uint64_t mLevelIndex = 0;
            NodeList mNodes;
            std::vector<std::vector<const Node*>> mNodesPerQueue;

            // Storage for queues that read at least one common resource. Resource state transitions
            // for such queues need to be handled differently.
            std::unordered_set<Node::QueueIndex> mQueuesInvoledInCrossQueueResourceReads;
            std::unordered_set<SubresourceName> mSubresourcesReadByMultipleQueues;
        };
    }
}