#include "precompiled.h"
#include "VulkanModel.h"

#include <tiny_obj_loader.h>

namespace Eklipse
{
	void VulkanModel::Load(char* modelPath, char* texturePath)
	{
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::vector<VulkanVertex> vertices;
        std::vector<uint32_t> indices;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath)) 
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<VulkanVertex, uint32_t, std::hash<VulkanVertex>> uniqueVertices{};

        for (const auto& shape : shapes) 
        {
            for (const auto& index : shape.mesh.indices) 
            {
                VulkanVertex vertex;

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                vertices.push_back(vertex);

                if (uniqueVertices.count(vertex) == 0) 
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        m_texture.Load(texturePath);
        m_vertexBuffer.Init(vertices);
        m_indexBuffer.Init(indices);
	}
    void VulkanModel::Shutdown()
    {
        m_texture.Shutdown();
        m_vertexBuffer.Shutdown();
        m_indexBuffer.Shutdown();
    }
    VulkanVertexBuffer& VulkanModel::VertexBuffer()
    {
        return m_vertexBuffer;
    }
    VulkanIndexBuffer& VulkanModel::IndexBuffer()
    {
        return m_indexBuffer;
    }
    VulkanTexture& VulkanModel::Texture()
    {
        return m_texture;
    }
}
