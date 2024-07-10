#pragma once

#include <unordered_map>
#include <memory>

#include "ResourceMediator.h"

#include "RHI/Interface/Device.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Configs/Barriers.h"
#include "RHI/Interface/Image.h"

namespace psm
{
    namespace graph
    {
        class ResourceStateManager
        {
        public:
            struct SImageResourceStateData
            {
                EImageLayout CurrentLayout;
                EPipelineStageFlags CurrentStageFlags;
                EAccessFlags CurrentAccessFlags;
            };

            struct SImageTransitionData
            {
                ImagePtr Image;
                EFormat Format;
                uint32_t MipLevel;
                EImageAspect Aspect;

                EImageLayout NewLayout;
                EPipelineStageFlags NewStageFlags;
                EAccessFlags NewAccessFlags;
            };

        public:
            ResourceStateManager(const DevicePtr& device, const ResourceMediatorPtr& mediator);

            void TransitionImageLayout(const CommandBufferPtr& commandBuffer,
                                       const SImageTransitionData& data);

        private:
            DevicePtr mDeviceInternal;
            ResourceMediatorPtr mMediator;

            std::unordered_map<ImagePtr, SImageResourceStateData> mImageStates;
        };

        using ResourceStateManagerPtr = std::shared_ptr<ResourceStateManager>;
    }
}