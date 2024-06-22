#pragma once

#include <optional>
#include <vector>

#include "ResourceState.h"
#include "RHI/Interface/IResource.h"

#include "RHI/RHICommon.h"

namespace psm
{
    namespace graph
    {
        class ResourceMemeoryBarrier
        {
        public:
            ResourceMemeoryBarrier(ResourceState beforeStateMask,
                                   ResourceState afterStateMask,
                                   const IResource* resource,
                                   std::optional<uint64_t> subresourceIndex = 0) :
                mResource(resource), mBeforeStates(beforeStateMask), mAfterStates(afterStateMask) {}

            virtual ~ResourceMemeoryBarrier();

            std::pair<ResourceMemeoryBarrier, ResourceMemeoryBarrier> Split() const;

        protected:
            std::optional<VkImageMemoryBarrier> mImageMemoryBarrier;
            std::optional<VkBufferMemoryBarrier> mBufferMemoryBarrier;

        private:
            const IResource* mResource = nullptr;
            ResourceState mBeforeStates = ResourceState::Common;
            ResourceState mAfterStates = ResourceState::Common;

        public:
            inline const auto AssosiatedResource() const
            {
                return mResource;
            }
            inline const auto BeforeStates() const
            {
                return mBeforeStates;
            }
            inline const auto AfterStates() const
            {
                return mAfterStates;
            }

            inline const auto& VulkanBarrier() const 
            {
                return nullptr; 
            };
        };

        class ImageMemoryBarrier : public ResourceMemeoryBarrier
        {

        };

        class BufferMemoryBarrier : public ResourceMemeoryBarrier
        {

        };

        class ResourceMemoryBarrierCollection
        {
        public:
            void AddBarrier(const ResourceMemeoryBarrier& barrier);
            void AddBarriers(const ResourceMemoryBarrierCollection& barriers);

        private:
            std::vector<ResourceMemeoryBarrier> mResourceMemoryBarriers;

        public:
            inline const ResourceMemeoryBarrier* D3DBarriers() const
            {
                return mResourceMemoryBarriers.data();
            }

            inline const size_t BarrierCount() const
            {
                return mResourceMemoryBarriers.size();
            }
        };
    }
}