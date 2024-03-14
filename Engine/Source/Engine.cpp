#include "Engine.h"

#include "RHI/RHI.h"
#include "RHI/Interface/Device.h"

namespace psm
{
    Engine* Engine::s_Instance = nullptr;

    Engine* Engine::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new Engine();
        }

        return s_Instance;
    }

    void Engine::Init(HWND hWnd, HINSTANCE hInstance)
    {
        //Log::Init();

        InitOptions options = 
        {
            .CurrentApi = API::Vulkan,
            .AppName = "Minecraft",
            .EngineName = "Posmac Engine",
            .AppVersion{
                .variant = 1,
                .major = 0,
                .minor = 0,
                .patch = 1,
            },
            .EngineVersion{
                .variant = 1,
                .major = 0,
                .minor = 0,
                .patch = 1,
            },
        };
        InitRHI(options);

        psm::PlatformConfig config = 
        {
           .win32 =
           {
               .hInstance = hInstance,
               .hWnd = hWnd,
           }
        };

        DevicePtr device;
        IDevice::CreateSystemDefaultDevice(device, config);
        Renderer::Instance()->Init(device, config);
    }

    void Engine::Dispose()
    {
        Renderer::Instance()->Deinit();
        ShutdownRHI();
    }
}