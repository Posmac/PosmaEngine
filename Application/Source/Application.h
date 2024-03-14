#pragma once

#include <vector>

#include "Render/Renderer.h"
#include "Utilities/ModelLoader.h"
#include "Utilities/TextureLoader.h"
#include "Render/Camera/Camera.h"
#include "Render/PerFrameData.h"
#include "Render/Actors/OpaqueInstances.h"

#include "RHI/VkCommon.h"

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

        //TODO:: should be moved to another class later
        void ProcessInput();
    private:
        Camera m_Camera;
        float m_CameraDefaultMoveSpeed;
        float m_CameraDefaultRotateSpeed;
    };
}