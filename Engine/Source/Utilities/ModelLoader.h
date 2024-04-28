#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_map>

#include "Model/Mesh.h"
#include "Model/Vertex.h"
#include "Model/Model.h"

#include "Materials/PbrMaterial.h"

#include "RHI/Interface/Types.h"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace psm
{
    class ModelLoader
    {
        //singleton realization
    public:
        ModelLoader(ModelLoader&) = delete;
        void operator=(const ModelLoader&) = delete;
        static ModelLoader* Instance();
    private:
        ModelLoader() = default;
        static ModelLoader* s_Instance;
        //class specific
    public:
        void Init(DevicePtr device, CommandPoolPtr commandPool);
        void LoadModel(const std::string& pathToModel, const std::string& modelName, Model* model, std::vector<MeshPbrMaterial>& modelMeshMaterials);
    private:
        void ProcessNode(aiNode* node, const aiScene* scene, Model* model, const std::string& pathToModel, std::vector<MeshPbrMaterial>& modelMeshMaterials);
        void ProcessMesh(aiMesh* mesh, const aiScene* scene, Model* model, const std::string& pathToModel, std::vector<MeshPbrMaterial>& modelMeshMaterials);
    private:
        DevicePtr mDevice;
        CommandPoolPtr mCommandPool;
    };
}