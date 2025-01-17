#include "ResourceMediator.h"

#include "RHI/Configs/ShadersConfig.h"

#include "RHI/Vulkan/CVkDevice.h"

namespace psm
{
    namespace graph
    {
        ResourceMediator::ResourceMediator(const DevicePtr& device)
            : mDeviceInternal(device)
        {
            mAllImages = {};
            mAllBuffers = {};
            mAllDescriptors = {};
            mAllDescriptorLayouts = {};
        }

        ResourceMediator::~ResourceMediator()
        {
            LogMessage(MessageSeverity::Info, "ResourceMediator destructor");

            mDeviceInternal = nullptr;

            for(auto& item : mAllImages)
                item.second = nullptr;
            mAllImages.clear();

            for(auto& item : mAllBuffers)
                item.second = nullptr;
            mAllBuffers.clear();

            for(auto& item : mAllSamplers)
                item.second = nullptr;
            mAllSamplers.clear();

            for(auto& item : mAllDescriptorLayouts)
                item.second = nullptr;
            mAllDescriptorLayouts.clear();

            for(auto& item : mAllDescriptors)
                item.second = nullptr;
            mAllDescriptors.clear();
        }

        void ResourceMediator::RegisterImageResource(const foundation::Name& name, const ImagePtr& image)
        {
            auto find = mAllImages.find(name);

            if(find != mAllImages.end())
            {
                LogMessage(MessageSeverity::Error, "Image resource already exists");
            }

            mAllImages.insert({ name, image });
        }

        void ResourceMediator::RegisterBufferResource(const foundation::Name& name, const BufferPtr & buffer)
        {
            auto find = mAllBuffers.find(name);

            if(find != mAllBuffers.end())
            {
                LogMessage(MessageSeverity::Error, "Buffer resource already exists");
            }

            mAllBuffers.insert({ name, buffer });
        }

        void ResourceMediator::RegisterSampler(const foundation::Name& name, const SamplerPtr& sampler)
        {
            auto find = mAllSamplers.find(name);

            if(find != mAllSamplers.end())
            {
                LogMessage(MessageSeverity::Error, "Sampler resource already exists");
            }

            mAllSamplers.insert({ name, sampler });
        }

        void ResourceMediator::RegisterDescriptorSetLayout(const foundation::Name& name, const DescriptorSetLayoutPtr& set)
        {
            auto find = mAllDescriptorLayouts.find(name);

            if(find != mAllDescriptorLayouts.end())
            {
                LogMessage(MessageSeverity::Error, "Descriptor set already exists");
            }

            mAllDescriptorLayouts.insert({ name, set });
        }

        void ResourceMediator::RegisterDescriptorSet(const foundation::Name & name, const DescriptorSetPtr & setLayout)
        {
            auto find = mAllDescriptors.find(name);

            if(find != mAllDescriptors.end())
            {
                LogMessage(MessageSeverity::Error, "Descriptor set layout already exists");
            }

            mAllDescriptors.insert({ name, setLayout });
        }

        void ResourceMediator::RegisterPipeline(const foundation::Name& name, const PipelinePtr& pipeline)
        {
            auto find = mAllPipelines.find(name);

            if(find != mAllPipelines.end())
            {
                LogMessage(MessageSeverity::Error, "Pipeline already exists");
            }

            mAllPipelines.insert({ name, pipeline });
        }

        void ResourceMediator::UpdateImageReference(const foundation::Name& name, const ImagePtr& image)
        {
            auto find = mAllImages.find(name);

            if(find == mAllImages.end())
            {
                LogMessage(MessageSeverity::Error, "Image resource doesn`t exists");
            }

            mAllImages[name] = image;
        }

        ImagePtr& ResourceMediator::GetImageByName(const foundation::Name & name)
        {
            auto find = mAllImages.find(name);

            if(find == mAllImages.end())
            {
                LogMessage(MessageSeverity::Error, "Image resource didn`t found");
            }

            return find->second;
        }

        BufferPtr& ResourceMediator::GetBufferByName(const foundation::Name& name)
        {
            auto find = mAllBuffers.find(name);

            if(find == mAllBuffers.end())
            {
                LogMessage(MessageSeverity::Error, "Buffer resource didn`t found");
            }

            return find->second;
        }

        SamplerPtr& ResourceMediator::GetSamplerByName(const foundation::Name& name)
        {
            auto find = mAllSamplers.find(name);

            if(find == mAllSamplers.end())
            {
                LogMessage(MessageSeverity::Error, "Sampler resource didn`t found");
            }

            return find->second;
        }

        DescriptorSetLayoutPtr& ResourceMediator::GetDescriptorSetLayoutByName(const foundation::Name& name)
        {
            auto find = mAllDescriptorLayouts.find(name);

            if(find == mAllDescriptorLayouts.end())
            {
                LogMessage(MessageSeverity::Error, "Descriptor set layout didn`t found");
            }

            return find->second;
        }

        DescriptorSetPtr& ResourceMediator::GetDescriptorSetByName(const foundation::Name& name)
        {
            auto find = mAllDescriptors.find(name);

            if(find == mAllDescriptors.end())
            {
                LogMessage(MessageSeverity::Error, "Descriptor set didn`t found");
            }

            return find->second;
        }

        PipelinePtr& ResourceMediator::GetPipelineByName(const foundation::Name& name)
        {
            auto find = mAllPipelines.find(name);

            if(find == mAllPipelines.end())
            {
                LogMessage(MessageSeverity::Error, "Pipeline set didn`t found");
            }

            return find->second;
        }

    }
}