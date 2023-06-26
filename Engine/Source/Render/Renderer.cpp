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

        //later move into windows class
        vk::CreateSwapchain(vk::Device, vk::PhysicalDevice, vk::Surface, vk::SurfData, &m_SwapChain, &m_SwapChainImageFormat,
            &m_SwapChainExtent);

        vk::QuerrySwapchainImages(vk::Device, m_SwapChain, m_SwapChainImageFormat, &m_SwapChainImages, &m_SwapchainImageViews);
        vk::CreateVkSemaphore(vk::Device, 0, &m_ImageAvailableSemaphore);
        vk::CreateVkSemaphore(vk::Device, 0, &m_RenderFinishedSemaphore);
        vk::CreateImageViews(vk::Device, m_SwapChainImages, m_SwapChainImageFormat,
            VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, &m_SwapchainImageViews);

        //render pass creation
        constexpr uint32_t colorAttachmentDescriptionCount = 1;
        VkAttachmentDescription colorAttachmentDescriptions[] =
        {
            0,                                //flags
            m_SwapChainImageFormat,           //attachment format
            VK_SAMPLE_COUNT_1_BIT,            //samples number
            VK_ATTACHMENT_LOAD_OP_CLEAR,      //load op
            VK_ATTACHMENT_STORE_OP_STORE,     //store op
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  //stencil load op
            VK_ATTACHMENT_STORE_OP_DONT_CARE, //stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,        //initialLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   //final layout
        };

        constexpr uint32_t colorAttachmentRefCount = 1;
        VkAttachmentReference colorAttachmentReferences[colorAttachmentRefCount] =
        {
            0,                                        //attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, //layout
        };

        constexpr uint32_t subpassDescrCount = 1;
        VkSubpassDescription subpassDescr[subpassDescrCount] =
        {
             0,                               // flags
             VK_PIPELINE_BIND_POINT_GRAPHICS, //pipeline bind point
             0,                               // input attachment count
             nullptr,                         //pInput attachments
             colorAttachmentRefCount,         // ColorAttachments Count
             colorAttachmentReferences,       //pColorAttachments
             nullptr,                         //pResolveAttachments
             nullptr,                         //pDepthStencilAttachment
             0,                               //preserveAttachmentCount
             nullptr                          //pPreserveAttachments
        };

        vk::CreateRenderPass(vk::Device, colorAttachmentDescriptions, colorAttachmentDescriptionCount,
            subpassDescr, subpassDescrCount, &m_RenderPass);

        vk::CreateFramebuffers(vk::Device, m_SwapchainImageViews, m_SwapChainExtent, m_SwapchainImageViews.size(),
            m_RenderPass, &m_Framebuffers);

        //command buffers
        vk::CreateCommandPool(vk::Device, vk::Queues.GraphicsFamily.value(), &m_CommandPool);
        vk::CreateCommandBuffers(vk::Device, m_CommandPool, m_SwapChainImages.size(), &m_CommandBuffers);

        {
            //create buffer for both shaders uniform buffers
            vk::CreateBufferAndMapMemory(vk::Device, vk::PhysicalDevice, sizeof(ShaderUBO),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &shaderBuffer, &shaderBufferMemory, &shaderBufferMapping);

            //image creation
            putils::RawTextureData textureData{ putils::RGB_Type::Rgb_alpha };
            putils::LoadRawTextureData("Textures/texture.jpg", &textureData);

            vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 }, 1, 1,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                &image, &imageMemory, &imageView);

            vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                textureData.Width * textureData.Height * textureData.Type,
                textureData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &image);

            putils::CleanRawTextureData(textureData.Data);

            //create image sampler
            vk::CreateTextureSampler(vk::Device, VK_FILTER_LINEAR,
                VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_REPEAT, false, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                false, VK_COMPARE_OP_ALWAYS, 0, 0.0, 0.0, 0.0, VK_SAMPLER_MIPMAP_MODE_LINEAR, false, &sampler);

            //load another texture
            putils::RawTextureData cobbleData{ putils::RGB_Type::Rgb_alpha };
            putils::LoadRawTextureData("Textures/Cobblestone/Cobblestone_albedo.png", &cobbleData);

            vk::CreateImageAndView(vk::Device, vk::PhysicalDevice,
                { (uint32_t)cobbleData.Width, (uint32_t)cobbleData.Height, 1 }, 1, 1,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
                &cobbleImage, &cobbleImageMemory, &cobbleImageView);

            vk::LoadDataIntoImageUsingBuffer(vk::Device, vk::PhysicalDevice,
                cobbleData.Width * cobbleData.Height * cobbleData.Type,
                cobbleData.Data, m_CommandPool, vk::Queues.GraphicsQueue,
                { (uint32_t)cobbleData.Width, (uint32_t)cobbleData.Height, 1 },
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &cobbleImage);

            putils::CleanRawTextureData(cobbleData.Data);
        }

        PrepareDescriptorSets();

        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.vert.txt", &vertexShader);
        vk::CreateShaderModule(vk::Device, "../Engine/Shaders/triangle.frag.txt", &fragmentShader);

        //pipeline layout
        constexpr uint32_t pushConstantsSize = 1;
        VkPushConstantRange pushConstants[pushConstantsSize] =
        {
            {
                VK_SHADER_STAGE_FRAGMENT_BIT, // stage flags
                0,                            // offset
                sizeof(float),                // size
            },
        };

        constexpr uint32_t descriptorSetLayoutsSize = 1;
        VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsSize] =
        {
            ShaderDescriptorSetLayout
        };

        vk::CreatePipelineLayout(vk::Device,
            descriptorSetLayouts,
            descriptorSetLayoutsSize,
            pushConstants,
            pushConstantsSize,
            &PipelineLayout);

        //shader stages
        constexpr size_t modulesSize = 2;
        vk::ShaderModuleInfo modules[modulesSize] =
        {
            {
                vertexShader,                 // shader module 
                VK_SHADER_STAGE_VERTEX_BIT,   // VkShaderStageFlag
                "main"                        // entry point
            },
            {
                fragmentShader,               // shader module 
                VK_SHADER_STAGE_FRAGMENT_BIT, // VkShaderStageFlag
                "main"                        // entry point
            },
        };

        VkPipelineShaderStageCreateInfo stages[modulesSize];
        vk::GetPipelineShaderStages(modules, modulesSize, stages);

        //vertex input
        constexpr size_t attribsSize = 2;
        VkVertexInputAttributeDescription vertexAttribDescr[attribsSize] =
        {
            {
                0,                              // location
                0,                              // binding
                VK_FORMAT_R32G32B32A32_SFLOAT,  // format
                offsetof(Vertex, Position)      // offset
            },
            {
                1,                              // location
                0,                              // binding
                VK_FORMAT_R32G32_SFLOAT,        // format
                offsetof(Vertex, TexCoord)      // offset
            },
        };

        constexpr size_t bindingsSize = 1;
        VkVertexInputBindingDescription bindingDescriptions[bindingsSize] =
        {
            {
                0,                          // binding
                sizeof(Vertex),             // string
                VK_VERTEX_INPUT_RATE_VERTEX // input rate
            },
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vk::GetVertexInputInfo(vertexAttribDescr, attribsSize, 
            bindingDescriptions, bindingsSize, &vertexInputState);

        //input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::GetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, &inputAssemblyInfo);

        //create graphics pipeline (a lot of default things)
        vk::CreateGraphicsPipeline(vk::Device, m_SwapChainExtent,
            m_RenderPass, PipelineLayout, stages, modulesSize,
            vertexInputState, inputAssemblyInfo, &Pipeline);

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
        VkResult result = vkAcquireNextImageKHR(vk::Device, m_SwapChain, UINT64_MAX,
            m_ImageAvailableSemaphore, nullptr, &imageIndex);
        VK_CHECK_RESULT(result);

        ShaderUBO ubo{};
        ubo.Offset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        ubo.Color = glm::vec4(1, 0, 0, 1);
        ShaderUBO* dataPtr = reinterpret_cast<ShaderUBO*>(shaderBufferMapping);
        *dataPtr = ubo;

        result = vkResetCommandBuffer(m_CommandBuffers[imageIndex], 0);
        VK_CHECK_RESULT(result);

        vk::BeginCommandBuffer(m_CommandBuffers[imageIndex], 0);

        std::array<VkClearValue, 2> clearColor{};
        clearColor[0].color = { {0.2f, 0.2f, 0.2f, 1.0f} };
        clearColor[1].depthStencil = { 1.0f, 0 };

        vk::BeginRenderPass(m_RenderPass, m_Framebuffers[imageIndex],
            {0, 0}, m_SwapChainExtent, clearColor.data(), clearColor.size(), 
            m_CommandBuffers[imageIndex], VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);

        vkCmdPushConstants(m_CommandBuffers[imageIndex],
            PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &deltaTime);

        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        //vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vk::SetViewPortAndScissors(m_CommandBuffers[imageIndex],
            0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width), 
            static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f,
            {0, 0}, m_SwapChainExtent);

        vk::BindDescriptorSets(m_CommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
            PipelineLayout, { shaderUniformDescriptorSet });

        vkCmdDraw(m_CommandBuffers[imageIndex], static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
        //vkCmdDrawIndexed(m_CommandBuffers[imageIndex], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //render IMGui
        //m_VkImgui.Render(m_CommandBuffers[imageIndex]);

        //continue
        vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

        vk::EndCommandBuffer(m_CommandBuffers[imageIndex]);

        vk::Submit(vk::Queues.GraphicsQueue, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            &m_ImageAvailableSemaphore, 1, &m_CommandBuffers[imageIndex], 1, 
            &m_RenderFinishedSemaphore, 1, VK_NULL_HANDLE);

        vk::Present(vk::Queues.PresentQueue, &m_RenderFinishedSemaphore, 1, 
            &m_SwapChain, 1, &imageIndex);

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

    void Renderer::PrepareDescriptorSets()
    {
        {
            std::vector<vk::DescriptorPoolSize> shaderDescriptors =
            {
                {
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    2
                },
                {
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    2
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
                },
                {
                    3,
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
            std::vector<vk::UpdateBuffersInfo> buffersInfo =
            {
                 {
                     {
                         //VkDescriptorBufferInfo
                         shaderBuffer,                // buffer
                         offsetof(ShaderUBO, Offset), // offset
                         sizeof(glm::vec4),           // range
                      },

                      0,                                 // binding
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
                 },
                 {
                     {
                         //VkDescriptorBufferInfo
                         shaderBuffer,               // buffer
                         offsetof(ShaderUBO, Color), // offset
                         sizeof(glm::vec4),          // range
                     },

                      1,                                 // binding
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
                 },
            };

            std::vector<vk::UpdateTextureInfo> imagesInfo =
            {
                {
                    {
                        //VkDescriptorImageInfo
                        sampler,                                    // sampler
                        imageView,                                  // image view
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,   // image layout
                    },

                    2,                                              // binding
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // descriptor type
                },

                {
                    {
                        //VkDescriptorImageInfo
                        sampler,                                    // sampler
                        cobbleImageView,                            // image view
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,   // image layout
                    },

                    3,                                              // binding
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // descriptor type
                },
            };

            vk::UpdateDescriptorSets(vk::Device, shaderUniformDescriptorSet, buffersInfo, imagesInfo,
                buffersInfo.size() + imagesInfo.size());
        }
    }
}