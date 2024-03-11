#include "CVkShader.h"

#include "CVkDevice.h"
#include "RHI/VkCommon.h"

#include <iostream>
#include <fstream>

namespace psm
{
    CVkShader::CVkShader(DevicePtr device, const std::string path, EShaderStageFlag shaderType)
    {
        mDeviceInternal = reinterpret_cast<VkDevice>(device->GetDeviceData().vkData.Device);

        std::ifstream fileStream(path, std::ios::ate | std::ios::binary);
        if(!fileStream.is_open())
        {
            LogMessage(MessageSeverity::Error, "File path: " + path + " is wrong!");
            return;
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

        VkResult result = vkCreateShaderModule(mDeviceInternal, &shaderModuleCreateInfo, nullptr, &mShader);
        VK_CHECK_RESULT(result);
    }

    CVkShader::~CVkShader()
    {
        vkDestroyShaderModule(mDeviceInternal, mShader, nullptr);
    }
}