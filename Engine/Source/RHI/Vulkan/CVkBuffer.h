#pragma once

#include "RenderBackend/Core.h"
#include "../Interface/Buffer.h"

namespace psm
{
    class CVkBuffer : CBuffer
    {
    public:
        CVkBuffer() = default;
        virtual ~CVkBuffer() = default;
        virtual RHI_RESULT Init(CDevice* pDevice,
                                SResourceSize size,
                                EBufferUsage usage,
                                EMemoryProperties memoryProperties) override;
        virtual RHI_RESULT Map() override;
        virtual RHI_RESULT Unmap() override;
    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;
    };
}