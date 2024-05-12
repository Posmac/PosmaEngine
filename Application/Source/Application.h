#pragma once

#include <vector>

#include "Render/Renderer.h"
#include "Utilities/ModelLoader.h"
#include "Utilities/TextureLoader.h"
#include "Render/Camera/Camera.h"
#include "Render/GlobalBuffer.h"
#include "Render/Actors/OpaqueInstances.h"

#include "RHI/RHICommon.h"

namespace psm
{
    class Application
    {
    public:
        Application() = default;
        void Init(uint32_t width, uint32_t height);
        void Update();
        void Deinit();
        void ResizeWindow(HWND hWnd, uint32_t width, uint32_t height);

        //TODO:: should be moved to another class later
        void ProcessInput();
    private:

        glm::vec2 m_Resolution;

        Camera m_Camera;
        float m_CameraDefaultMoveSpeed;
        float m_CameraDefaultRotateSpeed;

        //mouse rotation
        glm::vec2 m_PivotMousePosition;
        bool m_isMouseLBPressed;
    };
}