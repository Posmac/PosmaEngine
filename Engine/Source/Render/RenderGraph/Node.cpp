#include "Node.h"

#include "RenderGraph.h"
#include "RHI/RHICommon.h"

namespace psm
{
    namespace graph
    {
        Node::Node(const RenderPassMetadata& passMetaData, WriteDependencyRegistry* writeDependencyRegistry)
            : mPassMetaData(passMetaData), mWriteDependencyRegistry(writeDependencyRegistry)
        {

        }

        bool Node::operator==(const Node & node) const
        {
            return mPassMetaData.Name == node.mPassMetaData.Name;
        }

        bool Node::operator!=(const Node& node) const
        {
            return mPassMetaData.Name != node.mPassMetaData.Name;
        }

        void Node::AddReadDependency(foundation::Name resourceName, uint32_t subresourceCount)
        {
            AddReadDependency(resourceName, 0, subresourceCount - 1);
        }

        void Node::AddReadDependency(foundation::Name resourceName, uint32_t firstSubresourceIndex, uint32_t lastSubresourceIndex)
        {
            for(auto i = firstSubresourceIndex; i <= lastSubresourceIndex; ++i)
            {
                SubresourceName name = RenderPassGraph::ConstructSubresourceName(resourceName, i);
                mReadSubresources.insert(name);
                mReadAndWrittenSubresources.insert(name);
                mAllResources.insert(resourceName);
            }
        }

        void Node::AddReadDependency(foundation::Name resourceName, const SubresourceList& subresources)
        {
            if(subresources.empty())
            {
                AddReadDependency(resourceName, 1);
                return;
            }

            for(auto subresourceIndex : subresources)
            {
                SubresourceName name = RenderPassGraph::ConstructSubresourceName(resourceName, subresourceIndex);
                mReadSubresources.insert(name);
                mReadAndWrittenSubresources.insert(name);
                mAllResources.insert(resourceName);
            }
        }

        void Node::AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, uint32_t subresourceCount)
        {
            AddWriteDependency(resourceName, originalResourceName, 0, subresourceCount - 1);
        }

        void Node::AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, uint32_t firstSubresourceIndex, uint32_t lastSubresourceIndex)
        {
            for(auto i = firstSubresourceIndex; i <= lastSubresourceIndex; ++i)
            {
                SubresourceName name = RenderPassGraph::ConstructSubresourceName(resourceName, i);
                EnsureSingleWriteDependency(name);
                mWrittenSubresources.insert(name);
                mReadAndWrittenSubresources.insert(name);
                mAllResources.insert(resourceName);

                if(originalResourceName)
                {
                    SubresourceName originalSubresoruce = RenderPassGraph::ConstructSubresourceName(*originalResourceName, i);
                    mAliasedSubresources.insert(originalSubresoruce);
                    mAllResources.insert(*originalResourceName);
                }
            }
        }

        void Node::AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, const SubresourceList & subresources)
        {
            if(subresources.empty())
            {
                AddWriteDependency(resourceName, originalResourceName, 1);
            }
            else
            {
                for(auto subresourceIndex : subresources)
                {
                    SubresourceName name = RenderPassGraph::ConstructSubresourceName(resourceName, subresourceIndex);
                    EnsureSingleWriteDependency(name);
                    mWrittenSubresources.insert(name);
                    mReadAndWrittenSubresources.insert(name);
                    mAllResources.insert(resourceName);
                }
            }
        }

        bool Node::HasDependency(foundation::Name resourceName, uint32_t subresourceIndex) const
        {
            return HasDependency(RenderPassGraph::ConstructSubresourceName(resourceName, subresourceIndex));
        }

        bool Node::HasDependency(SubresourceName subresourceName) const
        {
            return mReadAndWrittenSubresources.find(subresourceName) != mReadAndWrittenSubresources.end();
        }

        bool Node::HasAnyDependencies() const
        {
            return !mReadAndWrittenSubresources.empty();
        }

        void Node::EnsureSingleWriteDependency(SubresourceName name)
        {
            auto [resourceName, subresourceIndex] = RenderPassGraph::DecodeSubresourceName(name);
            auto it = mWriteDependencyRegistry->find(name);

            if(it == mWriteDependencyRegistry->end())
            {
                LogMessage(MessageSeverity::Error, "Resource " + resourceName.ToString() + ", subresource " + std::to_string(subresourceIndex) + 
                           " already has a write dependency in " + it->second.ToString() + ". " +
                           "Use Aliases to perform multiple writes into the same resource in " + mPassMetaData.Name.ToString() + " pass.");
            }

            mWriteDependencyRegistry->insert({ name, mPassMetaData.Name });
        }

        void Node::Clear()
        {
            mReadSubresources.clear();
            mWrittenSubresources.clear();
            mReadAndWrittenSubresources.clear();

            //mAllResources.clear();

            mAliasedSubresources.clear();
            mNodesToSyncWith.clear();
            mSyncronizationIndexSet.clear();
            mDependencyLevelIndex = 0;
            mSyncSignalRequired = false;
            ExecutionQueueIndex = 0;
            mGlobalExecutionIndex = 0;
            mLocalToDependencyLevelExecutionIndex = 0;
        }
    }
}