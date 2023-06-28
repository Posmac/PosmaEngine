#include "ModelLoader.h"

namespace psm
{
    namespace putils
    {
        void LoadModel(const std::string& path, Model* model)
        {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
            {
                LOG_ERROR("Failed to load model: " + path);
                assert(true);
                return;
            }

            std::unordered_map<Vertex, uint32_t> uniqueVertices{};

            Mesh mesh{};

            for (const auto& shape : shapes) 
            {
                for (const auto& index : shape.mesh.indices)
                {
                    Vertex vertex{};

                    vertex.Position =
                    {
                        glm::vec4(
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                        1.0)
                    };

                    vertex.TexCoord = 
                    {
                        glm::vec2(
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1])
                    };

                    vertex.Normal =
                    {
                        glm::vec4(
                        attrib.normals[3 * index.normal_index + 0] ,
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                        1.0)
                    };

                    if (uniqueVertices.count(vertex) == 0) 
                    {
                        uniqueVertices[vertex] = 
                            static_cast<uint32_t>(mesh.MeshVertices.size());
                        mesh.MeshVertices.push_back(vertex);
                    }

                    mesh.MeshIndices.push_back(uniqueVertices[vertex]);
                }
            }

            mesh.Range.IndicesCount =  mesh.MeshIndices.size();
            mesh.Range.VerticesCount = mesh.MeshVertices.size();
            mesh.Range.IndicesOffset = 0;
            mesh.Range.VerticesOffset = 0;

            model->Meshes.push_back(mesh);
        }
    }
}
