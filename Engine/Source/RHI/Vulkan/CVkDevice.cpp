#include "CVkDevice.h"

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

#include <Windows.h>
#include <set>

#include "../RHICommon.h"
#include "../VkCommon.h"

#include "RenderBackend/BackedInfo.h"
//#include "RenderBackend/PhysicalDevice.h"
//#include "RenderBackend/LogicalDevice.h"

extern VkInstance Instance;

namespace psm
{
    DevicePtr CreateDefaultDeviceVk(PlatformConfig& config)
    {
        HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(config.win32.hInstance);
        HWND hWnd = reinterpret_cast<HWND>(config.win32.hWnd);

        VkPhysicalDevice gpu;

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

        if(gpu != VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

            LogMessage(psm::MessageSeverity::Info, "Unable to get discrette gpu, selected first one");
            LogMessage(psm::MessageSeverity::Info, "Production: " + std::to_string(deviceProps.vendorID) + ", " + deviceProps.deviceName);
            gpu = physicalDevicesAvailable[0];
        }

        //create surface
        std::shared_ptr<CVkSurface> surface = std::make_shared<CVkSurface>(config);
        return std::make_shared<CVkDevice>(gpu, surface);
    }

    CVkDevice::CVkDevice(VkPhysicalDevice physicalDevice, std::shared_ptr<CVkSurface> surface)
    {
        mPhysicalDevice = physicalDevice;
        mVkSurface = surface;

        //check for features to enable
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        VkPhysicalDeviceFeatures featuresToEnable{}; //for now we dont enable any features

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
            std::cout << "Failed to get physical device queue family properties" << std::endl;
        }

        std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

