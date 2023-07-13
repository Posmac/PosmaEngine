#pragma once

#include <vector>

#include "Render/Renderer.h"
#include "Utilities/ModelLoader.h"
#include "Utilities/TextureLoader.h"
#include "Render/Camera.h"
#include "Render/PerFrameData.h"

namespace psm
{
    class Application
    {
    public:
        Application() = default;
        void Init();
        void Update();
        void Deinit();
        void ResizeWindow(HWND hWnd);
    private:
        float m_Time = 0;
        Camera m_Camera;
        Model m_SkullModel;
        Texture m_SkullTexture;
    };
}