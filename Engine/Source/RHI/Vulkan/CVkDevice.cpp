#include "CVkDevice.h"

#include <memory>

#include "CVkSurface.h"
#include "CVkSwapchain.h"
#include "CVkFence.h"
#include "CVkSemaphore.h"
#include "CVkRenderPass.h"
#include "CVkCommandBuffer.h"
#include "CVkImage.h"
#include "CVkCommandPool.h"
#include "CVkFramebuffer.h"
#include "CVkDescriptorPool.h"
#include "CVkBuffer.h"
#include "CVkShader.h"
#include "CVkPipelineLayout.h"
#include "CVkSampler.h"
#include "CVkPipeline.h"
#include "CVkDescriptorSetLayout.h"
#include "CVkImGui.h"
#include "RHI/Memory/UntypedBuffer.h"

#include <Windows.h>
#include <set>

#include "RHI/RHICommon.h"
#include "VkUtils.h"

#include "Model/Mesh.h"

extern VkInstance Instance;

namespace psm
{
    extern DevicePtr RenderDevice;

    void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable, VkPhysicalDevice gpu);

    DevicePtr CreateDefaultDeviceVk(const PlatformConfig& config)
    {
        HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(config.win32.hInstance);
        HWND hWnd = reinterpret_cast<HWND>(config.win32.hWnd);

        VkPhysicalDevice gpu = nullptr;

        //select physical device
        uint32_t physicalDevicesCount;
        vkEnumeratePhysicalDevices(Instance, &physicalDevicesCount, nullptr);

        if(physicalDevicesCount == 0)
        {
            FatalError("Unable to enumerate physical devices");
        }

        std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
        vkEnumeratePhysicalDevices(Instance, &physicalDevicesCount, physicalDevicesAvailable.data());

        for(auto& device : physicalDevicesAvailable)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(device, &deviceProps);

            if(deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                LogMessage(psm::MessageSeverity::Info, "Found discrette device: ");
                LogMessage(psm::MessageSeverity::Info, "Production: " + std::to_string(deviceProps.vendorID) + ", " + deviceProps.deviceName);
                gpu = device;
            }
        }

        if(gpu == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

            LogMessage(psm::MessageSeverity::Info, "Unable to get discrette gpu, selected first one");
            LogMessage(psm::MessageSeverity::Info, "Production: " + std::to_string(deviceProps.vendorID) + ", " + deviceProps.deviceName);
            gpu = physicalDevicesAvailable[0];
        }

        return std::make_shared<CVkDevice>(gpu, config);
    }

    CVkDevice::CVkDevice(VkPhysicalDevice physicalDevice, const PlatformConfig& config)
    {
        mPhysicalDevice = physicalDevice;

        mVkSurface = CreateSurface(config);

        //check for features to enable
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        VkPhysicalDeviceFeatures featuresToEnable = 
        {
            //.robustBufferAccess = ,
            //.fullDrawIndexUint32 = ,
            //.imageCubeArray = ,
            //.independentBlend = ,
            .geometryShader = true,
            //.tessellationShader = ,
            //.sampleRateShading = ,
            //.dualSrcBlend = ,
            //.logicOp = ,
            //.multiDrawIndirect = ,
            //.drawIndirectFirstInstance = ,
            .depthClamp = true,
            .depthBiasClamp = true,
            //.fillModeNonSolid = ,
            //.depthBounds = ,
            //.wideLines = ,
            //.largePoints = ,
            //.alphaToOne = ,
            //.multiViewport = ,
            //.samplerAnisotropy = ,
            //.textureCompressionETC2 = ,
            //.textureCompressionASTC_LDR = ,
            //.textureCompressionBC = ,
            //.occlusionQueryPrecise = ,
            //.pipelineStatisticsQuery = ,
            //.vertexPipelineStoresAndAtomics = ,
            //.fragmentStoresAndAtomics = ,
            //.shaderTessellationAndGeometryPointSize = ,
            //.shaderImageGatherExtended = ,
            //.shaderStorageImageExtendedFormats = ,
            //.shaderStorageImageMultisample = ,
            //.shaderStorageImageReadWithoutFormat = ,
            //.shaderStorageImageWriteWithoutFormat = ,
            //.shaderUniformBufferArrayDynamicIndexing = ,
            //.shaderSampledImageArrayDynamicIndexing = ,
            //.shaderStorageBufferArrayDynamicIndexing = ,
            //.shaderStorageImageArrayDynamicIndexing = ,
            //.shaderClipDistance = ,
            //.shaderCullDistance = ,
            //.shaderFloat64 = ,
            //.shaderInt64 = ,
            //.shaderInt16 = ,
            //.shaderResourceResidency = ,
            //.shaderResourceMinLod = ,
            //.sparseBinding = ,
            //.sparseResidencyBuffer = ,
            //.sparseResidencyImage2D = ,
            //.sparseResidencyImage3D = ,
            //.sparseResidency2Samples = ,
            //.sparseResidency4Samples = ,
            //.sparseResidency8Samples = ,
            //.sparseResidency16Samples = ,
            //.sparseResidencyAliased = ,
            //.variableMultisampleRate = ,
            //.inheritedQueries = ,
        };

        //get device props
        VkPhysicalDeviceProperties deviceProps{};
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);

        //create device
        VerifyDeviceExtensionsSupport(DeviceExtensions, physicalDevice);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.enabledExtensionCount = DeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
        createInfo.enabledLayerCount = ValidationLayers.size();
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
        createInfo.pEnabledFeatures = &featuresToEnable;

        uint32_t queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

        if(queueFamilyPropertyCount == 0)
        {
            LogMessage(psm::MessageSeverity::Fatal, "Failed to get physical device queue family properties");
        }

        std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

        int i = 0;
        for(const auto& family : availableQueueFamilyProperties)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, reinterpret_cast<VkSurfaceKHR>(mVkSurface->Raw()), &presentSupport);
            if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                mQueues.GraphicsFamily = i;
            }
            if(presentSupport)
            {
                mQueues.PresentFamily = i;
            }
            if(mQueues.IsComplete())
            {
                break;
            }
            i++;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { mQueues.GraphicsFamily.value(),
                                                  mQueues.PresentFamily.value() };
        float queuePriority = 1.0f;

        for(uint32_t family : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = family;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &mDevice);

        if(result != VK_SUCCESS)
        {
            FatalError("Failed to create logical device");
        }

        vkGetDeviceQueue(mDevice, mQueues.GraphicsFamily.value(), 0, &mQueues.GraphicsQueue);
        vkGetDeviceQueue(mDevice, mQueues.PresentFamily.value(), 0, &mQueues.PresentQueue);

        mDeviceData.vkData =
        {
            .Device = mDevice,
            .Instance = Instance,
            .PhysicalDevice = physicalDevice,
            .GraphicsQueueIndex = mQueues.GraphicsFamily.value(),
            .GraphicsQueue = mQueues.GraphicsQueue,
        };
    }

    CVkDevice::~CVkDevice()
    {
        vkDestroyDevice(mDevice, nullptr);
    }

    void* CVkDevice::Raw()
    {
        return mDevice;
    }

    void* CVkDevice::Raw() const
    {
        return mDevice;
    }

    SurfacePtr CVkDevice::CreateSurface(const PlatformConfig& config)
    {
        return std::make_shared<CVkSurface>(config);
    }

    ImagePtr CVkDevice::CreateImage(const SImageConfig& config)
    {
        return std::make_shared<CVkImage>(RenderDevice, config);
    }

    ImagePtr CVkDevice::CreateImageWithData(const CommandPoolPtr& commandPool, const SImageConfig& config, const SUntypedBuffer& data, const SImageToBufferCopyConfig& copyConfig)
    {
        ImagePtr image = std::make_shared<CVkImage>(RenderDevice, config);

        SBufferConfig stagingBufferConfig =
        {
            .Size = data.size(),
            .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
            .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        BufferPtr stagingBuffer = CreateBuffer(stagingBufferConfig);

        void* pData;
        SBufferMapConfig mapping =
        {
            .Size = data.size(),
            .Offset = 0,
            .pData = &pData,
            .Flags = 0,
        };

        stagingBuffer->Map(mapping);

        memcpy(pData, data.data(), static_cast<size_t>(data.size()));

        stagingBuffer->Unmap();

        SCommandBufferConfig commandBufferConfig =
        {
            .Size = 1,
            .IsBufferLevelPrimary = true
        };

        std::vector<CommandBufferPtr> commandBuffers = CreateCommandBuffers(commandPool, commandBufferConfig);
        CommandBufferPtr commandBuffer = commandBuffers[0];

        SCommandBufferBeginConfig beginConfig =
        {
            .BufferIndex = 0,
            .Usage = ECommandBufferUsage::ONE_TIME_SUBMIT_BIT,
        };

        commandBuffer->Begin(beginConfig);

        SImageLayoutTransition imageLayoutTransition =
        {
            .Format = copyConfig.FormatBeforeTransition,
            .OldLayout = copyConfig.LayoutBeforeTransition,
            .NewLayout = EImageLayout::TRANSFER_DST_OPTIMAL,
            .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
            .DestinationStage = EPipelineStageFlags::TRANSFER_BIT,
            .SourceMask = EAccessFlags::NONE,
            .DestinationMask = EAccessFlags::TRANSFER_WRITE_BIT,
            .ImageAspectFlags = EImageAspect::COLOR_BIT,
            .MipLevel = 0,
        };

        ImageLayoutTransition(commandBuffer, image, imageLayoutTransition);

        CopyBufferToImage(commandBuffer, stagingBuffer, image, config.ImageSize, EImageAspect::COLOR_BIT, EImageLayout::TRANSFER_DST_OPTIMAL);

        if(config.MipLevels > 1)
        {
            /* GenerateMipMaps(physicalDevice, commandBuffer, *dstImage,
                  VK_FORMAT_R8G8B8A8_SRGB, size.width, size.height, mipLevels);*/
        }

        SImageLayoutTransition layoutTransition =
        {
            .Format = copyConfig.FormatAfterTransition,
            .OldLayout = EImageLayout::TRANSFER_DST_OPTIMAL, //or can be undefined
            .NewLayout = copyConfig.LayoutAfterTransition,
            .SourceStage = EPipelineStageFlags::TRANSFER_BIT,
            .DestinationStage = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
            .SourceMask = EAccessFlags::TRANSFER_WRITE_BIT,
            .DestinationMask = EAccessFlags::SHADER_READ_BIT,
            .ImageAspectFlags = EImageAspect::COLOR_BIT,
            .MipLevel = 0,
        };

        ImageLayoutTransition(commandBuffer, image, layoutTransition);

        commandBuffer->End();

        SFenceConfig fenceConfig =
        {
            .Signaled = false
        };

        FencePtr submitFence = CreateFence(fenceConfig);

        SSubmitConfig submitConfig =
        {
            .Queue = mQueues.GraphicsQueue, //not sure if Queue should be abstracted to CVk(IQueue)
            .SubmitCount = 1,
            .WaitStageFlags = EPipelineStageFlags::NONE,
            .WaitSemaphoresCount = 0,
            .pWaitSemaphores = nullptr,
            .CommandBuffersCount = 1,
            .pCommandBuffers = &commandBuffer,
            .SignalSemaphoresCount = 0,
            .pSignalSemaphores = nullptr,
            .Fence = submitFence,
        };

        Submit(submitConfig);

        SFenceWaitConfig waitConfig =
        {
            .WaitAll = true,
            .Timeout = static_cast<float>(100000000000)
        };

        submitFence->Wait(waitConfig);

        commandPool->FreeCommandBuffers({ commandBuffer });
        //WaitIdle();

        return image;
    }

    BufferPtr CVkDevice::CreateBuffer(const SBufferConfig& config)
    {
        return std::make_shared<CVkBuffer>(RenderDevice, config);
    }

    SamplerPtr CVkDevice::CreateSampler(const SSamplerConfig& config)
    {
        return std::make_shared<CVkSampler>(RenderDevice, config);
    }

    void VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable, VkPhysicalDevice gpu)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, nullptr);

        if(availableExtensionsCount == 0)
        {
            FatalError("Failed to enumerate available device extensions");
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &availableExtensionsCount, availableExtensions.data());

        std::vector<const char*> actualExtensionsToEnable;

        for(auto& property : availableExtensions)
        {
            for(auto& extension : extensionsToEnable)
            {
                if(strcmp(property.extensionName, extension) == 0)
                {
                    actualExtensionsToEnable.push_back(extension);
                    LogMessage(psm::MessageSeverity::Info, "Supported device extension: " + std::string(property.extensionName));
                }
                else
                {
                    LogMessage(psm::MessageSeverity::Warning, "Extension isn`t supported: " + std::string(property.extensionName));
                }
            }
        }

        extensionsToEnable = std::move(actualExtensionsToEnable);
    }

    SwapchainPtr CVkDevice::CreateSwapchain(const SSwapchainConfig& config)
    {
        mSwapchain = std::make_shared<CVkSwapchain>(RenderDevice, config);
        return mSwapchain;
    }

    PipelineLayoutPtr CVkDevice::CreatePipelineLayout(const SPipelineLayoutConfig& config)
    {
        return std::make_shared<CVkPipelineLayout>(RenderDevice, config);
    }

    RenderPassPtr CVkDevice::CreateRenderPass(const SRenderPassConfig& config)
    {
        return std::make_shared<CVkRenderPass>(RenderDevice, config);
    }

    CommandPoolPtr CVkDevice::CreateCommandPool(const SCommandPoolConfig& config)
    {
        return std::make_shared<CVkCommandPool>(RenderDevice, config);
    }

    std::vector<CommandBufferPtr> CVkDevice::CreateCommandBuffers(const CommandPoolPtr& commandPool, const SCommandBufferConfig& config)
    {
        std::vector<CommandBufferPtr> commandBuffers(config.Size);
        std::vector<VkCommandBuffer> vkCommandBuffers(config.Size);

        VkCommandPool vkPool = reinterpret_cast<VkCommandPool>(commandPool->Raw());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = vkPool;
        allocInfo.level = config.IsBufferLevelPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = config.Size;

        VkResult result = vkAllocateCommandBuffers(mDevice, &allocInfo, vkCommandBuffers.data());
        VK_CHECK_RESULT(result);

        for(int i = 0; i < commandBuffers.size(); i++)
        {
            commandBuffers[i] = std::make_shared<CVkCommandBuffer>(RenderDevice, vkCommandBuffers[i]);
        }

        return commandBuffers;
    }

    FramebufferPtr CVkDevice::CreateFramebuffer(const SFramebufferConfig& config)
    {
        return std::make_shared<CVkFramebuffer>(RenderDevice, config);
    }

    DescriptorPoolPtr CVkDevice::CreateDescriptorPool(const SDescriptorPoolConfig& config)
    {
        return std::make_shared<CVkDescriptorPool>(RenderDevice, config);
    }

    DescriptorSetLayoutPtr CVkDevice::CreateDescriptorSetLayout(const SDescriptorSetLayoutConfig& config)
    {
        return std::make_shared<CVkDescriptorSetLayout>(RenderDevice, config);
    }

    ImGuiPtr CVkDevice::CreateGui(const RenderPassPtr& renderPass, const CommandPoolPtr& commandPool, uint8_t swapchainImagesCount, ESamplesCount samplesCount)
    {
        return std::make_shared<CVkImGui>(RenderDevice, renderPass, commandPool, swapchainImagesCount, samplesCount);
    }

    DescriptorSetPtr CVkDevice::AllocateDescriptorSets(SDescriptorSetAllocateConfig& config)
    {
        VkDescriptorPool vkPool = reinterpret_cast<VkDescriptorPool>(config.DescriptorPool->Raw());
        VkDescriptorSet vkSet;

        std::vector<VkDescriptorSetLayout> layouts;
        layouts.resize(config.DescriptorSetLayouts.size());

        for(int i = 0; i < layouts.size(); i++)
        {
            layouts[i] = reinterpret_cast<VkDescriptorSetLayout>(config.DescriptorSetLayouts[i]->Raw());
        }

        VkDescriptorSetAllocateInfo setsAllocInfo{};
        setsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setsAllocInfo.pNext = nullptr;
        setsAllocInfo.descriptorPool = vkPool;
        setsAllocInfo.descriptorSetCount = config.MaxSets;
        setsAllocInfo.pSetLayouts = layouts.data();

        VkResult result = vkAllocateDescriptorSets(mDevice, &setsAllocInfo, &vkSet);
        VK_CHECK_RESULT(result);

        return std::make_shared<CVkDescriptorSet>(mDevice, vkSet, vkPool);
    }

    void CVkDevice::ImageLayoutTransition(const CommandBufferPtr& commandBuffer, const ImagePtr& image, const SImageLayoutTransition& config)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.oldLayout = ToVulkan(config.OldLayout);
        barrier.newLayout = ToVulkan(config.NewLayout);
        barrier.image = reinterpret_cast<VkImage>(image->Raw());
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = ToVulkan(config.ImageAspectFlags);
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.baseMipLevel = config.MipLevel;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = ToVulkan(config.SourceMask);
        barrier.dstAccessMask = ToVulkan(config.DestinationMask);

        vkCmdPipelineBarrier(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
            ToVulkan(config.SourceStage),
            ToVulkan(config.DestinationStage),
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    void CVkDevice::InsertImageMemoryBarrier(const SImageBarrierConfig& config)
    {
        VkImage vkImage = reinterpret_cast<VkImage>(config.Image->Raw());
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(config.CommandBuffer->Raw());

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.oldLayout = ToVulkan(config.OldLayout);
        barrier.newLayout = ToVulkan(config.NewLayout);
        barrier.image = vkImage;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = ToVulkan(config.AspectMask);
        barrier.subresourceRange.baseArrayLayer = config.BaseArrayLayer;
        barrier.subresourceRange.baseMipLevel = config.BaseMipLevel;
        barrier.subresourceRange.levelCount = config.LevelCount;
        barrier.subresourceRange.layerCount = config.LayerCount;
        barrier.srcAccessMask = ToVulkan(config.SrcAccessMask);
        barrier.dstAccessMask = ToVulkan(config.DstAccessMask);

        vkCmdPipelineBarrier(vkCommandBuffer,
            ToVulkan(config.SourceStage),
            ToVulkan(config.DestinationStage),
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    void CVkDevice::Submit(const SSubmitConfig& config)
    {
        std::vector<VkSemaphore> waitSemapthores(config.WaitSemaphoresCount);
        for(int i = 0; i < waitSemapthores.size(); i++)
        {
            waitSemapthores[i] = reinterpret_cast<VkSemaphore>(config.pWaitSemaphores[i]->Raw());
        }

        std::vector<VkSemaphore> signalSemapthores(config.WaitSemaphoresCount);
        for(int i = 0; i < signalSemapthores.size(); i++)
        {
            signalSemapthores[i] = reinterpret_cast<VkSemaphore>(config.pSignalSemaphores[i]->Raw());
        }

        std::vector<VkCommandBuffer> cmdBuffers(config.CommandBuffersCount);
        for(int i = 0; i < cmdBuffers.size(); i++)
        {
            cmdBuffers[i] = reinterpret_cast<VkCommandBuffer>(config.pCommandBuffers[i]->Raw());
        }

        VkPipelineStageFlags stageFlags = ToVulkan(config.WaitStageFlags);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = waitSemapthores.size();
        submitInfo.pWaitSemaphores = waitSemapthores.data();
        submitInfo.pWaitDstStageMask = &stageFlags;
        submitInfo.commandBufferCount = cmdBuffers.size();
        submitInfo.pCommandBuffers = cmdBuffers.data();
        submitInfo.signalSemaphoreCount = signalSemapthores.size();
        submitInfo.pSignalSemaphores = signalSemapthores.data();

        VkResult result = vkQueueSubmit(reinterpret_cast<VkQueue>(config.Queue), config.SubmitCount, &submitInfo, reinterpret_cast<VkFence>(config.Fence->Raw()));
    }

    void CVkDevice::Present(const SPresentConfig& config)
    {
        mSwapchain->Present(config);
    }

    bool CVkDevice::WaitIdle()
    {
        VkResult result = vkDeviceWaitIdle(mDevice);
        return result == VK_SUCCESS;
    }

    void CVkDevice::BindVertexBuffers(const CommandBufferPtr& commandBuffer, const SVertexBufferBindConfig& config)
    {
        std::vector<VkBuffer> buffers(config.BindingCount);
        for(int i = 0; i < buffers.size(); i++)
        {
            buffers[i] = reinterpret_cast<VkBuffer>(config.Buffers[i]->Raw());
        }

        vkCmdBindVertexBuffers(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                               config.FirstSlot, //first binding 
                               config.BindingCount, //binding count
                               buffers.data(),
                               config.Offsets.data());
    }

    void CVkDevice::BindIndexBuffer(const CommandBufferPtr& commandBuffer, const SIndexBufferBindConfig& config)
    {
        vkCmdBindIndexBuffer(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                             reinterpret_cast<VkBuffer>(config.Buffer->Raw()),
                             0, ToVulkan(config.Type));
    }

    void CVkDevice::CopyBuffer(const CommandBufferPtr& commandBuffer, uint64_t size, const BufferPtr& sourceBuffer, const BufferPtr& destinationBuffer)
    {
        //command buffer should be bind at this time!!

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;

        /* vk::InsertBufferMemoryBarrier(commandBuffer, srcBuffer, size,
             VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
             VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);*/

        vkCmdCopyBuffer(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                        reinterpret_cast<VkBuffer>(sourceBuffer->Raw()),
                        reinterpret_cast<VkBuffer>(destinationBuffer->Raw()),
                        1, &copyRegion);

        //vk::InsertBufferMemoryBarrier(commandBuffer, dstBuffer, size,
        //    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        //    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
    }

    void CVkDevice::CopyBufferToImage(const CommandBufferPtr& commandBuffer, const BufferPtr& sourceBuffer, const ImagePtr& destrinationImage, SResourceExtent3D copySize,
                                      EImageAspect imageAspect, EImageLayout imageLayoutAfterCopy)
    {
        VkBufferImageCopy copy{};
        copy.bufferOffset = 0;
        copy.bufferImageHeight = 0;
        copy.bufferRowLength = 0;

        copy.imageOffset = { 0, 0, 0 };
        copy.imageExtent = copySize;
        copy.imageSubresource.aspectMask = ToVulkan(imageAspect);
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;
        copy.imageSubresource.mipLevel = 0;

        vkCmdCopyBufferToImage(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                               reinterpret_cast<VkBuffer>(sourceBuffer->Raw()),
                               reinterpret_cast<VkImage>(destrinationImage->Raw()),
                               ToVulkan(imageLayoutAfterCopy), 1, &copy);
    }

    void CVkDevice::BindDescriptorSets(const CommandBufferPtr& commandBuffer, EPipelineBindPoint bindPoint, const PipelineLayoutPtr& pipelineLayout, const std::vector<DescriptorSetPtr>& descriptors)
    {
        std::vector<VkDescriptorSet> vkDescriptors(descriptors.size());
        for(int i = 0; i < vkDescriptors.size(); i++)
        {
            vkDescriptors[i] = reinterpret_cast<VkDescriptorSet>(descriptors[i]->Raw());
        }

        vkCmdBindDescriptorSets(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                                ToVulkan(bindPoint),
                                reinterpret_cast<VkPipelineLayout>(pipelineLayout->Raw()), 0,
                                vkDescriptors.size(), vkDescriptors.data(), 0, nullptr);
    }

    void CVkDevice::DrawIndexed(const CommandBufferPtr& commandBuffer, const MeshRange& range, uint32_t totalInstances, uint32_t firstInstance)
    {
        vkCmdDrawIndexed(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()), range.IndicesCount, totalInstances,
                     range.IndicesOffset, range.VerticesOffset, firstInstance);
    }

    void CVkDevice::SetDepthBias(const CommandBufferPtr& commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
    {
        vkCmdSetDepthBias(reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()),
                 depthBiasConstantFactor,
                 depthBiasClamp,
                 depthBiasSlopeFactor);
    }

    void CVkDevice::SetViewport(const CommandBufferPtr& commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw());
        VkViewport viewport =
        {
            .x = viewPortX,
            .y = viewPortY,
            .width = viewPortWidth,
            .height = viewPortHeight,
            .minDepth = viewPortMinDepth,
            .maxDepth = viewPortMaxDepth
        };

        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    }

    void CVkDevice::SetScissors(const CommandBufferPtr& commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent)
    {
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw());

        VkRect2D scissor =
        {
            .offset = scissorsOffet,
            .extent = scissorsExtent
        };
        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    }

    void CVkDevice::UpdateDescriptorSets(const DescriptorSetPtr& descriptorSet, const std::vector<SUpdateTextureConfig>& updateTextures, const std::vector<SUpdateBuffersConfig>& updateBuffers)
    {
        std::vector<VkWriteDescriptorSet> writeDescriptors(updateTextures.size() + updateBuffers.size());

        std::vector<VkDescriptorBufferInfo> buffersInfo(updateBuffers.size());
        for(int i = 0; i < updateBuffers.size(); i++)
        {
            buffersInfo[i] =
            {
                .buffer = reinterpret_cast<VkBuffer>(updateBuffers[i].Buffer->Raw()),
                .offset = updateBuffers[i].Offset,
                .range = updateBuffers[i].Range
            };

            writeDescriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptors[i].pNext = nullptr;
            writeDescriptors[i].dstBinding = updateBuffers[i].DstBinding;
            writeDescriptors[i].dstArrayElement = 0;
            writeDescriptors[i].descriptorType = ToVulkan(updateBuffers[i].DescriptorType);
            writeDescriptors[i].descriptorCount = 1;
            writeDescriptors[i].pBufferInfo = &buffersInfo[i];
            writeDescriptors[i].pImageInfo = nullptr;
            writeDescriptors[i].pTexelBufferView = nullptr;
            writeDescriptors[i].dstSet = reinterpret_cast<VkDescriptorSet>(descriptorSet->Raw());
        }

        std::vector<VkDescriptorImageInfo> imagesInfo(updateTextures.size());
        for(int i = 0; i < updateTextures.size(); i++)
        {
            imagesInfo[i] =
            {
                .sampler = reinterpret_cast<VkSampler>(updateTextures[i].Sampler->Raw()),
                .imageView = reinterpret_cast<VkImageView>(updateTextures[i].Image->RawImageView()),
                .imageLayout = ToVulkan(updateTextures[i].ImageLayout)
            };

            writeDescriptors[i + buffersInfo.size()].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptors[i + buffersInfo.size()].pNext = nullptr;
            writeDescriptors[i + buffersInfo.size()].dstBinding = updateTextures[i].DstBinding;
            writeDescriptors[i + buffersInfo.size()].dstArrayElement = 0;
            writeDescriptors[i + buffersInfo.size()].descriptorType = ToVulkan(updateTextures[i].DescriptorType);
            writeDescriptors[i + buffersInfo.size()].descriptorCount = 1;
            writeDescriptors[i + buffersInfo.size()].pBufferInfo = nullptr;
            writeDescriptors[i + buffersInfo.size()].pImageInfo = &imagesInfo[i];
            writeDescriptors[i + buffersInfo.size()].pTexelBufferView = nullptr;
            writeDescriptors[i + buffersInfo.size()].dstSet = reinterpret_cast<VkDescriptorSet>(descriptorSet->Raw());
        }

        vkUpdateDescriptorSets(mDevice, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
    }

    EFormat CVkDevice::FindSupportedFormat(const std::vector<EFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature)
    {
        VkPhysicalDevice gpu = reinterpret_cast<VkPhysicalDevice>(GetDeviceData().vkData.PhysicalDevice);

        VkFormatFeatureFlags vkFeatures = ToVulkan(feature);

        for(auto& format : desiredFormats)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(gpu, ToVulkan(format), &props);

            if(tiling == EImageTiling::LINEAR && (props.linearTilingFeatures & vkFeatures) == vkFeatures)
            {
                return format;
            }
            else if(tiling == EImageTiling::OPTIMAL && (props.optimalTilingFeatures & vkFeatures) == vkFeatures)
            {
                return format;
            }
        }

        __debugbreak();
    }

    DeviceData CVkDevice::GetDeviceData()
    {
        return mDeviceData;
    }

    SurfacePtr CVkDevice::GetSurface()
    {
        return mVkSurface;
    }

    void CVkDevice::SetDebugNameForResource(void* resource, VkDebugReportObjectTypeEXT type, const char* debugName)
    {

    }

    PipelinePtr CVkDevice::CreateRenderPipeline(const SPipelineConfig& config)
    {
        return std::make_shared<CVkPipeline>(RenderDevice, config);
    }

    ShaderPtr CVkDevice::CreateShaderFromFilename(const std::string& path, EShaderStageFlag shaderType)
    {
        return std::make_shared<CVkShader>(RenderDevice, path, shaderType);
    }

    FencePtr CVkDevice::CreateFence(const SFenceConfig& config)
    {
        return std::make_shared<CVkFence>(RenderDevice, config);
    }

    SemaphorePtr CVkDevice::CreateSemaphore(const SSemaphoreConfig& config)
    {
        return std::make_shared<CVkSemaphore>(RenderDevice, config);
    }
}