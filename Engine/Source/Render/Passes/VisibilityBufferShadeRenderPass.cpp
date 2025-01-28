#include "VisibilityBufferShadeRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

#include "RHI/Vulkan/CVkComputePipeline.h"

#include "../Actors/OpaqueInstances.h"

#include <random>

namespace psm
{
    namespace graph
    {
        VisibilityBufferShadeRenderPassNode::VisibilityBufferShadeRenderPassNode(const foundation::Name& name, 
                                                                                 const DevicePtr& device, 
                                                                                 const ResourceMediatorPtr& resourceMediator, 
                                                                                 CommandPoolPtr& computeCommandPool,
                                                                                 CommandPoolPtr& graphicsCommandPool,
                                                                                 DescriptorPoolPtr& descriptorPool,
                                                                                 SResourceExtent3D framebufferSize, 
                                                                                 uint32_t framebuffersCount)
            : RenderPassNode(name, resourceMediator)
        {
            mDeviceInternal = device;
            mFramebuffersSize = framebufferSize;
            mResourceMediator = resourceMediator;

            SSamplerConfig samplerConfig =
            {
                .MagFilter = EFilterMode::FILTER_NEAREST,
                .MinFilter = EFilterMode::FILTER_NEAREST,
                .UAddress = ESamplerAddressMode::SAMPLER_MODE_CLAMP_TO_BORDER,
                .VAddress = ESamplerAddressMode::SAMPLER_MODE_CLAMP_TO_BORDER,
                .WAddress = ESamplerAddressMode::SAMPLER_MODE_CLAMP_TO_BORDER,
                .BorderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .EnableComparision = false,
                .CompareOp = ECompareOp::COMPARE_OP_ALWAYS,
                .SamplerMode = ESamplerMipmapMode::SAMPLER_MIPMAP_MODE_NEAREST,
                .EnableAniso = false,
                .MaxAniso = 0.0f,
                .MaxLod = 0.0,
                .MinLod = 0.0,
                .MipLodBias = 0.0,
                .UnnormalizedCoords = false,
            };

            mSampler = mDeviceInternal->CreateSampler(samplerConfig);


            InitializeBuffers(graphicsCommandPool, framebufferSize, framebuffersCount);
            InitializeRenderTargets(graphicsCommandPool, framebufferSize, framebuffersCount);
            InitializeDescriptors(descriptorPool);
        }

        VisibilityBufferShadeRenderPassNode::~VisibilityBufferShadeRenderPassNode()
        {
            LogMessage(MessageSeverity::Info, "VisibilityBufferShadeRenderPassNode");
        }

        void VisibilityBufferShadeRenderPassNode::PreRender()
        {
            RenderPassNode::PreRender();

        }

        void VisibilityBufferShadeRenderPassNode::PostRender()
        {
            RenderPassNode::PostRender();
        }

        void VisibilityBufferShadeRenderPassNode::AddResourceReferences(uint32_t framesCount)
        {

        }

        void VisibilityBufferShadeRenderPassNode::CollectReferences(uint32_t framesCount)
        {
            mResourceMediator->RegisterDescriptorSet(graph::VISBUF_SHADE_SET, mSet);
            mResourceMediator->RegisterDescriptorSetLayout(graph::VISBUF_SHADE_SET_LAYOUT, mSetLayout);
        }

        void VisibilityBufferShadeRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {

        }

