#include "precompiled.h"
#include "Mesh.h"

#include <Eklipse/Scene/Assets.h>

namespace Eklipse
{
	Mesh::Mesh(const char* modelPath, const char* texturePath)
	{
		auto& meshLoadResult = AssetLoader::LoadMesh(modelPath);
		auto& textureLoadResult = AssetLoader::LoadTexture(texturePath);

		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(meshLoadResult.vertices);

		BufferLayout layout = {
			{ "inPosition", ShaderDataType::Float3, false },
			{ "inColor",	ShaderDataType::Float3, false },
			{ "inTexCoord", ShaderDataType::Float2, false }
		};

		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(IndexBuffer::Create(meshLoadResult.indices));

		//m_uniformBuffer = UniformBuffer::Create(sizeof(m_ubo), 0);

		TextureInfo texInfo{};
		texInfo.width = textureLoadResult.width;
		texInfo.height = textureLoadResult.height;
		texInfo.imageFormat = ImageFormat::RGBA8; // size = 4
		texInfo.mipMapLevel = 1;
		m_texture = Texture2D::Create(texInfo);
		m_texture->SetData(textureLoadResult.data, texInfo.width * texInfo.height * 4);
	}
}