#include "VulkanImGui.h"
#include "Render/Vk.h"

namespace psm
{
    namespace vk
    {
        void VulkanImGui::Init(HWND hWnd, uint32_t swapchainImagesCount,
            VkDescriptorPool* descriptorPool, VkRenderPass renderPass, 
            VkQueue graphicsQueue, uint32_t queueFamily,
            VkCommandPool cmdPool, VkCommandBuffer cmdBuf)
        {
            IMGUI_CHECKVERSION();
            ImGui::SetCurrentContext(ImGui::CreateContext());
            ImGui::StyleColorsDark();
            ImGui_ImplWin32_Init(hWnd);

            //create descriptor pool for ImGui
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
            VkResult result = vkCreateDescriptorPool(vk::Device, &pool_info, nullptr, descriptorPool);
            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create descriptor pool" << std::endl;
            }

            //init imgui
            ImGui_ImplVulkan_InitInfo imguiInfo{};
            imguiInfo.Instance = vk::Instance;
            imguiInfo.PhysicalDevice = vk::PhysicalDevice;
            imguiInfo.Device = vk::Device;
            imguiInfo.QueueFamily = queueFamily;
            imguiInfo.Queue = graphicsQueue;
            imguiInfo.PipelineCache = nullptr;
            imguiInfo.DescriptorPool = *descriptorPool;
            imguiInfo.Subpass = 0;
            imguiInfo.MinImageCount = swapchainImagesCount;
            imguiInfo.ImageCount = swapchainImagesCount;
            imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            imguiInfo.Allocator = nullptr;
            imguiInfo.CheckVkResultFn = nullptr;

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            ImGui_ImplVulkan_Init(&imguiInfo, renderPass);
            {
                // Use any command queue
                vkResetCommandPool(vk::Device, cmdPool, 0);
                VkCommandBufferBeginInfo begin_info = {};
                begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                vkBeginCommandBuffer(cmdBuf, &begin_info);

                ImGui_ImplVulkan_CreateFontsTexture(cmdBuf);

                VkSubmitInfo end_info = {};
                end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                end_info.commandBufferCount = 1;
                end_info.pCommandBuffers = &cmdBuf;
                vkEndCommandBuffer(cmdBuf);
                vkQueueSubmit(graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

                vkDeviceWaitIdle(vk::Device);
                ImGui_ImplVulkan_DestroyFontUploadObjects();
            }
        }

        void VulkanImGui::Deinit()
        {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        void VulkanImGui::Render(VkCommandBuffer commandBuffer)
        {
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
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        }
    }
}