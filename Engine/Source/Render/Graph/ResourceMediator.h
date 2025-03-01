#pragma once

#include <unordered_map>
#include <memory>

#include "ResourceAliases.h"
#include "Foundation/Name.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    namespace graph
    {
        class ResourceMediator
        {
        public:

            ResourceMediator(const DevicePtr& device);
            virtual ~ResourceMediator();

            void RegisterImageResource(const foundation::Name& name, const ImagePtr& image);
            void RegisterBufferResource(const foundation::Name& name, const BufferPtr& buffer);
            void RegisterSampler(const foundation::Name& name, const SamplerPtr& sampler);

            void RegisterDescriptorSetLayout(const foundation::Name& name, const DescriptorSetLayoutPtr& sampler);
            void RegisterDescriptorSet(const foundation::Name& name, const DescriptorSetPtr& sampler);

            void RegisterPipeline(const foundation::Name& name, const PipelinePtr& pipeline);

            void UpdateImageReference(const foundation::Name& name, const ImagePtr& image);

            ImagePtr& GetImageByName(const foundation::Name& name);
            BufferPtr& GetBufferByName(const foundation::Name& name);
            SamplerPtr& GetSamplerByName(const foundation::Name& name);
            DescriptorSetLayoutPtr& GetDescriptorSetLayoutByName(const foundation::Name& name);
            DescriptorSetPtr& GetDescriptorSetByName(const foundation::Name& name);
            PipelinePtr& GetPipelineByName(const foundation::Name& name);
        private:

            DevicePtr mDeviceInternal;

            ImageHash mAllImages;
            BufferHash mAllBuffers;
            SamplerHash mAllSamplers;
            PipelineHash mAllPipelines;

            DescriptorSetLayoutHash mAllDescriptorLayouts;
            DescriptorSetHash mAllDescriptors;
        };

        using ResourceMediatorPtr = std::shared_ptr<ResourceMediator>;
    }
}