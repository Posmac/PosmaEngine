#pragma once

#include <Windows.h>
#include <map>

#include "glm/glm.hpp"

#include "Keys.h"

namespace psm
{
    struct KeyState
    {
        bool WasDown;
        bool IsDown;
    };

    class InputSystem
    {
        //singleton realization
    public:
        InputSystem(InputSystem&) = delete;
        void operator=(const InputSystem&) = delete;
        static InputSystem* Instance();
    private:
        InputSystem();
        static InputSystem* s_Instance;
        //class specific
    public:
        bool IsKeyPressed(ControlKeys key);
        bool IsKeyReleased(ControlKeys key);
        bool IsKeyDown(ControlKeys key);
        bool IsKeyUp(ControlKeys key);
        bool IsMouseButtonDown(MouseKeys key);
        bool IsMouseButtonUp(MouseKeys key);
        bool IsMouseButtonPressed(MouseKeys key);
        bool IsMouseButtonReleased(MouseKeys key);
        glm::vec2 GetMousePosition();

        void Update();
        void ListenControlsKeyPressed(LPARAM lParam, WPARAM wParam);
        void ListenMouseButtonsPressed(LPARAM lParam, WPARAM wParam);
        void ListenScroll(WPARAM wParam);
    private:
        std::map<ControlKeys, KeyState> m_ControlKeys;
        std::map<MouseKeys, KeyState> m_MouseKeys;
        int32_t m_CurrentFrameScroll;
    };
}