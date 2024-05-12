#pragma once

#include <memory>

#include "RHI/Interface/ImGui.h"
#include "RHI/Interface/Types.h"
#include "RHI/Configs/ShadersConfig.h"
#include "RHI/Enums/ImageFormats.h"
#include "RHI/Vulkan/TypeConvertor.h"

namespace psm
{
    class CVkImGui : public IImGui, public std::enable_shared_from_this<CVkImGui>
    {
    public:
        CVkImGui(const DevicePtr& device, const RenderPassPtr& renderPass, const CommandPoolPtr& commandPool, uint8_t imagesCount, ESamplesCount samplesCount);
        virtual ~CVkImGui();

        virtual void PrepareNewFrame() override;
        virtual void Render(CommandBufferPtr commandBuffer) override;
    private:
        DevicePtr mDeviceInternal;
        DescriptorPoolPtr mDescriptorPool;
    };
}