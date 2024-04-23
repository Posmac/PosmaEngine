#include "ModelLoader.h"

#include "RHI/Common.h"
#include "RHI/VkCommon.h"

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

    void ModelLoader::LoadModel(const std::string& path, Model* model)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenUVCoords);

        model->Name = path;

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LogMessage(MessageSeverity::Error, "ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
            __debugbreak();
            return;
        }

        aiNode* rootNode = scene->mRootNode;
        ProcessNode(rootNode, scene, model);
        model->Init(mDevice, mCommandPool);
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Model* model)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            const glm::mat4 nodeToParent = reinterpret_cast<const glm::mat4&>(node->mTransformation/*.Transpose()*/);
            const glm::mat4 parentToNode = glm::inverse(nodeToParent);
            Mesh mesh =
            {
                .LocalMatrix = nodeToParent,
                .InvLocalMatrix = parentToNode
            };

            model->Meshes.push_back(mesh);
            ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, model);
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model);
        }
    }

    void ModelLoader::ProcessMesh(aiMesh * aiMesh, const aiScene * scene, Model* model)
    {
        //std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        Mesh& mesh = model->Meshes[model->Meshes.size() - 1];

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

            /*if(uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(mesh.MeshVertices.size());
            }*/

            mesh.MeshVertices.push_back(vertex);

            //mesh.MeshIndices.push_back(uniqueVertices[vertex]);
        }

        for(unsigned int i = 0; i < aiMesh->mNumFaces; i++)
        {
            aiFace face = aiMesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                mesh.MeshIndices.push_back(face.mIndices[j]);
            }
        }

        mesh.Range.IndicesCount = mesh.MeshIndices.size();
        mesh.Range.VerticesCount = mesh.MeshVertices.size();
        mesh.Range.IndicesOffset = 0;
        mesh.Range.VerticesOffset = 0;
    }

    void ModelLoader::Init(DevicePtr device, CommandPoolPtr commandPool)
    {
        mDevice = device;
        mCommandPool = commandPool;
    }
}
