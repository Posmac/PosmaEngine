#include "ModelLoader.h"

#include "RHI/Common.h"
#include "RHI/RHICommon.h"

#include "TextureLoader.h"
#include "Render/Renderer.h"

namespace psm
{
    ModelLoader* ModelLoader::s_Instance = nullptr;

    ModelLoader* ModelLoader::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new ModelLoader();
        }

        return s_Instance;
    }

    void ModelLoader::LoadModel(const std::string& pathToModel, const std::string& modelName, Model* model, std::vector<MeshPbrMaterial>& modelMeshMaterials)
    {
        Assimp::Importer importer;

        //importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, aiComponent_TANGENTS_AND_BITANGENTS);
        //importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, true);

        std::string fullPath = pathToModel + modelName;
        const aiScene* scene = importer.ReadFile(fullPath, aiProcess_PreTransformVertices);

        model->Name = fullPath;

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LogMessage(MessageSeverity::Error, "ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
            __debugbreak();
            return;
        }

        aiNode* rootNode = scene->mRootNode;
        ProcessNode(rootNode, scene, model, pathToModel, modelMeshMaterials);
        model->Init(mDevice, mCommandPool);
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Model* model, const std::string& pathToModel, std::vector<MeshPbrMaterial>& modelMeshMaterials)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            const glm::mat4 nodeToParent = reinterpret_cast<const glm::mat4&>(node->mTransformation.Transpose());
            const glm::mat4 parentToNode = glm::inverse(nodeToParent);
            Mesh mesh =
            {
                .LocalMatrix = nodeToParent,
                .InvLocalMatrix = parentToNode
            };

            model->Meshes.push_back(mesh);
            modelMeshMaterials.push_back({});
            ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, model, pathToModel, modelMeshMaterials);
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model, pathToModel, modelMeshMaterials);
        }
    }

    void ModelLoader::ProcessMesh(aiMesh* aiMesh, const aiScene* scene, Model* model, const std::string& pathToModel, std::vector<MeshPbrMaterial>& modelMeshMaterials)
    {
        //std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        unsigned meshIndex = model->Meshes.size() - 1;
        Mesh& mesh = model->Meshes[meshIndex];

        mesh.Range.VerticesOffset = model->MeshVertices.size();
        mesh.Range.IndicesOffset = model->MeshIndices.size();

        for(unsigned int i = 0; i < aiMesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.Position = { aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z, 1.0 };
            vertex.Normal = { aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z, 1.0 };
            if(aiMesh->mTextureCoords[0])
            {
                vertex.TexCoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.TexCoord = { 0, 0 };
            }

            model->MeshVertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < aiMesh->mNumFaces; i++)
        {
            aiFace face = aiMesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                model->MeshIndices.push_back(face.mIndices[j]);
            }
        }

        mesh.Range.VerticesCount = model->MeshVertices.size() - mesh.Range.VerticesOffset;
        mesh.Range.IndicesCount = model->MeshIndices.size() - mesh.Range.IndicesOffset;

        //load material
        if(scene->mNumMaterials)
        {
            aiMaterial* meshMaterial = scene->mMaterials[aiMesh->mMaterialIndex];
            if(meshMaterial != nullptr)
            {
                auto albedoTextureCount = meshMaterial->GetTextureCount(aiTextureType_BASE_COLOR);
                std::string pathToTexture;

                MeshPbrMaterial& pbrMat = modelMeshMaterials[meshIndex];

                if(albedoTextureCount)
                {
                    aiString path;
                    meshMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
                    pathToTexture = pathToModel + std::string(path.C_Str());
                    RawTextureData albedoTextureData{ Rgb_alpha };
                    TextureLoader::Instance()->LoadRawTextureData(pathToTexture, &albedoTextureData);
                    pbrMat.Albedo = Renderer::Instance()->LoadTextureIntoMemory(albedoTextureData, 1);
                }
                else
                {
                    pbrMat.Albedo = TextureLoader::Instance()->GetWhiteTexture();
                }
            }
        }
    }

    void ModelLoader::Init(DevicePtr device, CommandPoolPtr commandPool)
    {
        mDevice = device;
        mCommandPool = commandPool;
    }

    void ModelLoader::Deinit()
    {
        //empty for now
    }
}
