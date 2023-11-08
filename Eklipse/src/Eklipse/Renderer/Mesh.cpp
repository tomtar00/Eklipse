#include "precompiled.h"
#include "Mesh.h"

namespace Eklipse
{
	Mesh::Mesh(std::vector<float> vertices, std::vector<uint32_t> indices, const std::string& path) : m_path(path)
	{
		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);

		// TODO: Make mesh format dependent
		BufferLayout layout = {
			{ "inPosition", ShaderDataType::Float3, false },
			{ "inColor",	ShaderDataType::Float3, false },
			{ "inTexCoord", ShaderDataType::Float2, false }
		};

		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(IndexBuffer::Create(indices));
	}
}