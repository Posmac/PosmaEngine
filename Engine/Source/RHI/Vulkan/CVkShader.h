#pragma once

#include <string>
#include <memory>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/Shaders.h"
#include "RHI/Configs/ShadersConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkShader : public IShader, std::enable_shared_from_this<CVkShader>
    {
    public:
        CVkShader(DevicePtr device, const std::string path, EShaderStageFlag shaderType);
        virtual ~CVkShader();

        virtual void* GetPointer() override;
    private:
        VkDevice mDeviceInternal;
        VkShaderModule mShader;
        EShaderStageFlag mShaderType;
        std::string mName;
    };
}