        int i = 0;
        for(const auto& family : availableQueueFamilyProperties)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->GetSurface(), &presentSupport);
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
    }

    CVkDevice::~CVkDevice()
    {
        vkDestroyDevice(mDevice, nullptr);
    }

    ImagePtr CVkDevice::CreateImage(const SImageConfig& config)
    {
        return std::make_shared<CVkImage>(this, config);
    }

    ImagePtr CVkDevice::CreateImageWithData(const SImageConfig& config, const SUntypedBuffer& data, const SImageLayoutTransition& transition)
    {
        vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                               { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, mipLevels, 1,
                               VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                               VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                               &texture->Image, &texture->ImageMemory, &texture->ImageView);

        vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                                         , m_CommandPool, vk::Queues.GraphicsQueue,
                                         { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
                                         mipLevels,
                                         VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &texture->Image);
    }

    BufferPtr CVkDevice::CreateBuffer(const SBufferConfig& config)
    {
        return std::make_shared<CVkBuffer>(this, config);
    }

    SamplerPtr CVkDevice::CreateSampler(const SSamplerConfig& config)
    {
        return std::make_shared<CVkSampler>(this, config);
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
                    LogMessage(psm::MessageSeverity::Info, "Supported device extension: " + std::string(extension));
                }
                else
                {
                    LogMessage(psm::MessageSeverity::Warning, "Extension isn`t supported: " + std::string(extension));
                }
            }
        }

        extensionsToEnable = std::move(actualExtensionsToEnable);
    }

    SwapchainPtr CVkDevice::CreateSwapchain(const SSwapchainConfig& config)
    {
        return std::make_shared<CVkSwapchain>(this, config);
    }

    PipelineLayoutPtr CVkDevice::CreatePipelineLayout(const SPipelineLayoutConfig& config)
    {
        return std::make_shared<CVkPipelineLayout>(this, config);
    }

    RenderPassPtr CVkDevice::CreateRenderPass(const SRenderPassConfig& config)
    {
        return std::make_shared<CVkRenderPass>(this, config);
    }

    CommandPoolPtr CVkDevice::CreateCommandPool(const SCommandPoolConfig& config)
    {
        return std::make_shared<CVkCommandPool>(this, config);
    }

    CommandBufferPtr CVkDevice::CreateCommandBuffers(CommandPoolPtr commandPool, const SCommandBufferConfig& config)
    {
        return std::make_shared<CVkCommandBuffer>(this, commandPool, config);
    }

    FramebufferPtr CVkDevice::CreateFramebuffer(const SFramebufferConfig& config)
    {
        return std::make_shared<CVkFramebuffer>(this, config);
    }

    DescriptorPoolPtr CVkDevice::CreateDescriptorPool(const SDescriptorPoolConfig& config)
    {
        return std::make_shared<CVkDescriptorPool>(this, config);
    }

    DescriptorSetLayoutPtr CVkDevice::CreateDescriptorSetLayout(const SDescriptorSetLayoutConfig& config)
    {
        return std::make_shared<CVkDescriptorSetLayout>(this, config);
    }

    void CVkDevice::InsertImageMemoryBarrier(const SImageBarrierConfig& config)
    {
        VkImage vkImage = reinterpret_cast<VkImage>(config.Image->GetImage());
        VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(config.CommandBuffer->GetRawPointer());

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
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = waitSemaphoresCount;
        submitInfo.pWaitSemaphores = pWaitSemaphores;
        submitInfo.pWaitDstStageMask = &waitStageFlags;
        submitInfo.commandBufferCount = commandBuffersCount;
        submitInfo.pCommandBuffers = pCommandBuffers;
        submitInfo.signalSemaphoreCount = signalSemaphoresCount;
        submitInfo.pSignalSemaphores = pSignalSemaphores;

        VkResult result = vkQueueSubmit(queue, submitCount, &submitInfo, fence);
    }

    void CVkDevice::Present(const SPresentConfig& config)
    {
        VkSemaphore vkSemaphore = reinterpret_cast<VkSemaphore>(config.pWaitSemaphores[0]->GetRawData());
        VkSwapchainKHR vkSwapchain = reinterpret_cast<VkSwapchainKHR>(config.pSwapchains[0]->Present)
            VkResult result = vk::Present(mQueues.GraphicsQueue, &vkSemaphore, 1,
                        &m_SwapChain, 1, config.ImageIndices);
        VK_CHECK_RESULT(result);
    }

    void CVkDevice::WaitIndle()
    {
        vkDeviceWaitIdle(mDevice);
    }

    void CVkDevice::BindVertexBuffers(const SVertexBufferBindConfig& config)
    {
        VkDeviceSize offset = { 0 };
        vkCmdBindVertexBuffers(commandBuffer,
                               1, //first binding 
                               1, //binding count
                               &m_InstanceBuffer,
                               &offset);
    }

    void CVkDevice::BindIndexBuffer(const SIndexBufferBindConfig& config)
    {
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void CVkDevice::CopyBuffer(BufferPtr sourceBuffer, BufferPtr destinationBuffer)
    {
        VkCommandBuffer commandBuffer = putils::BeginSingleTimeCommandBuffer(vk::Device, commandPool);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            vertexStagingBuffer, m_VertexBuffer, vertexBufferSize);

        vk::CopyBuffer(vk::Device, commandBuffer, vk::Queues.GraphicsQueue,
            indexStagingBuffer, m_IndexBuffer, indexBufferSize);

        putils::EndSingleTimeCommandBuffer(vk::Device, commandPool, commandBuffer, vk::Queues.GraphicsQueue);
    }

    void CVkDevice::BindDescriptorSets(CommandBufferPtr commandBuffer, EPipelineBindPoint bindPoint, PipelinePtr pipeline, const std::vector<DescriptorSetPtr>& descriptors)
    {
        vk::BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        m_InstancedPipelineLayout, { m_InstanceDescriptorSet,
                                        perModel.PerMaterials[i].MaterialDescriptorSet });
    }

    void CVkDevice::DrawIndexed(CommandBufferPtr commandBuffer, const Mesh Range& range, uint32_t totalInstances, uint32_t firstInstance)
    {
        vkCmdDrawIndexed(commandBuffer, range.IndicesCount, totalInstances,
                     range.IndicesOffset, range.VerticesOffset, firstInstance);
    }

    void CVkDevice::SetDepthBias(CommandBufferPtr commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
    {
        vkCmdSetDepthBias(commandBuffer,
                 depthBias,
                 1.0f,
                 depthSlope);
    }

    void CVkDevice::UpdateDescriptorSets(DescriptorSetPtr* descriptorSets, uint32_t setsCount, const std::vector<SUpdateTextureConfig>& updateTextures, const std::vector<SUpdateBuffersConfig>& updateBuffers)
    {
        vk::UpdateDescriptorSets(vk::Device, *descriptorSet, {}, imagesInfo,
            imagesInfo.size());
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

    SurfacePtr CVkDevice::GetSurface()
    {
        return std::static_pointer_cast<ISurface>(mVkSurface);
    }

    PipelinePtr CVkDevice::CreateRenderPipeline(const SPipelineConfig& config)
    {
        return std::make_shared<CVkPipeline>(this, config);
    }

    ShaderPtr CVkDevice::CreateShaderFromFilename(const std::filesystem::path& path, EShaderStageFlag shaderType)
    {
        return std::make_shared<CVkShader>(this, path, shaderType);
    }

    FencePtr CVkDevice::CreateFence(const SFenceConfig& config)
    {
        return std::make_shared<CVkFence>(this, config);
    }

    SemaphorePtr CVkDevice::CreateSemaphore(const SSemaphoreConfig& config)
    {
        return std::make_shared<CVkSemaphore>(this, config);
    }
}