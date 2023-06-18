#include "Renderer.h"

namespace psm
{
    Renderer* Renderer::s_Instance = nullptr;;

    Renderer* Renderer::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Renderer();
        }

        return s_Instance;
    }

    void Renderer::Init(HINSTANCE hInstance, HWND hWnd)
    {
        //m_VkImgui.Init(hWnd, m_SwapChainImages.size(), *this, &m_ImGuiDescriptorsPool);

        vk::CreateSwapchain(vk::Device, vk::PhysicalDevice, vk::Surface, vk::SurfData, &m_SwapChain, &m_SwapChainImageFormat,
            &m_SwapChainExtent);

        vk::QuerrySwapchainImages(vk::Device, m_SwapChain, m_SwapChainImageFormat, &m_SwapChainImages, &m_SwapchainImageViews);
        vk::CreateVkSemaphore(vk::Device, 0, &m_ImageAvailableSemaphore);
        vk::CreateVkSemaphore(vk::Device, 0, &m_RenderFinishedSemaphore);
        vk::CreateImageViews(vk::Device, m_SwapChainImages, m_SwapChainImageFormat,
            VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &m_SwapchainImageViews);
        vk::CreateRenderPass(vk::Device, m_SwapChainImageFormat, &m_RenderPass);

        vk::CreateFramebuffers(vk::Device, m_SwapchainImageViews, m_SwapChainExtent, m_SwapchainImageViews.size(),
            m_RenderPass, &m_Framebuffers);

        vk::CreateCommandPool(vk::Device, vk::Queues.GraphicsFamily.value(), &m_CommandPool);
        vk::CreateCommandBuffers(vk::Device, m_CommandPool, m_SwapChainImages.size(), &m_CommandBuffers);

        {
            VkDeviceSize bufferSize = sizeof(VertexShaderUBO);

            //create buffer for vertex shader uniform buffer
            vk::CreateBuffer(vk::Device, vk::PhysicalDevice,
                bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &vertexBuffer, &vertexBufferMemory);
            vkMapMemory(vk::Device, vertexBufferMemory, 0,
                bufferSize, 0, &vertexBufferMapping);

            //create buffer for fragment shader uniform buffer
            vk::CreateBuffer(vk::Device,
                vk::PhysicalDevice,
                bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &fragmentBuffer, &fragmentBufferMemory);
            vkMapMemory(vk::Device, fragmentBufferMemory, 0,
                bufferSize, 0, &fragmentBufferMapping);
        }

        {
            {
                //moved to pipeline
            }

            {
                std::vector<vk::DescriptorSize> vertexShaderDescriptors =
                {
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        1
                    },
                };

                std::vector<vk::DescriptorSize> fragmentShaderDescriptors =
                {
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        1
                    },
                };

                vk::CreateDescriptorPool(vk::Device, vertexShaderDescriptors, 1, 0, &vertexUniformPool);
                vk::CreateDescriptorPool(vk::Device, fragmentShaderDescriptors, 1, 0, &fragmentUniformPool);
            }

            {
                
                vk::DescriptorLayoutInfo vertexInfo =
                {
                    0,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    1,
                    VK_SHADER_STAGE_VERTEX_BIT
                };

                vk::DescriptorLayoutInfo fragmentInfo =
                {
                    0,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    1,
                    VK_SHADER_STAGE_FRAGMENT_BIT
                };

                vk::CreateDestriptorSetLayout(vk::Device, { vertexInfo }, 0, &VertexDescriptorSetLayout);
                vk::CreateDestriptorSetLayout(vk::Device, { fragmentInfo }, 0, &FragmentDescriptorSetLayout);

                vk::AllocateDescriptorSets(vk::Device, vertexUniformPool, { VertexDescriptorSetLayout },
                    1, &vertexUniformDescriptorSet);
                vk::AllocateDescriptorSets(vk::Device, fragmentUniformPool, { FragmentDescriptorSetLayout },
                    1, &fragmentUniformDescriptorSet);
            }

            {
                vk::UpdateBuffersInfo vertexBufferInfo =
                {
                    vertexBuffer,
                    0,
                    VK_WHOLE_SIZE,
                };

                vk::UpdateBuffersInfo fragmentBufferInfo =
                {
                    fragmentBuffer,
                    0,
                    VK_WHOLE_SIZE,
                };

                vk::UpdateDescriptorSets(vk::Device, { vertexBufferInfo }, 0, 0,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    1, vertexUniformDescriptorSet);

                vk::UpdateDescriptorSets(vk::Device, { fragmentBufferInfo },
                    0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    1, fragmentUniformDescriptorSet);
            }
        }

        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

        vk::DescriptorLayoutInfo vertexInfo =
        {
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            VK_SHADER_STAGE_VERTEX_BIT
        };

        vk::DescriptorLayoutInfo fragmentInfo =
        {
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT
        };

        vk::CreateDestriptorSetLayout(vk::Device, { vertexInfo }, 0, &VertexDescriptorSetLayout);
        vk::CreateDestriptorSetLayout(vk::Device, { fragmentInfo }, 0, &FragmentDescriptorSetLayout);

        VkPushConstantRange constantRange{};
        constantRange.size = sizeof(float);
        constantRange.offset = 0;
        constantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        vk::CreatePipeline(vk::Device, vertexShader, fragmentShader, sizeof(Vertex),
            m_SwapChainExtent, m_RenderPass,
            { constantRange }, { VertexDescriptorSetLayout, FragmentDescriptorSetLayout }, &PipelineLayout, &Pipeline);

        vk::DestroyShaderModule(vk::Device, vertexShader);
        vk::DestroyShaderModule(vk::Device, fragmentShader);
    }

    void Renderer::Deinit()
    {
        vk::DestroyBuffer(vk::Device, m_VertexBuffer);
        vk::FreeMemory(vk::Device, m_VertexBufferMemory);

        vk::DestroyPipeline(vk::Device, Pipeline);
        vk::DestroyPipelineLayout(vk::Device, PipelineLayout);

        //vk::DestroyDescriptorPool(*vk::Device, m_ImGuiDescriptorsPool);

        vk::DestroyBuffer(vk::Device, vertexBuffer);
        vk::UnmapMemory(vk::Device, vertexBufferMemory);
        vk::FreeMemory(vk::Device, vertexBufferMemory);
        vk::DestroyDescriptorPool(vk::Device, vertexUniformPool);

        vk::DestroyBuffer(vk::Device, fragmentBuffer);
        vk::UnmapMemory(vk::Device, fragmentBufferMemory);
        vk::FreeMemory(vk::Device, fragmentBufferMemory);
        vk::DestroyDescriptorPool(vk::Device, fragmentUniformPool);

        //m_VkImgui.Deinit();

        vk::DestroyFramebuffers(vk::Device, m_Framebuffers);
        vk::DestroyRenderPass(vk::Device, m_RenderPass);
        vk::DestroyImageViews(vk::Device, m_SwapchainImageViews);
        vk::DestroySwapchain(vk::Device, m_SwapChain);
        vk::DestroySemaphore(vk::Device, m_ImageAvailableSemaphore);
        vk::DestroySemaphore(vk::Device, m_RenderFinishedSemaphore);

        vk::Vk::GetInstance()->Deinit();
    }

    void Renderer::Render(float deltaTime)
    {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(vk::Device, m_SwapChain, UINT64_MAX,
            m_ImageAvailableSemaphore, nullptr, &imageIndex);

        VertexShaderUBO ubo{};
        ubo.Offset = glm::vec4(0.2f, 0.5f, 0.0f, 0.0f) * std::sin(deltaTime);
        VertexShaderUBO* dataPtr = reinterpret_cast<VertexShaderUBO*>(vertexBufferMapping);
        *dataPtr = ubo;

        FragmentShaderUBO fubo{};
        fubo.color = glm::vec4(0, 1, 1, 0);
        FragmentShaderUBO* fData = reinterpret_cast<FragmentShaderUBO*>(fragmentBufferMapping);
        *fData = fubo;

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
        vkCmdBindPipeline(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);

        vkCmdPushConstants(m_CommandBuffers[imageIndex],
            PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &deltaTime);

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

        vk::BindDescriptorSets(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
            PipelineLayout, { vertexUniformDescriptorSet, fragmentUniformDescriptorSet });

        vkCmdDraw(m_CommandBuffers[imageIndex], static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
        //vkCmdDrawIndexed(m_CommandBuffers[imageIndex], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //render IMGui
        //m_VkImgui.Render(m_CommandBuffers[imageIndex]);

        //continue
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

        vkQueueSubmit(vk::Queues.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_SwapChain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(vk::Queues.PresentQueue, &presentInfo);

        vkDeviceWaitIdle(vk::Device);
    }

    void Renderer::LoadDataIntoBuffer()
    {
        m_Vertices.reserve(3);
        m_Vertices.push_back({ glm::vec3(-1, 1, 0) });
        m_Vertices.push_back({ glm::vec3(0, -1, 0) });
        m_Vertices.push_back({ glm::vec3(1, 1, 0) });

        VkDeviceSize bufferSize = sizeof(Vertex) * m_Vertices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.flags = 0;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.queueFamilyIndexCount = 1;
        bufferInfo.pQueueFamilyIndices = &vk::Queues.GraphicsFamily.value();
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.size = sizeof(Vertex) * m_Vertices.size();

        VkResult result = vkCreateBuffer(vk::Device, &bufferInfo, nullptr, &m_VertexBuffer);

        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to create vertex buffer" << std::endl;
        }

        psm::vk::CreateBuffer(vk::Device, vk::PhysicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer, &stagingBufferMemory);

        void* data;
        vkMapMemory(vk::Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_Vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(vk::Device, stagingBufferMemory);

        psm::vk::CreateBuffer(vk::Device, vk::PhysicalDevice,
            bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_VertexBuffer, &m_VertexBufferMemory);

        psm::vk::CopyBuffer(vk::Device, m_CommandPool, vk::Queues.GraphicsQueue,
            stagingBuffer, m_VertexBuffer, bufferSize);

        psm::vk::DestroyBuffer(vk::Device, stagingBuffer);
        psm::vk::FreeMemory(vk::Device, stagingBufferMemory);
    }
}