#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Window/WindowsWindow.h"
#include "Render/Renderer.h"
#include "Core/Log.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

FILE* pCout;
void CreateConsole();
void CloseConsole();

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

    if (!RegisterClassEx(&wndClassDescr))
    {
        std::cout << "Failed to register class" << std::endl;
        return -1;
    }

    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, 
        windowClassName, windowName, WS_OVERLAPPEDWINDOW, 
        320, 180, 1280, 720, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);

    IMGUI_CHECKVERSION();
    ImGui::SetCurrentContext(ImGui::CreateContext());
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hWnd);

    psm::Log::Init();
    psm::Renderer::Instance()->Init(hInstance, hWnd);


    bool isAppRuning = true;
    while (isAppRuning)
    {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                isAppRuning = false;
                break;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (isAppRuning)
        {
            psm::Renderer::Instance()->Render();
        }
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    psm::Renderer::Instance()->Deinit();
    CloseConsole();

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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