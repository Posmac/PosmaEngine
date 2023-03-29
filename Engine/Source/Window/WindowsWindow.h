#pragma once

#include "Include/vulkan/vulkan.h"
#include "GLFW/glfw3.h"

namespace psm
{
    class WindowsWindow
    {
    public:
        WindowsWindow();
        void Init(int width, int height);
        void Deinit();
        bool IsOpen();
        void PollEvents();
    private:
        GLFWwindow* m_Window;
    };
}