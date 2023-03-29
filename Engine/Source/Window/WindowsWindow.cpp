#include "WindowsWindow.h"
#include <iostream>

namespace psm
{
    WindowsWindow::WindowsWindow() 
        : m_Window(nullptr)
    {

    }

    void WindowsWindow::Init(int width, int height)
    {
        if (!glfwInit())
        {
            std::cout << "Cannot init GLFW\n";
        }

        m_Window = glfwCreateWindow(width, height, "Psm Engine", NULL, NULL);
        glfwMakeContextCurrent(m_Window);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    void WindowsWindow::Deinit()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool WindowsWindow::IsOpen()
    {
        return !glfwWindowShouldClose(m_Window);
    }

    void WindowsWindow::PollEvents()
    {
        glfwPollEvents();
    }
}
