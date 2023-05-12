#pragma once

#include "Render/Renderer.h"
#include "Vulkan/BaseVulkan.h"

namespace sandbox
{
    class Application
    {
    public:
        Application() = default;
        void Init();
        void Render();
        void Deinit();
    private:

    };
}