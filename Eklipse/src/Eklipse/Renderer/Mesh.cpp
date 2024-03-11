#include "precompiled.h"
#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <Eklipse/Utils/Hash.h>

namespace Eklipse
{
	// TODO: Optimize buffer layout
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

		std::unordered_map<size_t, uint32_t> vertexMap;

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vec<float> currentVertex;

				currentVertex.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				currentVertex.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				currentVertex.push_back(attrib.vertices[3 * index.vertex_index + 2]);

				if (hasNormals)
				{
					currentVertex.push_back(attrib.normals[3 * index.normal_index + 0]);
					currentVertex.push_back(attrib.normals[3 * index.normal_index + 1]);
					currentVertex.push_back(attrib.normals[3 * index.normal_index + 2]);
				}
				else
				{
                    currentVertex.push_back(0.0f);
                    currentVertex.push_back(0.0f);
                    currentVertex.push_back(0.0f);
                }

				if (hasColors)
				{
					currentVertex.push_back(attrib.colors[3 * index.vertex_index + 0]);
					currentVertex.push_back(attrib.colors[3 * index.vertex_index + 1]);
					currentVertex.push_back(attrib.colors[3 * index.vertex_index + 2]);
				}
				else
				{
                    currentVertex.push_back(1.0f);
                    currentVertex.push_back(1.0f);
                    currentVertex.push_back(1.0f);
                }

				if (hasTexCoords)
				{
					currentVertex.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
					currentVertex.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
				}
				else
				{
                    currentVertex.push_back(0.0f);
                    currentVertex.push_back(0.0f);
                }

				size_t hash = HashVertex(currentVertex);
				if (vertexMap.count(hash) == 0)
                {
                    vertexMap[hash] = vertices.size() / currentVertex.size();
                    vertices.insert(vertices.end(), currentVertex.begin(), currentVertex.end());
                }

				indices.push_back(vertexMap[hash]);
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

	// TODO: Should take account of the buffer layout
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

			Triangle triangle{};
			triangle.a = v1;
			triangle.b = v2;
			triangle.c = v3;
			triangles.push_back(triangle);
        }
		return triangles;
	}

	// TODO: Should take account of the buffer layout
	Vec<float> Mesh::GetVertices() const
	{
		Vec<float> vertices;
		uint32_t stride = m_meshData.layout.GetStride() / sizeof(float);
		for (uint32_t i = 0; i < m_meshData.vertices.size(); i += stride)
		{
            vertices.push_back(m_meshData.vertices[i + 0]);
            vertices.push_back(m_meshData.vertices[i + 1]);
            vertices.push_back(m_meshData.vertices[i + 2]);
        }
		return vertices;
	}

	// TODO: Should take account of the buffer layout
	Vec<uint32_t> Mesh::GetIndices() const
	{
		return m_meshData.indices;
	}

	// TODO: Should take account of the buffer layout
	Bounds Mesh::GetBounds() const // TODO: NOT WORKING
	{
		Bounds bounds{};
		bounds.min = { FLT_MAX, FLT_MAX, FLT_MAX };
		bounds.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		uint32_t stride = m_meshData.layout.GetStride() / sizeof(float);
		for (uint32_t i = 0; i < m_meshData.vertices.size(); i += stride)
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