#include "ResourceBarrier.h"

namespace psm
{
    namespace graph
    {
        void ResourceMemoryBarrierCollection::AddBarrier(const ResourceMemeoryBarrier& barrier)
        {
            mResourceMemoryBarriers.push_back(barrier);
        }

        void ResourceMemoryBarrierCollection::AddBarriers(const ResourceMemoryBarrierCollection& barriers)
        {
            mResourceMemoryBarriers.insert(mResourceMemoryBarriers.end(), barriers.mResourceMemoryBarriers.begin(), barriers.mResourceMemoryBarriers.end());
        }

        ResourceMemeoryBarrier::~ResourceMemeoryBarrier()
        {

        }
    }
}