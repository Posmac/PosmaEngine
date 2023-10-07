#pragma once

#include <vector>

#include "Render/Renderer.h"
#include "Utilities/ModelLoader.h"
#include "Utilities/TextureLoader.h"
#include "Render/Camera.h"
#include "Render/PerFrameData.h"
#include "Render/Instances/OpaqueInstances.h"

namespace psm
{
    class Application
    {
    public:
        Application() = default;
        void Init();
        void Update();
        void Deinit();
        void ResizeWindow(HWND hWnd, uint32_t width, uint32_t height);
    private:
        float m_Time = 0;
        Camera m_Camera;
    };
}