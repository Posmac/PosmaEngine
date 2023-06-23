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
            //create buffer for both shaders uniform buffers
            vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice, sizeof(ShaderUBO),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &shaderBuffer, &shaderBufferMemory, &shaderBufferMapping);

            //image creation
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;

            putils::RawTextureData textureData{ putils::RGB_Type::Rgb_alpha };

            putils::LoadRawTextureData("Textures/texture.jpg", &textureData);

            VkDeviceSize imageSize = textureData.Width * textureData.Height * textureData.Type;

            vk::CreateBuffer(vk::Device, vk::PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &stagingBuffer, &stagingBufferMemory);

            void* data;
            VkResult result = vkMapMemory(vk::Device, stagingBufferMemory, 0, imageSize, 0, &data);
            VK_CHECK_RESULT(result);

            memcpy(data, textureData.Data, static_cast<size_t>(imageSize));
            vkUnmapMemory(vk::Device, stagingBufferMemory);

            putils::CleanRawTextureData(textureData.Data);

            vk::CreateImage(vk::Device, vk::PhysicalDevice,
                { static_cast<uint32_t>(textureData.Width),
                static_cast<uint32_t>(textureData.Height), 1 },
                1, 1, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0, &image, &imageMemory);
            vk::CreateImageView(vk::Device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D,
                VK_IMAGE_ASPECT_COLOR_BIT, &imageView);

            vk::ImageLayoutTransition(vk::Device, m_CommandPool, vk::Queues.GraphicsQueue,
                image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            vk::CopyBufferToImage(vk::Device, m_CommandPool, vk::Queues.GraphicsQueue, stagingBuffer, image,
                { static_cast<uint32_t>(textureData.Width), static_cast<uint32_t>(textureData.Height), 1 });
            vk::ImageLayoutTransition(vk::Device, m_CommandPool, vk::Queues.GraphicsQueue,
                image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            //create image sampler
            vk::CreateTextureSampler(vk::Device, VK_FILTER_LINEAR,
                VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_REPEAT, false, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                false, VK_COMPARE_OP_ALWAYS, 0, 0.0, 0.0, 0.0, VK_SAMPLER_MIPMAP_MODE_LINEAR, false, &sampler);
        }

        {
            {
                //moved to pipeline
            }

            {
                std::vector<vk::DescriptorPoolSize> shaderDescriptors =
                {
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        2
                    },
                    {
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        1
                    }
                };

                vk::CreateDescriptorPool(vk::Device, shaderDescriptors, 1, 0, &shaderUniformPool);
            }

            {
                std::vector<vk::DescriptorLayoutInfo> shaderDescriptorInfo =
                {
                    {
                        0,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        1,
                        VK_SHADER_STAGE_VERTEX_BIT
                    },
                    {
                        1,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        1,
                        VK_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        2,
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        1,
                        VK_SHADER_STAGE_FRAGMENT_BIT
                    }
                };

                vk::CreateDestriptorSetLayout(vk::Device, { shaderDescriptorInfo }, 0, &ShaderDescriptorSetLayout);

                vk::AllocateDescriptorSets(vk::Device, shaderUniformPool, { ShaderDescriptorSetLayout },
                    1, &shaderUniformDescriptorSet);
            }

            {
                std::vector<VkDescriptorBufferInfo> buffersInfo =
                {
                     {
                         shaderBuffer,
                         offsetof(ShaderUBO, Offset),
                         sizeof(glm::vec4),
                    },
                    {
                        shaderBuffer,
                        offsetof(ShaderUBO, Color),
                        sizeof(glm::vec4),
                    },
                };

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageView = imageView;
                imageInfo.sampler = sampler;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                std::vector<VkWriteDescriptorSet> writeDescriptors(3);
                writeDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptors[0].pNext = nullptr;
                writeDescriptors[0].dstBinding = 0;
                writeDescriptors[0].dstArrayElement = 0;
                writeDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptors[0].descriptorCount = 1;
                writeDescriptors[0].pBufferInfo = &buffersInfo[0];
                writeDescriptors[0].pImageInfo = nullptr;
                writeDescriptors[0].pTexelBufferView = nullptr;
                writeDescriptors[0].dstSet = shaderUniformDescriptorSet;

                writeDescriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptors[1].pNext = nullptr;
                writeDescriptors[1].dstBinding = 1;
                writeDescriptors[1].dstArrayElement = 0;
                writeDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptors[1].descriptorCount = 1;
                writeDescriptors[1].pBufferInfo = &buffersInfo[1];
                writeDescriptors[1].pImageInfo = nullptr;
                writeDescriptors[1].pTexelBufferView = nullptr;
                writeDescriptors[1].dstSet = shaderUniformDescriptorSet;

                writeDescriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptors[2].pNext = nullptr;
                writeDescriptors[2].dstBinding = 2;
                writeDescriptors[2].dstArrayElement = 0;
                writeDescriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writeDescriptors[2].descriptorCount = 1;
                writeDescriptors[2].pBufferInfo = nullptr;
                writeDescriptors[2].pImageInfo = &imageInfo;
                writeDescriptors[2].pTexelBufferView = nullptr;
                writeDescriptors[2].dstSet = shaderUniformDescriptorSet;

                vk::UpdateDescriptorSets(vk::Device, writeDescriptors);
            }
        }

        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

        VkPushConstantRange constantRange{};
        constantRange.size = sizeof(float);
        constantRange.offset = 0;
        constantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        vk::CreatePipeline(vk::Device, vertexShader, fragmentShader, sizeof(Vertex),
            m_SwapChainExtent, m_RenderPass,
            { constantRange }, { ShaderDescriptorSetLayout }, &PipelineLayout, &Pipeline);

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

        vk::DestroyBuffer(vk::Device, shaderBuffer);
        vk::UnmapMemory(vk::Device, shaderBufferMemory);
        vk::FreeMemory(vk::Device, shaderBufferMemory);
        vk::DestroyDescriptorPool(vk::Device, shaderUniformPool);

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

        ShaderUBO ubo{};
        ubo.Offset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        ubo.Color = glm::vec4(1, 0, 0, 1);
        ShaderUBO* dataPtr = reinterpret_cast<ShaderUBO*>(shaderBufferMapping);
        *dataPtr = ubo;

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
            PipelineLayout, { shaderUniformDescriptorSet });

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
        m_Vertices.reserve(6);
        m_Vertices.push_back({ glm::vec4(-1, -1, 0, 1), glm::vec2(0, 0) });
        m_Vertices.push_back({ glm::vec4(-1, 1, 0, 1), glm::vec2(0, 1) });
        m_Vertices.push_back({ glm::vec4(1, 1, 0, 1), glm::vec2(1, 1) });

        m_Vertices.push_back({ glm::vec4(-1, -1, 0, 1), glm::vec2(0, 0) });
        m_Vertices.push_back({ glm::vec4(1, 1, 0, 1), glm::vec2(1, 1) });
        m_Vertices.push_back({ glm::vec4(1, -1, 0, 1), glm::vec2(1, 0) });

        VkDeviceSize bufferSize = sizeof(Vertex) * m_Vertices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer, &stagingBufferMemory);

        vk::CreateBuffer(vk::Device, vk::PhysicalDevice, bufferSize, 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            &m_VertexBuffer, &m_VertexBufferMemory);

        void* data;
        vkMapMemory(vk::Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_Vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(vk::Device, stagingBufferMemory);

        vk::CopyBuffer(vk::Device, m_CommandPool, vk::Queues.GraphicsQueue,
            stagingBuffer, m_VertexBuffer, bufferSize);

        vk::DestroyBuffer(vk::Device, stagingBuffer);
        vk::FreeMemory(vk::Device, stagingBufferMemory);
    }
}