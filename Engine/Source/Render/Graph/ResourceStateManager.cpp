#include "ResourceStateManager.h"

#include "RHI/Vulkan/CVkDevice.h"

namespace psm
{
    namespace graph
    {
        ResourceStateManager::ResourceStateManager(const DevicePtr& device, const ResourceMediatorPtr& mediator)
            : mMediator(mediator), mDeviceInternal(device)
        {

        }

        void ResourceStateManager::TransitionImageLayout(const CommandBufferPtr& commandBuffer, 
                                                         const SImageTransitionData& data)
        {
            auto found = mImageStates.find(data.Image);

            SImageResourceStateData imageStateData = {};

            if(found != mImageStates.end())
            {
                imageStateData = found->second;
            }
            else
            {
                //test
                imageStateData.CurrentLayout = EImageLayout::UNDEFINED;
                imageStateData.CurrentAccessFlags = EAccessFlags::NONE;
                imageStateData.CurrentStageFlags = EPipelineStageFlags::TOP_OF_PIPE_BIT;

                mImageStates.insert({data.Image, imageStateData});
            }

            SImageLayoutTransition imageLayoutTransition =
            {
                .Format = data.Format,

                .OldLayout = imageStateData.CurrentLayout,
                .NewLayout = data.NewLayout,

                .SourceStage = imageStateData.CurrentStageFlags,
                .DestinationStage = data.NewStageFlags,

                .SourceMask = imageStateData.CurrentAccessFlags,
                .DestinationMask = data.NewAccessFlags,

                .ImageAspectFlags = data.Aspect,
                .MipLevel = data.MipLevel,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, data.Image, imageLayoutTransition);

            imageStateData =
            {
                .CurrentLayout = data.NewLayout,
                .CurrentStageFlags = data.NewStageFlags,
                .CurrentAccessFlags = data.NewAccessFlags
            };

            found->second = imageStateData;
        }
    }
}