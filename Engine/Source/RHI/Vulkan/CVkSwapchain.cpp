#include "CVkSwapchain.h"

#include "CVkDevice.h"

namespace psm
{
    RHI_RESULT CVkSwapchain::Init(CDevice* device, HINSTANCE hInstance, HWND hWnd)
    {
        m_ParentDevice = (CVkDevice*)device;

        vk::CreateSurface(hInstance, hWnd, m_ParentDevice->GetInstance(), &m_Surface);
        vk::PopulateSurfaceData(m_ParentDevice->GetInstance(), m_ParentDevice->GetPhysicalDevice(), m_Surface, &m_SurfaceData);

        return VK_SUCCESS;
    }

    VkSurfaceKHR CVkSwapchain::GetSurface()
    {
        return m_Surface;
    }
}