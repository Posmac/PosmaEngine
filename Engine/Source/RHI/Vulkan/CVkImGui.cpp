#include "CVkImGui.h"

#include "CVkDevice.h"
#include "CVkRenderPass.h"

#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Enums/ImageFormats.h"
#include "RHI/Vulkan/TypeConvertor.h"

#include "RHI/Vulkan/CVkCommandBuffer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

namespace psm
{
    CVkImGui::CVkImGui(const DevicePtr& device, const RenderPassPtr& renderPass, const CommandPoolPtr& cmdPool, uint8_t imagesCount, ESamplesCount samplesCount)
    {
        mDeviceInternal = device;

        constexpr unsigned maximumNumberOfDescriptors = 1000;
        constexpr unsigned maximumNumberOfSets = 1000;

        //create descriptor pool for everything
        std::vector<SDescriptorPoolSize> shaderDescriptors =
        {
            { EDescriptorType::SAMPLER, maximumNumberOfDescriptors },
            { EDescriptorType::COMBINED_IMAGE_SAMPLER, maximumNumberOfDescriptors },
            { EDescriptorType::SAMPLED_IMAGE, maximumNumberOfDescriptors },
            { EDescriptorType::STORAGE_IMAGE, maximumNumberOfDescriptors },
            { EDescriptorType::UNIFORM_TEXEL_BUFFER, maximumNumberOfDescriptors },
            { EDescriptorType::STORAGE_TEXEL_BUFFER, maximumNumberOfDescriptors },
            { EDescriptorType::UNIFORM_BUFFER, maximumNumberOfDescriptors },
            { EDescriptorType::STORAGE_BUFFER, maximumNumberOfDescriptors },
            { EDescriptorType::UNIFORM_BUFFER_DYNAMIC, maximumNumberOfDescriptors },
            { EDescriptorType::STORAGE_BUFFER_DYNAMIC, maximumNumberOfDescriptors },
            { EDescriptorType::INPUT_ATTACHMENT, maximumNumberOfDescriptors }
        };

        SDescriptorPoolConfig descriptorPoolConfig =
        {
            .DesciptorPoolSizes = shaderDescriptors,
            .MaxDesciptorPools = maximumNumberOfSets
        };

        mDescriptorPool = mDeviceInternal->CreateDescriptorPool(descriptorPoolConfig);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        /*if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }*/

        // Setup Platform/Renderer backends
        DeviceData data = mDeviceInternal->GetDeviceData();

        ImGui_ImplVulkan_InitInfo info =
        {
            .Instance = reinterpret_cast<VkInstance>(data.vkData.Instance),
            .PhysicalDevice = reinterpret_cast<VkPhysicalDevice>(data.vkData.PhysicalDevice),
            .Device = reinterpret_cast<VkDevice>(data.vkData.Device),
            .QueueFamily = data.vkData.GraphicsQueueIndex,
            .Queue = reinterpret_cast<VkQueue>(data.vkData.GraphicsQueue),
            .PipelineCache = nullptr,
            .DescriptorPool = reinterpret_cast<VkDescriptorPool>(mDescriptorPool->Raw()),
            .Subpass = 0,
            .MinImageCount = imagesCount,          // >= 2
            .ImageCount = imagesCount,             // >= MinImageCount
            .MSAASamples = ToVulkan(samplesCount),            // >= VK_SAMPLE_COUNT_1_BIT (0 -> default to VK_SAMPLE_COUNT_1_BIT)
        };

        ImGui_ImplVulkan_Init(&info, reinterpret_cast<VkRenderPass>(renderPass->Raw()));

        {
            SCommandBufferConfig bufferConfig =
            {
                 .Size = 1,
                 .IsBufferLevelPrimary = true,
            };

            std::vector<CommandBufferPtr> cmdBuffs = mDeviceInternal->CreateCommandBuffers(cmdPool, bufferConfig);
            CommandBufferPtr& cmdBuff = cmdBuffs[0];

            SCommandBufferBeginConfig begin =
            {
                .Usage = ECommandBufferUsage::ONE_TIME_SUBMIT_BIT 
            };

            cmdBuff->Begin(begin);

            ImGui_ImplVulkan_CreateFontsTexture(reinterpret_cast<VkCommandBuffer>(cmdBuff->Raw()));

            cmdBuff->End();

            SFenceConfig fenceConfig =
            {
                .Signaled = false
            };

            FencePtr submitFence = mDeviceInternal->CreateFence(fenceConfig);

            SSubmitConfig submitConfig =
            {
                .Queue = mDeviceInternal->GetDeviceData().vkData.GraphicsQueue, //not sure if Queue should be abstracted to CVk(IQueue)
                .SubmitCount = 1,
                .WaitStageFlags = {},
                .WaitSemaphores = {},
                .CommandBuffers = {cmdBuff},
                .SignalSemaphores = {},
                .Fence = submitFence,
            };
            mDeviceInternal->SubmitQueue(submitConfig);
            mDeviceInternal->WaitIdle();

            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    CVkImGui::~CVkImGui()
    {
        LogMessage(psm::MessageSeverity::Info, "CVkImGui destructor");

        mDeviceInternal = nullptr;
        mDescriptorPool = nullptr;
        ImGui_ImplVulkan_Shutdown();
    }

    void CVkImGui::PrepareNewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void CVkImGui::Render(CommandBufferPtr commandBuffer)
    {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), reinterpret_cast<VkCommandBuffer>(commandBuffer->Raw()));
    }
}