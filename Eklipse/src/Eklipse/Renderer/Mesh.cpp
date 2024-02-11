#include "precompiled.h"
#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Eklipse
{
	static Ref<VertexArray> LoadOBJ(const Path& filePath)
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
			return nullptr;
		}

		Vec<float> vertices;
		Vec<uint32_t> indices;

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
				vertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

				indices.push_back(indices.size());
			}
		}

		BufferLayout layout = {
			{ "inPosition", ShaderDataType::FLOAT3, false },
			{ "inColor",	ShaderDataType::FLOAT3, false },
			{ "inTexCoord", ShaderDataType::FLOAT2, false }
		};

		Ref<VertexArray> vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);

		vertexBuffer->SetLayout(layout);
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);

		return vertexArray;
	}

	Mesh::Mesh(const Path& filePath, const AssetHandle handle)
	{
		EK_CORE_PROFILE();
		Handle = handle;
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
}