#include "CVkBuffer.h"

#include "RenderBackend/Buffer.h"
#include "TypeConvertor.h"
#include "CVkDevice.h"

namespace psm
{
    RHI_RESULT CVkBuffer::Init(CDevice* pDevice, SResourceSize size, EBufferUsage usage, EMemoryProperties memoryProperties)
    {
        CVkDevice* device = (CVkDevice*)pDevice;

        vk::CreateBuffer(device->GetDevice(),
                         device->GetPhysicalDevice(),
                         size,
                         ToVulkan(usage),
                         ToVulkan(memoryProperties),
                         &m_Buffer,
                         &m_Memory);

        return VK_SUCCESS;
    }

    RHI_RESULT CVkBuffer::Map()
    {
        return VK_SUCCESS;
    }

    RHI_RESULT CVkBuffer::Unmap()
    {
        return VK_SUCCESS;
    }
}