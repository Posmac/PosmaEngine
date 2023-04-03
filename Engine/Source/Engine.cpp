#include "Engine.h"

namespace psm
{
    Engine* Engine::s_Instance = nullptr;;

    Engine* Engine::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Engine();
        }

        return s_Instance;
    }

    Engine::Engine()
    {
    }

    void Engine::Init(int width, int height)
    {
    }

    void Engine::Dinit()
    {
    }
}