        void VisibilityBufferShadeRenderPassNode::InitializeBuffers(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount)
        {
            auto& instanceBuffer = OpaqueInstances::GetInstance()->GetInstances();
            auto& models = OpaqueInstances::GetInstance()->GetModels();

            uint64_t totalVertices = 0;
            uint64_t totalIndices = 0;
            for(auto& model : models)
            {
                totalVertices += model->MeshVertices.size();
                totalIndices += model->MeshIndices.size();
            }

            //get staging buffers size
            SResourceSize vertexBufferSize = totalVertices * sizeof(Vertex);
            SResourceSize indexBufferSize = totalIndices * sizeof(uint32_t);
            SResourceSize instanceBufferSize = instanceBuffer.size() * sizeof(OpaqueInstances::Instance);

            //fill create structs
            SBufferConfig vertexBufferConfig =
            {
                .Size = vertexBufferSize,
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT,
            };

            SBufferConfig indexBufferConfig =
            {
                .Size = indexBufferSize,
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT,
            };

            SBufferConfig instanceBufferConfig =
            {
                .Size = instanceBufferSize, 
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT,
            };

            //create staging buffers
            BufferPtr vertexStagingBuffer = mDeviceInternal->CreateBuffer(vertexBufferConfig);
            BufferPtr indexStagingBuffer = mDeviceInternal->CreateBuffer(indexBufferConfig);
            BufferPtr instanceStagingBuffer = mDeviceInternal->CreateBuffer(instanceBufferConfig);

            //map them
            void* pVertexStagingData = nullptr;
            void* pIndexStagingData = nullptr;
            void* pInstanceStagingData = nullptr;

            SBufferMapConfig vertexMapConfig =
            {
                .Size = vertexBufferSize,
                .Offset = 0,
                .pData = &pVertexStagingData,
                .Flags = 0,
            };

            SBufferMapConfig indexMapConfig =
            {
                .Size = indexBufferSize,
                .Offset = 0,
                .pData = &pIndexStagingData,
                .Flags = 0,
            };

            SBufferMapConfig instanceMapConfig =
            {
                .Size = instanceBufferSize,
                .Offset = 0,
                .pData = &pInstanceStagingData,
                .Flags = 0,
            };

            vertexStagingBuffer->Map(vertexMapConfig);
            indexStagingBuffer->Map(indexMapConfig);
            instanceStagingBuffer->Map(instanceMapConfig);

            if(!pVertexStagingData || !pInstanceStagingData || !pIndexStagingData)
            {
                __debugbreak();
                return;
            }

            //fill with data
            Vertex* vData = reinterpret_cast<Vertex*>(pVertexStagingData);
            uint32_t* iData = reinterpret_cast<uint32_t*>(pIndexStagingData);
            uint64_t vertexOffset = 0;
            uint64_t indexOffset = 0;

            for(auto& model : models)
            {
                memcpy(vData + vertexOffset, model->MeshVertices.data(), model->MeshVertices.size() * sizeof(Vertex));
                vertexOffset += model->MeshVertices.size() * sizeof(Vertex);

                memcpy(iData + indexOffset, model->MeshIndices.data(), model->MeshIndices.size() * sizeof(uint32_t));
                indexOffset += model->MeshIndices.size() * sizeof(uint32_t);
            }

            memcpy(pInstanceStagingData, instanceBuffer.data(), instanceBufferSize);

            vertexStagingBuffer->Unmap();
            indexStagingBuffer->Unmap();
            instanceStagingBuffer->Unmap();

            //create buffers for work

            vertexBufferConfig =
            {
                .Size = vertexBufferSize,
                .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_STORAGE_BUFFER_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };

            indexBufferConfig =
            {
                .Size = indexBufferSize,
                .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_STORAGE_BUFFER_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };

            instanceBufferConfig =
            {
                .Size = instanceBufferSize,
                .Usage = EBufferUsage::USAGE_TRANSFER_DST_BIT | EBufferUsage::USAGE_STORAGE_BUFFER_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };

            mVertices = mDeviceInternal->CreateBuffer(vertexBufferConfig);
            mIndices = mDeviceInternal->CreateBuffer(indexBufferConfig);
            mInstances = mDeviceInternal->CreateBuffer(instanceBufferConfig);

            CommandBufferPtr cmdBuf = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(commandPool);

            mDeviceInternal->CopyBuffer(cmdBuf, vertexBufferSize, vertexStagingBuffer, mVertices);
            mDeviceInternal->CopyBuffer(cmdBuf, indexBufferSize, indexStagingBuffer, mIndices);
            mDeviceInternal->CopyBuffer(cmdBuf, instanceBufferSize, instanceStagingBuffer, mInstances);

            mDeviceInternal->SubmitSingleTimeCommandBuffer(commandPool, cmdBuf);
        }

