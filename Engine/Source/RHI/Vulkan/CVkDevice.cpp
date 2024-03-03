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

    EImageFormat CVkDevice::FindSupportedFormat(const std::vector<EImageFormat>& desiredFormats, const EImageTiling tiling, const EFeatureFormat feature)
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

    FencePtr CVkDevice::CreateFence(const SFenceConfig& config)
    {
        return std::make_shared<CVkFence>(this, config);
    }

    SemaphorePtr CVkDevice::CreateSemaphore(const SSemaphoreConfig& config)
    {
        return std::make_shared<CVkSemaphore>(this, config);
    }
}