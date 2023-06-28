#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_map>

#include "Core/Log.h"

#include "tiny_obj_loader.h"

#include "Model/Mesh.h"
#include "Model/Vertex.h"
#include "Model/Model.h"

namespace psm
{
    namespace putils
    {
        void LoadModel(const std::string& path, Model* model);
    }
}