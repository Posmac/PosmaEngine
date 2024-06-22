#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "RenderPassMetadata.h"
#include "Foundation/Name.h"

namespace psm
{
    namespace graph
    {
        class RenderPassGraph;

        using SubresourceName = uint64_t;
        using WriteDependencyRegistry = std::unordered_map<SubresourceName, foundation::Name>;

        class Node
        {
        public:
            inline static const foundation::Name BackbufferName = "Backbuffer_";

            using SubresourceList = std::vector<uint32_t>;
            using QueueIndex = uint64_t;

            Node(const RenderPassMetadata& passMetaData, WriteDependencyRegistry* writeDependencyRegistry);

            bool operator==(const Node& node) const;
            bool operator!=(const Node& node) const;

            void AddReadDependency(foundation::Name resourceName, uint32_t subresourceCount);
            void AddReadDependency(foundation::Name resourceName, uint32_t firstSubresourceIndex, uint32_t lastSubresourceIndex);
            void AddReadDependency(foundation::Name resourceName, const SubresourceList& subresource);

            void AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, uint32_t subresourceCount);
            void AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, uint32_t firstSubresourceIndex, uint32_t lastSubresourceIndex);
            void AddWriteDependency(foundation::Name resourceName, std::optional<foundation::Name> originalResourceName, const SubresourceList& subresources);

            bool HasDependency(foundation::Name resourceName, uint32_t subresourceIndex) const;
            bool HasDependency(SubresourceName subresourceName) const;
            bool HasAnyDependencies() const;

            uint64_t ExecutionQueueIndex = 0;
        public:
            inline const auto& PassMetadata() const
            {
                return mPassMetaData;
            }

            inline const auto& ReadSubresources() const
            {
                return mReadSubresources;
            }

            inline const auto& WrittenSubresources() const
            {
                return mWrittenSubresources;
            }

            inline const auto& ReadAndWritten() const
            {
                return mReadAndWrittenSubresources;
            }

            inline const auto& AllResources() const
            {
                return mAllResources;
            }

            inline const auto& NodesToSyncWith() const
            {
                return mNodesToSyncWith;
            }

            inline auto GlobalExecutionIndex() const
            {
                return mGlobalExecutionIndex;
            }

            inline auto DependencyLevelIndex() const
            {
                return mDependencyLevelIndex;
            }

            inline auto LocalToDependencyLevelExecutionIndex() const
            {
                return mLocalToDependencyLevelExecutionIndex;
            }

            inline auto LocalToQueueExecutionIndex() const
            {
                return mLocalToQueueExecutionIndex;
            }

            inline bool IsSyncSignalRequired() const
            {
                return mSyncSignalRequired;
            }

        private:

            using SyncronizationIndexSet = std::vector<uint64_t>;
            inline static const uint64_t InvalidSyncrozationIndex = std::numeric_limits<uint64_t>::max();

            friend RenderPassGraph;

            void EnsureSingleWriteDependency(SubresourceName name);
            void Clear();

            uint64_t mGlobalExecutionIndex = 0;
            uint64_t mDependencyLevelIndex = 0;
            uint64_t mLocalToDependencyLevelExecutionIndex = 0;
            uint64_t mLocalToQueueExecutionIndex = 0;
            uint64_t mIndexInUnorderedList = 0;

            RenderPassMetadata mPassMetaData;
            WriteDependencyRegistry* mWriteDependencyRegistry = nullptr;

            std::unordered_set<SubresourceName> mReadSubresources;
            std::unordered_set<SubresourceName> mWrittenSubresources;
            std::unordered_set<SubresourceName> mReadAndWrittenSubresources;

            std::unordered_set<SubresourceName> mAliasedSubresources;

            std::unordered_set<foundation::Name> mAllResources;

            SyncronizationIndexSet mSyncronizationIndexSet;
            std::vector<const Node*> mNodesToSyncWith;
            bool mSyncSignalRequired = false;
        };
    }
}