#include "ResourceStateTracker.h"

namespace psm
{
    namespace graph
    {
        void ResourceStateTracker::StartTrakingResource(const IResource* resource)
        {
            ResourceStateTracker::SubresourceStateList& currentStates = mCurrentResourceStates[resource];
            currentStates.resize(resource->SubresourceCount(), {0, resource->InitialStates()});
            
            for(auto subresourceIdx = 0u; subresourceIdx < resource->SubresourceCount(); ++subresourceIdx)
            {
                currentStates[subresourceIdx].SubresourceIndex = subresourceIdx;
            }
        }

        void ResourceStateTracker::StopTrakingResource(const IResource* resource)
        {
            mCurrentResourceStates.erase(resource);
        }

        void ResourceStateTracker::RequestTransition(const IResource* resource, ResourceState newState)
        {
            ResourceStateTracker::SubresourceStateList& pendingStates = mPendingResourceStates[resource];
            pendingStates.resize(resource->SubresourceCount(), { 0, newState });

            for(auto subresourceIdx = 0; subresourceIdx < resource->SubresourceCount(); ++subresourceIdx)
            {
                pendingStates[subresourceIdx].SubresourceIndex = subresourceIdx;
            }
        }

        void ResourceStateTracker::RequestTransitions(const IResource* resource, const SubresourceStateList& newStates)
        {
            ResourceStateTracker::SubresourceStateList& pendingStates = mPendingResourceStates[resource];
            pendingStates.insert(pendingStates.end(), newStates.begin(), newStates.end());
        }

        ResourceMemoryBarrierCollection ResourceStateTracker::ApplyRequestedTransitions(bool tryApplyImplicitly)
        {
            ResourceMemoryBarrierCollection barriers{};

            for(auto& [resource, subresourceStates] : mPendingResourceStates)
            {
                ResourceMemoryBarrierCollection resourceBarriers = TransitionToStatesImmediately(resource, subresourceStates, tryApplyImplicitly);
                barriers.AddBarriers(resourceBarriers);
            }

            mPendingResourceStates.clear();
            return barriers;
        }

        ResourceMemoryBarrierCollection ResourceStateTracker::TransitionToStateImmediately(const IResource* resource, ResourceState newState, bool tryApplyImplicitly)
        {
            SubresourceStateList& currentSubresourceStates = GetResourceCurrentStatesInternal(resource);
            ResourceMemoryBarrierCollection newStateBarriers = {};
            ResourceState firstCurrentState = currentSubresourceStates.front().ResourceState;

            bool subresourceStatesMatch = true;

            for(SubresourceState& subresourceState : currentSubresourceStates)
            {
                ResourceState oldState = subresourceState.ResourceState;

                subresourceState.ResourceState = newState;

                newStateBarriers.AddBarrier({ oldState, newState, resource, subresourceState.SubresourceIndex });

                if(oldState != firstCurrentState)
                {
                    subresourceStatesMatch = false;
                }
            }

            if(subresourceStatesMatch && newStateBarriers.BarrierCount() > 1)
            {
                ResourceMemoryBarrierCollection singleBarrierCollection{};
                singleBarrierCollection.AddBarrier({ firstCurrentState, newState, resource });
                return singleBarrierCollection;
            }

            return newStateBarriers;
        }

        ResourceMemoryBarrierCollection ResourceStateTracker::TransitionToStatesImmediately(const IResource* resource, const ResourceStateTracker::SubresourceStateList& newStates, bool tryApplyImplicitly)
        {
            SubresourceStateList& currentSubresourceStates = GetResourceCurrentStatesInternal(resource);

            ResourceMemoryBarrierCollection newStateBarriers{};

            bool statesMatch = true;

            ResourceState firstNewState = newStates.front().ResourceState;
            ResourceState firstOldState = currentSubresourceStates.front().ResourceState;

            for(const SubresourceState& newSubresourceState : newStates)
            {
                SubresourceState& currentState = currentSubresourceStates[newSubresourceState.SubresourceIndex];
                ResourceState oldState = currentState.ResourceState;
                ResourceState newState = newSubresourceState.ResourceState;

                currentState.ResourceState = newSubresourceState.ResourceState;

                newStateBarriers.AddBarrier({ oldState, newState, resource, newSubresourceState.SubresourceIndex });

                // If any old subresource states do not match or any of the new states do not match
                // then performing single transition barrier for all subresources is not possible
                if(oldState != firstOldState || newState != firstNewState)
                {
                    statesMatch = false;
                }
            }

            // If multiple transitions were requested, but it's possible to make just one - do it
            if(statesMatch && newStateBarriers.BarrierCount() > 1)
            {
                ResourceMemoryBarrierCollection singleBarrierCollection{};
                singleBarrierCollection.AddBarrier({ firstOldState, firstNewState, resource });
                return singleBarrierCollection;
            }

            return newStateBarriers;
        }

        std::optional<ResourceMemeoryBarrier> ResourceStateTracker::TransitionToStateImmediately(const IResource* resource, ResourceState newState, uint64_t subresourceIndex, bool tryApplyImplicitly)
        {
            SubresourceStateList& currentSubresourceStates = GetResourceCurrentStatesInternal(resource);
            ResourceState oldState = currentSubresourceStates[subresourceIndex].ResourceState;

            currentSubresourceStates[subresourceIndex].ResourceState = newState;

            ResourceMemeoryBarrier barrier = { oldState, newState, resource, subresourceIndex };
            return barrier;
        }

        const ResourceStateTracker::SubresourceStateList& ResourceStateTracker::ResourceCurrentStates(const IResource* resource) const
        {
            auto it = mCurrentResourceStates.find(resource);
            //assert_format(it != mCurrentResourceStates.end(), "Resource is not registered / not being tracked. It may have been deallocated before transitions were applied.");
            return it->second;
        }

        ResourceStateTracker::SubresourceStateList& ResourceStateTracker::GetResourceCurrentStatesInternal(const IResource* resource)
        {
            auto it = mCurrentResourceStates.find(resource);
            //assert_format(it != mCurrentResourceStates.end(), "Resource is not registered / not being tracked");
            return it->second;
        }
    }
}