#include "precompiled.h"
#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Eklipse
{
	static Ref<VertexArray> LoadOBJ(const Path& filePath)
	{
		EK_PROFILE();
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		String warn, err;

		const char* meshPath = filePath.string().c_str();
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath))
		{
			EK_CORE_ERROR("Failed to load model at location: {0}. {1}", meshPath, warn + err);
			return nullptr;
		}

		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

				vertices.push_back(attrib.colors[3 * index.vertex_index + 0]);
				vertices.push_back(attrib.colors[3 * index.vertex_index + 1]);
				vertices.push_back(attrib.colors[3 * index.vertex_index + 2]);

				vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
				vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);

				indices.push_back(indices.size());
			}
		}

		BufferLayout layout = {
			{ "inPosition", ShaderDataType::Float3, false },
			{ "inColor",	ShaderDataType::Float3, false },
			{ "inTexCoord", ShaderDataType::Float2, false }
		};

		Ref<VertexArray> vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);

		vertexBuffer->SetLayout(layout);
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);

		return vertexArray;
	}

	Mesh::Mesh(const Path& filePath)
	{
		String extension = filePath.extension().string();
		if (extension == ".obj")
		{
			m_vertexArray = LoadOBJ(filePath);
		}
		else
		{
			EK_ASSERT(false, "Mesh format '{0}' is not supported!", extension);
		}
	}
	Mesh::Mesh(const MeshData& data)
	{
		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(data.vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(data.indices);

		vertexBuffer->SetLayout(data.layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(indexBuffer);
	}
	Ref<Mesh> Mesh::Create(const Path& filePath)
	{
		return CreateRef<Mesh>(filePath);
	}
	Ref<Mesh> Mesh::Create(const MeshData& data)
	{
		return CreateRef<Mesh>(data);
	}

	const Ref<VertexArray>& Mesh::GetVertexArray() const
	{
		return m_vertexArray;
	}
}