#include "BaseVulkan.h"

namespace psm
{
    namespace vk
    {
        BaseVulkan* BaseVulkan::s_Instance = nullptr;;

        BaseVulkan* BaseVulkan::Instance()
        {
            if (s_Instance == nullptr)
            {
                s_Instance = new BaseVulkan();
            }

            return s_Instance;
        }

        void BaseVulkan::Init(HINSTANCE hInstance, HWND hWnd)
        {
            m_VulkanData.Init(hInstance, hWnd);
            m_VkImgui.Init(hWnd, m_VulkanData.SwapChainImages.size(), m_VulkanData, &m_ImGuiDescriptorsPool);
            LoadModelData();
        }

        void BaseVulkan::Deinit()
        {
            vk::DestroyBuffer(m_VulkanData.Device, m_VertexBuffer);
            vk::FreeMemory(m_VulkanData.Device, m_VertexBufferMemory);

            vkDestroyDescriptorPool(m_VulkanData.Device, m_ImGuiDescriptorsPool, nullptr);
            m_VkImgui.Deinit();

            m_VulkanData.Deinit();
        }

        void BaseVulkan::Render()
        {
            uint32_t imageIndex;
            vkAcquireNextImageKHR(m_VulkanData.Device, m_VulkanData.SwapChain, UINT64_MAX,
                m_VulkanData.ImageAvailableSemaphore, nullptr, &imageIndex);

            vkResetCommandBuffer(m_VulkanData.CommandBuffers[imageIndex], 0);
            VkCommandBufferBeginInfo begin{};
            begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin.flags = 0;
            begin.pNext = nullptr;
            begin.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(m_VulkanData.CommandBuffers[imageIndex], &begin) != VK_SUCCESS)
            {
                std::cout << "Failed to begin command buffer" << std::endl;
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_VulkanData.RenderPass;
            renderPassInfo.framebuffer = m_VulkanData.Framebuffers[imageIndex];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_VulkanData.SwapChainExtent;

            std::array<VkClearValue, 2> clearColor{};
            clearColor[0].color = { {0.2f, 0.2f, 0.2f, 1.0f} };
            clearColor[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearColor.size());
            renderPassInfo.pClearValues = clearColor.data();

            vkCmdBeginRenderPass(m_VulkanData.CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(m_VulkanData.CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanData.Pipeline);

            VkBuffer vertexBuffers[] = { m_VertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(m_VulkanData.CommandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
            //vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(m_VulkanData.SwapChainExtent.width);
            viewport.height = static_cast<float>(m_VulkanData.SwapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(m_VulkanData.CommandBuffers[imageIndex], 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = m_VulkanData.SwapChainExtent;
            vkCmdSetScissor(m_VulkanData.CommandBuffers[imageIndex], 0, 1, &scissor);

            //vkCmdBindDescriptorSets(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
            //    m_PipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

            vkCmdDraw(m_VulkanData.CommandBuffers[imageIndex], static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
            //vkCmdDrawIndexed(m_CommandBuffers[imageIndex], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            //render IMGui
            m_VkImgui.Render(m_VulkanData.CommandBuffers[imageIndex]);

            //continue
            vkCmdEndRenderPass(m_VulkanData.CommandBuffers[imageIndex]);

            if (vkEndCommandBuffer(m_VulkanData.CommandBuffers[imageIndex]) != VK_SUCCESS)
            {
                std::cout << "Failed to end command buffer" << std::endl;
            }

            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = nullptr;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &m_VulkanData.ImageAvailableSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_VulkanData.CommandBuffers[imageIndex];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &m_VulkanData.RenderFinishedSemaphore;

            vkQueueSubmit(m_VulkanData.Queues.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &m_VulkanData.RenderFinishedSemaphore;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &m_VulkanData.SwapChain;
            presentInfo.pImageIndices = &imageIndex;

            vkQueuePresentKHR(m_VulkanData.Queues.PresentQueue, &presentInfo);

            vkDeviceWaitIdle(m_VulkanData.Device);
        }

        void BaseVulkan::LoadModelData()
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
            bufferInfo.pQueueFamilyIndices = &m_VulkanData.Queues.GraphicsFamily.value();
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferInfo.size = sizeof(Vertex) * m_Vertices.size();

            VkResult result = vkCreateBuffer(m_VulkanData.Device, &bufferInfo, nullptr, &m_VertexBuffer);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create vertex buffer" << std::endl;
            }

            vk::CreateBuffer(m_VulkanData.Device, m_VulkanData.PhysicalDevice, bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &stagingBuffer, &stagingBufferMemory);

            void* data;
            vkMapMemory(m_VulkanData.Device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, m_Vertices.data(), (size_t)bufferSize);
            vkUnmapMemory(m_VulkanData.Device, stagingBufferMemory);

            vk::CreateBuffer(m_VulkanData.Device, m_VulkanData.PhysicalDevice,
                bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_VertexBuffer, &m_VertexBufferMemory);

            vk::CopyBuffer(m_VulkanData.Device, m_VulkanData.CommandPool, m_VulkanData.Queues.GraphicsQueue,
                stagingBuffer, m_VertexBuffer, bufferSize);

            vk::DestroyBuffer(m_VulkanData.Device, stagingBuffer);
            vk::FreeMemory(m_VulkanData.Device, stagingBufferMemory);
        }
    }
}