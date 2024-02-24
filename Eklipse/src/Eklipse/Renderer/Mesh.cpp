#include "precompiled.h"
#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Eklipse
{
	static MeshData LoadOBJ(const Path& filePath)
	{
		EK_CORE_PROFILE();
		tinyobj::attrib_t attrib;
		Vec<tinyobj::shape_t> shapes;
		Vec<tinyobj::material_t> materials;
		String warn, err;

		String meshPath = filePath.string();
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str()))
		{
			EK_CORE_ERROR("Failed to load model at location: {0}. {1}", meshPath.c_str(), warn + err);
			return {};
		}

		Vec<float> vertices;
		Vec<uint32_t> indices;

		bool hasNormals = !attrib.normals.empty();
		bool hasColors = !attrib.colors.empty();
		bool hasTexCoords = !attrib.texcoords.empty();

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

				if (hasNormals)
				{
					vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
					vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
					vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
				}
				else
				{
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

				if (hasColors)
				{
					vertices.push_back(attrib.colors[3 * index.vertex_index + 0]);
					vertices.push_back(attrib.colors[3 * index.vertex_index + 1]);
					vertices.push_back(attrib.colors[3 * index.vertex_index + 2]);
				}
				else
				{
                    vertices.push_back(1.0f);
                    vertices.push_back(1.0f);
                    vertices.push_back(1.0f);
                }

				if (hasTexCoords)
				{
					vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
					vertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
				}
				else
				{
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

				indices.push_back(indices.size());
			}
		}

		BufferLayout layout = {
			{ "inPosition",	ShaderDataType::FLOAT3, false },
			{ "inNormal",	ShaderDataType::FLOAT3, false },
			{ "inColor",	ShaderDataType::FLOAT3, false },
            { "inTexCoord", ShaderDataType::FLOAT2, false }
		};

		MeshData meshData;
		meshData.layout = layout;
		meshData.vertices = vertices;
		meshData.indices = indices;
		return meshData;
	}

	Mesh::Mesh(const Path& filePath, const AssetHandle handle)
	{
		EK_CORE_PROFILE();
		Handle = handle;
		String extension = filePath.extension().string();
		if (extension == ".obj")
		{
			m_meshData = LoadOBJ(filePath);
		}
		else
		{
			EK_ASSERT(false, "Mesh format '{0}' is not supported!", extension);
		}

		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(m_meshData.vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_meshData.indices);

		vertexBuffer->SetLayout(m_meshData.layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(indexBuffer);
	}
	Mesh::Mesh(const MeshData& data) : m_meshData(data)
	{
		EK_CORE_PROFILE();
		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(data.vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(data.indices);

		vertexBuffer->SetLayout(data.layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(indexBuffer);
	}
	Ref<Mesh> Mesh::Create(const Path& filePath, const AssetHandle handle)
	{
		EK_CORE_PROFILE();
		return CreateRef<Mesh>(filePath, handle);
	}
	Ref<Mesh> Mesh::Create(const MeshData& data)
	{
		EK_CORE_PROFILE();
		return CreateRef<Mesh>(data);
	}

	void Mesh::Dispose()
	{
		EK_CORE_PROFILE();
		EK_CORE_TRACE("Disposing mesh with handle: {0}", Handle);
	    m_vertexArray->Dispose();
		EK_CORE_DBG("Mesh disposed with handle: {0}", Handle);
	}

	const Ref<VertexArray>& Mesh::GetVertexArray() const
	{
		return m_vertexArray;
	}
	const MeshData& Mesh::GetMeshData() const
	{
		return m_meshData;
	}
	Vec<Triangle> Mesh::GetTriangles() const
	{
		Vec<Triangle> triangles;
		for (uint32_t i = 0; i < m_meshData.indices.size(); i += 3)
		{
			uint32_t stride = m_meshData.layout.GetStride() / sizeof(float);
			uint32_t idx1 = m_meshData.indices[i];
			uint32_t idx2 = m_meshData.indices[i + 1];
			uint32_t idx3 = m_meshData.indices[i + 2];

			glm::vec3 v1(m_meshData.vertices[idx1 * stride], m_meshData.vertices[idx1 * stride + 1], m_meshData.vertices[idx1 * stride + 2]);
			glm::vec3 v2(m_meshData.vertices[idx2 * stride], m_meshData.vertices[idx2 * stride + 1], m_meshData.vertices[idx2 * stride + 2]);
			glm::vec3 v3(m_meshData.vertices[idx3 * stride], m_meshData.vertices[idx3 * stride + 1], m_meshData.vertices[idx3 * stride + 2]);

			triangles.push_back({ v1, v2, v3 });
        }
		return triangles;
	}
	Bounds Mesh::GetBounds() const
	{
		Bounds bounds{};
		bounds.min = { FLT_MAX, FLT_MAX, FLT_MAX };
		bounds.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		for (uint32_t i = 0; i < m_meshData.vertices.size(); i += m_meshData.layout.GetStride())
		{
			glm::vec3 vertex = {
                m_meshData.vertices[i + 0],
                m_meshData.vertices[i + 1],
                m_meshData.vertices[i + 2]
            };
            bounds.min = glm::min(bounds.min, vertex);
            bounds.max = glm::max(bounds.max, vertex);
        }
		return bounds;
	}
}