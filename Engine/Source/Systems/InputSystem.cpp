#include "InputSystem.h"

#include <iostream>

namespace psm
{
    InputSystem* InputSystem::s_Instance = nullptr;;

    InputSystem* InputSystem::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new InputSystem();
        }

        return s_Instance;
    }

    InputSystem::InputSystem()
    {
        for(uint32_t key = 0; key < MAX; key++)
        {
            m_MouseKeys[static_cast<MouseKeys>(key)].IsDown = false;
            m_MouseKeys[static_cast<MouseKeys>(key)].WasDown = true;
        }
        m_CurrentFrameScroll = 0;
    }

    bool InputSystem::IsKeyPressed(ControlKeys key)
    {
        if(m_ControlKeys[key].WasDown == false &&
           m_ControlKeys[key].IsDown == true)
        {
            m_ControlKeys[key].WasDown = true;
            m_ControlKeys[key].IsDown = true;

            return true;
        }

        return false;
    }

    bool InputSystem::IsKeyReleased(ControlKeys key)
    {
        if(m_ControlKeys[key].WasDown == true &&
           m_ControlKeys[key].IsDown == false)
        {
            m_ControlKeys[key].WasDown = false;
            m_ControlKeys[key].IsDown = false;

            return true;
        }

        return false;
    }

    bool InputSystem::IsKeyDown(ControlKeys key)
    {
        return m_ControlKeys[key].IsDown;
    }

    bool InputSystem::IsKeyUp(ControlKeys key)
    {
        return !m_ControlKeys[key].IsDown;
    }

    bool InputSystem::IsMouseButtonDown(MouseKeys key)
    {
        return m_MouseKeys[key].IsDown;
    }

    bool InputSystem::IsMouseButtonUp(MouseKeys key)
    {
        return !m_MouseKeys[key].IsDown;
    }

    bool InputSystem::IsMouseButtonPressed(MouseKeys key)
    {
        if(m_MouseKeys[key].IsDown && m_MouseKeys[key].WasDown)
        {
            m_MouseKeys[key].WasDown = false;
            return true;
        }
        return false;
    }

    bool InputSystem::IsMouseButtonReleased(MouseKeys key)
    {
        if(!m_MouseKeys[LEFT].IsDown && !m_MouseKeys[key].WasDown)
        {
            m_MouseKeys[key].WasDown = true;
            return true;
        }
        return false;
    }

    glm::vec2 InputSystem::GetMousePosition()
    {
        POINT point{};
        if(!GetCursorPos(&point))
        {
            DebugBreak();
        }

        return glm::vec2(point.x, point.y);
    }

    void InputSystem::Update()
    {
        m_CurrentFrameScroll = 0;
    }

    void InputSystem::ListenControlsKeyPressed(LPARAM lParam, WPARAM wParam)
    {
        uint32_t keyCode = wParam;
        bool WasDown = (lParam & (1 << 30)) != 0;
        bool IsDown = (lParam & (1 << 31)) == 0;

        if(keyCode >= 'A' && keyCode <= 'Z')
        {
            ControlKeys dewcinKeycode = static_cast<ControlKeys>(keyCode - 'A');
            m_ControlKeys[dewcinKeycode].IsDown = IsDown;
            m_ControlKeys[dewcinKeycode].WasDown = WasDown;
        }
        else if(keyCode == VK_UP)
        {
            m_ControlKeys[KEY_UP].IsDown = IsDown;
            m_ControlKeys[KEY_UP].WasDown = WasDown;
        }
        else if(keyCode == VK_DOWN)
        {
            m_ControlKeys[KEY_DOWN].IsDown = IsDown;
            m_ControlKeys[KEY_DOWN].WasDown = WasDown;
        }
        else if(keyCode == VK_LEFT)
        {
            m_ControlKeys[KEY_LEFT].IsDown = IsDown;
            m_ControlKeys[KEY_LEFT].WasDown = WasDown;
        }
        else if(keyCode == VK_RIGHT)
        {
            m_ControlKeys[KEY_RIGHT].IsDown = IsDown;
            m_ControlKeys[KEY_RIGHT].WasDown = WasDown;
        }
        else if(keyCode >= '0' && keyCode <= '9')
        {
            ControlKeys keycode = static_cast<ControlKeys>(keyCode - '0' + KEY_0);
            m_ControlKeys[keycode].IsDown = IsDown;
            m_ControlKeys[keycode].WasDown = WasDown;
        }
        else if(keyCode == VK_OEM_MINUS)
        {
            m_ControlKeys[KEY_MINUS].IsDown = IsDown;
            m_ControlKeys[KEY_MINUS].WasDown = WasDown;
        }
        else if(keyCode == VK_OEM_PLUS)
        {
            m_ControlKeys[KEY_PLUS].IsDown = IsDown;
            m_ControlKeys[KEY_PLUS].WasDown = WasDown;
        }
        else if(keyCode == VK_SHIFT)
        {
            m_ControlKeys[KEY_SHIFT].IsDown = IsDown;
            m_ControlKeys[KEY_SHIFT].WasDown = WasDown;
        }
        else if(keyCode == VK_CONTROL)
        {
            m_ControlKeys[KEY_CONTROL].IsDown = IsDown;
            m_ControlKeys[KEY_CONTROL].WasDown = WasDown;
        }
        else if(keyCode == VK_MENU)
        {
            m_ControlKeys[KEY_ALT].IsDown = IsDown;
            m_ControlKeys[KEY_ALT].WasDown = WasDown;
        }
        else if(keyCode == VK_SPACE)
        {
            m_ControlKeys[KEY_SPACE].IsDown = IsDown;
            m_ControlKeys[KEY_SPACE].WasDown = WasDown;
        }
        else if(keyCode == VK_ESCAPE)
        {
            m_ControlKeys[KEY_ESCAPE].IsDown = IsDown;
            m_ControlKeys[KEY_ESCAPE].WasDown = WasDown;
        }
        else if(keyCode == VK_CAPITAL)
        {
            m_ControlKeys[KEY_CAPSLOCK].IsDown = IsDown;
            m_ControlKeys[KEY_CAPSLOCK].WasDown = WasDown;
        }
        else if(keyCode == VK_TAB)
        {
            m_ControlKeys[KEY_TAB].IsDown = IsDown;
            m_ControlKeys[KEY_TAB].WasDown = WasDown;
        }
        else if(keyCode == VK_RETURN)
        {
            m_ControlKeys[KEY_ENTER].IsDown = IsDown;
            m_ControlKeys[KEY_ENTER].WasDown = WasDown;
        }
        else if(keyCode == VK_BACK)
        {
            m_ControlKeys[KEY_BACKSPACE].IsDown = IsDown;
            m_ControlKeys[KEY_BACKSPACE].WasDown = WasDown;
        }
        else if(keyCode == VK_OEM_3)
        {
            m_ControlKeys[KEY_TILDE].IsDown = IsDown;
            m_ControlKeys[KEY_TILDE].WasDown = WasDown;
        }
    }

    void InputSystem::ListenMouseButtonsPressed(LPARAM lParam, WPARAM wParam)
    {
        bool lmb = MK_LBUTTON & wParam;
        if(lmb && !m_MouseKeys[LEFT].IsDown)
            m_MouseKeys[LEFT].WasDown = true;

        bool rmb = MK_RBUTTON & wParam;
        if(rmb && !m_MouseKeys[RIGHT].IsDown)
            m_MouseKeys[RIGHT].WasDown = true;

        bool mmb = MK_MBUTTON & wParam;
        if(mmb && !m_MouseKeys[MIDDLE].IsDown)
            m_MouseKeys[MIDDLE].WasDown = true;

        m_MouseKeys[LEFT].IsDown = lmb;
        m_MouseKeys[RIGHT].IsDown = rmb;
        m_MouseKeys[MIDDLE].IsDown = mmb;
    }

    void InputSystem::ListenScroll(WPARAM wParam)
    {
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        m_CurrentFrameScroll += zDelta;
        m_CurrentFrameScroll /= 120;
        std::cout << m_CurrentFrameScroll << std::endl;
    }
}