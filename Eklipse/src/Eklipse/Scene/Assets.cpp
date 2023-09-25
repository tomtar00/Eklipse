#include "precompiled.h"
#include "Assets.h"

#include <Eklipse/Renderer/Vertex.h>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace std
{
    template<> struct hash<Eklipse::Vertex>
    {
        size_t operator()(Eklipse::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Eklipse
{
    MeshLoadResult AssetLoader::LoadMesh(const char* modelPath)
	{
        MeshLoadResult result{};

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath))
        {
            EK_CORE_ERROR("Failed to load model at location: {0}. {1}", modelPath, warn + err);
        }

        std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (attrib.texcoords.size() > 0)
                {
                    vertex.texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(result.vertices.size() / 8);
                    result.vertices.push_back(vertex.pos.x);
                    result.vertices.push_back(vertex.pos.y);
                    result.vertices.push_back(vertex.pos.z);
                    result.vertices.push_back(vertex.color.r);
                    result.vertices.push_back(vertex.color.g);
                    result.vertices.push_back(vertex.color.b);
                    result.vertices.push_back(vertex.texCoord.x);
                    result.vertices.push_back(vertex.texCoord.y);
                }

                result.indices.push_back(uniqueVertices[vertex]);
            }
        }

        EK_CORE_INFO("Loaded model from path {0}. Vertices: {1} Indices: {2}", modelPath, result.vertices.size() / 8, result.indices.size());

        return result;
	}
    TextureLoadResult AssetLoader::LoadTexture(const char* texturePath)
    {
        TextureLoadResult result{};
        result.data = stbi_load(texturePath, &result.width, &result.height, nullptr, STBI_rgb_alpha);
        EK_ASSERT(result.data, "Failed to load texture image from location: {0}", texturePath);
        EK_CORE_INFO("Loaded texture from path {0}. Width: {1} Height: {2}", texturePath, result.width, result.height);
        return result;
    }
}
