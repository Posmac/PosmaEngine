#include "VisibilityBufferShadeRenderPass.h"

#include "Render/Graph/GraphResourceNames.h"

#include "RHI/Vulkan/CVkComputePipeline.h"

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
            InitializeBuffers(graphicsCommandPool, framebufferSize, framebuffersCount);
            InitializeDescriptors(descriptorPool);
            UpdateDescriptors(framebuffersCount);
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
            mResourceMediator->RegisterDescriptorSet(graph::VISBUF_SHADE_SET, mShaderStorageSet);
            mResourceMediator->RegisterDescriptorSetLayout(graph::VISBUF_SHADE_SET_LAYOUT, mShaderStorageSetLayout);
        }

        void VisibilityBufferShadeRenderPassNode::RecreateFramebuffers(const SwapchainPtr swapchain)
        {

        }

        void VisibilityBufferShadeRenderPassNode::InitializeBuffers(CommandPoolPtr& commandPool, SResourceExtent3D framebufferSize, uint32_t framebuffersCount)
        {
            mShaderStorageBuffers.resize(framebuffersCount);

            // Initialize particles
            std::default_random_engine rndEngine((unsigned)time(nullptr));
            std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

            // Initial particle positions on a circle
            std::vector<Particle> particles(PARTICLE_COUNT);
            for(auto& particle : particles)
            {
                float r = 0.25f * sqrt(rndDist(rndEngine));
                float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
                float x = r * cos(theta) * framebufferSize.height / framebufferSize.width;
                float y = r * sin(theta);
                particle.Position = glm::vec2(x, y);
                particle.Velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
                particle.Color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
            }

            SResourceSize size = sizeof(Particle) * PARTICLE_COUNT;

            SBufferConfig config =
            {
                .Size = size,
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT,
            };

            BufferPtr stagingBuffer = mDeviceInternal->CreateBuffer(config);

            void* data;
            SBufferMapConfig mapCfg =
            {
                .Size = static_cast<uint64_t>(size),
                .Offset = 0,
                .pData = &data,
                .Flags = 0
            };

            stagingBuffer->Map(mapCfg);
            memcpy(data, particles.data(), static_cast<size_t>(size));
            stagingBuffer->Unmap();

            CommandBufferPtr cmdBuf = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(commandPool);

            for(int i = 0; i < framebuffersCount; i++)
            {
                SBufferConfig config =
                {
                    .Size = size,
                    .Usage = EBufferUsage::USAGE_STORAGE_BUFFER_BIT | EBufferUsage::USAGE_VERTEX_BUFFER_BIT | EBufferUsage::USAGE_TRANSFER_DST_BIT,
                    .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                };

                mShaderStorageBuffers[i] = mDeviceInternal->CreateBuffer(config);

                mDeviceInternal->CopyBuffer(cmdBuf, static_cast<uint64_t>(size), stagingBuffer, mShaderStorageBuffers[i]);
            }

            cmdBuf->End();

            mDeviceInternal->SubmitSingleTimeCommandBuffer(commandPool, cmdBuf);
        }

        void VisibilityBufferShadeRenderPassNode::InitializeDescriptors(DescriptorPoolPtr& descriptorPool)
        {
            //global buffer only (set 0)
            std::vector<SDescriptorLayoutInfo> shaderBuffersInfo =
            {
                {
                    .Binding = 0, //binding
                    .DescriptorType = EDescriptorType::UNIFORM_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 1, //binding
                    .DescriptorType = EDescriptorType::STORAGE_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
                {
                    .Binding = 2, //binding
                    .DescriptorType = EDescriptorType::STORAGE_BUFFER, //descriptor type
                    .DescriptorCount = 1, //count
                    .ShaderStage = EShaderStageFlag::COMPUTE_BIT //vertex stage
                },
            };

            SDescriptorSetLayoutConfig shaderBuffersConfig =
            {
                .pLayoutsInfo = shaderBuffersInfo.data(),
                .LayoutsCount = static_cast<uint32_t>(shaderBuffersInfo.size())
            };

            mShaderStorageSetLayout = mDeviceInternal->CreateDescriptorSetLayout(shaderBuffersConfig);

            SDescriptorSetAllocateConfig globalBufferAlloc =
            {
                 .DescriptorPool = descriptorPool,
                 .DescriptorSetLayouts = {mShaderStorageSetLayout},
                 .MaxSets = 1,
            };

            mShaderStorageSet = mDeviceInternal->AllocateDescriptorSets(globalBufferAlloc);
        }

        void VisibilityBufferShadeRenderPassNode::UpdateDescriptors(uint32_t framesCount)
        {
            for(size_t i = 0; i < framesCount; i++)
            {
                foundation::Name gbName = graph::GetResourceIndexedName(GLOBAL_CBUFFER, i);
                BufferPtr globalBuffer = mResourceMediator->GetBufferByName(gbName);

                std::vector<SUpdateBuffersConfig> buffersInfo =
                {
                   {
                       .Buffer = globalBuffer,
                       .Offset = 0,
                       .Range = globalBuffer->Size(),
                       .DstBinding = 0,
                       .DescriptorType = EDescriptorType::UNIFORM_BUFFER,
                   },
                   {
                       .Buffer = mShaderStorageBuffers[i],
                       .Offset = 0,
                       .Range = mShaderStorageBuffers[i]->Size(),
                       .DstBinding = 1,
                       .DescriptorType = EDescriptorType::STORAGE_BUFFER,
                   },
                   {
                       .Buffer = mShaderStorageBuffers[i],
                       .Offset = 0,
                       .Range = mShaderStorageBuffers[i]->Size(),
                       .DstBinding = 2,
                       .DescriptorType = EDescriptorType::STORAGE_BUFFER,
                   },
                };

                mDeviceInternal->UpdateDescriptorSets(mShaderStorageSet, {}, buffersInfo);
            }
        }
    }
}