#include <iostream>

#include "Window/WindowsWindow.h"

int main(void)
{
    psm::WindowsWindow window;
    window.Init(1080, 720);

    while (window.IsOpen())
    {
        window.PollEvents();
    }

    return 0;
}