#pragma once

#include <Windows.h>
#include <map>

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
        InputSystem() = default;
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

        void ListenControlsKeyPressed(uint32_t keyCode, bool wasDown, bool isDown);
        void ListenMouseButtonsPressed();
    private:
        std::map<ControlKeys, KeyState> m_ControlKeys;
        std::map<MouseKeys, KeyState> m_MouseKeys;
    };
}