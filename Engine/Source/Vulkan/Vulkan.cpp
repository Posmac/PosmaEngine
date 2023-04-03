#include "Vulkan.h"

namespace psm
{
    Vulkan* Vulkan::s_Instance = nullptr;;

    Vulkan* Vulkan::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Vulkan();
        }

        return s_Instance;
    }

    Vulkan::Vulkan()
    {
        m_Instance = VK_NULL_HANDLE;
        m_PhysicalDevice = VK_NULL_HANDLE;
        m_LogicalDevice = VK_NULL_HANDLE;
        m_Surface = VK_NULL_HANDLE;
        m_DebugUtilsMessenger = VK_NULL_HANDLE;
        m_SwapChain = VK_NULL_HANDLE;
        m_RenderPass = VK_NULL_HANDLE;
        m_PipelineLayout = VK_NULL_HANDLE;
        m_Pipeline = VK_NULL_HANDLE;
        m_DescriptorSetLayout = VK_NULL_HANDLE;
        m_CommandPool = VK_NULL_HANDLE;
        m_DescriptorsPool = VK_NULL_HANDLE;
        m_ImageAvailableSemaphore = VK_NULL_HANDLE;
        m_RenderFinishedSemaphore = VK_NULL_HANDLE;
        m_VertexBuffer = VK_NULL_HANDLE;
        m_VertexBufferMemory = VK_NULL_HANDLE;
    }

    void Vulkan::Init(HINSTANCE hInstance, HWND hWnd)
    {
        InitVulkanInstace();
        CreateDebugUtilsMessenger();
        CreateSurface(hInstance, hWnd);
        SelectPhysicalDevice();
        PopulateSurfaceData();
        CreateLogicalDevice();
        CreateSwapchain();
        QuerrySwapchainImages();
        CreateRenderPass();
        CreateDescriptorPool();
        CreatePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateDescriptorPool();
        LoadModelData();

        ImGui_ImplVulkan_InitInfo imguiInfo{};
        imguiInfo.Instance = m_Instance;
        imguiInfo.PhysicalDevice = m_PhysicalDevice;
        imguiInfo.Device = m_LogicalDevice;
        imguiInfo.QueueFamily = m_QueueIndices.GraphicsFamily.value();
        imguiInfo.Queue = m_QueueIndices.GraphicsQueue;
        imguiInfo.PipelineCache = nullptr;
        imguiInfo.DescriptorPool = m_DescriptorsPool;
        imguiInfo.Subpass = 0;
        imguiInfo.MinImageCount = m_SwapChainImages.size();
        imguiInfo.ImageCount = m_SwapChainImages.size();
        imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        imguiInfo.Allocator = nullptr;
        imguiInfo.CheckVkResultFn = nullptr;

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui_ImplVulkan_Init(&imguiInfo, m_RenderPass);
        {
            // Use any command queue
            vkResetCommandPool(m_LogicalDevice, m_CommandPool, 0);
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer(m_CommandBuffers[0], &begin_info);

            ImGui_ImplVulkan_CreateFontsTexture(m_CommandBuffers[0]);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &m_CommandBuffers[0];
            vkEndCommandBuffer(m_CommandBuffers[0]);
            vkQueueSubmit(m_QueueIndices.GraphicsQueue, 1, &end_info, VK_NULL_HANDLE);

            vkDeviceWaitIdle(m_LogicalDevice);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void Vulkan::Deinit()
    {
        for (auto& framebuffer : m_Framebuffers)
        {
            vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);
        }
        vkDestroyPipeline(m_LogicalDevice, m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
        vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);
        for (auto& imageView : m_SwapchainImageViews)
        {
            vkDestroyImageView(m_LogicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance,
            "vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            func(m_Instance, m_DebugUtilsMessenger, nullptr);
        }

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_LogicalDevice, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void Vulkan::InitVulkanInstace()
    {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pEngineName = "Vulkan Engine";
        applicationInfo.pApplicationName = "Vulkan Engine Application";
        applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        VerifyLayersSupport(m_ValidationLayers);
        VerifyInstanceExtensionsSupport(m_InstanceExtensions);

        VkDebugUtilsMessengerCreateInfoEXT debugMessegerCreateInfoExt{};
        PopulateDebugUtilsMessenger(debugMessegerCreateInfoExt);

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessegerCreateInfoExt;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = m_ValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        instanceCreateInfo.enabledExtensionCount = m_InstanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();

        VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to create VkInstance" << std::endl;
        }
    }

    void Vulkan::Render()
    {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, nullptr, &imageIndex);

        vkResetCommandBuffer(m_CommandBuffers[imageIndex], 0);
        VkCommandBufferBeginInfo begin{};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = 0;
        begin.pNext = nullptr;
        begin.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &begin) != VK_SUCCESS)
        {
            std::cout << "Failed to begin command buffer" << std::endl;
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_Framebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChainExtent;

        std::array<VkClearValue, 2> clearColor{};
        clearColor[0].color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearColor.size());
        renderPassInfo.pClearValues = clearColor.data();

        vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        //vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChainExtent.width);
        viewport.height = static_cast<float>(m_SwapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_SwapChainExtent;
        vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

        //vkCmdBindDescriptorSets(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
        //    m_PipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

        vkCmdDraw(m_CommandBuffers[imageIndex], static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
        //vkCmdDrawIndexed(m_CommandBuffers[imageIndex], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        // Render ImGUI widgets
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Test widget");
            //ImGui::Text("This is some text.");
            bool show_demo_window = false;
            float f = 0;
            glm::vec4 colorClear = glm::vec4(1.0);

            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);// Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit4("clear color", &colorClear[0]);

            ImGui::End();
            ImGui::Render();
        }

        // Integrate ImGUI into Vulkan rendering pipeline
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffers[imageIndex]);

        vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

        if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
        {
            std::cout << "Failed to end command buffer" << std::endl;
        }

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphore;

        vkQueueSubmit(m_QueueIndices.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_SwapChain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_QueueIndices.PresentQueue, &presentInfo);

        vkDeviceWaitIdle(m_LogicalDevice);
    }

    void Vulkan::LoadModelData()
    {
        m_Vertices.reserve(3);
        m_Vertices.push_back({ -1, 1, 0 });
        m_Vertices.push_back({ 0, -1, 0 });
        m_Vertices.push_back({ 1, 1, 0 });

        VkDeviceSize bufferSize = sizeof(Vertex) * m_Vertices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.flags = 0;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.queueFamilyIndexCount = 1;
        bufferInfo.pQueueFamilyIndices = &m_QueueIndices.GraphicsFamily.value();
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.size = sizeof(Vertex) * m_Vertices.size();

        VkResult result = vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &m_VertexBuffer);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create vertex buffer" << std::endl;
        }

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(m_LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_Vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

        CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);
    }

    void Vulkan::VerifyLayersSupport(std::vector<const char*>& layersToEnable)
    {
        uint32_t availableLayersCount;
        vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);

        if (availableLayersCount == 0)
        {
            std::cout << "Failed to enumerate available layers count\n";
        }

        std::vector<VkLayerProperties> availableLayers(availableLayersCount);
        vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data());

        std::vector<const char*> actualLayersToEnable;
        for (auto& property : availableLayers)
        {
            for (auto& layerName : layersToEnable)
            {
                if (strcmp(layerName, property.layerName) == 0)
                {
                    actualLayersToEnable.push_back(layerName);
                    std::cout << "Layer: " << layerName;
                    std::cout << " is supported" << std::endl;
                }
                else
                {
                    //std::cout << " isn`t supported" << std::endl;
                }
            }
        }

        layersToEnable = std::move(actualLayersToEnable);
    }

    void Vulkan::VerifyInstanceExtensionsSupport(std::vector<const char*>& extensionsToEnable)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

        if (availableExtensionsCount == 0)
        {
            std::cout << "Failed to enumerate available instance extensions" << std::endl;
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

        std::vector<const char*> actualExtensionsToEnable;

        for (auto& property : availableExtensions)
        {
            for (auto& extension : extensionsToEnable)
            {
                if (strcmp(property.extensionName, extension) == 0)
                {
                    actualExtensionsToEnable.push_back(extension);
                    std::cout << "Instance extension: " << extension;
                    std::cout << " is supported" << std::endl;
                }
                else
                {
                    //std::cout << " isn`t supported" << std::endl;
                }
            }
        }

        extensionsToEnable = actualExtensionsToEnable;
    }

    void Vulkan::VerifyDeviceExtensionsSupport(std::vector<const char*>& extensionsToEnable)
    {
        uint32_t availableExtensionsCount;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionsCount, nullptr);

        if (availableExtensionsCount == 0)
        {
            std::cout << "Failed to enumerate available device extensions" << std::endl;
        }

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionsCount, availableExtensions.data());

        std::vector<const char*> actualExtensionsToEnable;

        for (auto& property : availableExtensions)
        {
            for (auto& extension : extensionsToEnable)
            {
                if (strcmp(property.extensionName, extension) == 0)
                {
                    actualExtensionsToEnable.push_back(extension);
                    std::cout << "Device extension: " << extension;
                    std::cout << " is supported" << std::endl;
                }
                else
                {
                    //std::cout << " isn`t supported" << std::endl;
                }
            }
        }

        extensionsToEnable = actualExtensionsToEnable;
    }

    void Vulkan::CreateSurface(HINSTANCE hInstance, HWND hWnd)
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = hInstance;
        surfaceInfo.hwnd = hWnd;

        VkResult result = vkCreateWin32SurfaceKHR(m_Instance, &surfaceInfo, nullptr, &m_Surface);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create surface" << std::endl;
        }
    }

    void Vulkan::PopulateSurfaceData()
    {
        //get surface capabilities, formats and present modes
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceData.Capabilities);

        uint32_t surfaceSuportedFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceSuportedFormatsCount, nullptr);

        if (surfaceSuportedFormatsCount == 0)
        {
            std::cout << "Failed to get supported surface formats" << std::endl;
        }

        m_SurfaceData.Formats.resize(surfaceSuportedFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceSuportedFormatsCount,
            m_SurfaceData.Formats.data());

        uint32_t surfaceSupportedPresentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &surfaceSupportedPresentModesCount, nullptr);

        if (surfaceSupportedPresentModesCount == 0)
        {
            std::cout << "Failed to get supported surface present modes" << std::endl;
        }

        m_SurfaceData.PresentModes.resize(surfaceSupportedPresentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &surfaceSupportedPresentModesCount,
            m_SurfaceData.PresentModes.data());
    }

    void Vulkan::SelectPhysicalDevice()
    {
        uint32_t physicalDevicesCount;
        vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, nullptr);

        if (physicalDevicesCount == 0)
        {
            std::cout << "Unable to enumerate physical devices" << std::endl;
        }

        std::vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDevicesCount);
        vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, physicalDevicesAvailable.data());

        for (auto& device : physicalDevicesAvailable)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(device, &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                std::cout << "Found discrette device: " << std::endl;
                std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
                m_PhysicalDevice = device;
                m_PhysicalDeviceProperties = deviceProps;
                m_PhysicalDeviceFeatures = deviceFeatures;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(physicalDevicesAvailable[0], &deviceProps);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevicesAvailable[0], &deviceFeatures);

            std::cout << "Unable to get discrette gpu, selected first one" << std::endl;
            std::cout << "Production: " << deviceProps.vendorID << ", " << deviceProps.deviceName << std::endl;
            m_PhysicalDevice = physicalDevicesAvailable[0];
            m_PhysicalDeviceProperties = deviceProps;
            m_PhysicalDeviceFeatures = deviceFeatures;
        }
    }

    void Vulkan::CreateLogicalDevice()
    {
        VerifyDeviceExtensionsSupport(m_DeviceExtensions);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.enabledExtensionCount = m_DeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
        createInfo.enabledLayerCount = m_ValidationLayers.size();
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        createInfo.pEnabledFeatures = &m_PhysicalDeviceFeatures;

        uint32_t queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertyCount, nullptr);

        if (queueFamilyPropertyCount == 0)
        {
            std::cout << "Failed to get physical device queue family properties" << std::endl;
        }

        std::vector<VkQueueFamilyProperties> availableQueueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertyCount, availableQueueFamilyProperties.data());

        int i = 0;
        for (const auto& family : availableQueueFamilyProperties)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_QueueIndices.GraphicsFamily = i;
            }
            if (presentSupport)
            {
                m_QueueIndices.PresentFamily = i;
            }
            if (m_QueueIndices.IsComplete())
            {
                break;
            }
            i++;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { m_QueueIndices.GraphicsFamily.value(),
                                                  m_QueueIndices.PresentFamily.value() };
        float queuePriority = 1.0f;

        for (uint32_t family : uniqueQueueFamilies)
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

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create logical device" << std::endl;
        }

        vkGetDeviceQueue(m_LogicalDevice, m_QueueIndices.GraphicsFamily.value(), 0, &m_QueueIndices.GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, m_QueueIndices.PresentFamily.value(), 0, &m_QueueIndices.PresentQueue);
    }

    void Vulkan::PopulateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo)
    {
        debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerCreateInfo.flags = 0;
        debugMessengerCreateInfo.pNext = nullptr;
        debugMessengerCreateInfo.pfnUserCallback = &DebugMessengerCallback;
        debugMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    }

    void Vulkan::CreateDebugUtilsMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        PopulateDebugUtilsMessenger(createInfo);

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            VkResult result = func(m_Instance, &createInfo, nullptr, &m_DebugUtilsMessenger);
            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create Debug utils messenger EXT" << std::endl;
            }
        }
    }

    void Vulkan::CreateSwapchain()
    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.flags = 0;
        swapchainCreateInfo.surface = m_Surface;
        swapchainCreateInfo.minImageCount =
            (m_SurfaceData.Capabilities.minImageCount + 1 <= m_SurfaceData.Capabilities.maxImageCount) ?
            m_SurfaceData.Capabilities.minImageCount + 1 : m_SurfaceData.Capabilities.maxImageCount;

        VkFormat imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkPresentModeKHR presetMode = VK_PRESENT_MODE_MAILBOX_KHR;

        CheckFormatSupport(imageFormat);
        CheckColorSpaceSupport(colorSpace);
        CheckPresentModeSupport(presetMode);

        swapchainCreateInfo.imageFormat = imageFormat;
        swapchainCreateInfo.imageColorSpace = colorSpace;
        swapchainCreateInfo.imageExtent = m_SurfaceData.Capabilities.currentExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presetMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = nullptr;

        VkResult result = vkCreateSwapchainKHR(m_LogicalDevice, &swapchainCreateInfo, nullptr,
            &m_SwapChain);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create swapchain" << std::endl;
        }

        m_SwapChainImageFormat = imageFormat;
        m_SwapChainExtent = m_SurfaceData.Capabilities.currentExtent;

        std::cout << "Current extent is: " << m_SwapChainExtent.width <<
            " " << m_SwapChainExtent.height << std::endl;

        //create semaphore for swapchain
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        result = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create semaphore" << std::endl;
        }

        semaphoreInfo.flags = 0;

        result = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create semaphore" << std::endl;
        }
    }

    void Vulkan::CheckColorSpaceSupport(VkColorSpaceKHR& colorSpace)
    {
        for (auto& formatProperty : m_SurfaceData.Formats)
        {
            if (formatProperty.colorSpace == colorSpace)
            {
                break;
            }
        }

        std::cout << "Desired color mode isn`t supported. Assigned VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color mode" << std::endl;
        colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }

    void Vulkan::CheckPresentModeSupport(VkPresentModeKHR& presentMode)
    {
        for (auto& availablePresentMode : m_SurfaceData.PresentModes)
        {
            if (availablePresentMode == presentMode)
            {
                break;
            }
        }

        std::cout << "Desired present mode isn`t supported. Assigned VK_PRESENT_MODE_IMMEDIATE_KHR present mode" << std::endl;
        presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    void Vulkan::QuerrySwapchainImages()
    {
        uint32_t imagesCount;
        vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imagesCount, nullptr);
        m_SwapChainImages.resize(imagesCount);
        vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imagesCount, m_SwapChainImages.data());

        std::cout << "Total swapchain images retrived is: " << m_SwapChainImages.size() << std::endl;

        m_SwapchainImageViews.resize(imagesCount);

        for (int i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.image = m_SwapChainImages[i];
            createInfo.format = m_SwapChainImageFormat;
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_LogicalDevice, &createInfo,
                nullptr, &m_SwapchainImageViews[i]);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create swapchain image view" << std::endl;
            }
        }
    }

    void Vulkan::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.flags = 0;
        colorAttachmentDescription.format = m_SwapChainImageFormat;
        colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescr{};
        subpassDescr.flags = 0;
        subpassDescr.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescr.inputAttachmentCount = 0;
        subpassDescr.pInputAttachments = nullptr;
        subpassDescr.colorAttachmentCount = 1;
        subpassDescr.pColorAttachments = &colorAttachmentReference;
        subpassDescr.pResolveAttachments = nullptr;
        subpassDescr.pDepthStencilAttachment = nullptr;
        subpassDescr.preserveAttachmentCount = 0;
        subpassDescr.pPreserveAttachments = nullptr;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.flags = 0;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachmentDescription;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescr;
        renderPassInfo.dependencyCount = 0;
        renderPassInfo.pDependencies = nullptr;

        VkResult result = vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr,
            &m_RenderPass);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create render pass" << std::endl;
        }
    }

    void Vulkan::CreateDescriptorPool()
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        VkResult result = vkCreateDescriptorPool(m_LogicalDevice, &pool_info, nullptr, &m_DescriptorsPool);
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create descriptor pool" << std::endl;
        }
    }

    void Vulkan::CreatePipeline()
    {
        VkDescriptorSetLayoutCreateInfo descriptorSetInfo{};
        descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetInfo.pNext = nullptr;
        descriptorSetInfo.flags = 0;
        descriptorSetInfo.bindingCount = 0;
        descriptorSetInfo.pBindings = nullptr;

        vkCreateDescriptorSetLayout(m_LogicalDevice, &descriptorSetInfo, nullptr,
            &m_DescriptorSetLayout);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pNext = nullptr;
        pipelineLayoutInfo.flags = 0;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkResult result = vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr,
            &m_PipelineLayout);

        VkShaderModule vertexShaderModule = CreateShaderModule("../Engine/Shaders/triangle.vert.txt");
        VkShaderModule fragmentShaderModule = CreateShaderModule("../Engine/Shaders/triangle.frag.txt");

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].pName = "main";
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[0].module = vertexShaderModule;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;
        shaderStages[1].pName = "main";
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pSpecializationInfo = nullptr;
        shaderStages[1].module = fragmentShaderModule;

        VkVertexInputAttributeDescription vertexAttribDescr{};
        vertexAttribDescr.binding = 0;
        vertexAttribDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttribDescr.location = 0;
        vertexAttribDescr.offset = 0;

        VkVertexInputBindingDescription bindingDescr{};
        bindingDescr.binding = 0;
        bindingDescr.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescr.stride = sizeof(Vertex);

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.pNext = nullptr;
        vertexInputStateCreateInfo.flags = 0;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescr;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexAttribDescr;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = nullptr;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkViewport viewPort{};
        viewPort.x = 0;
        viewPort.y = 0;
        viewPort.width = m_SwapChainExtent.width;
        viewPort.height = m_SwapChainExtent.height;
        viewPort.minDepth = 0;
        viewPort.maxDepth = 1;

        VkRect2D scriccors{};
        scriccors.extent = m_SwapChainExtent;
        scriccors.offset = { 0, 0 };

        VkPipelineViewportStateCreateInfo viewPortInfo{};
        viewPortInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewPortInfo.pNext = nullptr;
        viewPortInfo.flags = 0;
        viewPortInfo.viewportCount = 1;
        viewPortInfo.pViewports = &viewPort;
        viewPortInfo.scissorCount = 1;
        viewPortInfo.pScissors = &scriccors;

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext = nullptr;
        rasterizationStateInfo.flags = 0;
        rasterizationStateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateInfo.depthBiasConstantFactor = 0;
        rasterizationStateInfo.depthBiasClamp = 1.0f;
        rasterizationStateInfo.depthBiasSlopeFactor = 0;
        rasterizationStateInfo.lineWidth = 1;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.pNext = nullptr;
        dynamicStateCreateInfo.flags = 0;
        dynamicStateCreateInfo.dynamicStateCount = 2;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
        graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineInfo.flags = 0;
        graphicsPipelineInfo.pNext = nullptr;
        graphicsPipelineInfo.stageCount = shaderStages.size();
        graphicsPipelineInfo.pStages = shaderStages.data();
        graphicsPipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        graphicsPipelineInfo.pTessellationState = nullptr;
        graphicsPipelineInfo.pViewportState = &viewPortInfo;
        graphicsPipelineInfo.pRasterizationState = &rasterizationStateInfo;
        graphicsPipelineInfo.pMultisampleState = nullptr;
        graphicsPipelineInfo.pDepthStencilState = nullptr;
        graphicsPipelineInfo.pColorBlendState = &colorBlending;
        graphicsPipelineInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineInfo.layout = m_PipelineLayout;
        graphicsPipelineInfo.renderPass = m_RenderPass;
        graphicsPipelineInfo.subpass = 0;
        graphicsPipelineInfo.basePipelineHandle = nullptr;
        graphicsPipelineInfo.basePipelineIndex = 0;

        result = vkCreateGraphicsPipelines(m_LogicalDevice, nullptr, 1,
            &graphicsPipelineInfo, nullptr, &m_Pipeline);
    }

    VkShaderModule Vulkan::CreateShaderModule(const std::string& path)
    {
        std::ifstream fileStream(path, std::ios::ate | std::ios::binary);
        if (!fileStream.is_open())
        {
            std::cout << "File path: " << path << " is wrong!" << std::endl;
            return nullptr;
        }

        size_t fileSize = (size_t)fileStream.tellg();
        std::vector<char> buffer(fileSize);

        fileStream.seekg(0);
        fileStream.read(buffer.data(), fileSize);
        fileStream.close();

        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.pNext = nullptr;
        shaderModuleCreateInfo.flags = 0;
        shaderModuleCreateInfo.codeSize = fileSize;
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

        VkShaderModule module{};
        VkResult result = vkCreateShaderModule(m_LogicalDevice, &shaderModuleCreateInfo,
            nullptr, &module);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create shader module" << std::endl;
            return nullptr;
        }

        return module;
    }

    void Vulkan::CreateFramebuffers()
    {
        m_Framebuffers.resize(2);

        for (int i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.pNext = nullptr;
            framebufferInfo.flags = 0;
            framebufferInfo.width = m_SwapChainExtent.width;
            framebufferInfo.height = m_SwapChainExtent.height;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = &m_SwapchainImageViews[i];
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr,
                &m_Framebuffers[i]);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create framebuffer" << std::endl;
            }
        }
    }

    void Vulkan::CreateCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = m_QueueIndices.GraphicsFamily.value();

        VkResult result = vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create command pool" << std::endl;
        }
    }

    void Vulkan::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 2;//because we have 2 framebufers

        m_CommandBuffers.resize(2);

        VkResult result = vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data());
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to allocate command buffers" << std::endl;
        }
    }

    void Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
        VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer");
        }

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);

        if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory");
        }

        vkBindBufferMemory(m_LogicalDevice, buffer, bufferMemory, 0);
    }

    uint32_t Vulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
    {
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProps);

        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) &&
                (memProps.memoryTypes[i].propertyFlags & props) == props) {
                return i;
            }
        }
    }

    void Vulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = m_CommandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_QueueIndices.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_QueueIndices.GraphicsQueue);

        vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
    }

    void Vulkan::CheckFormatSupport(VkFormat& format)
    {
        for (auto& formatProperty : m_SurfaceData.Formats)
        {
            if (formatProperty.format == format)
            {
                break;
            }
        }

        std::cout << "Desired image format isn`t supported. Assigned VK_FORMAT_R8G8B8A8_UNORM format for image" << std::endl;
        format = VK_FORMAT_R8G8B8A8_UNORM;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        std::cout << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
}