        void VisibilityBufferShadeRenderPassNode::InitializeDescriptors(DescriptorPoolPtr& descriptorPool)
        {
            //global buffer only (set 0)
            std::vector<SDescriptorLayoutInfo> shaderBuffersInfo =
            {
                {
                    .Binding = 0, //binding
                    .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 1, //binding
                    .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 2, //binding
                    .DescriptorType = EDescriptorType::STORAGE_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 3, //binding
                    .DescriptorType = EDescriptorType::STORAGE_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 4, //binding
                    .DescriptorType = EDescriptorType::STORAGE_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 5, //binding
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 6, //binding
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 7, //binding
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
            };

            SDescriptorSetLayoutConfig shaderBuffersConfig =
            {
                .pLayoutsInfo = shaderBuffersInfo.data(),
                .LayoutsCount = static_cast<uint32_t>(shaderBuffersInfo.size())
            };

            mSetLayout = mDeviceInternal->CreateDescriptorSetLayout(shaderBuffersConfig);

            SDescriptorSetAllocateConfig globalBufferAlloc =
            {
                 .DescriptorPool = descriptorPool,
                 .DescriptorSetLayouts = {mSetLayout},
                 .MaxSets = 1,
            };

            mSet = mDeviceInternal->AllocateDescriptorSets(globalBufferAlloc);
        }

        void VisibilityBufferShadeRenderPassNode::InitializeRenderTargets(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount)
        {
            for(int i = 0; i < framebuffersCount; i++)
            {
                SImageConfig imageConfig =
                {
                    .ImageSize = framebufferSize,
                    .MipLevels = 1,
                    .ArrayLevels = 1,
                    .Type = EImageType::TYPE_2D,
                    .Format = EFormat::R16G16B16A16_SFLOAT,
                    .Tiling = EImageTiling::OPTIMAL,
                    .InitialLayout = EImageLayout::UNDEFINED,
                    .Usage = EImageUsageType::USAGE_COLOR_ATTACHMENT_BIT | EImageUsageType::USAGE_STORAGE_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
                    .SharingMode = ESharingMode::EXCLUSIVE,
                    .SamplesCount = ESamplesCount::COUNT_1,
                    .Flags = EImageCreateFlags::NONE,
                    .ViewFormat = EFormat::R16G16B16A16_SFLOAT,
                    .ViewType = EImageViewType::TYPE_2D,
                    .ViewAspect = EImageAspect::COLOR_BIT
                };

                mShaderStorageAlbedoResult.push_back(mDeviceInternal->CreateImage(imageConfig));
                mShaderStorageNormalResult.push_back(mDeviceInternal->CreateImage(imageConfig));
                mShaderStorageWorldPosResult.push_back(mDeviceInternal->CreateImage(imageConfig));
            };

            auto cmdBuf = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(commandPool);

            for(int i = 0; i < framebuffersCount; i++)
            {
                SImageLayoutTransition transition =
                {
                    .Format = EFormat::R16G16B16A16_SFLOAT,
                    .OldLayout = EImageLayout::UNDEFINED,
                    .NewLayout = EImageLayout::GENERAL,
                    .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                    .DestinationStage = EPipelineStageFlags::COMPUTE_SHADER_BIT,
                    .SourceMask = EAccessFlags::NONE,
                    .DestinationMask = EAccessFlags::SHADER_WRITE_BIT,
                    .ImageAspectFlags = EImageAspect::COLOR_BIT,
                    .MipLevel = 0,
                };

                mDeviceInternal->ImageLayoutTransition(cmdBuf, mShaderStorageAlbedoResult[i], transition);
                mDeviceInternal->ImageLayoutTransition(cmdBuf, mShaderStorageNormalResult[i], transition);
                mDeviceInternal->ImageLayoutTransition(cmdBuf, mShaderStorageWorldPosResult[i], transition);
            }

            mDeviceInternal->SubmitSingleTimeCommandBuffer(commandPool, cmdBuf);
        }

        void VisibilityBufferShadeRenderPassNode::UpdateDescriptors()
        {
            foundation::Name gbName = graph::GetResourceIndexedName(GLOBAL_CBUFFER, mCurrentFramebufferIndex);
            BufferPtr globalBuffer = mResourceMediator->GetBufferByName(gbName);

            std::vector<SUpdateBuffersConfig> buffersInfo =
            {
               {
                   .Buffer = globalBuffer,
                   .Offset = 0,
                   .Range = globalBuffer->Size(),
                   .DstBinding = 1,
                   .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
               },
               {
                   .Buffer = mVertices,
                   .Offset = 0,
                   .Range = mVertices->Size(),
                   .DstBinding = 2,
                   .DescriptorType = EDescriptorType::STORAGE_BUFFER,
               },
               {
                   .Buffer = mIndices,
                   .Offset = 0,
                   .Range = mIndices->Size(),
                   .DstBinding = 3,
                   .DescriptorType = EDescriptorType::STORAGE_BUFFER,
               },
               {
                   .Buffer = mInstances,
                   .Offset = 0,
                   .Range = mInstances->Size(),
                   .DstBinding = 4,
                   .DescriptorType = EDescriptorType::STORAGE_BUFFER,
               },
            };

            foundation::Name visBufResultName = graph::GetResourceIndexedName(VISBUF_GENERATION_RENDERTARGET_NAME, mCurrentFramebufferIndex);
            ImagePtr visBufResultImage = mResourceMediator->GetImageByName(visBufResultName);

            std::vector<SUpdateTextureConfig> texturesInfo =
            {
                {
                    .Sampler = mSampler,
                    .Image = visBufResultImage,
                    .ImageLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                    .DstBinding = 0,
                    .DescriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER,
                },
                {
                    .Sampler = nullptr,
                    .Image = mShaderStorageAlbedoResult[mCurrentFramebufferIndex],
                    .ImageLayout = EImageLayout::GENERAL,
                    .DstBinding = 5,
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE,
                },
                {
                    .Sampler = nullptr,
                    .Image = mShaderStorageNormalResult[mCurrentFramebufferIndex],
                    .ImageLayout = EImageLayout::GENERAL,
                    .DstBinding = 6,
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE,
                },
                {
                    .Sampler = nullptr,
                    .Image = mShaderStorageWorldPosResult[mCurrentFramebufferIndex],
                    .ImageLayout = EImageLayout::GENERAL,
                    .DstBinding = 7,
                    .DescriptorType = EDescriptorType::STORAGE_IMAGE,
                },
            };

            mDeviceInternal->UpdateDescriptorSets(mSet, texturesInfo, buffersInfo);
        }
    }
}