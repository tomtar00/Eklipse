#pragma once
#include "Transform.h"

#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Renderer/Uniforms.h>
#include <Eklipse/Renderer/Vertex.h>

namespace Eklipse
{
	class Model
	{
	public:
		void Load(const char* modelPath, const char* texturePath);
		void OnUpdate(glm::mat4 viewProjMatrix);

		Transform m_transform;

		ModelViewProjUBO m_ubo;
		TextureData m_textureData;

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	};
}