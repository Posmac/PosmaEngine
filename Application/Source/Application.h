#pragma once

#include <vector>

#include "Render/Renderer.h"
#include "Utilities/ModelLoader.h"
#include "Utilities/TextureLoader.h"

namespace psm
{
    class Application
    {
    public:
        Application() = default;
        void Init();
        void Update();
        void Deinit();
    private:
        float time = 0;
    };
}