
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Application.h"
#include "Engine.h"
#include "Core/Log.h"
#include "Systems/InputSystem.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

FILE* pCout;
void CreateConsole();
void CloseConsole();

psm::Application app;

constexpr uint32_t Width = 1280;
constexpr uint32_t Height = 720;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    CreateConsole();

    auto windowClassName = L"Posmac Vulkan Engine";
    auto windowName = L"Engine";

    WNDCLASSEX wndClassDescr{};
    wndClassDescr.cbSize = sizeof(WNDCLASSEX);
    wndClassDescr.style = CS_HREDRAW | CS_VREDRAW;
    wndClassDescr.cbClsExtra = 0;
    wndClassDescr.cbWndExtra = 0;
    wndClassDescr.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);;
    wndClassDescr.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wndClassDescr.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wndClassDescr.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
    wndClassDescr.hInstance = hInstance;
    wndClassDescr.lpfnWndProc = WindowProc;
    wndClassDescr.lpszClassName = windowClassName;
    wndClassDescr.lpszMenuName = NULL;

    if(!RegisterClassEx(&wndClassDescr))
    {
        std::cout << "Failed to register class" << std::endl;
        return -1;
    }

    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
        windowClassName, windowName, WS_OVERLAPPEDWINDOW,
        320, 180, Width, Height, NULL, NULL, hInstance, NULL);

    if(hWnd == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);

    psm::Engine::Instance()->Init(hWnd, hInstance);
    app.Init(Width, Height);

    bool isAppRuning = true;
    while(isAppRuning)
    {
        MSG message;
        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if(message.message == WM_QUIT)
            {
                isAppRuning = false;
                break;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        app.Update();
    }

    psm::Engine::Instance()->Dispose();
    CloseConsole();

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch(uMsg)
    {
        case WM_MOUSEWHEEL:
            psm::InputSystem::Instance()->ListenScroll(wParam);
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            psm::InputSystem::Instance()->ListenMouseButtonsPressed(lParam, wParam);
            break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            psm::InputSystem::Instance()->ListenControlsKeyPressed(lParam, wParam);
            break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            app.ResizeWindow(hWnd, width, height);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CreateConsole()
{
    AllocConsole();
    freopen_s(&pCout, "conout$", "w", stdout);
    std::cout << "CONSOLE INIT" << std::endl;
}

void CloseConsole()
{
    fclose(pCout);
    FreeConsole();
}