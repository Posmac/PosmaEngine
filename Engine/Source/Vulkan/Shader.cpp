#include "Shader.h"

namespace psm
{
    namespace vk
    {
        VkShaderModule CreateShaderModule(VkDevice logicalDevice, const std::string& path)
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
            VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo,
                nullptr, &module);

            if (result != VK_SUCCESS)
            {
                std::cout << "Failed to create shader module" << std::endl;
                return nullptr;
            }

            return module;
        }
    }
}