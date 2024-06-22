#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "RHI/Interface/IResource.h"
#include "RHI/Enums/ImageFormats.h"

#include "ResourceState.h"
#include "ResourceBarrier.h"

namespace psm
{
    namespace graph
    {
        //later move to HAL namespace
        class ResourceStateTracker
        {
        public:
            struct SubresourceState
            {
                uint64_t SubresourceIndex = 0;
                ResourceState ResourceState = ResourceState::Common;
            };

            using SubresourceStateList = std::vector<SubresourceState>;

            void StartTrakingResource(const IResource* resource);
            void StopTrakingResource(const IResource* resource);

            // Queue state update but wait until ApplyRequestedTransitions
            void RequestTransition(const IResource* resource, ResourceState newState);
            void RequestTransitions(const IResource* resource, const SubresourceStateList& newStates);

            // Register new resource states that are currently pending and return a corresponding barrier collection
            ResourceMemoryBarrierCollection ApplyRequestedTransitions(bool tryApplyImplicitly = false);

            // Immediately record new state for a resource 
            ResourceMemoryBarrierCollection TransitionToStateImmediately(const IResource* resource, ResourceState newState, bool tryApplyImplicitly = false);
            ResourceMemoryBarrierCollection TransitionToStatesImmediately(const IResource* resource, const SubresourceStateList& newStates, bool tryApplyImplicitly = false);
            std::optional<ResourceMemeoryBarrier> TransitionToStateImmediately(const IResource* resource, ResourceState newState, uint64_t subresourceIndex, bool tryApplyImplicitly = false);

            const SubresourceStateList& ResourceCurrentStates(const IResource* resource) const;

        private:
            using ResourceStateMap = std::unordered_map<const IResource*, SubresourceStateList>;
            using ResourceStateIterator = ResourceStateMap::iterator;

            SubresourceStateList& GetResourceCurrentStatesInternal(const IResource* resource);

            ResourceStateMap mCurrentResourceStates;
            ResourceStateMap mPendingResourceStates;
        };
    }
}