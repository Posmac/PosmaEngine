#pragma once

#include <vector>

#include "Render/Renderer.h"

#include "Mesh/Mesh.h"

namespace psm
{
    class Application
    {
    public:
        Application() = default;
        void Init();
        void Update();
        void Deinit();
        void LoadModelData();
    private:
        float time = 0;
  
    };
}